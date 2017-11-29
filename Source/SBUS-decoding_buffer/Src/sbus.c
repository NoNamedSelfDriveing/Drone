/**********************************************************************
 *     sbus.c                                                         *
 *     written by Soomin Lee (MagmaTart)                              *
 *     Last modify date : 2017.11.27                                  *
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
SBUS_pwm sbus_pwm;
extern DMA_HandleTypeDef hdma_usart1_rx;

void init_sbus_pwm(){
  sbus_pwm.min_duty = 4598;
  sbus_pwm.max_duty = 8126;
  sbus_pwm.max_pwm = 1696;
  sbus_pwm.min_pwm = 352;
  sbus_pwm.pwm = 0;
}

void init_sbus(){
  sbus.prev_ndt = DMA_RECEIVE_SIZE;
  sbus.curr_ndt = DMA_RECEIVE_SIZE;
  sbus.received_size = 0;

  sbus.front = 0;
  sbus.rear = 0;
  
  sbus.new_packet_flag = 1;
  sbus.packet_ok_flag = 0;
}

/*
void update_buffer(){
  uint16_t subtract_ndt;
  sbus.prev_ndt = sbus.curr_ndt;
  sbus.curr_ndt = __HAL_DMA_GET_COUNTER(&hdma_usart1_rx);
  
  subtract_ndt = sbus.prev_ndt - sbus.curr_ndt;
  
  if(sbus.curr_ndt < sbus.prev_ndt){
    sbus.received_size += subtract_ndt;

    sbus.rear += subtract_ndt;
    sbus.rear = sbus.rear >= DMA_RECEIVE_SIZE ? sbus.rear - DMA_RECEIVE_SIZE : sbus.rear;
	
	//printf("RECV : %d", sbus.received_size);	
  }
  else if(sbus.curr_ndt > sbus.prev_ndt){
	sbus.received_size += (subtract_ndt + DMA_RECEIVE_SIZE);
	
  	sbus.rear += (subtract_ndt + DMA_RECEIVE_SIZE);
    sbus.rear = sbus.rear >= DMA_RECEIVE_SIZE ? sbus.rear - DMA_RECEIVE_SIZE : sbus.rear;
	
	//printf("RECV : %d", sbus.received_size);
  }
  printf("");
}
*/
void make_next_decodeable_buffer()
{
  //static int sbus.prev_ndt = DMA_RECEIVE_SIZE, sbus.curr_ndt = 0;                                //이전 ndt, 현재 ndt
    uint16_t i;                                                            // 버퍼의 내용을 검사할 인덱스와 그 옆의 인덱스                                                                                         
    uint8_t received_data_size = 0;                                                             //수신한 데이터 수
    
    /* 현재 ndt 구하기 */
    sbus.curr_ndt = __HAL_DMA_GET_COUNTER(&hdma_usart1_rx);

    /* 이전 ndt와 현재 ndt의 차이에 따른 처리 */
    // 여기 돌려보고 안되면 sbus.prev, sbus.curr로 변경
    if(sbus.prev_ndt > sbus.curr_ndt)
    {
      received_data_size = sbus.prev_ndt - sbus.curr_ndt;
    }
    else if(sbus.prev_ndt < sbus.curr_ndt)
    {
      received_data_size = sbus.prev_ndt + (DMA_RECEIVE_SIZE - sbus.curr_ndt);
    }
    else
    {
      return;
    }

    /* DMA로 수신되는 버퍼의 현재 인덱스(마지막으로 들어온 인덱스 + 1) 구하기 */
    //rx_size += received_data_size;
	//sbus.received_size += received_data_size;
    sbus.next_data_start_idx = (sbus.new_data_start_idx + received_data_size) % DMA_RECEIVE_SIZE;  // 다음 데이터를 쌓을 인덱스 결정

    /* 현재 들어온 데이터의 시작 인덱스부터 마지막 인덱스까지 검사 */
    //check_idx = new_data_start_idx -1 ;     // 근재같은경우 스타트 바이트가 2개이므로, 바이트 두개가 나뉘어져 들어오는 경우를 위한 처리
    sbus.check_idx = sbus.new_data_start_idx;
    //next_check_idx = check_idx + 1;

    // 음수처리
    if(sbus.check_idx == -1) 
    {
      sbus.check_idx = DMA_RECEIVE_SIZE - 1;
      //next_check_idx = 0;
    }

    /* 현재 들어온 데이터의 마지막 인덱스까지 검사를 마치면 break */
    while(!(sbus.check_idx == sbus.next_data_start_idx))
    {  
      /* 스타트 바이트 검사 */
      if((sbus.new_packet_flag) && (sbus.dma_receive_buff[sbus.check_idx] == START_BYTE))
      {
		sbus.new_packet_flag = 0;
        sbus.packet_start_idx = sbus.check_idx;
        sbus.packet_end_idx = (sbus.packet_start_idx + (SBUS_DATA_SIZE - 1)) % DMA_RECEIVE_SIZE;  // 데이터 바이트만큼 적용
      }
      
      /* 링버퍼로 인한 인덱스 변화 처리 */
      sbus.check_idx = (sbus.check_idx+1) % DMA_RECEIVE_SIZE;
      //next_check_idx = (check_idx+1) % DMA_RECEIVE_SIZE;
      
	  /* END 바이트가 들어왔다면*/
      if(sbus.check_idx == sbus.packet_end_idx && sbus.dma_receive_buff[sbus.check_idx]%16 == END_BYTE)
      {
			/* 패킷의 시작 , 끝 인덱스의 차이에 따른 처리
            온전한 한 패킷만을 담아놓는 버퍼로 복사 */
        if(sbus.packet_start_idx < sbus.packet_end_idx)
        {              
		  memcpy(sbus.packet_buff, (sbus.dma_receive_buff + sbus.packet_start_idx), ((sbus.packet_end_idx - sbus.packet_start_idx) + 1));
        }
        else if(sbus.packet_start_idx > sbus.packet_end_idx)
        {
		  memcpy(sbus.packet_buff, (sbus.dma_receive_buff + sbus.packet_start_idx), (DMA_RECEIVE_SIZE - sbus.packet_start_idx));
          memcpy((sbus.packet_buff + (DMA_RECEIVE_SIZE - sbus.packet_start_idx)), sbus.dma_receive_buff, (sbus.packet_end_idx + 1));
        }
		
		sbus.new_packet_flag = 1;
        sbus.packet_ok_flag = 1;
      }
    }
    
    /* 현재 값을 이전 값으로 세팅 */
    sbus.prev_ndt = sbus.curr_ndt;
    sbus.new_data_start_idx = sbus.next_data_start_idx;
	printf("");
}

