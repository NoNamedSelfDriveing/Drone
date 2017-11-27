#include "stm32f4xx_hal.h"
#include "i2c.h"
#include <math.h>

#define GY63_ADDR 0xec
#define D1_4096 0x48
#define D2_4096 0x58
#define ADC_READ 0x00
#define PROM_READ 0xa0
#define D_TIME 9

typedef struct _gy63
{
  float dt;
  float temp;
  float off;
  float sens;
  float p;
  float h_temp;
  float altitude;
  float altitude_meter;
}GY63;

void get_gy63_calibration_data();
void get_altitude();

extern GY63 gy63;