#ifndef __MS5611_H__
#define __MS5611_H__

#include "stm32f4xx_hal.h"

#define MS5611_ADDR_W 0xEE
#define MS5611_ADDR_R 0xEF

#define CMD_RESET     0x1E
#define CMD_PROM_READ 0xA0
#define CMD_ADC_READ  0x00
#define CMD_ADC_D1    0x48
#define CMD_ADC_D2    0x58

typedef struct _ms5611_state
{
  uint8_t adc_start_flag;
  uint8_t adc_finish_flag;
  uint8_t adc_time_cnt;
  uint8_t adc_type;
}MS5611State;

typedef struct _LPF
{
  uint8_t fc;
  float dt;
  float lambda;
  float x;
  float filtered_x;
  float prev_filtered_x;
}LPF;

extern MS5611State ms5611_state;

void init_ms5611();
void read_ms5611();
void reset_ms5611();
void read_ms5611_prom();
void start_ms5611_adc(uint8_t type);
void read_ms5611_adc(uint8_t type);
void calculate_pressure();
void low_pass_filter();

#endif