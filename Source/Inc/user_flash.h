#ifndef USER_FLASH_H
#define USER_FLASH_H

#include "stm32f4xx_hal.h"
#include "stm32f4xx_hal_flash.h"
#include "control.h"

#define ADDR_ROLL_P  0x0800C000
#define ADDR_ROLL_I  0x0800C004
#define ADDR_ROLL_D  0x0800C008
#define ADDR_PITCH_P   0x0800C00C
#define ADDR_PITCH_I   0x0800C010
#define ADDR_PITCH_D   0x0800C014
#define ADDR_YAW_P    0x0800C018
#define ADDR_YAW_I    0x0800C01C
#define ADDR_YAW_D    0x0800C020
#define ADDR_CHECK    0x0800C024

#define ROLL_P     0
#define ROLL_I     1
#define ROLL_D     2
#define PITCH_P    3
#define PITCH_I    4
#define PITCH_D    5
#define YAW_P      6
#define YAW_I      7
#define YAW_D      8

#define ROLL 0
#define PITCH 1
#define YAW 2

#define COUNT_OF_GAIN 9

typedef union _GainData{
  uint32_t int_value;
  float float_value;
}GainData;

void init_flash(void);
void Flash_StartRead(void);
void Flash_StartWrite(void);
uint32_t Flash_ReadData(uint32_t addr);
void Flash_WriteData(uint32_t addr, uint32_t data);
float* get_gain();
void change_gain(uint8_t gain_type, float gain_value);
void save_gain(uint8_t gain_type, float gain_value);

#endif