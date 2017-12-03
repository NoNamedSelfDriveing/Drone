#include "stm32f4xx_hal.h"
#include "stm32f4xx_hal_flash.h"

#define ADDRESS_PITCH_P    0x0800C000
#define ADDRESS_PITCH_I    0x0800C004
#define ADDRESS_PITCH_D    0x0800C008
#define ADDRESS_ROLL_P    0x0800C00C
#define ADDRESS_ROLL_I    0x0800C010
#define ADDRESS_ROLL_D    0x0800C014
#define ADDRESS_YAW_P    0x0800C018
#define ADDRESS_YAW_I    0x0800C01C
#define ADDRESS_YAW_D    0x0800C020
#define ADDRESS_CHECK    0x0800C024

#define PITCH_P     0
#define PITCH_I     1
#define PITCH_D     2
#define ROLL_P      3
#define ROLL_I      4
#define ROLL_D      5
#define YAW_P       6
#define YAW_I       7
#define YAW_D       8

typedef union value{
  int iValue;
  float fValue;
}value;

void init_flash(void);

uint8_t confirm_flash(void);

void flash_startWriting(void);
void flash_startReading(void);

void setGain_PitchP(float);
void setGain_PitchI(float);
void setGain_PitchD(float);
void setGain_RollP(float);
void setGain_RollI(float);
void setGain_RollD(float);
void setGain_YawP(float);
void setGain_YawI(float);
void setGain_YawD(float);

float getGain_PitchP(void);
float getGain_PitchI(void);
float getGain_PitchD(void);
float getGain_RollP(void);
float getGain_RollI(void);
float getGain_RollD(void);
float getGain_YawP(void);
float getGain_YawI(void);
float getGain_YawD(void);

void readGain_All(void);