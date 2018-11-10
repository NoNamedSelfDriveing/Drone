#include "zigbee.h"
#include "usart.h"
#include "user_flash.h"
#include <stdlib.h>
#include <string.h>

ZIGBEEData zigbee_data;
ZIGBEEState zigbee_state;

uint8_t zigbee_dma_rx_buff[ZIGBEE_DMA_RX_SIZE];
uint8_t zigbee_packet_buff[ZIGBEE_PACKET_SIZE];

/* Initialize Flags and Variables */
void init_zigbee()
{
  zigbee_state.new_packet_flag = 1;
  zigbee_state.packet_rx_flag = 0;
  zigbee_state.checksum_flag = 0;
  zigbee_state.decode_finish_flag = 0;
  zigbee_state.count = 0;
}

/* Whole Sequence Function for Zigbee
  Receive Packet, Check Packet, Decode Packet */
void read_zigbee()
{
  receive_zigbee_packet();
  if(zigbee_state.packet_rx_flag)
  {
    check_zigbee_packet();
    if(zigbee_state.checksum_flag)
    {
      decode_zigbee_packet();
    }
  }
}
  
/* Receive Zigebee Packet by using DMA
  Find just one packet and copy to another buffer for decoding*/
void receive_zigbee_packet()
{
  static int prev_ndt = ZIGBEE_DMA_RX_SIZE, curr_ndt = 0;
  static int new_data_start_idx = 0, next_data_start_idx = 0;
  static int packet_start_idx = 0, packet_end_idx = ZIGBEE_DMA_RX_SIZE;
  int check_idx, next_check_idx;
  uint8_t received_data_size = 0;
  
  curr_ndt = __HAL_DMA_GET_COUNTER(&hdma_uart4_rx);
  
  if(prev_ndt > curr_ndt)
  {
    received_data_size = prev_ndt - curr_ndt;
  }
  else if(prev_ndt < curr_ndt)
  {
    received_data_size = prev_ndt + (ZIGBEE_DMA_RX_SIZE - curr_ndt);
  }
  else
  {
    return;
  }
  
  next_data_start_idx = (new_data_start_idx + received_data_size) % ZIGBEE_DMA_RX_SIZE;

  check_idx = new_data_start_idx - 1;
  next_check_idx = check_idx + 1;
  if(check_idx == -1)
  {
    check_idx = ZIGBEE_DMA_RX_SIZE - 1;
    next_check_idx = 0;
  }
  
  while(!(next_check_idx == next_data_start_idx))
  {
    if((zigbee_dma_rx_buff[check_idx] == 0x53) && (zigbee_dma_rx_buff[next_check_idx] == 0x4D))
    {
      zigbee_state.new_packet_flag = 0;
      packet_start_idx = check_idx;
      packet_end_idx = (packet_start_idx + (ZIGBEE_PACKET_SIZE - 1)) % ZIGBEE_DMA_RX_SIZE;
    }
    
    check_idx = (check_idx + 1) % ZIGBEE_DMA_RX_SIZE;
    next_check_idx = (check_idx + 1) % ZIGBEE_DMA_RX_SIZE;
  
    if(check_idx == packet_end_idx)
    {
      if(packet_start_idx < packet_end_idx)
      {
        memcpy(zigbee_packet_buff, (zigbee_dma_rx_buff + packet_start_idx), ((packet_end_idx - packet_start_idx) + 1));
      }
      else if(packet_start_idx > packet_end_idx)
      {
        memcpy(zigbee_packet_buff, (zigbee_dma_rx_buff + packet_start_idx), (ZIGBEE_DMA_RX_SIZE - packet_start_idx));
        memcpy((zigbee_packet_buff + (ZIGBEE_DMA_RX_SIZE - packet_start_idx)), zigbee_dma_rx_buff, (packet_end_idx + 1));
      }
      zigbee_state.packet_rx_flag = 1;
    }
  }
  
  prev_ndt = curr_ndt;
  new_data_start_idx = next_data_start_idx;
}

/* Check Received Zigbee Packet */
void check_zigbee_packet()
{
  int i;
  uint16_t result = 0, checksum = 0;
  
  zigbee_state.packet_rx_flag = 0;
  zigbee_state.checksum_flag = 0;
  
  for(i = 0; i < ZIGBEE_PACKET_SIZE; i++)
  {
    result += zigbee_packet_buff[i];
  }
  
  checksum = result & 0x00ff;
  
  if(checksum == 0x0000)
  {
    zigbee_state.checksum_flag = 1;
  }
}

/* Decode Received Zigbee Packet */
void decode_zigbee_packet()
{
  uint8_t gain_type;
  
  zigbee_state.decode_finish_flag = 0;
  
  gain_type = zigbee_packet_buff[2];
  zigbee_data.buff[0] = zigbee_packet_buff[3];
  zigbee_data.buff[1] = zigbee_packet_buff[4];
  zigbee_data.buff[2] = zigbee_packet_buff[5];
  zigbee_data.buff[3] = zigbee_packet_buff[6];
  
  printf("changed gain type : ");
  
  switch(gain_type)
  {
    /* Roll P Gain */
    case 0x41 :
      printf("Roll P\n\r");
      change_gain(ROLL_P, zigbee_data.value);
      break;
      
    /* Roll I Gain */
    case 0x42 :
      printf("Roll I\n\r");
      change_gain(ROLL_I, zigbee_data.value);
      break;
      
    /* Roll D Gain */
    case 0x43 : 
      printf("Roll D\n\r");
      change_gain(ROLL_D, zigbee_data.value);
      break;
      
    /* Pitch P Gain */
    case 0x44 : 
      printf("Pitch P\n\r");
      change_gain(PITCH_P, zigbee_data.value);
      break;
    
    /* Pitch I Gain */
    case 0x45 : 
      printf("Pitch I\n\r");
      change_gain(PITCH_I, zigbee_data.value);
      break;
      
    /* Pitch D Gain */
    case 0x46 : 
      printf("Pitch D\n\r");
      change_gain(PITCH_D, zigbee_data.value);
      break;
      
    /* Yaw P Gain */
    case 0x47 : 
      printf("Yaw P\n\r");
      change_gain(YAW_P, zigbee_data.value);
      break;
      
    /* Yaw I Gain */
    case 0x48 : 
      printf("Yaw I\n\r");
      change_gain(YAW_I, zigbee_data.value);
      break;
      
    /* Yaw D Gain */
    case 0x49 : 
      printf("Yaw D\n\r");
      change_gain(YAW_D, zigbee_data.value);
      break;
      
    /* Altitude P Gain */
    case 0x77 :
      printf("Alt P\n\r");
      change_gain(ALT_P, zigbee_data.value);
      break;
    
    /* Altitude I Gain */
    case 0x78 :
      printf("Alt I\n\r");
      change_gain(ALT_I, zigbee_data.value);
      break;
      
    /* Altitude D Gain */
    case 0x79 :
      printf("Alt D\n\r");
      change_gain(ALT_D, zigbee_data.value);
      break;
  }
  
  printf("changed value : %.3f\n\r\n\r", zigbee_data.value);
  zigbee_state.decode_finish_flag = 1;
}
