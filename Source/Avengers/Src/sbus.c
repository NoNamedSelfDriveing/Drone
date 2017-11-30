/**********************************************************************
 *     sbus.c                                                         *
 *     written by Soomin Lee (MagmaTart)                              *
 *     Last modify date : 2017.11.30                                  *
 *     Description : Implements of Functions to use SBUS protocol.    *
 **********************************************************************/

#include <string.h>
#include <stdio.h>
#include "stm32f4xx_hal.h"
#include "sbus.h"
#include "main.h"
#include "tim.h"
#include "gpio.h"
#include "usart.h"

SBUS sbus;
uint8_t sbus_dma_receive_buff[SBUS_DMA_RECEIVE_SIZE];
uint8_t sbus_packet_buff[SBUS_DATA_SIZE];
uint16_t data_buff[18];
uint16_t sbus_pwm_pulse[6];

extern DMA_HandleTypeDef hdma_usart1_rx;

int read_sbus()
{
  make_next_decodeable_buffer();
  if(sbus.packet_ok_flag)
  {
    decode_sbus_data();
	make_sbus_pwm_value();
	return 1;
  }else{
	return 0;
  }
}

void init_sbus(){
  sbus.new_packet_flag = 1;
  sbus.packet_ok_flag = 0;
}

void make_next_decodeable_buffer()
{
    static int prev_ndt = SBUS_DMA_RECEIVE_SIZE;
    static int curr_ndt = 0;
    static int new_data_start_idx = 0;
    static int next_data_start_idx = 0;
    static int packet_start_idx = 0;
    static int packet_end_idx = SBUS_DMA_RECEIVE_SIZE;
    static int check_idx = 0;
    uint16_t received_data_size = 0;                                                             //������ ������ ��
    
    /* ���� ndt ���ϱ� */
    curr_ndt = __HAL_DMA_GET_COUNTER(&hdma_usart1_rx);
	
	/* ���� ndt�� ���� ndt�� ���̿� ���� ó�� */
    if(prev_ndt > curr_ndt)
    {
      received_data_size = prev_ndt - curr_ndt;
    }
    else if(prev_ndt < curr_ndt)
    {
      received_data_size = prev_ndt + (SBUS_DMA_RECEIVE_SIZE - curr_ndt);
    }
    else
    {
      return;
    }

    /* DMA�� ���ŵǴ� ������ ���� �ε���(���������� ���� �ε��� + 1) ���ϱ� */
    next_data_start_idx = (new_data_start_idx + received_data_size) % SBUS_DMA_RECEIVE_SIZE;  // ���� �����͸� ���� �ε��� ����

    /* ���� ���� �������� ���� �ε������� ������ �ε������� �˻� */
    check_idx = new_data_start_idx;

    /* ���� ���� �������� ������ �ε������� �˻縦 ��ġ�� break */
    while(!(check_idx == next_data_start_idx))
    {  
      /* ��ŸƮ ����Ʈ �˻� */
      if((sbus.new_packet_flag) && (sbus_dma_receive_buff[check_idx] == START_BYTE))
      {
	    sbus.new_packet_flag = 0;
        packet_start_idx = check_idx;
        packet_end_idx = (packet_start_idx + (SBUS_DATA_SIZE - 1)) % SBUS_DMA_RECEIVE_SIZE;  // ������ ����Ʈ��ŭ ����
      }
      
      /* �����۷� ���� �ε��� ��ȭ ó�� */
      check_idx = (check_idx+1) % SBUS_DMA_RECEIVE_SIZE;
      
	  /* END ����Ʈ�� ���Դٸ�*/
      if((check_idx == packet_end_idx) && ((sbus_dma_receive_buff[check_idx] & 0x04) == END_BYTE))
      {
		if(packet_start_idx < packet_end_idx)
        {              
          memcpy(sbus_packet_buff, (sbus_dma_receive_buff + packet_start_idx), ((packet_end_idx - packet_start_idx) + 1));
        }
        else if(packet_start_idx > packet_end_idx)
        {
          memcpy(sbus_packet_buff, (sbus_dma_receive_buff + packet_start_idx), (SBUS_DMA_RECEIVE_SIZE - packet_start_idx));
          memcpy((sbus_packet_buff + (SBUS_DMA_RECEIVE_SIZE - packet_start_idx)), sbus_dma_receive_buff, (packet_end_idx + 1));
        }
		
        sbus.new_packet_flag = 1;
        sbus.packet_ok_flag = 1;
      }
    }
    
    /* ���� ���� ���� ������ ���� */
    prev_ndt = curr_ndt;
    new_data_start_idx = next_data_start_idx;
}

void decode_sbus_data()
{
  uint8_t i;
  
  sbus.packet_ok_flag = 0;
  
  data_buff[0] = (uint16_t)sbus_packet_buff[1] + (uint16_t)((sbus_packet_buff[2]&0x07)<<8)-4;
  data_buff[1] = (uint16_t)((sbus_packet_buff[2]&0xf8)>>3) + (uint16_t)((sbus_packet_buff[3]&0x3f)<<5);
  data_buff[2] = (uint16_t)((sbus_packet_buff[3]&0xc0)>>6) + (uint16_t)(sbus_packet_buff[4]<<2) + (uint16_t)((sbus_packet_buff[5]&0x01)<<10);
  data_buff[3] = (uint16_t)((sbus_packet_buff[5]&0xfe)>>1) + (uint16_t)((sbus_packet_buff[6]&0x0f)<<7);
  data_buff[4] = (uint16_t)((sbus_packet_buff[6]&0xf0)>>4) + (uint16_t)((sbus_packet_buff[7]&0x7f)<<4);
  data_buff[5] = (uint16_t)((sbus_packet_buff[7]&0x80)>>7) + (uint16_t)(sbus_packet_buff[8]<<1) + (uint16_t)((sbus_packet_buff[9]&0x03)<<9);
  data_buff[6] = (uint16_t)((sbus_packet_buff[9]&0xfc)>>2) + (uint16_t)((sbus_packet_buff[10]&0x1f)<<6);
}

void make_sbus_pwm_value(){
  const float min_duty = 4598;
  const float max_duty = 8126;
  const float max_pwm = 1696;
  const float min_pwm = 352;
  int i;
 
  for(i = 0; i < 4; i++){
    sbus_pwm_pulse[i] = data_buff[i] / ((max_pwm - min_pwm) / (max_duty - min_duty)) + 3696;
  }
}

void generate_sbus_pwm(TIM_HandleTypeDef * htim){  
  htim -> Instance -> CCR1 = sbus_pwm_pulse[0];
  htim -> Instance -> CCR2 = sbus_pwm_pulse[1];
  htim -> Instance -> CCR3 = sbus_pwm_pulse[2];
  htim -> Instance -> CCR4 = sbus_pwm_pulse[3];
}
