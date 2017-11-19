#include <stdlib.h>
#include "mti.h"

SensData euler_data;
SensData acc_data;
SensData delta_v_data;
SensData gyro_data;
SensData delta_q_data;
SensData mag_field_data;

Sensor sensor;

volatile uint8_t mti_rx_flag;
volatile uint8_t mti_checksum_flag;
uint16_t mti_msg_len;
uint8_t mti_it_buff[1024];
uint8_t mti_buff[1024];


void init_mti()
{
  mti_msg_len = 99;
}

/* mti 패킷 무결성 체크 함수 */
/* 리턴값 : 무결성 패킷 - 1, Not 무결성 패킷 - 0*/
uint8_t filter_mti()
{
  uint8_t idx, checksum_flag = 0;
  uint16_t result = 0;
  uint16_t checksum;
  
  for(idx = 1; idx < mti_msg_len; idx++)
  {
    result = result + mti_buff[idx];
  }
  checksum = result & 0x00ff;
  
  if(checksum == 0x0000)
  {
    checksum_flag = 1;
    //printf("%x", mti_buff[idx-2]);
    for(idx = 0; idx < mti_msg_len; idx++)
    {
      printf("%4x", mti_buff[idx]); 
    }
    printf("\r\n");
  }
  
  return checksum_flag;
}

/* mti 패킷 디코딩 함수 */
/* 인자 : type - 센서 데이터 종류 */
/* mti.h의 SensorType enum 참고 */
void decode_mti(SensorType type)
{
  SensData *target_union;
  float *target_value;
  uint8_t target_size;
  uint8_t target_xdi[2];
  uint8_t idx= 0, i, j;
  
  switch(type)
  {
    case EULER_ANGLES :
      target_union = &(euler_data);
      target_value = sensor.euler;
      target_size = SIZE(sensor.euler) * 4;
      target_xdi[0] = 0x20; target_xdi[1] = 0x30;
      break;
      
    case ACCELERATION :
      target_union = &(acc_data);
      target_value = sensor.acc;
      target_size = SIZE(sensor.acc) * 4;
      target_xdi[0] = 0x40; target_xdi[1] = 0x20;
      break;
      
    case DELTA_V :
      target_union = &(delta_v_data);
      target_value = sensor.delta_v;
      target_size = SIZE(sensor.delta_v) * 4;
      target_xdi[0] = 0x40; target_xdi[1] = 0x10;
      break;
      
    case GYRO :
      target_union = &(gyro_data);
      target_value = sensor.pqr;
      target_size = SIZE(sensor.pqr) * 4;
      target_xdi[0] = 0x80; target_xdi[1] = 0x20;
      break;
      
    case DELTA_Q : 
      target_union = &(delta_q_data);
      target_value = sensor.delta_q;
      target_size = SIZE(sensor.delta_q) * 4;
      target_xdi[0] = 0x80; target_xdi[1] = 0x30;
      break;
      
    case MAGNETIC_FIELD:
      target_union = &(mag_field_data);
      target_value = sensor.mag_field;
      target_size = SIZE(sensor.mag_field) * 4;
      target_xdi[0] = 0xc0; target_xdi[1] = 0x20;
      break;
  }
  
  //printf("%x", target_size);
  for(idx = 0; idx < mti_msg_len -2; idx++)
  {
    if((mti_buff[idx] == target_xdi[0]) && (mti_buff[idx+1] == target_xdi[1]) && (mti_buff[idx+2] == target_size))
    {
      idx = idx + 3;
      break;
    }
  }
  
  //printf("%4x", idx);
  /*target_union->buff[10] = mti_buff[idx+1];
  target_union->buff[9] = mti_buff[idx+2];
  target_union->buff[8] = mti_buff[idx+3];
  target_union->buff[7] = mti_buff[idx+4];
  target_union->buff[6] = mti_buff[idx+5];
  target_union->buff[5] = mti_buff[idx+6];
  target_union->buff[4] = mti_buff[idx+7];
  target_union->buff[3] = mti_buff[idx+8];
  target_union->buff[2] = mti_buff[idx+9];
  target_union->buff[1] = mti_buff[idx+10];
  target_union->buff[0] = mti_buff[idx+11];
  
  target_value[0] = target_union->value[0];*/
  
  /*for(i = 0; i < target_size/4; i++)
  {
    for(buff_idx = 3; buff_idx >= 0; buff_idx--)
    {
      target_union->buff[buff_idx] = mti_buff[idx++];
    }
    target_value[i] = target_union->value;
  }
  printf("%x\r\n", target_value[0]);
*/
    
   printf("\r\n");
   for(i = 0; i < target_size/4; i++)
   {
     for(j = 4; j >= 1 ; j--)
     {
       printf("%4x", idx);
       target_union->buff[j-1] = mti_buff[idx++];
       printf("%4x\r\n", target_union->buff[j-1]);
     }
     target_value[i] = target_union->value;
     printf("%f\r\n\r\n", target_value[i]);
   }
  printf("\r\n");
  
 
}