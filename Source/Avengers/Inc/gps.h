#ifndef _GPS_H
#define _GPS_H

#include "stm32f4xx_hal.h"
#include "usart.h"
#include "gpio.h"
#include "math.h"
#include "string.h"

#define GPS_DMA_RECEIVE_SIZE 1024
#define GPS_PACKET_SIZE 80
#define NAV_POSLLH_SIZE 36
#define NAV_VELNED_SIZE 44

#define PI 3.141592653589

#define STD_LONGITUDE_DEGREES 127
#define STD_LONGITUDE_MINUTE 24
#define STD_LONGITUDE_SECOND 51

#define STD_LATITUDE_DEGREES 36
#define STD_LATITUDE_MINUTE 19
#define STD_LATITUDE_SECOND 10

#define LATITUDE_DEGREES_DISTANCE 111
#define LATITUDE_MINUTE_DISTANCE 1.85
#define LATITUDE_SECOND_DISTANCE 30.8


typedef struct _GPS_COMMON
{
  uint8_t       new_packet_flag;
  uint8_t       packet_complete_flag;
  uint8_t       posllh_checksum_flag;
  uint8_t       velned_checksum_flag;
  uint8_t       check_flag;
  uint8_t       count;
  
  int           received_size;
  int           posllh_loop_counter;
  int           velned_loop_counter;
  
}GpsCommon;

typedef union _GPS_UNION
{
  uint8_t       buff[4];
  float         data;
  int           data_int;
}GpsUnion; 

typedef struct _GPS_POSLLH
{
  int           packet_complete_flag;
  int           check_sum_complete_flag;
  
  int           start_packet_idx;
  int           read_rx_waiting;
  int           posllh_buff_idx;
  int           rx_data_buff_idx;
  int           dma_data_buff_idx;
  
  int           read_packet_count;
  
  int           posllh_data_buff[7];
  
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
  
  float         distance;
  
}GpsPosllh;

typedef struct _GPS_VELNED
{
  int           velned_data_buff[9];
  
  float         vel_N;
  float         vel_E;
  float         vel_D;

  float         speed;
  float         gSpeed;         //ground speed
  float         heading;

  float         sAcc;           //speed accuracy estimate
  float         cAcc;           //course/heading accuracy estimate
}GpsVelned;

extern		GpsCommon gps_state;
extern          GpsPosllh posllh_data;
extern          GpsVelned velned_data;
extern          GpsUnion posllh_union;
extern          GpsUnion velned_union;

extern uint8_t gps_dma_receive_buff[GPS_DMA_RECEIVE_SIZE];
extern uint8_t gps_packet_buff[GPS_PACKET_SIZE];

void            init_gps();
void            read_gps();
void            receive_gps_packet();
void            check_gps_packet();
void            decode_gps_posllh_packet();
void            decode_gps_velned_packet();

#endif