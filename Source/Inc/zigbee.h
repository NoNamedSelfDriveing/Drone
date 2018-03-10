#ifndef ZIGBEE_H
#define ZIGBEE_H

#include "stm32f4xx_hal.h"
#include <stdlib.h>
#include "usart.h"
#include "user_flash.h"

#define ZIGBEE_DMA_RX_SIZE 1024
#define ZIGBEE_PACKET_SIZE 9

typedef struct _zigbee_state
{
  uint8_t       new_packet_flag;
  uint8_t       packet_rx_flag;
  uint8_t       checksum_flag;
  uint8_t       decode_finish_flag;
  uint16_t      count;
}ZIGBEEState;

typedef union _zigbee_data
{
  uint8_t     buff[4];
  float       value;
}ZIGBEEData;

void init_zigbee();
void read_zigbee();
void receive_zigbee_packet();
void check_zigbee_packet();
void decode_zigbee_packet();

extern uint8_t zigbee_dma_rx_buff[ZIGBEE_DMA_RX_SIZE];

#endif