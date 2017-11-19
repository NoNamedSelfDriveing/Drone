#include "gps.h"

GpsPosllh gps_posllh;
PosllhUnion posllh_union;

void gps_posllh_init()
{
  gps_posllh.CK_A = 0;
  gps_posllh.CK_B = 0;
  
  gps_posllh.rx_buff_idx = 0;
  gps_posllh.check_byte = -1;
  gps_posllh.read_rx_pause = 0;
  gps_posllh.packet_complete_flag = 0;
  gps_posllh.check_sum_complete_flag = 0;
  
}

void check_gps_packet()
{
  int idx = 0;
  
  if(gps_posllh.check_byte != -1 && gps_posllh.read_rx_pause > 0 )
  { 
    if(gps_posllh.read_rx_pause == 4)
    {
    }
    memcpy(gps_posllh.posllh_rx_data_buff + gps_posllh.posllh_buff_idx, gps_posllh.data_dma_receive_buff, RECEIVE_BUFF_SIZE);
    gps_posllh.posllh_buff_idx += RECEIVE_BUFF_SIZE;
    gps_posllh.read_rx_pause --;
  }
  else if(gps_posllh.check_byte != -1 && gps_posllh.rx_buff_idx == 0)
    gps_posllh.packet_complete_flag = 1;
  
  if(gps_posllh.check_byte == -1)
  {
    for(i=0; i<RECEIVE_BUFF_SIZE; i++)
    {
      if(gps_posllh.data_dma_receive_buff[i] == 0xB5)
      {
        switch(7-i)
        {
        case 0: // i = 7
            gps_posllh.check_byte = 3;
            gps_posllh.start_packet_idx = gps_posllh.rx_buff_idx + i + 1;
            gps_posllh.read_rx_pause = 4;
            gps_posllh.posllh_buff_idx = 7-i + 1; // 1
            memcpy(gps_posllh.posllh_rx_data_buff, gps_posllh.data_dma_receive_buff + 7, 1);
          
          break;
          
        case 1: // i = 6
          if(gps_posllh.data_dma_receive_buff[i+1] == 0x62) 
          {
            gps_posllh.check_byte = 2;
            gps_posllh.start_packet_idx = gps_posllh.rx_buff_idx + i + 1;
            gps_posllh.read_rx_pause = 4;
            gps_posllh.posllh_buff_idx = 7-i + 1; // 2
            memcpy(gps_posllh.posllh_rx_data_buff, gps_posllh.data_dma_receive_buff + 6, 2);
          }
          
          break;
          
        case 2: // i= 5
          if(gps_posllh.data_dma_receive_buff[i+1] == 0x62 && gps_posllh.data_dma_receive_buff[i+2] == 0x01)
          {
            gps_posllh.check_byte = 1;
            gps_posllh.start_packet_idx = gps_posllh.rx_buff_idx + i + 1;
            gps_posllh.read_rx_pause = 4;
            gps_posllh.posllh_buff_idx = 7-i + 1; // 3
            memcpy(gps_posllh.posllh_rx_data_buff, gps_posllh.data_dma_receive_buff + 5, 3);
          }
                
          break;
          
        case 3: // i = 4
        case 4:
        case 5:
        case 6:
        case 7:
          if(gps_posllh.data_dma_receive_buff[i+1] == 0x62 && gps_posllh.data_dma_receive_buff[i+2] == 0x01 && gps_posllh.data_dma_receive_buff[i+3] == 0x02)
          {
            gps_posllh.check_byte = 0;
            gps_posllh.start_packet_idx = gps_posllh.rx_buff_idx + i + 1;
            gps_posllh.read_rx_pause = 4;
            gps_posllh.posllh_buff_idx = 7-i + 1; // 4
            memcpy(gps_posllh.posllh_rx_data_buff, gps_posllh.data_dma_receive_buff + 4, 4);
          }
          
          break; 
        }
      }
    }
  }
  
  memcpy(gps_posllh.rx_data_buff+gps_posllh.rx_buff_idx, gps_posllh.data_dma_receive_buff, RECEIVE_BUFF_SIZE);
  gps_posllh.rx_buff_idx += RECEIVE_BUFF_SIZE;
}

