/**********************************************************************
 *     sbus.c                                                         *
 *     written by Soomin Lee (MagmaTart)                              *
 *     Last modify date : 2017.11.14                                  *
 *     Description : Implements of Functions to use SBUS protocol.    *
 **********************************************************************/

#include <string.h>
#include <stdio.h>
#include "stm32f4xx_hal.h"
#include "sbus.h"
#include "main.h"
#include "tim.h"
#include "gpio.h"

SBUS sbus;
SBUS_pwm sbus_pwm;

void init_sbus_pwm(){
  sbus_pwm.min_duty = 4598;
  sbus_pwm.max_duty = 8126;
  sbus_pwm.max_pwm = 1696;
  sbus_pwm.min_pwm = 352;
  sbus_pwm.pwm = 0;
}

void init_sbus(){
  //sbus.sbus_pwm = sbus_pwm;

  sbus.uart_rx_stacking_idx = 0;
  sbus.sb_index_saver = 0;
  sbus.rx_flag = 0;
  sbus.start_flag = 0;
}

void make_next_decodeable_buffer()
{  
  sbus.uart_rx_stacking_idx = 0;
  memcpy(sbus.uart_rx_decoding_buff, sbus.remained_after_decoding, sizeof(uint8_t) * sbus.sb_index_saver);
  memcpy(sbus.uart_rx_decoding_buff + sbus.sb_index_saver, sbus.uart_rx_stacking_buff, sizeof(uint8_t) * (STACKING_SIZE - sbus.sb_index_saver) );
}

void check_sbus_data_packet()
{ 
  uint8_t sb_index = 0;        // Start byte index
  uint8_t is_index_over = 0;
  uint8_t uart_rx_row_index = 0;

  while(sb_index < STACKING_SIZE){

  // Increase index until find Start byte
    if(sbus.uart_rx_decoding_buff[sb_index] != START_BYTE && sbus.start_flag == 0)
    {
      sbus.start_flag = 0;
      sb_index++;
      continue;
    }

    if(sbus.uart_rx_decoding_buff[sb_index] == START_BYTE && sbus.start_flag == 0)
    {
      // Search to full packet
      while( !(sbus.uart_rx_decoding_buff[sb_index] == START_BYTE && sbus.uart_rx_decoding_buff[sb_index + (UART_DATA_SIZE-1)]%16 == END_BYTE) )
      {
        sb_index++;
        if(sb_index + (UART_DATA_SIZE-1) > STACKING_SIZE-1){
          is_index_over = 1;
          break;
        }
      }

      if(is_index_over)
        break;
    
      sbus.start_flag = 1;
    }
  
    // If start flag is on
    if(sbus.start_flag)
    {
	  memcpy((sbus.uart_rx_buff[uart_rx_row_index]), (sbus.uart_rx_decoding_buff+sb_index), sizeof(uint8_t)*UART_DATA_SIZE);
  	  uart_rx_row_index++;

     // location of next packet's start byte
      sb_index++;
  	  sbus.start_flag = 0;
    }
  }

  memcpy(sbus.remained_after_decoding, sbus.uart_rx_decoding_buff + sb_index, sizeof(uint8_t) * (STACKING_SIZE - sb_index)); 
  memset(sbus.uart_rx_decoding_buff, 0, sizeof(uint8_t)*STACKING_SIZE);
  
  sbus.uart_rx_row_idx = uart_rx_row_index;
  sbus.sb_index_saver = (STACKING_SIZE - sb_index);
}

void decode_sbus_data()
{ 
  uint8_t row_index = 0;
  for(row_index = 0 ; row_index < sbus.uart_rx_row_idx ; row_index++)
  {
    sbus.data_buff[row_index][0] = sbus.uart_rx_buff[row_index][1] + (uint16_t)((sbus.uart_rx_buff[row_index][2]&0x07)<<8);
    sbus.data_buff[row_index][1] = (uint16_t)((sbus.uart_rx_buff[row_index][2]&0xf8)>>3) + (uint16_t)((sbus.uart_rx_buff[row_index][3]&0x3f)<<5);
    sbus.data_buff[row_index][2] = (uint16_t)((sbus.uart_rx_buff[row_index][3]&0xc0)>>6) + (uint16_t)(sbus.uart_rx_buff[row_index][4]<<2) + (uint16_t)((sbus.uart_rx_buff[row_index][5]&0x01)<<10);
    sbus.data_buff[row_index][3] = (uint16_t)((sbus.uart_rx_buff[row_index][5]&0xfe)>>1) + (uint16_t)((sbus.uart_rx_buff[row_index][6]&0x0f)<<7);
    sbus.data_buff[row_index][4] = (uint16_t)((sbus.uart_rx_buff[row_index][6]&0xf0)>>4) + (uint16_t)((sbus.uart_rx_buff[row_index][7]&0x7f)<<4);
    sbus.data_buff[row_index][5] = (uint16_t)((sbus.uart_rx_buff[row_index][7]&0x80)>>7) + (uint16_t)(sbus.uart_rx_buff[row_index][8]<<1) + (uint16_t)((sbus.uart_rx_buff[row_index][9]&0x03)<<9);
    sbus.data_buff[row_index][6] = (uint16_t)((sbus.uart_rx_buff[row_index][9]&0xfc)>>2) + (uint16_t)((sbus.uart_rx_buff[row_index][10]&0x1f)<<6);
  }
  printf("%.4d %.4d %.4d %.4d %.4d %.4d %.4d\n\r", sbus.data_buff[0][0], sbus.data_buff[0][1], sbus.data_buff[0][2], sbus.data_buff[0][3], sbus.data_buff[0][4], sbus.data_buff[0][5], sbus.data_buff[0][6]);
}

void sbus_pwm_make_with_value(TIM_HandleTypeDef * htim){
  int i;
  uint16_t pulse[4];
 
  for(i = 0; i < 4; i++){
    pulse[i] = sbus.data_buff[0][i] / ((sbus_pwm.max_pwm - sbus_pwm.min_pwm) / (sbus_pwm.max_duty - sbus_pwm.min_duty)) + 3696; 
  }
  
  htim -> Instance -> CCR1 = pulse[0];
  htim -> Instance -> CCR2 = pulse[1];
  htim -> Instance -> CCR3 = pulse[2];
  htim -> Instance -> CCR4 = pulse[3];
}