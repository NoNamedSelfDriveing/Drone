#include "gps.h"

GpsCommon       gps_state;
GpsPosllh       posllh_data;
GpsVelned       velned_data;
GpsUnion        posllh_union;
GpsUnion        velned_union;

uint8_t         gps_dma_receive_buff[GPS_DMA_RECEIVE_SIZE];
uint8_t         gps_packet_buff[GPS_PACKET_SIZE];

uint8_t         posllh_rx_data_buff[NAV_POSLLH_SIZE];
uint8_t         velned_rx_data_buff[NAV_VELNED_SIZE];
  
static int      received_size = 0;

void read_gps()
{
  receive_gps_packet();

  if(gps_state.packet_complete_flag== 1)
  {
    check_gps_packet();
    gps_state.packet_complete_flag = 0;
    
    if(gps_state.posllh_checksum_flag == 1)
    {
      gps_state.posllh_checksum_flag = 0;
      
      decode_gps_posllh_packet();
      
      gps_state.posllh_loop_counter++;
    }
    
    if(gps_state.velned_checksum_flag == 1)
    {
      gps_state.velned_checksum_flag = 0;
      
      decode_gps_velned_packet();
      
      gps_state.velned_loop_counter++;
    }
    
  }
  
}

void init_gps()
{
  gps_state.new_packet_flag = 1;
  gps_state.packet_complete_flag = 0;
  gps_state.check_flag = 0;
  gps_state.count = 0;
  gps_state.posllh_loop_counter = 0;
  gps_state.velned_loop_counter = 0;
        
}



void receive_gps_packet()
{
  static int prev_ndt = GPS_DMA_RECEIVE_SIZE, curr_ndt = 0;
  static int new_data_start_idx = 0, next_data_start_idx = 0;
  static int packet_start_idx = 0, packet_end_idx = GPS_DMA_RECEIVE_SIZE;
  int check_idx, next_check_idx;
  uint8_t received_gps_data_size = 0;
  
  curr_ndt = __HAL_DMA_GET_COUNTER(&hdma_usart6_rx);
        //printf("%4d %4d\n\r", curr_ndt, __HAL_DMA_GET_COUNTER(&hdma_usart6_rx));
        
  if (prev_ndt > curr_ndt)
  {
    received_gps_data_size = prev_ndt - curr_ndt;
  }
  else if (prev_ndt < curr_ndt)
  {
    received_gps_data_size = prev_ndt + (GPS_DMA_RECEIVE_SIZE - curr_ndt);
  }
  else
  {
    return;
  }
  
  received_size += received_gps_data_size;
  next_data_start_idx = (new_data_start_idx + received_gps_data_size) % GPS_DMA_RECEIVE_SIZE;
  
  check_idx = new_data_start_idx - 1;
  next_check_idx = check_idx + 1;
  if (check_idx == -1)
  {
    check_idx = GPS_DMA_RECEIVE_SIZE - 1;
    next_check_idx = 0;
  }
  
  while (!(next_check_idx == next_data_start_idx))
  {
    if ((gps_state.new_packet_flag) && (gps_dma_receive_buff[check_idx] == 0xB5) && (gps_dma_receive_buff[next_check_idx] == 0x62))
    {
      gps_state.new_packet_flag = 0;
      packet_start_idx = check_idx;
      packet_end_idx = (packet_start_idx + (GPS_PACKET_SIZE - 1)) % GPS_DMA_RECEIVE_SIZE;
    }
    
    check_idx = (check_idx + 1) % GPS_DMA_RECEIVE_SIZE;
    next_check_idx = (check_idx + 1) % GPS_DMA_RECEIVE_SIZE;
    
    
    if (check_idx == packet_end_idx)
    {
      if (packet_start_idx < packet_end_idx)
      {
        memcpy(gps_packet_buff, (gps_dma_receive_buff + packet_start_idx), ((packet_end_idx - packet_start_idx) + 1));
      }
      else if (packet_start_idx < packet_end_idx)
      {
        memcpy(gps_packet_buff, (gps_dma_receive_buff + packet_start_idx), (GPS_DMA_RECEIVE_SIZE - packet_start_idx));
        memcpy((gps_packet_buff + (GPS_DMA_RECEIVE_SIZE - packet_start_idx)), gps_dma_receive_buff, (packet_end_idx + 1));
      }
      gps_state.new_packet_flag = 1;
      gps_state.packet_complete_flag = 1;
    }
  }
  prev_ndt = curr_ndt;
  new_data_start_idx = next_data_start_idx;
}


