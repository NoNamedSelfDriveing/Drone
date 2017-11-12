// 다 작업한 후, sbus_struct에 sbus_pwm_struct 통합 고려

// 1차 작업 완료

#include <string.h>

sbus_pwm_struct init_sbus_pwm(TIM_HandleTypeDef * htim, uint32_t channel){
    sbus_pwm_struct sbus_pwm;
    sbus_pwm.min_duty = 4598;
    sbus_pwm.max_duty = 8126;
    sbus_pwm.max_pwm = 1696;
    sbus_pwm.min_pwm = 352;
    sbus_pwm.pwm = 0;

    sbus_pwm.htim = htim;
    sbus_pwm.channel = channel;
}

void init_sbus(sbus_struct * sbus, sbus_pwm_struct sbus_pwm){
    sbus -> sbus_pwm = sbus_pwm;

    sbus -> uart_rx_data_idx = 0;
    sbus -> uart_rx_stacking_idx = 0;
    sbus -> sb_index_saver = 0;
    sbus -> rx_flag = 0;
    sbus -> start_flag = 0;
}

void make_next_decodeable_buffer(sbus_struct * sbus)
{  
  memcpy(sbus -> uart_rx_decoding_buff, sbus -> remained_after_decoding, sizeof(uint8_t) * sbus -> sb_index_saver);
  memcpy(sbus -> uart_rx_decoding_buff + sbus -> sb_index_saver, sbus -> uart_rx_stacking_buff, sizeof(uint8_t) * (STACKING_SIZE - sbus -> sb_index_saver) );
}

// check sbus data packet (start byte, end byte)
// return : index of decodeable row number
void check_sbus_data_packet(sbus_struct * sbus)
{ 
  // int -> uint8_t
  uint8_t sb_index = 0;        // Start byte index
  uint8_t is_index_over = 0;
  uint8_t uart_rx_row_index = 0;
  
  while(sb_index < STACKING_SIZE){

    // Start byte 찾을 때 까지 index를 올림
    if(sbus -> uart_rx_decoding_buff[sb_index] != START_BYTE && sbus -> start_flag == 0)
    {
      sbus -> start_flag = 0;
      sb_index++;
    }

    if(sbus -> uart_rx_decoding_buff[sb_index] == START_BYTE && sbus -> start_flag == 0)
    {
      // 온전한 패킷 찾기
      while( !(sbus -> uart_rx_decoding_buff[sb_index] == START_BYTE && sbus -> uart_rx_decoding_buff[sb_index + (UART_DATA_SIZE-1)] == END_BYTE) )
      {
        sb_index++;
		if(sb_index + (UART_DATA_SIZE-1) > STACKING_SIZE-1){
          is_index_over = 1;
		  break;
		}
      }

      if(is_index_over)
        break;
      
      sbus -> start_flag = 1;
    }
    
    // 스타트 플래그가 올라가 있으면
    if(sbus -> start_flag)
    {
      // copy data
      memcpy((sbus->uart_rx_buff[uart_rx_row_index]), (sbus->uart_rx_decoding_buff+sb_index), sizeof(uint8_t)*UART_DATA_SIZE);

      uart_rx_row_index++;

      // location of next packet's start byte
      sb_index = sb_index + UART_DATA_SIZE;
      sbus -> start_flag = 0;
    }
  }

  memcpy(sbus -> remained_after_decoding, sbus -> uart_rx_decoding_buff + sb_index, sizeof(uint8_t) * (STACKING_SIZE - sb_index)); 
  
  sbus -> uart_rx_row_idx = uart_rx_row_index;
  sbus -> sb_index_saver = (STACKING_SIZE - sb_index);  // 다음 버퍼에서 뒤에 이어붙이기 시작할 인덱스
}

void stack_sbus_data(sbus_struct * sbus){
  // Stack sbus uart rx data 
  sbus -> uart_rx_stacking_buff[ (sbus -> uart_rx_stacking_idx)++ ] = sbus -> uart_rx_receive_buff[0];
  /*
  if( sbus -> uart_rx_stacking_idx == STACKING_SIZE-1){
    check_sbus_data_packet(sbus);
    sbus -> uart_rx_stacking_idx = 0;
  } 
  */

  
}


 // decode sbus data
 // Usage : sbus_data_decoding(&sbus, &sbus_pwm)
