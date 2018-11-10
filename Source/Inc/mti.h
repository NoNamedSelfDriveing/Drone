#ifndef MTI_H
#define MTI_H

#include "stm32f4xx_hal.h"

#define    SIZE(__BUFFER__)    (sizeof(__BUFFER__)/sizeof(*(__BUFFER__)))
#define    MTI_DMA_RX_SIZE     1024
#define    MTI_PACKET_SIZE     99

typedef struct _mti_state
{
  uint8_t     new_packet_flag;
  uint8_t     packet_rx_flag;
  uint8_t     checksum_flag;
  uint8_t     decode_finish_flag;
  uint16_t    count;
}MTIState;

typedef union _mti_data
{
  uint8_t    buff[76];
  float      value[19];
}MTIData;

typedef struct _mti
{
  float     euler[3];
  float     pqr[3];
  float     acc[3];
  float     delta_v[3];
  float     delta_q[4];
  float     mag[3];
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

extern uint8_t mti_checksum_flag;
extern uint8_t mti_dma_rx_buff[MTI_DMA_RX_SIZE];
extern uint8_t mti_packet_buff[MTI_PACKET_SIZE];
extern MTI mti;
extern MTIState mti_state;
extern int rx_size;

void init_mti();
void read_mti();
void receive_mti_packet();
void check_mti_packet();
void decode_mti_packet();

#endif