#ifndef _GPS_H
#define _GPS_H

#include "stm32f4xx_hal.h"
#include "usart.h"
#include "gpio.h"
#include "math.h"

#define GPS_BUFF_SIZE 256
#define NAV_VELNED_SIZE 44
#define NAV_VELNED_LENGTH 36

typedef union _GPS_VELNED_UNION
{
  uint8_t       buff[4];
  float         data;
  int           data_int;
}VelnedUnion; 

typedef struct _GPS_VELNED
{
  uint8_t       gps_data_receive_buff[2];
  uint8_t       gps_data_buff[NAV_VELNED_SIZE];
  uint8_t       gps_rx_buff[GPS_BUFF_SIZE];
  
  uint8_t       CK_A;
  uint8_t       CK_B;

  int           gps_flag;
  int           gps_start_flag;
  int           idx;
  int           count;
  
  int           gps_velned_data_buff[9];
  
  float         vel_N;
  float         vel_E;
  float         vel_D;
  
  float         speed;
  float         gSpeed;         //ground speed
  float         heading;     
  float         sAcc;           //speed accuracy estimate
  float         cAcc;           //course/heading accuracy estimate

}GpsVelned;

extern GpsVelned gps_velned;
extern VelnedUnion velned_union;

void read_gps_packet();

#endif