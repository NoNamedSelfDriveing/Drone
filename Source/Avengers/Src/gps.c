#include "gps.h"

GpsPosllh gps_posllh;
PosllhUnion posllh_union;

void read_gps_packet()
{
  int   i = 0;
  int   j = 0;
  int   k = 0;
   
  int   buff_idx = 6; // 6~34
  int   temp = 0;
  float float_temp = 0.0;
    
  for(i=0; i<GPS_BUFF_SIZE; i++)
  {
    if(gps_posllh.gps_rx_buff[i] == 0xB5 && gps_posllh.gps_rx_buff[i+1] == 0x62)
    {
      if(gps_posllh.gps_rx_buff[i+2] == 0x01 && gps_posllh.gps_rx_buff[i+3] == 0x02)
      {
        for(j=0; j<NAV_POSLLH_SIZE ; j++)
        {
          gps_posllh.gps_data_buff[j] = gps_posllh.gps_rx_buff[i++];
        }
        
        gps_posllh.CK_A = 0;
        gps_posllh.CK_B = 0;
        
        for(j=2; j<NAV_POSLLH_SIZE-2; j++)
        {
          gps_posllh.CK_A = gps_posllh.CK_A + gps_posllh.gps_data_buff[j];
          gps_posllh.CK_B = gps_posllh.CK_B + gps_posllh.CK_A;
        }
        
        if(gps_posllh.CK_A != gps_posllh.gps_data_buff[NAV_POSLLH_SIZE-2] || gps_posllh.CK_B != gps_posllh.gps_data_buff[NAV_POSLLH_SIZE-1])
          continue;
        else
/*
          for(j=0; j<NAV_POSLLH_SIZE ; j++)
            printf("%4X", gps_posllh.gps_data_buff[j]);
                 
        printf("%4X %4X", gps_posllh.CK_A, gps_posllh.CK_B);
        printf("\r\n");
  */      
        for(k=0; k<7; k++)
        {
          for(j=0; j<4; j++)
          {
            posllh_union.buff[j] = gps_posllh.gps_data_buff[buff_idx++];
          }
          gps_posllh.gps_posllh_data_buff[k] = posllh_union.data_int;
        }
      
        //gps_posllh.gps_posllh_data_buff[1] = 1270251886;
        //gps_posllh.gps_posllh_data_buff[2] = 374938136;
        
  /*
        for(j=0; j<7; j++)
          printf("%4d ", gps_posllh.gps_posllh_data_buff[j]);
      
        printf("\r\n");
    */  
        gps_posllh.time = gps_posllh.gps_posllh_data_buff[0] / 1000 + ((float)(gps_posllh.gps_posllh_data_buff[0] % 1000) / 1000);
        
        /* Calculate Longitude....*/
        gps_posllh.longitude_degrees = gps_posllh.gps_posllh_data_buff[1] / 10000000;
          
        gps_posllh.longitude_minute = gps_posllh.gps_posllh_data_buff[1] % 10000000;
        temp = gps_posllh.longitude_minute * 60;
        gps_posllh.longitude_minute = temp / 10000000;
        
        temp = (temp % 10000000) * 60;
        gps_posllh.longitude_second = temp / 10000000; //+ ((float)(temp % 10000000) / 10000000);
        gps_posllh.longitude_second += ((float)(temp % 10000000) / 10000000);
        /* Calculate Longitude....*/
        
        /* Calculate Latitude....*/
        gps_posllh.latitude_degrees = gps_posllh.gps_posllh_data_buff[2] / 10000000;
        
        gps_posllh.latitude_minute = gps_posllh.gps_posllh_data_buff[2] % 10000000;
        temp = gps_posllh.latitude_minute * 60;
        gps_posllh.latitude_minute = temp / 10000000;
        
        temp = (temp % 10000000) * 60;
        gps_posllh.latitude_second = temp / 10000000; //+ ((float)(temp % 10000000) / 10000000);
        gps_posllh.latitude_second += ((float)(temp % 10000000) / 10000000);
        /* Calculate Latitude....*/
        
        gps_posllh.ellipsoid = gps_posllh.gps_posllh_data_buff[3] / 1000;
        gps_posllh.ellipsoid += ((float)(gps_posllh.gps_posllh_data_buff[3] % 1000) / 1000);
        
        gps_posllh.mean_sealevel = gps_posllh.gps_posllh_data_buff[4] / 1000;
        gps_posllh.mean_sealevel += ((float)(gps_posllh.gps_posllh_data_buff[4] % 1000) / 1000);
                  
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
       
        gps_posllh.gps_distance = pow(gps_posllh.longitude_distance, 2) + pow(gps_posllh.latitude_distance, 2);
        gps_posllh.gps_distance = sqrt(gps_posllh.gps_distance);
        
        printf("%f %f %f \r\n", gps_posllh.longitude_distance, gps_posllh.latitude_distance, gps_posllh.gps_distance);
          
        printf("%f %d %d %f ", gps_posllh.time, gps_posllh.longitude_degrees, gps_posllh.longitude_minute, gps_posllh.longitude_second);
        printf("%d %d %f %f %f\r\n", gps_posllh.latitude_degrees, gps_posllh.latitude_minute, gps_posllh.latitude_second ,gps_posllh.ellipsoid, gps_posllh.mean_sealevel);
        printf("\r\n");
      }
    }
  }
  
  gps_posllh.gps_flag = 0;
  gps_posllh.count = 1;
  HAL_UART_Receive_IT(&huart6,gps_posllh.gps_data_receive_buff,1);

}