void decode_sbus_data()
{
  uint8_t i;
  
  sbus.packet_ok_flag = 0;
  sbus.data_buff[0] = sbus.packet_buff[1] + (uint16_t)((sbus.packet_buff[2]&0x07)<<8);
  sbus.data_buff[1] = (uint16_t)((sbus.packet_buff[2]&0xf8)>>3) + (uint16_t)((sbus.packet_buff[3]&0x3f)<<5);
  sbus.data_buff[2] = (uint16_t)((sbus.packet_buff[3]&0xc0)>>6) + (uint16_t)(sbus.packet_buff[4]<<2) + (uint16_t)((sbus.packet_buff[5]&0x01)<<10);
  sbus.data_buff[3] = (uint16_t)((sbus.packet_buff[5]&0xfe)>>1) + (uint16_t)((sbus.packet_buff[6]&0x0f)<<7);
  sbus.data_buff[4] = (uint16_t)((sbus.packet_buff[6]&0xf0)>>4) + (uint16_t)((sbus.packet_buff[7]&0x7f)<<4);
  sbus.data_buff[5] = (uint16_t)((sbus.packet_buff[7]&0x80)>>7) + (uint16_t)(sbus.packet_buff[8]<<1) + (uint16_t)((sbus.packet_buff[9]&0x03)<<9);
  sbus.data_buff[6] = (uint16_t)((sbus.packet_buff[9]&0xfc)>>2) + (uint16_t)((sbus.packet_buff[10]&0x1f)<<6);

  printf("%d : %.4d %.4d %.4d %.4d %.4d %.4d %.4d\n\n\r", HAL_GetTick(), sbus.data_buff[0], sbus.data_buff[1], sbus.data_buff[2], sbus.data_buff[3], sbus.data_buff[4], sbus.data_buff[5], sbus.data_buff[6]);
  memset(sbus.packet_buff, 0, SBUS_DATA_SIZE);
  //printf("%d %d\n\r", sbus.prev_ndt, sbus.curr_ndt);
}

void sbus_pwm_make_with_value(TIM_HandleTypeDef * htim){
  int i;
  uint16_t pulse[4];
 
  for(i = 0; i < 4; i++){
    pulse[i] = sbus.data_buff[i] / ((sbus_pwm.max_pwm - sbus_pwm.min_pwm) / (sbus_pwm.max_duty - sbus_pwm.min_duty)) + 3696; 
  }
  
  htim -> Instance -> CCR1 = pulse[0];
  htim -> Instance -> CCR2 = pulse[1];
  htim -> Instance -> CCR3 = pulse[2];
  htim -> Instance -> CCR4 = pulse[3];
}
