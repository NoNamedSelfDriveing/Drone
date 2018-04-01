/**********************************************************************
 *     sbus.c                                                         *
 *     written by Soomin Lee (MagmaTart)                              *
 *     Last modify date : 2017.11.30                                  *
 *     Description : Implements of Functions to use SBUS protocol.    *
 **********************************************************************/

#include <string.h>
#include <stdio.h>
#include "stm32f4xx_hal.h"
#include "mti.h"
#include "sbus.h"
#include "main.h"
#include "tim.h"
#include "gpio.h"
#include "usart.h"

SBUS sbus;
uint8_t sbus_dma_receive_buff[SBUS_DMA_RECEIVE_SIZE];
uint8_t sbus_packet_buff[SBUS_PACKET_SIZE];
uint16_t sbus_data_buff[18];
uint16_t sbus_pwm_pulse[6];

extern DMA_HandleTypeDef hdma_usart1_rx;

void read_sbus()
{
  make_next_decodeable_buffer();
  if(sbus.packet_ok_flag)
  {
    decode_sbus_data();
    sbus.count++;
  }
}

void init_sbus()
{
  sbus.new_packet_flag = 1;
  sbus.packet_ok_flag = 0;
  sbus.count = 0;
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
    uint16_t received_data_size = 0;                                                             //수신한 데이터 수
    
    /* 현재 ndt 구하기 */
    curr_ndt = __HAL_DMA_GET_COUNTER(&hdma_usart1_rx);
	
	/* 이전 ndt와 현재 ndt의 차이에 따른 처리 */
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

    /* DMA로 수신되는 버퍼의 현재 인덱스(마지막으로 들어온 인덱스 + 1) 구하기 */
    next_data_start_idx = (new_data_start_idx + received_data_size) % SBUS_DMA_RECEIVE_SIZE;  // 다음 데이터를 쌓을 인덱스 결정

    /* 현재 들어온 데이터의 시작 인덱스부터 마지막 인덱스까지 검사 */
    check_idx = new_data_start_idx;

    /* 현재 들어온 데이터의 마지막 인덱스까지 검사를 마치면 break */
    while(!(check_idx == next_data_start_idx))
    {  
      /* 스타트 바이트 검사 */
      if((sbus.new_packet_flag) && (sbus_dma_receive_buff[check_idx] == START_BYTE))
      {
	    sbus.new_packet_flag = 0;
        packet_start_idx = check_idx;
        packet_end_idx = (packet_start_idx + (SBUS_PACKET_SIZE - 1)) % SBUS_DMA_RECEIVE_SIZE;  // 데이터 바이트만큼 적용
      }
      
      /* 링버퍼로 인한 인덱스 변화 처리 */
      check_idx = (check_idx+1) % SBUS_DMA_RECEIVE_SIZE;
      
	  /* END 바이트가 들어왔다면*/
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
    
    /* 현재 값을 이전 값으로 세팅 */
    prev_ndt = curr_ndt;
    new_data_start_idx = next_data_start_idx;
}

void decode_sbus_data()
{
  sbus.packet_ok_flag = 0;
  
  sbus_data_buff[CH1] = (uint16_t)sbus_packet_buff[1] + (uint16_t)((sbus_packet_buff[2]&0x07)<<8);
  sbus_data_buff[CH2] = (uint16_t)((sbus_packet_buff[2]&0xf8)>>3) + (uint16_t)((sbus_packet_buff[3]&0x3f)<<5);
  sbus_data_buff[CH3] = (uint16_t)((sbus_packet_buff[3]&0xc0)>>6) + (uint16_t)(sbus_packet_buff[4]<<2) + (uint16_t)((sbus_packet_buff[5]&0x01)<<10);
  sbus_data_buff[CH4] = (uint16_t)((sbus_packet_buff[5]&0xfe)>>1) + (uint16_t)((sbus_packet_buff[6]&0x0f)<<7);
  sbus_data_buff[CH5] = (uint16_t)((sbus_packet_buff[6]&0xf0)>>4) + (uint16_t)((sbus_packet_buff[7]&0x7f)<<4);
  sbus_data_buff[CH6] = (uint16_t)((sbus_packet_buff[7]&0x80)>>7) + (uint16_t)(sbus_packet_buff[8]<<1) + (uint16_t)((sbus_packet_buff[9]&0x03)<<9);
  sbus_data_buff[CH7] = (uint16_t)((sbus_packet_buff[9]&0xfc)>>2) + (uint16_t)((sbus_packet_buff[10]&0x1f)<<6);
  sbus_data_buff[CH8] = (uint16_t)((sbus_packet_buff[10]&0xe0)>>5) + (uint16_t)(sbus_packet_buff[11]<<3);
  sbus_data_buff[CH9] = (uint16_t)sbus_packet_buff[12] + (uint16_t)((sbus_packet_buff[13]&0x07)<<8);
  sbus_data_buff[CH10] = (uint16_t)((sbus_packet_buff[13]&0xf8)>>3) + (uint16_t)((sbus_packet_buff[14]&0x3f)<<5);
  sbus_data_buff[CH11] = (uint16_t)((sbus_packet_buff[14]&0xc0)>>6) + (uint16_t)(sbus_packet_buff[15]<<2) + (uint16_t)((sbus_packet_buff[16]&0x01)<<10);
  sbus_data_buff[CH12] = (uint16_t)((sbus_packet_buff[16]&0xfe)>>1) + (uint16_t)((sbus_packet_buff[17]&0x0f)<<7); 
  sbus_data_buff[CH13] = (uint16_t)((sbus_packet_buff[17]&0xf0)>>4) + (uint16_t)((sbus_packet_buff[18]&0x7f)<<4);
  sbus_data_buff[CH14] = (uint16_t)((sbus_packet_buff[18]&0x80)>>7) + (uint16_t)(sbus_packet_buff[19]<<1) + (uint16_t)((sbus_packet_buff[20]&0x03)<<9);
  sbus_data_buff[CH15] = (uint16_t)((sbus_packet_buff[20]&0xfc)>>2) + (uint16_t)((sbus_packet_buff[21]&0x1f)<<6);
  sbus_data_buff[CH16] = (uint16_t)((sbus_packet_buff[21]&0xe0)>>5) + (uint16_t)(sbus_packet_buff[22]<<3);
}