void decode_sbus_data(sbus_struct * sbus)
{ 
  int row_index = 0;
  for(row_index = 0;row_index < sbus->uart_rx_row_idx;row_index++)
  {
    sbus -> data_buff[row_index][0] = sbus -> uart_rx_buff[row_index][1] + (uint16_t)((sbus -> uart_rx_buff[row_index][2]&0x07)<<8);
    sbus -> data_buff[row_index][1] = (uint16_t)((sbus -> uart_rx_buff[row_index][2]&0xf8)>>3) + (uint16_t)((sbus -> uart_rx_buff[row_index][3]&0x3f)<<5);
    sbus -> data_buff[row_index][2] = (uint16_t)((sbus -> uart_rx_buff[row_index][3]&0xc0)>>6) + (uint16_t)(sbus -> uart_rx_buff[row_index][4]<<2) + (uint16_t)((sbus -> uart_rx_buff[row_index][5]&0x01)<<10);
    sbus -> data_buff[row_index][3] = (uint16_t)((sbus -> uart_rx_buff[row_index][5]&0xfe)>>1) + (uint16_t)((sbus -> uart_rx_buff[row_index][6]&0x0f)<<7);
    sbus -> data_buff[row_index][4] = (uint16_t)((sbus -> uart_rx_buff[row_index][6]&0xf0)>>4) + (uint16_t)((sbus -> uart_rx_buff[row_index][7]&0x7f)<<4);
    sbus -> data_buff[row_index][5] = (uint16_t)((sbus -> uart_rx_buff[row_index][7]&0x80)>>7) + (uint16_t)(sbus -> uart_rx_buff[row_index][8]<<1) + (uint16_t)((sbus -> uart_rx_buff[row_index][9]&0x03)<<9);
    sbus -> data_buff[row_index][6] = (uint16_t)((sbus -> uart_rx_buff[row_index][9]&0xfc)>>2) + (uint16_t)((sbus -> uart_rx_buff[row_index][10]&0x1f)<<6);
  }
  
  printf("%.4d %.4d %.4d %.4d %.4d %.4d %.4d \r\n",sbus -> data_buff[0],sbus -> data_buff[1], sbus -> data_buff[2], sbus -> data_buff[3], sbus -> data_buff[4], sbus -> data_buff[5], sbus -> data_buff[6]); 
}

// make pwm with sbus.data_buff[0]
// Usage : sbus_pwm_make(&sbus)
void make_sbus_pwm(sbus_struct * sbus){
    uint16_t data_value;
    float pulse;
    uint8_t row_index;
    
    for(row_index = 0;row_index < sbus -> uart_rx_row_idx; row_index++)
    {
      data_value = sbus -> data_buff[row_index][0];
      pulse = data_value / ((sbus -> sbus_pwm.max_pwm - sbus -> sbus_pwm.min_pwm) / (sbus -> sbus_pwm.max_duty - sbus -> sbus_pwm.min_duty)) + 3695;
      sbus -> sbus_pwm.pwm = pulse;
      //__HAL_TIM_SetCompare(sbus -> htim, sbus -> channel, sbus_pwm -> pwm);
     
      sbus -> sbus_pwm.htim -> Instance -> CCR1 = sbus -> sbus_pwm.pwm;
    }
    //printf("%.4d %.4f \r\n", data_value, sbus -> sbus_pwm.pwm);
}

// make pwm only with sbus struct
// Usage : sbus_pwm_make_with_value(&sbus, &sbus_pwm, data_value)
void make_sbus_pwm_with_value(sbus_struct * sbus, uint16_t data_value){
    //pwm = value / ((max_pwm - min_pwm) / (max_duty - min_duty)) + 3695
    float pulse;
    
    pulse = data_value / ((sbus -> sbus_pwm.max_pwm - sbus -> sbus_pwm.min_pwm) / (sbus -> sbus_pwm.max_duty - sbus -> sbus_pwm.min_duty)) + 3695;
    sbus -> sbus_pwm.pwm = pulse;
    //__HAL_TIM_SetCompare(sbus -> htim, sbus_pwm -> channel, sbus_pwm -> pwm);
     
    sbus -> sbus_pwm.htim -> Instance -> CCR1 = sbus -> sbus_pwm.pwm;
    
    printf("%.4d %.4f \r\n", data_value, sbus -> sbus_pwm.pwm);
 }
