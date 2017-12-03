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

typedef union value{
  uint32_t iValue;
  float fValue;
}value;

void flash_startWriting(void);
void flash_startReading(void);

uint8_t writeGain_PitchP(float);
uint8_t writeGain_PitchI(float);
uint8_t writeGain_PitchD(float);
uint8_t writeGain_RollP(float);
uint8_t writeGain_RollI(float);
uint8_t writeGain_RollD(float);
uint8_t writeGain_YawP(float);
uint8_t writeGain_YawI(float);
uint8_t writeGain_YawD(float);

void readGain_All(void);