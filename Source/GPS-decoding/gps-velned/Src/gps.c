#include "gps.h"

GpsVelned gps_velned;
VelnedUnion velned_union;

void read_gps_packet()
{
  int i = 0;
  int j = 0;
  int k = 0;
   
  int buff_idx = 6; // 6 ~ 42
  
  for(i=0; i<GPS_BUFF_SIZE; i++)
  {
    if(gps_velned.gps_rx_buff[i] == 0xB5 && gps_velned.gps_rx_buff[i+1] == 0x62)
    {
      if(gps_velned.gps_rx_buff[i+2] == 0x01 && gps_velned.gps_rx_buff[i+3] == 0x12)
      {
       
        for(j=0; j<NAV_VELNED_SIZE ; j++)
        {
          gps_velned.gps_data_buff[j] = gps_velned.gps_rx_buff[i++];
        }
        
        gps_velned.CK_A = 0;
        gps_velned.CK_B = 0;
        
        for(j=2; j<NAV_VELNED_SIZE-2; j++)
        {
          gps_velned.CK_A = gps_velned.CK_A + gps_velned.gps_data_buff[j];
          gps_velned.CK_B = gps_velned.CK_B + gps_velned.CK_A;
        }
        
        if(gps_velned.CK_A != gps_velned.gps_data_buff[NAV_VELNED_SIZE-2] || gps_velned.CK_B != gps_velned.gps_data_buff[NAV_VELNED_SIZE-1])
          continue;
        else
          for(j=0; j<NAV_VELNED_SIZE ; j++)
            printf("%4X", gps_velned.gps_data_buff[j]);
                 
        printf("%4X %4X", gps_velned.CK_A, gps_velned.CK_B);
        printf("\r\n");
        
        for(k=0; k<9; k++)
        {
          for(j=0; j<4; j++)
          {
            velned_union.buff[j] = gps_velned.gps_data_buff[buff_idx++];
          }
          gps_velned.gps_velned_data_buff[k] = velned_union.data_int;
        }
        
        for(j=0; j<4; j++)
          printf("%4d ", gps_velned.gps_velned_data_buff[j]);
      
        printf("\r\n");
  
        gps_velned.vel_N = gps_velned.gps_velned_data_buff[1];     
        gps_velned.vel_E = gps_velned.gps_velned_data_buff[2];
        gps_velned.vel_D = gps_velned.gps_velned_data_buff[3];
        gps_velned.speed = gps_velned.gps_velned_data_buff[4];
        gps_velned.gSpeed = gps_velned.gps_velned_data_buff[5];
        gps_velned.heading = gps_velned.gps_velned_data_buff[6];
        gps_velned.sAcc = gps_velned.gps_velned_data_buff[7];
        gps_velned.cAcc = gps_velned.gps_velned_data_buff[8];
        
        printf("%.2f, %.2f %.2f %.2f", gps_velned.vel_N, gps_velned.vel_E, gps_velned.vel_D, gps_velned.sAcc); 
        printf("\r\n");
      }
    }
  }
  
  gps_velned.gps_flag = 0;
  gps_velned.count = 1;
  HAL_UART_Receive_IT(&huart6,gps_velned.gps_data_receive_buff,1);

}