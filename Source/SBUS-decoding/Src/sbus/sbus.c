// 다 작업한 후, sbus_struct에 sbus_pwm_struct 통합 고려
// 함수 이름 수정 : 동사 - 부사 순

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
    sbus -> rx_flag = 0;
    sbus -> start_flag = 0;
}

// check sbus data packet (start byte, end byte)
 // Usage : sbus_data_filtering(&sbus, &sbus_pwm)
 void check_sbus_data_packet(sbus_struct * sbus)
 { 
   int inner_index = 0;     // 255까지 순환할 index
   // 인덱스가 0이면서 스태킹 버퍼 인덱스에 들어있는 바이트가 START_BYTE가 아니면
   // 스타트 플래그를 내림
   while(inner_index < STACKING_SIZE){
     if(sbus -> uart_rx_data_idx == 0 && sbus -> uart_rx_stacking_buff[inner_index] != START_BYTE)
       sbus -> start_flag = 0;
   
     // 인덱스가 0이면서 스태킹 버퍼 인덱스에 들어있는 바이트가 START_BYTE가 아니면
     // 스타트 플래그를 내림
     if(sbus -> uart_rx_data_idx == 0 && sbus -> uart_rx_stacking_buff[inner_index] == START_BYTE && sbus -> start_flag == 0)
       sbus -> start_flag = 1;
   
     // 스타트 플래그가 올라가 있으면
     if(sbus -> start_flag)
     {
       sbus -> uart_rx_buff[sbus -> uart_rx_data_idx++] = sbus -> uart_rx_stacking_buff[inner_index++];
   
       if(inner_index % UART_DATA_SIZE-1 == 0)  // 25바이트 적재 완료시
       {
         sbus -> uart_rx_data_idx = 0;  // 적재 인덱스 초기화
         sbus -> start_flag = 0;        // 스타트 플래그 내림
       
         if((sbus -> uart_rx_buff[24]&0x0f) == 0x04)    // End byte 검사
           sbus_data_decoding(sbus);                    // 데이터 디코딩
       }
     }
   }
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
   sbus -> data_buff[0] = sbus -> uart_rx_buff[1] + (uint16_t)((sbus -> uart_rx_buff[2]&0x07)<<8);
   sbus -> data_buff[1] = (uint16_t)((sbus -> uart_rx_buff[2]&0xf8)>>3) + (uint16_t)((sbus -> uart_rx_buff[3]&0x3f)<<5);
   sbus -> data_buff[2] = (uint16_t)((sbus -> uart_rx_buff[3]&0xc0)>>6) + (uint16_t)(sbus -> uart_rx_buff[4]<<2) + (uint16_t)((sbus -> uart_rx_buff[5]&0x01)<<10);
   sbus -> data_buff[3] = (uint16_t)((sbus -> uart_rx_buff[5]&0xfe)>>1) + (uint16_t)((sbus -> uart_rx_buff[6]&0x0f)<<7);
   sbus -> data_buff[4] = (uint16_t)((sbus -> uart_rx_buff[6]&0xf0)>>4) + (uint16_t)((sbus -> uart_rx_buff[7]&0x7f)<<4);
   sbus -> data_buff[5] = (uint16_t)((sbus -> uart_rx_buff[7]&0x80)>>7) + (uint16_t)(sbus -> uart_rx_buff[8]<<1) + (uint16_t)((sbus -> uart_rx_buff[9]&0x03)<<9);
   sbus -> data_buff[6] = (uint16_t)((sbus -> uart_rx_buff[9]&0xfc)>>2) + (uint16_t)((sbus -> uart_rx_buff[10]&0x1f)<<6);
    
   sbus_pwm_make_with_value(sbus, sbus -> data_buff[0]);
   
   printf("%.4d %.4d %.4d %.4d %.4d %.4d %.4d \r\n",sbus -> data_buff[0],sbus -> data_buff[1], sbus -> data_buff[2], sbus -> data_buff[3], sbus -> data_buff[4], sbus -> data_buff[5], sbus -> data_buff[6]);
 }

// make pwm with sbus.data_buff[0]
// Usage : sbus_pwm_make(&sbus)
// sbus_pwm이랑 합치니까 코드 보기 되게힘드네...
void make_sbus_pwm(sbus_struct * sbus){
    uint16_t data_value = sbus -> data_buff[0];
    float pulse;

    pulse = data_value / ((sbus -> sbus_pwm.max_pwm - sbus -> sbus_pwm.min_pwm) / (sbus -> sbus_pwm.max_duty - sbus -> sbus_pwm.min_duty)) + 3695;
    sbus -> sbus_pwm.pwm = pulse;
    //__HAL_TIM_SetCompare(sbus -> htim, sbus -> channel, sbus_pwm -> pwm);
     
    sbus -> sbus_pwm.htim -> Instance -> CCR1 = sbus -> sbus_pwm.pwm;
    
    printf("%.4d %.4f \r\n", data_value, sbus -> sbus_pwm.pwm);
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