int calculate_gps_check_sum()
{
  int i=0;
  
#if 0
  printf("-- ");
  
  for(i = 0; i<NAV_POSLLH_SIZE; i++)
    printf("%4X ", gps_posllh.rx_data_buff[gps_posllh.start_packet_idx + i]);

  printf("\r\n\r\n");

  for(i = 0; i<NAV_POSLLH_SIZE; i++)
    gps_posllh.posllh_rx_data_buff[i] = gps_posllh.rx_data_buff[gps_posllh.start_packet_idx + i];
  
  for(i = 0; i<NAV_POSLLH_SIZE; i++)
    printf("%4X ", gps_posllh.posllh_rx_data_buff[i]);

  printf("\r\n\r\n");
#endif
  
  memcpy();
  
  gps_posllh.CK_A = 0;
  gps_posllh.CK_B = 0;
  
  for(i=gps_posllh.start_packet_idx+2; i< gps_posllh.start_packet_idx + NAV_POSLLH_SIZE-2; i++)
  {
    gps_posllh.CK_A = gps_posllh.CK_A + gps_posllh.posllh_rx_data_buff[i];
    gps_posllh.CK_B = gps_posllh.CK_B + gps_posllh.CK_A;
  }
  
  if(gps_posllh.CK_A != gps_posllh.posllh_rx_data_buff[NAV_POSLLH_SIZE-2] || gps_posllh.CK_B != gps_posllh.posllh_rx_data_buff[NAV_POSLLH_SIZE-1])
    return -1;
  else
  {
    
    gps_posllh.check_sum_complete_flag = 1;
    
    for(i=0; i<NAV_POSLLH_SIZE; i++)
      printf("%4X ", gps_posllh.posllh_rx_data_buff[i + gps_posllh.start_packet_idx]);
      
    printf("\r\n");
  }
  
  return 0;
}

