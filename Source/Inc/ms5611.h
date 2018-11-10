#ifndef __MS5611_H__
#define __MS5611_H__

#include "stm32f4xx_hal.h"
#include "lpf.h"

#define MS5611_ADDR_W 0xEC
#define MS5611_ADDR_R 0xED

#define CMD_RESET     0x1E
#define CMD_PROM_READ 0xA0
#define CMD_ADC_READ  0x00
#define CMD_ADC_D1    0x48
#define CMD_ADC_D2    0x58

#define ADC_DELAY_TIME          10    //10ms
#define COUNT_OF_AVG_DATA 101

//#define PI 3.14159265359

typedef struct _ms5611_state
{
  uint8_t adc_start_flag;
  uint8_t adc_finish_flag;
  uint8_t cali_flag;
  uint8_t new_hgt_flag;
  uint8_t adc_time_cnt;
  uint8_t adc_type;
  uint8_t alt_rx_cnt;
}MS5611State;

typedef struct _ms5611
{
  float temp;
  float p;
  float start_alt;
  float prev_alt;
  float now_alt;
  float d_alt;
  float change_alt;
  float hgt;
}MS5611;

void init_ms5611();
void read_ms5611();
void reset_ms5611();
void read_ms5611_prom();
void start_ms5611_adc(uint8_t type);
void read_ms5611_adc(uint8_t type);
void calc_press();
void calc_alt();

extern MS5611State ms5611_state;
extern MS5611 ms5611;
extern LPF alt_lpf;

#endif