#ifndef MTI_H
#define MTI_H

#include "stm32f4xx_hal.h"

#define SIZE(__BUFFER__) (sizeof(__BUFFER__)/sizeof(*(__BUFFER__)))

typedef union _sens_data{
  uint8_t buff[4];
  float value;
}SensData;

/*typedef struct _sensor
{
  SensData *sens_data;
  uint8_t xdi[2];
  uint8_t size;
  uint8_t data_num;
}Sensor;*/

typedef struct _sensor
{
  float euler[3];
  float pqr[3];
  float acc[3];
  float delta_v[3];
  float delta_q[4];
  float mag_field[3];
}Sensor;

typedef enum _SensorType
{
  EULER_ANGLES,
  ACCELERATION, 
  DELTA_V,
  GYRO,
  DELTA_Q,
  MAGNETIC_FIELD
}SensorType;


/*Sensor euler_angles;
Sensor acc;
Sensor delta_v;
Sensor gyro;
Sensor delta_q;
Sensor mag_field;*/

/* extern uint8_t mti_rx_flag;
extern uint16_t mti_msg_len;
extern uint8_t mti_it_buff[1024];
extern uint8_t mti_buff[1024];
extern uint8_t mti_checksum_flag; */

void init_mti();
uint8_t filter_mti();
void decode_mti(SensorType type);

#endif