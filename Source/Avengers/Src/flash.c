#include "flash.h"

value V;
float gains[9];
uint32_t check;
uint32_t addresses[9] = {ADDRESS_PITCH_P, ADDRESS_PITCH_I, ADDRESS_PITCH_D, ADDRESS_ROLL_P, ADDRESS_ROLL_I, ADDRESS_ROLL_D, ADDRESS_YAW_P, ADDRESS_YAW_I, ADDRESS_YAW_D};

void init_flash(void)
{
  int i;
  
  flash_startReading();
  
  check = *(__IO uint32_t*)ADDRESS_CHECK;
  if(check != 1)
  {
    //printf("CHECK %d\n\r", check);
	
    flash_startWriting();
	
    gains[PITCH_P] = 6;
    gains[PITCH_I] = 0;
    gains[PITCH_D] = 1.5;	
    gains[ROLL_P] = 6;
    gains[ROLL_I] = 0;
    gains[ROLL_D] = 1.5;
    gains[YAW_P] = 10;
    gains[YAW_I] = 0;
    gains[YAW_D] = 0;

    setGain_PitchP( gains[PITCH_P] );
    setGain_PitchI( gains[PITCH_I] );
    setGain_PitchD( gains[PITCH_D] );
    setGain_RollP( gains[ROLL_P] );
    setGain_RollI( gains[ROLL_I] );
    setGain_RollD( gains[ROLL_D] );
    setGain_YawP( gains[YAW_P] );
    setGain_YawI( gains[YAW_I] );
    setGain_YawD( gains[YAW_D] );
    
    confirm_flash();
  }
  
  V.iValue = *(__IO uint32_t*)ADDRESS_PITCH_P;
  gains[PITCH_P] = V.fValue;
  V.iValue = *(__IO uint32_t*)ADDRESS_PITCH_I;
  gains[PITCH_I] = V.fValue;
  V.iValue = *(__IO uint32_t*)ADDRESS_PITCH_D;
  gains[PITCH_D] = V.fValue;
  V.iValue = *(__IO uint32_t*)ADDRESS_ROLL_P;
  gains[ROLL_P] = V.fValue;
  V.iValue = *(__IO uint32_t*)ADDRESS_ROLL_I;
  gains[ROLL_I] = V.fValue;
  V.iValue = *(__IO uint32_t*)ADDRESS_ROLL_D;
  gains[ROLL_D] = V.fValue;
  V.iValue = *(__IO uint32_t*)ADDRESS_YAW_P;
  gains[YAW_P] = V.fValue;
  V.iValue = *(__IO uint32_t*)ADDRESS_YAW_I;
  gains[YAW_I] = V.fValue;
  V.iValue = *(__IO uint32_t*)ADDRESS_YAW_D;
  gains[YAW_D] = V.fValue;
}

void flash_startWriting(void)
{
  HAL_FLASH_Unlock();
  __HAL_FLASH_CLEAR_FLAG(
						   FLASH_FLAG_EOP    | FLASH_FLAG_OPERR  |
						   FLASH_FLAG_WRPERR | FLASH_FLAG_PGAERR |
						   FLASH_FLAG_PGPERR | FLASH_FLAG_PGSERR
						 );
}

void flash_startReading(void)
{
  HAL_FLASH_Unlock();
  __HAL_FLASH_CLEAR_FLAG(FLASH_FLAG_EOP | FLASH_FLAG_OPERR | FLASH_FLAG_WRPERR | FLASH_FLAG_PGAERR | FLASH_FLAG_PGPERR | FLASH_FLAG_PGSERR);
}


uint8_t confirm_flash(void)
{
  int i;
 
  flash_startWriting();
  FLASH_Erase_Sector(FLASH_SECTOR_3, VOLTAGE_RANGE_3);
  
  for(i=0;i<9;i++){
	V.fValue = gains[i];
	if(HAL_FLASH_Program(TYPEPROGRAM_WORD, addresses[i], V.iValue) != HAL_OK)
	{
	  HAL_FLASH_Lock();
	  return 0;
	}
  }
  
  if(HAL_FLASH_Program(TYPEPROGRAM_WORD, ADDRESS_CHECK, (uint32_t)1) != HAL_OK)
  {
    HAL_FLASH_Lock();
	return 0;
  }
  
  return 1;
}

void setGain_PitchP(float p)
{ 
  gains[PITCH_P] = p;
}

void setGain_PitchI(float i)
{
  gains[PITCH_I] = i;
}

void setGain_PitchD(float d)
{
  gains[PITCH_D] = d;
}

void setGain_RollP(float p)
{
  gains[ROLL_P] = p;
}

void setGain_RollI(float i)
{
  gains[ROLL_I] = i;
}

void setGain_RollD(float d)
{
  gains[ROLL_D] = d;
}

void setGain_YawP(float p)
{
  gains[YAW_P] = p;
}

void setGain_YawI(float i)
{
  gains[YAW_I] = i;
}

void setGain_YawD(float d)
{
  gains[YAW_D] = d;
}

float getGain_PitchP(void)
{
  return gains[PITCH_P];
}

float getGain_PitchI(void)
{
  return gains[PITCH_I];
}

float getGain_PitchD(void)
{
  return gains[PITCH_D];
}

float getGain_RollP(void)
{
  return gains[ROLL_P];
}

float getGain_RollI(void)
{
  return gains[ROLL_I];
}

float getGain_RollD(void)
{
  return gains[ROLL_D];
}

float getGain_YawP(void)
{
  return gains[YAW_P];
}

float getGain_YawI(void)
{
  return gains[YAW_I];
}

float getGain_YawD(void)
{
  return gains[YAW_D];
}

void readGain_All(void)
{
  HAL_FLASH_Lock();
  
/*
  printf("Pitch P : %.4f\n\r", getGain_PitchP());
  printf("Pitch I : %.2f\n\r", getGain_PitchI());
  printf("Pitch D : %.2f\n\r", getGain_PitchD());
  printf("Roll P : %.2f\n\r", getGain_RollP());
  printf("Roll I : %.2f\n\r", getGain_RollI());
  printf("Roll D : %.2f\n\r", getGain_RollD());
  printf("Yaw P : %.2f\n\r", getGain_YawP());
  printf("Yaw I : %.2f\n\r", getGain_YawI());
  printf("Yaw D : %.2f\n\r", getGain_YawD());
*/
}