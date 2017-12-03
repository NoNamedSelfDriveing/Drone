#include "stm32f4xx_hal.h"
#include "i2c.h"
#include <math.h>

#define GY63_ADDR_W 0xEC
#define GY63_ADDR_R 0xED

#define CMD_RESET     0x1E
#define CMD_ADC_READ  0x00
#define CMD_ADC_D1    0x40
#define CMD_ADC_D2    0x50
#define CMD_ADC_256   0x00
#define CMD_ADC_512   0x02
#define CMD_ADC_1024  0x04
#define CMD_ADC_2048  0x06
#define CMD_ADC_4096  0x08
#define CMD_PROM_READ 0xA0

#define GY63_ADDR 0xee
#define D1_4096 0x48
#define D2_4096 0x58
#define ADC_READ 0x00
#define PROM_READ 0xa0
#define D_TIME 10



typedef struct _gy63
{
  uint32_t digital_temp;
  uint32_t digital_p;
  double dt;
  double temp;
  double off;
  double sens;
  double p;
  double h_temp;
  double altitude;
  double altitude_meter;
}GY63;

void init_gy63();
void get_gy63_calibration_data();
void get_altitude();
void send_gy63_reset_cmd();
void read_gy63_adc(uint8_t cmd);
void read_gy63_calibration_data();
void calculate_gy63_altitude();

extern GY63 gy63;