void check_gps_packet()
{
  int i;
  uint8_t CK_A, CK_B;
    
  CK_A = 0;
  CK_B = 0;
  
  for(i = 2; i < NAV_POSLLH_SIZE-2; i++)
  {
    CK_A = CK_A + gps_packet_buff[i];
    CK_B = CK_B + CK_A;
  }
  
  if(CK_A != gps_packet_buff[NAV_POSLLH_SIZE-2] || CK_B != gps_packet_buff[NAV_POSLLH_SIZE-1])
  {
    
    gps_state.posllh_checksum_flag = 0;
  }
  else
  {
    gps_state.posllh_checksum_flag = 1;

#if 0
    for(i=0; i<NAV_POSLLH_SIZE; i++)
      printf("%4X ", gps_packet_buff[i]);
      
    printf("\r\n");
#endif
  }
  
  CK_A = 0;
  CK_B = 0;
  
  for(i = NAV_POSLLH_SIZE + 2; i < GPS_PACKET_SIZE - 2; i++)
  {
    CK_A = CK_A + gps_packet_buff[i];
    CK_B = CK_B + CK_A;
  }
  
  if(CK_A != gps_packet_buff[GPS_PACKET_SIZE - 2] || CK_B != gps_packet_buff[GPS_PACKET_SIZE - 1])
  {
    gps_state.velned_checksum_flag = 0;
  }
  else
  {
    gps_state.velned_checksum_flag = 1;
    
#if 0
    for(i = NAV_POSLLH_SIZE; i<GPS_PACKET_SIZE; i++)
      printf("%4X ", gps_packet_buff[i]);
      
    printf("\r\n");
#endif
  }
}

