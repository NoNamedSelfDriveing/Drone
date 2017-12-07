#include "xbee.h"
#include "flash.h"

XbeeState       xbee_state;
XbeeData        xbee_data;
XbeeUnion       xbee_union;

uint8_t         xbee_dma_receive_buff[XBEE_RX_BUFF_SIZE];
uint8_t         xbee_packet_buff[XBEE_PACKET_SIZE];
uint8_t         xbee_check_tx_buff[XBEE_TX_BUFF_SIZE];

void init_xbee()
{
  xbee_state.new_packet_flag = 1;
  xbee_state.read_packet_flag = 0;
  xbee_state.check_packet_flag = 0;
  xbee_data.check_sum = 0;
  xbee_data.data_type = 0;
  xbee_data.value = 0.0;
}

void change_gain_value()
{

  switch(xbee_data.data_type)
  {
  
  case 0x41: // A type - Roll P gain value change
    setGain_RollP(xbee_data.value);
    //printf("Xbee type : %4c, Xbee value : %4f, flash value : %4f \r\n", xbee_data.data_type, xbee_data.value, getGain_RollP());
    
    break;
    
  case 0x42: // B type - Roll I gain value change
    setGain_RollI(xbee_data.value);
    //printf("Xbee type : %4c, Xbee value : %4f, flash value : %4f \r\n", xbee_data.data_type, xbee_data.value, getGain_RollI());
    
    break;
    
  case 0x43: // C type - Roll D gain value change
    setGain_RollD(xbee_data.value);
    //printf("Xbee type : %4c, Xbee value : %4f, flash value : %4f \r\n", xbee_data.data_type, xbee_data.value, getGain_RollD());
    
    break;
    
  case 0x44: // D type - Pitch P gain value change
    setGain_PitchP(xbee_data.value);
    //printf("Xbee type : %4c, Xbee value : %4f, flash value : %4f \r\n", xbee_data.data_type, xbee_data.value, getGain_PitchP());
    
    break;
    
  case 0x45: // E type - Pitch I gain value change
    setGain_PitchI(xbee_data.value);
    //printf("Xbee type : %4c, Xbee value : %4f, flash value : %4f \r\n", xbee_data.data_type, xbee_data.value, getGain_PitchI());
    
    break;
    
  case 0x46: // F type - Pitch D gain value change
    setGain_PitchD(xbee_data.value);
    //printf("Xbee type : %4c, Xbee value : %4f, flash value : %4f \r\n", xbee_data.data_type, xbee_data.value, getGain_PitchD());
    
    break;
    
  case 0x47: // G type - Yaw P gain value change
    setGain_YawP(xbee_data.value);
    //printf("Xbee type : %4c, Xbee value : %4f, flash value : %4f \r\n", xbee_data.data_type, xbee_data.value, getGain_YawP());
    
    break;
    
  case 0x48: // H type - Yaw I gain value change
    setGain_YawI(xbee_data.value);
    //printf("Xbee type : %4c, Xbee value : %4f, flash value : %4f \r\n", xbee_data.data_type, xbee_data.value, getGain_YawI());
    
    break;
    
  case 0x49: // I type - Yaw D gain value change
    setGain_YawD(xbee_data.value);
    //printf("Xbee type : %4c, Xbee value : %4f, flash value : %4f \r\n", xbee_data.data_type, xbee_data.value, getGain_YawD());
    
    break;
    
  default:
    
    printf("Xbee Type value Error!!! / %4X %4f \r\n", xbee_data.data_type, xbee_data.value);
    return ;
    
    break;
  }
  
  printf("Xbee type : %4X %4c, Xbee value : %4f \r\n", xbee_data.data_type, xbee_data.data_type, xbee_data.value);
  confirm_flash();
  readGain_All();
}