void decode_gps_posllh_packet()
{
  int   i       = 0;
  int   j       = 0;
  
  int   temp    = 0;
  int   buff_idx  = 6; // 6~34
  float float_temp = 0.0;

  for(i=0; i<7; i++)
  {
    gps_posllh.posllh_data_buff[i] = posllh_union.data_int;
  }
   
//  for(i=0; i<7; i++)
//    printf("%4d ", gps_posllh.posllh_data_buff[i]);
// 
//  printf("\r\n");
  
  gps_posllh.time = gps_posllh.posllh_data_buff[0] / 1000 + ((float)(gps_posllh.posllh_data_buff[0] % 1000) / 1000);
  
  /* Calculate Longitude....*/
  gps_posllh.longitude_degrees = gps_posllh.posllh_data_buff[1] / 10000000;
    
  gps_posllh.longitude_minute = gps_posllh.posllh_data_buff[1] % 10000000;
  temp = gps_posllh.longitude_minute * 60;
  gps_posllh.longitude_minute = temp / 10000000;
  
  temp = (temp % 10000000) * 60;
  gps_posllh.longitude_second = temp / 10000000; //+ ((float)(temp % 10000000) / 10000000);
  gps_posllh.longitude_second += ((float)(temp % 10000000) / 10000000);
  /* Calculate Longitude....*/
  
  /* Calculate Latitude....*/
  gps_posllh.latitude_degrees = gps_posllh.posllh_data_buff[2] / 10000000;
  
  gps_posllh.latitude_minute = gps_posllh.posllh_data_buff[2] % 10000000;
  temp = gps_posllh.latitude_minute * 60;
  gps_posllh.latitude_minute = temp / 10000000;
  
  temp = (temp % 10000000) * 60;
  gps_posllh.latitude_second = temp / 10000000; //+ ((float)(temp % 10000000) / 10000000);
  gps_posllh.latitude_second += ((float)(temp % 10000000) / 10000000);
  /* Calculate Latitude....*/
  
  gps_posllh.ellipsoid = gps_posllh.posllh_data_buff[3] / 1000;
  gps_posllh.ellipsoid += ((float)(gps_posllh.posllh_data_buff[3] % 1000) / 1000);
  
  gps_posllh.mean_sealevel = gps_posllh.posllh_data_buff[4] / 1000;
  gps_posllh.mean_sealevel += ((float)(gps_posllh.posllh_data_buff[4] % 1000) / 1000);
            
  /** Calculate longitude degrees distance **/
  float_temp = gps_posllh.latitude_degrees + ((float)(gps_posllh.latitude_minute) / 100);
  float_temp = float_temp * (PI / 180);        
  gps_posllh.longitude_degrees_distance = cos(float_temp);        
  gps_posllh.longitude_degrees_distance *= PI * 6370 * 2 / 360;
  
  gps_posllh.longitude_minute_distance = gps_posllh.longitude_degrees_distance / 60;
  gps_posllh.longitude_second_distance = gps_posllh.longitude_minute_distance / 60;
  
  gps_posllh.longitude_second_distance *= 1000;
  /** Calculate longitude degrees distance **/
  
  /** Calculate longitude, latitude gap**/
  gps_posllh.longitude_degrees_gap = STD_LONGITUDE_DEGREES - gps_posllh.longitude_degrees;
  if(gps_posllh.longitude_degrees_gap < 0)
    gps_posllh.longitude_degrees_gap *= -1;
  
  gps_posllh.longitude_minute_gap = STD_LONGITUDE_MINUTE - gps_posllh.longitude_minute;
  if(gps_posllh.longitude_minute_gap < 0)
    gps_posllh.longitude_minute_gap *= -1;
  
  gps_posllh.longitude_second_gap = STD_LONGITUDE_SECOND - gps_posllh.longitude_second;
  if(gps_posllh.longitude_second_gap < 0.0)
    gps_posllh.longitude_second_gap *= -1;
    
  gps_posllh.latitude_degrees_gap = STD_LATITUDE_DEGREES - gps_posllh.latitude_degrees;
  if(gps_posllh.latitude_degrees_gap < 0)
    gps_posllh.latitude_degrees_gap *= -1;
  
  gps_posllh.latitude_minute_gap = STD_LATITUDE_MINUTE - gps_posllh.latitude_minute;
  if(gps_posllh.latitude_minute_gap < 0)
    gps_posllh.latitude_minute_gap *= -1;
  
  gps_posllh.latitude_second_gap = STD_LATITUDE_SECOND - gps_posllh.latitude_second;
  if(gps_posllh.latitude_second_gap < 0.0)
    gps_posllh.latitude_second_gap *= -1;
  
  // printf("%d %d %f %d %d %f \r\n", gps_posllh.longitude_degrees_gap, gps_posllh.longitude_minute_gap, gps_posllh.longitude_second_gap, gps_posllh.latitude_degrees_gap, gps_posllh.latitude_minute_gap, gps_posllh.latitude_second_gap);
  
  /** Calculate longitude, latitude gap**/
  
  /** Calculate longitude distance**/
  gps_posllh.longitude_distance = gps_posllh.longitude_degrees_gap * gps_posllh.longitude_degrees_distance;
  gps_posllh.longitude_distance += gps_posllh.longitude_minute_gap * gps_posllh.longitude_minute_distance;
 
  gps_posllh.longitude_distance *= 1000;
    
  gps_posllh.longitude_distance += gps_posllh.longitude_second_gap * gps_posllh.longitude_second_distance;
  
  /** Calculate longitude distance**/
  
  /** Calculate latitude distance**/
  gps_posllh.latitude_distance = gps_posllh.latitude_degrees_gap * LATITUDE_DEGREES_DISTANCE;
  gps_posllh.latitude_distance += gps_posllh.latitude_minute_gap * LATITUDE_MINUTE_DISTANCE;
  
  gps_posllh.latitude_distance *= 1000;
  
  gps_posllh.latitude_distance += gps_posllh.latitude_second_gap * LATITUDE_SECOND_DISTANCE;
  
  /** Calculate latitude distance**/
  
  gps_posllh.distance = pow(gps_posllh.longitude_distance, 2) + pow(gps_posllh.latitude_distance, 2);
  gps_posllh.distance = sqrt(gps_posllh.distance);
  
//  printf("%f %f %f \r\n", gps_posllh.longitude_distance, gps_posllh.latitude_distance, gps_posllh.distance);
//    
//  printf("%f %d %d %f ", gps_posllh.time, gps_posllh.longitude_degrees, gps_posllh.longitude_minute, gps_posllh.longitude_second);
//  printf("%d %d %f %f %f\r\n", gps_posllh.latitude_degrees, gps_posllh.latitude_minute, gps_posllh.latitude_second ,gps_posllh.ellipsoid, gps_posllh.mean_sealevel);
//  printf("\r\n");

}