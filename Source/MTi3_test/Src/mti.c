#include <stdlib.h>
#include <string.h>
#include "usart.h"
#include "mti.h"

/*SensData euler_data;
SensData acc_data;
SensData delta_v_data;
SensData gyro_data;
SensData delta_q_data;
SensData mag_field_data;*/

MTIData mti_data;
MTI mti;

uint8_t mti_rx_flag;
uint8_t mti_checksum_flag;
uint16_t mti_msg_len = 99;
uint8_t mti_rx_buff[1024];
uint8_t mti_packet_buff[1024];

/* mti로부터 DMA로 패킷 받아오는 함수 */
void receive_mti_packet()
{
  static int prev_idx = 0, current_idx = 0;
  static int start_idx= 0 , end_idx = 0;
  int i;
  
  HAL_UART_Receive_DMA(&huart3, (uint8_t*)(mti_rx_buff+current_idx), 8);
  while(!(HAL_UART_GetState(&huart3) == HAL_UART_STATE_READY));
  
  prev_idx = current_idx;
  current_idx += 8;
  
  /* 다음 데이터를 받을 인덱스가 버퍼의 마지막 인덱스를 넘어버리면 
      마지막 패킷의 인덱스를 기준으로 다른 버퍼로 패킷 복사 */
  if(current_idx >= SIZE(mti_rx_buff))
  {
    memcpy(mti_packet_buff, (mti_rx_buff+start_idx), (end_idx-start_idx)+1);
    current_idx = 0;
    mti_rx_flag = 1;
  }
  
  for(i = prev_idx; i < current_idx; i++)
  {
    if((mti_rx_buff[i] == 0xfa) && (mti_rx_buff[i+1] == 0xff))
    {
      /* end_idx가 버퍼의 마지막 요소를 넘어서면 */
      if((i + (mti_msg_len-1)) > (SIZE(mti_rx_buff)-1))
      {
        break;
      }
      /* 패킷의 시작 인덱스와 끝 인덱스 저장 */
      else
      {
        start_idx = i;
        end_idx = start_idx + (mti_msg_len - 1);
        break;
      }
    }
  }
}

/* mti 패킷 무결성 체크 함수 */
/* 리턴값 : 무결성 패킷 - 1, Not 무결성 패킷 - 0*/
void check_mti_packet()
{
  int i;
  uint16_t result = 0, checksum = 0;
  
  mti_rx_flag = 0;
  mti_checksum_flag = 0;
  
  for(i = 1; i < mti_msg_len; i++)
  {
    result += mti_packet_buff[i];
  }
  checksum = result & 0x00ff;
  
  /* 패킷의 무결성이 결정되면 flag SET */
  if(checksum == 0x0000)
  {
    //printf("checksum : %4x\r\n", checksum);
    mti_checksum_flag = 1;
  }
}

/* mti 패킷 디코딩 함수 */
/* 인자 : type - 센서 데이터 종류 */
/* mti.h의 SensorType enum 참고 */