void decode_xbee_packet()
{
  int i, idx = 0;
  int temp = 0;
  
  xbee_data.data_type = xbee_packet_buff[2];
  xbee_data.value = 0.0;
  
  for(i=3; i<XBEE_PACKET_SIZE-1; i++)
  {
    if(xbee_packet_buff[i] == 0x2E)
    {
      idx = i;
    }
  }
  
  switch(idx)
  {
  case 0:
    
      xbee_data.value = (int)(xbee_packet_buff[3]-0x30) * 1000 + (int)(xbee_packet_buff[4]-0x30) * 100 + (int)(xbee_packet_buff[5]-0x30) * 10 + (int)(xbee_packet_buff[6]-0x30);
    
    break;
    
  case 3:
    
    temp = (int)((xbee_packet_buff[4]-0x30) * 100) + (int)((xbee_packet_buff[5]-0x30) * 10) + (int)((xbee_packet_buff[6] - 0x30));
    xbee_data.value = (float)temp/1000;
    
    break;
    
  case 4:
    xbee_data.value = xbee_packet_buff[3]-0x30;
    
    temp = (int)((xbee_packet_buff[5] - 0x30) * 10) + (int)((xbee_packet_buff[6] - 0x30));
    
    xbee_data.value += (float)temp/100;

    break;
    
  case 5:
    
    xbee_data.value = (int)(xbee_packet_buff[3]-0x30) * 100 + (int)(xbee_packet_buff[4]-0x30) * 10 + (int)(xbee_packet_buff[6] - 0x30);
    
    //temp = (int)((xbee_packet_buff[6] - 0x30));
    
    //xbee_data.value += (float)temp/10;
    xbee_data.value /= 10;
    
    break;
   
  }
  

  

  printf("%4d, %4f \r\n", temp, xbee_data.value);
}

void read_xbee()
{
  int i = 0;
  
  receive_xbee();
  
  if(xbee_state.read_packet_flag == 1)
  {
    xbee_state.read_packet_flag = 0;
    
    for(i=0; i<XBEE_PACKET_SIZE; i++)
    {
      printf("%4X ", xbee_packet_buff[i]);
    }
    printf("\r\n");
    
    decode_xbee_packet();
    change_gain_value();
  }
  
}


void receive_xbee()
{
  static int prev_ndt = GPS_DMA_RECEIVE_SIZE, curr_ndt = 0;
  static int new_data_start_idx = 0, next_data_start_idx = 0;
  static int packet_start_idx = 0, packet_end_idx = XBEE_RX_BUFF_SIZE;
  static int received_size = 0;
  int check_idx, next_check_idx;
  uint8_t received_xbee_data_size = 0;
  
  curr_ndt = __HAL_DMA_GET_COUNTER(&hdma_uart4_rx);
  //printf("%4d %4d\n\r", curr_ndt, __HAL_DMA_GET_COUNTER(&hdma_usart6_rx));
        
  if (prev_ndt > curr_ndt)
  {
    received_xbee_data_size = prev_ndt - curr_ndt;
  }
  else if (prev_ndt < curr_ndt)
  {
    received_xbee_data_size = prev_ndt + (XBEE_RX_BUFF_SIZE - curr_ndt);
  }
  else
  {
    return;
  }
  
  received_size += received_xbee_data_size;
  next_data_start_idx = (new_data_start_idx + received_xbee_data_size) % XBEE_RX_BUFF_SIZE;
  
  check_idx = new_data_start_idx - 1;
  next_check_idx = check_idx + 1;
  if (check_idx == -1)
  {
    check_idx = XBEE_RX_BUFF_SIZE - 1;
    next_check_idx = 0;
  }
  
  while (!(next_check_idx == next_data_start_idx))
  {
    if ((gps_state.new_packet_flag) && (xbee_dma_receive_buff[check_idx] == 0x53) && (xbee_dma_receive_buff[next_check_idx] == 0x4D))
      
    {
      xbee_state.new_packet_flag = 0;
      packet_start_idx = check_idx;
      packet_end_idx = (packet_start_idx + (XBEE_PACKET_SIZE - 1)) % XBEE_RX_BUFF_SIZE;
    }
    
    check_idx = (check_idx + 1) % XBEE_RX_BUFF_SIZE;
    next_check_idx = (check_idx + 1) % XBEE_RX_BUFF_SIZE;
    
    
    if (check_idx == packet_end_idx && xbee_dma_receive_buff[packet_end_idx] == 0x50)
    {
      if (packet_start_idx < packet_end_idx)
      {
        memcpy(xbee_packet_buff, (xbee_dma_receive_buff + packet_start_idx), ((packet_end_idx - packet_start_idx) + 1));
      }
      else if (packet_start_idx < packet_end_idx)
      {
        memcpy(xbee_packet_buff, (xbee_dma_receive_buff + packet_start_idx), (XBEE_RX_BUFF_SIZE - packet_start_idx));
        memcpy((xbee_packet_buff + (XBEE_RX_BUFF_SIZE - packet_start_idx)), xbee_dma_receive_buff, (packet_end_idx + 1));
      }
      xbee_state.new_packet_flag = 1;
      xbee_state.read_packet_flag = 1;
      
    }
  }
  prev_ndt = curr_ndt;
  new_data_start_idx = next_data_start_idx;
  
}