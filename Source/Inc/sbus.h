/**********************************************************************
 *     sbus.h                                                         *
 *     written by Soomin Lee (MagmaTart)                              *
 *     Last modify date : 2017.11.30                                  *
 *     Description : Structures and Functions to use SBUS protocol.   *
 **********************************************************************/

#ifndef __SBUS_H__
#define __SBUS_H__

#include "stm32f4xx_hal.h"

#define COUNTOF(__BUFFER__) (sizeof(__BUFFER__)/sizeof(*(__BUFFER__)))
#define BUFFERSIZE(buffer) (COUNTOF(buffer))

#define SBUS_PACKET_SIZE      25
#define SBUS_DMA_RECEIVE_SIZE 1024
#define START_BYTE            0x0F
#define END_BYTE              0x04

#define CH1  0
#define CH2  1
#define CH3  2
#define CH4  3
#define CH5  4
#define CH6  5
#define CH7  6
#define CH8  7
#define CH9  8
#define CH10 9
#define CH11 10
#define CH12 11
#define CH13 12
#define CH14 13
#define CH15 14
#define CH16 15

typedef struct _SBUS
{ 
  uint8_t     new_packet_flag;
  uint8_t     packet_ok_flag;
  uint16_t    count;
}SBUS;

// Initialize SBUS struct
void init_sbus();

// READ FUNCTION
void read_sbus();

// make next decodeable buffer in DMA receive buffer
void make_next_decodeable_buffer();
  
// decode sbus data
void decode_sbus_data();

// extern variables
extern SBUS sbus;
extern uint8_t sbus_dma_receive_buff[SBUS_DMA_RECEIVE_SIZE];
extern uint8_t sbus_packet_buff[SBUS_PACKET_SIZE];
extern uint16_t sbus_data_buff[18];
// Buffer of PWM value should be controlled
extern uint16_t sbus_pwm_pulse[6];
#endif