void decode_mti()
{
  /* euler angles 디코딩 */
  mti_data.buff[0] = mti_packet_buff[10];
  mti_data.buff[1] = mti_packet_buff[9];
  mti_data.buff[2] = mti_packet_buff[8];
  mti_data.buff[3] = mti_packet_buff[7];
  mti_data.buff[4] = mti_packet_buff[14];
  mti_data.buff[5] = mti_packet_buff[13];
  mti_data.buff[6] = mti_packet_buff[12];
  mti_data.buff[7] = mti_packet_buff[11];
  mti_data.buff[8] = mti_packet_buff[18];
  mti_data.buff[9] = mti_packet_buff[17];
  mti_data.buff[10] = mti_packet_buff[16];
  mti_data.buff[11] = mti_packet_buff[15];
  
  /* acceleration  디코딩 */
  mti_data.buff[12] = mti_packet_buff[25];
  mti_data.buff[13] = mti_packet_buff[24];
  mti_data.buff[14] = mti_packet_buff[23];
  mti_data.buff[15] = mti_packet_buff[22];
  mti_data.buff[16] = mti_packet_buff[29];
  mti_data.buff[17] = mti_packet_buff[28];
  mti_data.buff[18] = mti_packet_buff[27];
  mti_data.buff[19] = mti_packet_buff[26];
  mti_data.buff[20] = mti_packet_buff[33];
  mti_data.buff[21] = mti_packet_buff[32];
  mti_data.buff[22] = mti_packet_buff[31];
  mti_data.buff[23] = mti_packet_buff[30];

  /* delta_v 디코딩 */
  mti_data.buff[24] = mti_packet_buff[40];
  mti_data.buff[25] = mti_packet_buff[39];
  mti_data.buff[26] = mti_packet_buff[38];
  mti_data.buff[27] = mti_packet_buff[37];
  mti_data.buff[28] = mti_packet_buff[44];
  mti_data.buff[29] = mti_packet_buff[43];
  mti_data.buff[30] = mti_packet_buff[42];
  mti_data.buff[31] = mti_packet_buff[41];
  mti_data.buff[32] = mti_packet_buff[48];
  mti_data.buff[33] = mti_packet_buff[47];
  mti_data.buff[34] = mti_packet_buff[46];
  mti_data.buff[35] = mti_packet_buff[45];
  
  /* gyro 디코딩 */ 
  mti_data.buff[36] = mti_packet_buff[55];
  mti_data.buff[37] = mti_packet_buff[54]; 
  mti_data.buff[38] = mti_packet_buff[53]; 
  mti_data.buff[39] = mti_packet_buff[52]; 
  mti_data.buff[40] = mti_packet_buff[59]; 
  mti_data.buff[41] = mti_packet_buff[58]; 
  mti_data.buff[42] = mti_packet_buff[57]; 
  mti_data.buff[43] = mti_packet_buff[56]; 
  mti_data.buff[44] = mti_packet_buff[63]; 
  mti_data.buff[45] = mti_packet_buff[62]; 
  mti_data.buff[46] = mti_packet_buff[61]; 
  mti_data.buff[47] = mti_packet_buff[60]; 
  
  /* delta_q 디코딩 */
  mti_data.buff[48] = mti_packet_buff[70];
  mti_data.buff[49] = mti_packet_buff[69];
  mti_data.buff[50] = mti_packet_buff[68];
  mti_data.buff[51] = mti_packet_buff[67];
  mti_data.buff[52] = mti_packet_buff[74];
  mti_data.buff[53] = mti_packet_buff[73];
  mti_data.buff[54] = mti_packet_buff[72];
  mti_data.buff[55] = mti_packet_buff[71];
  mti_data.buff[56] = mti_packet_buff[78];
  mti_data.buff[57] = mti_packet_buff[77];
  mti_data.buff[58] = mti_packet_buff[76];
  mti_data.buff[59] = mti_packet_buff[75];
  mti_data.buff[60] = mti_packet_buff[82];
  mti_data.buff[61] = mti_packet_buff[81];
  mti_data.buff[62] = mti_packet_buff[80];
  mti_data.buff[63] = mti_packet_buff[79];
  
  /* magnetic_field 디코딩 */
  mti_data.buff[64] = mti_packet_buff[89];
  mti_data.buff[65] = mti_packet_buff[88];
  mti_data.buff[66] = mti_packet_buff[87];
  mti_data.buff[67] = mti_packet_buff[86];
  mti_data.buff[68] = mti_packet_buff[93];
  mti_data.buff[69] = mti_packet_buff[92];
  mti_data.buff[70] = mti_packet_buff[91];
  mti_data.buff[71] = mti_packet_buff[90];
  mti_data.buff[72] = mti_packet_buff[97];
  mti_data.buff[73] = mti_packet_buff[96];
  mti_data.buff[74] = mti_packet_buff[95];
  mti_data.buff[75] = mti_packet_buff[94];
  
  mti.euler[0] = mti_data.value[0];
  mti.euler[1] = mti_data.value[1];
  mti.euler[2] = mti_data.value[2];
  
  mti.acc[0] = mti_data.value[3];
  mti.acc[1] = mti_data.value[4];
  mti.acc[2] = mti_data.value[5];
  
  mti.delta_v[0] = mti_data.value[6];
  mti.delta_v[1] = mti_data.value[7];
  mti.delta_v[2] = mti_data.value[8];
   
  mti.pqr[0] = mti_data.value[9];
  mti.pqr[1] = mti_data.value[10];
  mti.pqr[2] = mti_data.value[11];
  
  mti.delta_q[0] = mti_data.value[12];
  mti.delta_q[1] = mti_data.value[13];
  mti.delta_q[2] = mti_data.value[14];
  mti.delta_q[3] = mti_data.value[15];
  
  mti.mag[0] = mti_data.value[16];
  mti.mag[1] = mti_data.value[17];
  mti.mag[2] = mti_data.value[18];
  
  //printf("%f\r\n%f\r\n%f\r\n", mti.euler[0], mti.euler[1], mti.euler[2]);
  //printf("%f\r\n%f\r\n%f\r\n", mti.acc[0], mti.acc[1], mti.acc[2]);
  //printf("%f\r\n%f\r\n%f\r\n", mti.delta_v[0], mti.delta_v[1], mti.delta_v[2]);
  //printf("%f\r\n%f\r\n%f\r\n", mti.pqr[0], mti.pqr[1], mti.pqr[2]);
  //printf("%f\r\n%f\r\n%f\r\n%f\r\n", mti.delta_q[0], mti.delta_q[1], mti.delta_q[2], mti.delta_q[3]);
  printf("%f\r\n%f\r\n%f\r\n", mti.mag[0], mti.mag[1], mti.mag[2]);
  printf("\r\n");
}