void decode_gps_posllh_packet()
{
  int   i = 0;
  int   j = 0;
  int   temp = 0;
  float float_temp = 0.0;
  int   buff_idx = 6;
  
  for (i = 0; i<7; i++)
  {
    for (j = 0; j<4; j++)
    {
      posllh_union.buff[j] = gps_packet_buff[buff_idx++];
    }
    posllh_data.posllh_data_buff[i] = posllh_union.data_int;
  }
  
  
  //  for(i=0; i<7; i++)
  //    printf("%4d ", posllh_data.posllh_data_buff[i]);
  // 
  //  printf("\r\n");
  
  posllh_data.time = posllh_data.posllh_data_buff[0] / 1000 + ((float)(posllh_data.posllh_data_buff[0] % 1000) / 1000);
  
  /* Calculate Longitude....*/
  posllh_data.longitude_degrees = posllh_data.posllh_data_buff[1] / 10000000;
  
  posllh_data.longitude_minute = posllh_data.posllh_data_buff[1] % 10000000;
  temp = posllh_data.longitude_minute * 60;
  posllh_data.longitude_minute = temp / 10000000;
  
  temp = (temp % 10000000) * 60;
  posllh_data.longitude_second = temp / 10000000; //+ ((float)(temp % 10000000) / 10000000);
  posllh_data.longitude_second += ((float)(temp % 10000000) / 10000000);
  /* Calculate Longitude....*/
  
  /* Calculate Latitude....*/
  posllh_data.latitude_degrees = posllh_data.posllh_data_buff[2] / 10000000;
  
  posllh_data.latitude_minute = posllh_data.posllh_data_buff[2] % 10000000;
  temp = posllh_data.latitude_minute * 60;
  posllh_data.latitude_minute = temp / 10000000;
  
  temp = (temp % 10000000) * 60;
  posllh_data.latitude_second = temp / 10000000; //+ ((float)(temp % 10000000) / 10000000);
  posllh_data.latitude_second += ((float)(temp % 10000000) / 10000000);
  /* Calculate Latitude....*/
  
  posllh_data.ellipsoid = posllh_data.posllh_data_buff[3] / 1000;
  posllh_data.ellipsoid += ((float)(posllh_data.posllh_data_buff[3] % 1000) / 1000);
  
  posllh_data.mean_sealevel = posllh_data.posllh_data_buff[4] / 1000;
  posllh_data.mean_sealevel += ((float)(posllh_data.posllh_data_buff[4] % 1000) / 1000);
  
  /** Calculate longitude degrees distance **/
  float_temp = posllh_data.latitude_degrees + ((float)(posllh_data.latitude_minute) / 100);
  float_temp = float_temp * (PI / 180);
  posllh_data.longitude_degrees_distance = cos(float_temp);
  posllh_data.longitude_degrees_distance *= PI * 6370 * 2 / 360;
  
  posllh_data.longitude_minute_distance = posllh_data.longitude_degrees_distance / 60;
  posllh_data.longitude_second_distance = posllh_data.longitude_minute_distance / 60;
  
  posllh_data.longitude_second_distance *= 1000;
  /** Calculate longitude degrees distance **/
  
  /** Calculate longitude, latitude gap**/
  posllh_data.longitude_degrees_gap = STD_LONGITUDE_DEGREES - posllh_data.longitude_degrees;
  if (posllh_data.longitude_degrees_gap < 0)
          posllh_data.longitude_degrees_gap *= -1;
  
  posllh_data.longitude_minute_gap = STD_LONGITUDE_MINUTE - posllh_data.longitude_minute;
  if (posllh_data.longitude_minute_gap < 0)
          posllh_data.longitude_minute_gap *= -1;
  
  posllh_data.longitude_second_gap = STD_LONGITUDE_SECOND - posllh_data.longitude_second;
  if (posllh_data.longitude_second_gap < 0.0)
          posllh_data.longitude_second_gap *= -1;
  
  posllh_data.latitude_degrees_gap = STD_LATITUDE_DEGREES - posllh_data.latitude_degrees;
  if (posllh_data.latitude_degrees_gap < 0)
          posllh_data.latitude_degrees_gap *= -1;
  
  posllh_data.latitude_minute_gap = STD_LATITUDE_MINUTE - posllh_data.latitude_minute;
  if (posllh_data.latitude_minute_gap < 0)
          posllh_data.latitude_minute_gap *= -1;
  
  posllh_data.latitude_second_gap = STD_LATITUDE_SECOND - posllh_data.latitude_second;
  if (posllh_data.latitude_second_gap < 0.0)
          posllh_data.latitude_second_gap *= -1;
  
  // printf("%d %d %f %d %d %f \r\n", posllh_data.longitude_degrees_gap, posllh_data.longitude_minute_gap, posllh_data.longitude_second_gap, posllh_data.latitude_degrees_gap, posllh_data.latitude_minute_gap, posllh_data.latitude_second_gap);
  
  /** Calculate longitude, latitude gap**/
  
  /** Calculate longitude distance**/
  posllh_data.longitude_distance = posllh_data.longitude_degrees_gap * posllh_data.longitude_degrees_distance;
  posllh_data.longitude_distance += posllh_data.longitude_minute_gap * posllh_data.longitude_minute_distance;
  
  posllh_data.longitude_distance *= 1000;
  
  posllh_data.longitude_distance += posllh_data.longitude_second_gap * posllh_data.longitude_second_distance;
  
  /** Calculate longitude distance**/
  
  /** Calculate latitude distance**/
  posllh_data.latitude_distance = posllh_data.latitude_degrees_gap * LATITUDE_DEGREES_DISTANCE;
  posllh_data.latitude_distance += posllh_data.latitude_minute_gap * LATITUDE_MINUTE_DISTANCE;
  
  posllh_data.latitude_distance *= 1000;
  
  posllh_data.latitude_distance += posllh_data.latitude_second_gap * LATITUDE_SECOND_DISTANCE;
  
  /** Calculate latitude distance**/
  
  posllh_data.distance = pow(posllh_data.longitude_distance, 2) + pow(posllh_data.latitude_distance, 2);
  posllh_data.distance = sqrt(posllh_data.distance);
  
  
  //printf("%f %f %f \r\n", posllh_data.longitude_distance, posllh_data.latitude_distance, posllh_data.distance);
  
#if 0
  printf("\r\n");
  printf("%f %d %d %f ", posllh_data.time, posllh_data.longitude_degrees, posllh_data.longitude_minute, posllh_data.longitude_second);
  printf("%d %d %f %f %f\r\n", posllh_data.latitude_degrees, posllh_data.latitude_minute, posllh_data.latitude_second, posllh_data.ellipsoid, posllh_data.mean_sealevel);
  printf("%f \r\n", posllh_data.distance);
#endif
  
}


void decode_gps_velned_packet()
{
  int i = 0;
  int j = 0;
  int buff_idx = NAV_POSLLH_SIZE - 1; 

  for (i = 0; i < 9; i++)
  {
    for (j = 0; j < 4; j++)
    {
      velned_union.buff[j] = gps_packet_buff[buff_idx++];
    }
    velned_data.velned_data_buff[i] = velned_union.data_int;
  }

  /*
  for (i = 0; i < 4; i++)
    printf("%4d ", velned_data.velned_data_buff[i]);

  printf("\r\n");
  */

  velned_data.vel_N = velned_data.velned_data_buff[1];
  velned_data.vel_E = velned_data.velned_data_buff[2];
  velned_data.vel_D = velned_data.velned_data_buff[3];
  velned_data.speed = velned_data.velned_data_buff[4];
  velned_data.gSpeed = velned_data.velned_data_buff[5];
  velned_data.heading = velned_data.velned_data_buff[6];
  velned_data.sAcc = velned_data.velned_data_buff[7];
  velned_data.cAcc = velned_data.velned_data_buff[8];

  //printf("%.2f, %.2f %.2f %.2f", gps_velned.vel_N, gps_velned.vel_E, gps_velned.vel_D, gps_velned.sAcc);
  //printf("\r\n");
}
  