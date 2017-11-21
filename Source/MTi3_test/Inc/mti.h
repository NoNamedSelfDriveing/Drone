#ifndef MTI_H
#define MTI_H

#include "stm32f4xx_hal.h"

#define SIZE(__BUFFER__) (sizeof(__BUFFER__)/sizeof(*(__BUFFER__)))

typedef union _sens_data{
  uint8_t buff[76];
  float value[19];
}MTIData;

typedef struct _sensor
{
  float euler[3];
  float pqr[3];
  float acc[3];
  float delta_v[3];
  float delta_q[4];
  float mag[3];
}MTI;

typedef enum _SensorType
{
  EULER_ANGLES,
  ACCELERATION, 
  DELTA_V,
  GYRO,
  DELTA_Q,
  MAGNETIC_FIELD
}SensorType;

extern uint8_t mti_rx_flag;
extern uint8_t mti_checksum_flag;
extern uint16_t mti_msg_len;
extern uint8_t mti_rx_buff[1024];
extern uint8_t mti_packet_buff[1024];
extern MTI mti;

void receive_mti_packet();
void check_mti_packet();
void decode_mti();

#endif