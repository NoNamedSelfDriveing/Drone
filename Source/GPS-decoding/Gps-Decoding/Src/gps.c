#include "gps.h"

//GpsPosllh gps_posllh;
GpsCommon gps_posllh;
GpsCommon gps_velned;
PosllhUnion posllh_union;
VelnedUnion velned_union;

void gps_init()
{
  gps_posllh.CK_A = 0;
  gps_posllh.CK_B = 0;
  
  gps_posllh.rx_buff_idx = 0;
  gps_posllh.check_byte = -1;
  gps_posllh.read_rx_pause = -1;
  gps_posllh.packet_complete_flag = 0;
  gps_posllh.check_sum_complete_flag = 0;

  gps_velned.CK_A = 0;
  gps_velned.CK_B = 0;

  gps_velned.rx_buff_idx = 0;
  gps_velned.check_byte = -1;
  gps_velned.read_rx_pause = -1;
  gps_velned.packet_complete_flag = 0;
  gps_velned.check_sum_complete_flag = 0;
}

void check_gps_packet(struct GpsCommon gps_protocol)
{
  int i = 0;
  
  if(gps_protocol.rx_buff_idx > GPS_BUFF_SIZE || gps_protocol.rx_buff_idx < 0 )
    gps_protocol.rx_buff_idx = 0;
    
  memcpy(gps_protocol.rx_data_buff + gps_protocol.rx_buff_idx, gps_protocol.data_dma_receive_buff, RECEIVE_BUFF_SIZE);
  gps_protocol.rx_buff_idx += RECEIVE_BUFF_SIZE;
  
  
  if(gps_protocol.read_rx_pause > 0 && gps_protocol.read_rx_pause <= 4)
  {    
    gps_protocol.read_rx_pause--;
    
    memcpy(gps_protocol.rx_data_buff + gps_protocol.buff_idx, gps_protocol.data_dma_receive_buff, RECEIVE_BUFF_SIZE);
    
    gps_protocol.buff_idx += RECEIVE_BUFF_SIZE;
    
    if(gps_protocol.read_rx_pause == 0)
    {
      gps_protocol.read_rx_pause = -1;
      gps_protocol.buff_idx = 0;
      gps_protocol.packet_complete_flag = 1;
    }
  }
  
  if(gps_protocol.read_rx_pause == -1)
  {
    for(i = 0; i < RECEIVE_BUFF_SIZE; i++)
    {     
      if(gps_protocol.data_dma_receive_buff[i] == 0xB5 && gps_protocol.data_dma_receive_buff[i+1] == 0x62)
      {
        if(gps_protocol.data_dma_receive_buff[i+2] == 0x01 && gps_protocol.data_dma_receive_buff[i+3] == 0x02)		
        {
          gps_protocol.read_rx_pause = 4;
          
          memcpy(gps_protocol.rx_data_buff, gps_protocol.data_dma_receive_buff + i, RECEIVE_BUFF_SIZE - i);
          gps_protocol.buff_idx = RECEIVE_BUFF_SIZE - i;       
    
        }
      }
    }
  }  
}

int calculate_gps_check_sum(struct GpsCommon gps_protocol, int NAV_SIZE)
{
  int i = 0;

  gps_protocol.CK_A = 0;
  gps_protocol.CK_B = 0;
  
  for(i = gps_protocol.start_packet_idx + 2; i < gps_protocol.start_packet_idx + NAV_SIZE - 2; i++)
  {
	gps_protocol.CK_A = gps_protocol.CK_A + gps_protocol.rx_data_buff[i];
	gps_protocol.CK_B = gps_protocol.CK_B + gps_protocol.CK_A;
  }
  
  if(gps_protocol.CK_A != gps_protocol.rx_data_buff[NAV_SIZE-2] || gps_protocol.CK_B != gps_protocol.rx_data_buff[NAV_SIZE-1])
  {
	gps_protocol.check_sum_complete_flag = 0;
    return -1;
  }
  else
  {
	gps_protocol.check_sum_complete_flag = 1;
    
    /*
    for(i=0; i<NAV_SIZE; i++)
      printf("%4X ", gps_protocol.posllh_rx_data_buff[i]);
      
    printf("\r\n");
	*/
  }
  
  return 0;
}

