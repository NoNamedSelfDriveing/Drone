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
}

void update_buffer(){
  uint16_t subtract_ndt;
  sbus.prev_ndt = sbus.curr_ndt;
  sbus.curr_ndt = __HAL_DMA_GET_COUNTER(&hdma_usart1_rx);
  
  subtract_ndt = sbus.prev_ndt - sbus.curr_ndt;
  
  if(sbus.curr_ndt < sbus.prev_ndt){
    sbus.received_size += subtract_ndt;

    sbus.rear += subtract_ndt;
    sbus.rear = sbus.rear > DMA_RECEIVE_SIZE-1 ? sbus.rear - DMA_RECEIVE_SIZE : sbus.rear;
	
  }
  else if(sbus.curr_ndt > sbus.prev_ndt){
	sbus.received_size += (subtract_ndt + DMA_RECEIVE_SIZE);
  	sbus.rear += (subtract_ndt + DMA_RECEIVE_SIZE);
    sbus.rear = sbus.rear > DMA_RECEIVE_SIZE-1 ? sbus.rear - DMA_RECEIVE_SIZE : sbus.rear;
  }
  printf("");
}

uint8_t make_next_decodeable_buffer()
{
  if(sbus.received_size > SBUS_DATA_SIZE - 1){
    sbus.rear--;

    while(1){
	  if(sbus.front < sbus.rear){
        if(sbus.front+(SBUS_DATA_SIZE-1) > sbus.rear){
          return 0;
        }
        else{
          if(sbus.dma_receive_buff[sbus.front]==START_BYTE && sbus.dma_receive_buff[sbus.front+(SBUS_DATA_SIZE-1)] % 16 == END_BYTE){
            memcpy(sbus.packet_buff, sbus.dma_receive_buff+sbus.front, sizeof(uint8_t)*SBUS_DATA_SIZE);
            sbus.front = sbus.front+SBUS_DATA_SIZE == DMA_RECEIVE_SIZE ? 0 : sbus.front+SBUS_DATA_SIZE;
            sbus.received_size -= SBUS_DATA_SIZE;
			sbus.rear++;
            return 1;
          }else{
            sbus.front = sbus.front+1 > DMA_RECEIVE_SIZE-1 ? 0 : sbus.front+1;
          }
        }
      }
      else if(sbus.front > sbus.rear){
        if( (SBUS_DATA_SIZE-(DMA_RECEIVE_SIZE-sbus.front)) > sbus.rear){
          return 0;
        }
		else{
          if(sbus.dma_receive_buff[sbus.front]==START_BYTE && sbus.dma_receive_buff[(SBUS_DATA_SIZE-1)-(DMA_RECEIVE_SIZE-sbus.front)]%16 == END_BYTE){
			memcpy(sbus.packet_buff, sbus.dma_receive_buff+sbus.front, sizeof(uint8_t)*DMA_RECEIVE_SIZE-sbus.front);
            memcpy(sbus.packet_buff+(DMA_RECEIVE_SIZE-sbus.front), sbus.dma_receive_buff, sizeof(uint8_t)*((SBUS_DATA_SIZE-(DMA_RECEIVE_SIZE-sbus.front))));
            sbus.front = (SBUS_DATA_SIZE-(DMA_RECEIVE_SIZE-sbus.front));
            sbus.received_size -= SBUS_DATA_SIZE;
            return 1;
		  }else{
			sbus.front = sbus.front+1 > DMA_RECEIVE_SIZE-1 ? 0 : sbus.front+1;
		  }
        }
      }
    }

  }else{
    return 0;
  }

}

void decode_sbus_data()
{
  sbus.data_buff[0] = sbus.packet_buff[1] + (uint16_t)((sbus.packet_buff[2]&0x07)<<8);
  sbus.data_buff[1] = (uint16_t)((sbus.packet_buff[2]&0xf8)>>3) + (uint16_t)((sbus.packet_buff[3]&0x3f)<<5);
  sbus.data_buff[2] = (uint16_t)((sbus.packet_buff[3]&0xc0)>>6) + (uint16_t)(sbus.packet_buff[4]<<2) + (uint16_t)((sbus.packet_buff[5]&0x01)<<10);
  sbus.data_buff[3] = (uint16_t)((sbus.packet_buff[5]&0xfe)>>1) + (uint16_t)((sbus.packet_buff[6]&0x0f)<<7);
  sbus.data_buff[4] = (uint16_t)((sbus.packet_buff[6]&0xf0)>>4) + (uint16_t)((sbus.packet_buff[7]&0x7f)<<4);
  sbus.data_buff[5] = (uint16_t)((sbus.packet_buff[7]&0x80)>>7) + (uint16_t)(sbus.packet_buff[8]<<1) + (uint16_t)((sbus.packet_buff[9]&0x03)<<9);
  sbus.data_buff[6] = (uint16_t)((sbus.packet_buff[9]&0xfc)>>2) + (uint16_t)((sbus.packet_buff[10]&0x1f)<<6);

  printf("%.4d %.4d %.4d %.4d %.4d %.4d %.4d\n\r", sbus.data_buff[0], sbus.data_buff[1], sbus.data_buff[2], sbus.data_buff[3], sbus.data_buff[4], sbus.data_buff[5], sbus.data_buff[6]);
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