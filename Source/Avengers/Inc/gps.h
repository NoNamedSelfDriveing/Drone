#ifndef _GPS_H
#define _GPS_H

#include "stm32f4xx_hal.h"
#include "usart.h"
#include "gpio.h"
#include "math.h"

#define GPS_BUFF_SIZE 256
#define NAV_POSLLH_SIZE 36
#define NAV_POSLLH_LENGTH 28

#define PI 3.141592653589

#define STD_LONGITUDE_DEGREES 127
#define STD_LONGITUDE_MINUTE 07
#define STD_LONGITUDE_SECOND 43

#define STD_LATITUDE_DEGREES 36
#define STD_LATITUDE_MINUTE 47
#define STD_LATITUDE_SECOND 15

#define LATITUDE_DEGREES_DISTANCE 111
#define LATITUDE_MINUTE_DISTANCE 1.85
#define LATITUDE_SECOND_DISTANCE 30.8


typedef union _GPS_POSLLH_UNION
{
  uint8_t       buff[4];
  float         data;
  int           data_int;
}PosllhUnion; 

typedef struct _GPS_POSLLH
{
  uint8_t       gps_data_receive_buff[2];
  uint8_t       gps_data_buff[NAV_POSLLH_SIZE];
  uint8_t       gps_rx_buff[GPS_BUFF_SIZE];
  
  uint8_t       CK_A;
  uint8_t       CK_B;

  int           gps_flag;
  int           gps_start_flag;
  int           idx;
  int           count;
  
  int           gps_posllh_data_buff[7];
  
  float         time;
  float         ellipsoid;
  float         mean_sealevel;
  
  int           longitude_degrees;
  int           longitude_minute;
  float         longitude_second;
  
  int           latitude_degrees;
  int           latitude_minute;
  float         latitude_second;
  
  float         longitude_degrees_distance;
  float         longitude_minute_distance;
  float         longitude_second_distance;
  
  int           longitude_minute_gap;
  int           longitude_degrees_gap;
  float         longitude_second_gap;
  
  int           latitude_degrees_gap;
  int           latitude_minute_gap;
  float         latitude_second_gap;
  
  float         longitude_distance;
  float         latitude_distance;
  
  float         gps_distance;
  
}GpsPosllh;

extern GpsPosllh gps_posllh;
extern PosllhUnion posllh_union;

void read_gps_packet();

#endif