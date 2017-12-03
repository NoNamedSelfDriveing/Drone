#include "flash.h"

value V;

void flash_startWriting(void)
{
  HAL_FLASH_Unlock();
  __HAL_FLASH_CLEAR_FLAG(FLASH_FLAG_EOP | FLASH_FLAG_OPERR | FLASH_FLAG_WRPERR | FLASH_FLAG_PGAERR | FLASH_FLAG_PGPERR | FLASH_FLAG_PGSERR);

  FLASH_Erase_Sector(FLASH_SECTOR_3, VOLTAGE_RANGE_3);
}

void flash_startReading(void)
{
  HAL_FLASH_Unlock();
  __HAL_FLASH_CLEAR_FLAG(FLASH_FLAG_EOP | FLASH_FLAG_OPERR | FLASH_FLAG_WRPERR | FLASH_FLAG_PGAERR | FLASH_FLAG_PGPERR | FLASH_FLAG_PGSERR);
}

uint8_t writeGain_PitchP(float p)
{  
  V.fValue = p;
  if (HAL_FLASH_Program(TYPEPROGRAM_WORD, ADDRESS_PITCH_P, V.iValue) != HAL_OK)
  {
      HAL_FLASH_Lock();
      return 0;
  }
  else
  {
	return 1;
  }
}

uint8_t writeGain_PitchI(float i)
{
  V.fValue = i;
  if (HAL_FLASH_Program(TYPEPROGRAM_WORD, ADDRESS_PITCH_I, V.iValue) != HAL_OK)
  {
      HAL_FLASH_Lock();
      return 0;
  }
  else
  {
	return 1;
  }
}

uint8_t writeGain_PitchD(float d)
{
  V.fValue = d;
  if (HAL_FLASH_Program(TYPEPROGRAM_WORD, ADDRESS_PITCH_D, V.iValue) != HAL_OK)
  {
      HAL_FLASH_Lock();
      return 0;
  }
  else
  {
	return 1;
  }
}

uint8_t writeGain_RollP(float p)
{
  V.fValue = p;
  if (HAL_FLASH_Program(TYPEPROGRAM_WORD, ADDRESS_ROLL_P, V.iValue) != HAL_OK)
  {
      HAL_FLASH_Lock();
      return 0;
  }
  else
  {
	return 1;
  }
}

uint8_t writeGain_RollI(float i)
{
  V.fValue = i;
  if (HAL_FLASH_Program(TYPEPROGRAM_WORD, ADDRESS_ROLL_I, V.iValue) != HAL_OK)
  {
      HAL_FLASH_Lock();
      return 0;
  }
  else
  {
	return 1;
  }
}

uint8_t writeGain_RollD(float d)
{
  V.fValue = d;
  if (HAL_FLASH_Program(TYPEPROGRAM_WORD, ADDRESS_ROLL_D, V.iValue) != HAL_OK)
  {
      HAL_FLASH_Lock();
      return 0;
  }
  else
  {
	return 1;
  }
}

uint8_t writeGain_YawP(float p)
{
  V.fValue = p;
  if (HAL_FLASH_Program(TYPEPROGRAM_WORD, ADDRESS_YAW_P, V.iValue) != HAL_OK)
  {
      HAL_FLASH_Lock();
      return 0;
  }
  else
  {
	return 1;
  }
}

uint8_t writeGain_YawI(float i)
{
  V.fValue = i;
  if (HAL_FLASH_Program(TYPEPROGRAM_WORD, ADDRESS_YAW_I, V.iValue) != HAL_OK)
  {
      HAL_FLASH_Lock();
      return 0;
  }
  else
  {
	return 1;
  }
}

uint8_t writeGain_YawD(float d)
{
  V.fValue = d;
  if (HAL_FLASH_Program(TYPEPROGRAM_WORD, ADDRESS_YAW_D, V.iValue) != HAL_OK)
  {
      HAL_FLASH_Lock();
      return 0;
  }
  else
  {
	return 1;
  }
}

void readGain_All(void)
{
  HAL_FLASH_Lock();
  
  V.iValue = *(__IO uint32_t*)ADDRESS_PITCH_P;
  printf("Pitch P : %.2f\n\r", V.fValue);
  
  V.iValue = *(__IO uint32_t*)ADDRESS_PITCH_I;
  printf("Pitch I : %.2f\n\r", V.fValue);
  
  V.iValue = *(__IO uint32_t*)ADDRESS_PITCH_D;
  printf("Pitch D : %.2f\n\r", V.fValue);
  
  V.iValue = *(__IO uint32_t*)ADDRESS_ROLL_P;
  printf("Roll P : %.2f\n\r", V.fValue);
  
  V.iValue = *(__IO uint32_t*)ADDRESS_ROLL_I;
  printf("Roll I : %.2f\n\r", V.fValue);
  
  V.iValue = *(__IO uint32_t*)ADDRESS_ROLL_D;
  printf("Roll D : %.2f\n\r", V.fValue);
  
  V.iValue = *(__IO uint32_t*)ADDRESS_YAW_P;
  printf("Yaw P : %.2f\n\r", V.fValue);
  
  V.iValue = *(__IO uint32_t*)ADDRESS_YAW_I;
  printf("Yaw P : %.2f\n\r", V.fValue);
  
  V.iValue = *(__IO uint32_t*)ADDRESS_YAW_D;
  printf("Yaw P : %.2f\n\r", V.fValue);
}