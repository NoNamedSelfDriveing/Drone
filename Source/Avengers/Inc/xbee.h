#ifndef _XBEE_H
#define _XBEE_H

#include "usart.h"
#include "gpio.h"
#include "stm32f4xx_hal.h"

#define XBEE_RX_BUFF_SIZE 1024
#define XBEE_TX_BUFF_SIZE 2
#define XBEE_PACKET_SIZE 8

typedef union _XBEE_UNION
{
  uint8_t       buff[4];
  int           int_value;
  float         float_value;
  
}XbeeUnion;

typedef struct _XBEE_STATE
{
  int           new_packet_flag;
  int           read_packet_flag;
  int           check_packet_flag;
  
  
}XbeeState;

typedef struct _XBEE_DATA
{
    
  uint8_t       check_sum;
  uint8_t       data_type;
  
  float         value;
  
}XbeeData;


extern XbeeState        xbee_state; 
extern XbeeData         xbee_data;
extern XbeeUnion        xbee_union;

extern uint8_t          xbee_dma_receive_buff[XBEE_RX_BUFF_SIZE];
extern uint8_t          xbee_packet_buff[XBEE_PACKET_SIZE];

void                    receive_xbee();
void                    read_xbee();
void                    check_xbee_packet();
void                    decode_xbee_packet();
void                    init_xbee();
void                    change_gain_value();

#endif