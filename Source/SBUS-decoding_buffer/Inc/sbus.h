/**********************************************************************
 *     sbus.h                                                         *
 *     written by Soomin Lee (MagmaTart)                              *
 *     Last modify date : 2017.11.20                                  *
 *     Description : Structures and Functions to use SBUS protocol.   *
 **********************************************************************/

#ifndef __SBUS_H__
#define __SBUS_H__

#include "stm32f4xx_hal.h"

#define COUNTOF(__BUFFER__) (sizeof(__BUFFER__)/sizeof(*(__BUFFER__)))
#define BUFFERSIZE(buffer) (COUNTOF(buffer))
#define COPY_TO_STACK() memcpy(sbus.uart_rx_stacking_buff+sbus.uart_rx_stacking_idx, sbus.uart_rx_receive_buff, sizeof(uint8_t)*ndtr)
#define SHOULD_CHECK_BYTE_OK() sbus.uart_rx_stacking_buff[sbus.uart_rx_stacking_idx-25] == START_BYTE && sbus.uart_rx_stacking_buff[sbus.uart_rx_stacking_idx-1] % 16 == END_BYTE
#define IS_NOT_NEXT_CIRCLE() (huart1.hdmarx->Instance->NDTR > 24)

#define SBUS_DATA_SIZE 23
#define UART_DATA_SIZE 25
#define DMA_RECEIVE_SIZE 25
#define STACKING_SIZE 250
#define ROW_SIZE 10
#define START_BYTE 0x0f
#define END_BYTE 0x04

typedef struct _SBUS_PWM
{
  float pwm;
  float min_pwm;
  float max_pwm;
  float min_duty;
  float max_duty; 
}SBUS_pwm;

typedef struct _SBUS
{
  uint8_t         uart_rx_receive_buff[DMA_RECEIVE_SIZE];
  uint8_t         uart_rx_stacking_buff[STACKING_SIZE];
  uint8_t         uart_rx_decoding_buff[STACKING_SIZE];
  uint8_t         remained_after_decoding[STACKING_SIZE];
  uint8_t         uart_rx_buff[ROW_SIZE][UART_DATA_SIZE];

  uint16_t        data_buff[ROW_SIZE][18];
  
  uint16_t        uart_rx_stacking_idx;
  uint16_t        uart_rx_row_idx;       
  uint16_t        sb_index_saver;        // start byte index saver to make next decodeable buffer
  
  uint16_t        idx;
  
  volatile        uint8_t rx_flag;
  volatile        uint8_t start_flag;
}SBUS;

void SystemClock_Config(void);

void init_sbus_pwm();

void init_sbus();

// make next decodeable buffer
void make_next_decodeable_buffer();
  
// check sbus data packet (start byte, end byte) when sbus data stacked 25 bytes
void check_sbus_data_packet();

// decode sbus data
void decode_sbus_data();

// make pwm signal based on sbus data
void sbus_pwm_make_with_value(TIM_HandleTypeDef*);

//extern SBUS sbus;
//extern SBUS_pwm sbus_pwm;

#endif