void decode_gps_posllh_packet()
{
  int   i       = 0;
  int   j       = 0;
  int   temp    = 0;
  float float_temp = 0.0;
  int   buff_idx = 6;
  
  for(i=0; i<7; i++)
  {
    for(j=0; j<4; j++)
    {
      posllh_union.buff[j] = gps_posllh.gps_data_buff[buff_idx++];
    }
    gps_posllh.packet_rx_data_buff[i] = posllh_union.data_int;
  }
  
  
//  for(i=0; i<7; i++)
//    printf("%4d ", gps_posllh.posllh_data_buff[i]);
// 
//  printf("\r\n");
  
  posllh_data.time = gps_posllh.gps_data_buff[0] / 1000 + ((float)(gps_posllh.packet_rx_data_buff[0] % 1000) / 1000);
  
  /* Calculate Longitude....*/
  posllh_data.longitude_degrees = gps_posllh.packet_rx_data_buff[1] / 10000000;
    
  posllh_data.longitude_minute = gps_posllh.packet_rx_data_buff[1] % 10000000;
  temp = posllh_data.longitude_minute * 60;
  posllh_data.longitude_minute = temp / 10000000;
  
  temp = (temp % 10000000) * 60;
  posllh_data.longitude_second = temp / 10000000; //+ ((float)(temp % 10000000) / 10000000);
  posllh_data.longitude_second += ((float)(temp % 10000000) / 10000000);
  /* Calculate Longitude....*/
  
  /* Calculate Latitude....*/
  posllh_data.latitude_degrees = gps_posllh.packet_rx_data_buff[2] / 10000000;
  
  posllh_data.latitude_minute = gps_posllh.packet_rx_data_buff[2] % 10000000;
  temp = posllh_data.latitude_minute * 60;
  posllh_data.latitude_minute = temp / 10000000;
  
  temp = (temp % 10000000) * 60;
  posllh_data.latitude_second = temp / 10000000; //+ ((float)(temp % 10000000) / 10000000);
  posllh_data.latitude_second += ((float)(temp % 10000000) / 10000000);
  /* Calculate Latitude....*/
  
  posllh_data.ellipsoid = gps_posllh.packet_rx_data_buff[3] / 1000;
  posllh_data.ellipsoid += ((float)(gps_posllh.packet_rx_data_buff[3] % 1000) / 1000);
  
  posllh_data.mean_sealevel = gps_posllh.packet_rx_data_buff[4] / 1000;
  posllh_data.mean_sealevel += ((float)(gps_posllh.packet_rx_data_buff[4] % 1000) / 1000);
            
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
  if(posllh_data.longitude_degrees_gap < 0)
    posllh_data.longitude_degrees_gap *= -1;
  
  posllh_data.longitude_minute_gap = STD_LONGITUDE_MINUTE - posllh_data.longitude_minute;
  if(posllh_data.longitude_minute_gap < 0)
    posllh_data.longitude_minute_gap *= -1;
  
  posllh_data.longitude_second_gap = STD_LONGITUDE_SECOND - posllh_data.longitude_second;
  if(posllh_data.longitude_second_gap < 0.0)
    posllh_data.longitude_second_gap *= -1;
    
  posllh_data.latitude_degrees_gap = STD_LATITUDE_DEGREES - posllh_data.latitude_degrees;
  if(posllh_data.latitude_degrees_gap < 0)
    posllh_data.latitude_degrees_gap *= -1;
  
  posllh_data.latitude_minute_gap = STD_LATITUDE_MINUTE - posllh_data.latitude_minute;
  if(posllh_data.latitude_minute_gap < 0)
    posllh_data.latitude_minute_gap *= -1;
  
  posllh_data.latitude_second_gap = STD_LATITUDE_SECOND - posllh_data.latitude_second;
  if(posllh_data.latitude_second_gap < 0.0)
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
  
  /*
  printf("%f %f %f \r\n", gps_posllh.longitude_distance, gps_posllh.latitude_distance, gps_posllh.distance);
    
  printf("%f %d %d %f ", gps_posllh.time, gps_posllh.longitude_degrees, gps_posllh.longitude_minute, gps_posllh.longitude_second);
  printf("%d %d %f %f %f\r\n", gps_posllh.latitude_degrees, gps_posllh.latitude_minute, gps_posllh.latitude_second ,gps_posllh.ellipsoid, gps_posllh.mean_sealevel);
  printf("\r\n");
*/

}

void decode_gps_velned_packet()
{
	int i = 0;
	int j = 0;
	int k = 0;
	int buff_idx = 6; 

	for (k = 0; k<9; k++)
	{
		for (j = 0; j<4; j++)
		{
			velned_union.buff[j] = gps_velned.gps_data_buff[buff_idx++];
		}
		gps_velned.packet_rx_data_buff[k] = velned_union.data_int;
	}

	/*
	for (j = 0; j<4; j++)
		printf("%4d ", gps_velned.gps_velned_data_buff[j]);

	printf("\r\n");
	*/

	velned_data.vel_N = gps_velned.packet_rx_data_buff[1];
	velned_data.vel_E = gps_velned.packet_rx_data_buff[2];
	velned_data.vel_D = gps_velned.packet_rx_data_buff[3];
	velned_data.speed = gps_velned.packet_rx_data_buff[4];
	velned_data.gSpeed = gps_velned.packet_rx_data_buff[5];
	velned_data.heading = gps_velned.packet_rx_data_buff[6];
	velned_data.sAcc = gps_velned.packet_rx_data_buff[7];
	velned_data.cAcc = gps_velned.packet_rx_data_buff[8];

	//printf("%.2f, %.2f %.2f %.2f", gps_velned.vel_N, gps_velned.vel_E, gps_velned.vel_D, gps_velned.sAcc);
	//printf("\r\n");

}
