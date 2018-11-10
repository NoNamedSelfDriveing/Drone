#include "user_flash.h"
#include "stm32f4xx_hal_flash.h"
#include "control.h"

Gain gain;
float now_gain[COUNT_OF_GAIN];
uint32_t gain_addr[COUNT_OF_GAIN] = {ADDR_ROLL_P, ADDR_ROLL_I, ADDR_ROLL_D, ADDR_PITCH_P, \
                         ADDR_PITCH_I, ADDR_PITCH_D, ADDR_YAW_P, ADDR_YAW_I, ADDR_YAW_D, \
                         ADDR_ALT_P, ADDR_ALT_I, ADDR_ALT_D};

/* get last gain values from flash
   if gain values were not saved, set gain value */
void init_flash(void)
{
  uint32_t check = 100; 
  int axis, type;
  
  Flash_StartRead();
  check = Flash_ReadData(ADDR_CHECK);
  HAL_FLASH_Lock();
  
  /* if gain values were not saved(flash have not been used) */
  if(check != 1)
  {
    printf("Flash has not been ever used\n\r");
    Flash_StartWrite();
    
    FLASH_Erase_Sector(FLASH_SECTOR_3, VOLTAGE_RANGE_3);
    Flash_WriteData(ADDR_CHECK, (uint32_t)1);
       
    atti_pid.gain_roll[P] = 6.0f;
    atti_pid.gain_roll[I] = 0.0f;
    atti_pid.gain_roll[D] = 1.5f;
    
    atti_pid.gain_pitch[P] = 6.0f;
    atti_pid.gain_pitch[I] = 0.0f;
    atti_pid.gain_pitch[D] = 1.5f;
    
    atti_pid.gain_yaw[P] = 2.0f;
    atti_pid.gain_yaw[I] = 0.0f;
    atti_pid.gain_yaw[D] = 4.0f;
      
    alt_pid.gain[P] = 30.0f;
    alt_pid.gain[I] = 15.0f;
    alt_pid.gain[D] = 0.4f;
    
    /* write initial gain */
    for(axis = 0; axis <= 3; axis++)
    {
      for(type = 0; type <= 2; type++)
      {
        if(axis == ROLL)
        {
          gain.float_value = atti_pid.gain_roll[type];
          Flash_WriteData(gain_addr[type], gain.int_value);
        }
        else if(axis == PITCH)
        {
          gain.float_value = atti_pid.gain_pitch[type];
          Flash_WriteData(gain_addr[type+3], gain.int_value);
        }
        else if(axis == YAW)
        {
          gain.float_value = atti_pid.gain_yaw[type];
          Flash_WriteData(gain_addr[type+6], gain.int_value);
        }
        else if(axis == ALT)
        {
          gain.float_value = alt_pid.gain[type];
          Flash_WriteData(gain_addr[type+9], gain.int_value);
        }
      }
    }
    HAL_FLASH_Lock();
  }
  
  /* load last gain values from flash */
  else
  {    
    printf("Flash was used\n\r");
    
    //saved_gain = get_gain();
    get_now_gain();
    
    /*
    atti_pid.gain_roll[P] = saved_gain[ROLL_P];
    atti_pid.gain_roll[I] = saved_gain[ROLL_I];
    atti_pid.gain_roll[D] = saved_gain[ROLL_D];
    
    atti_pid.gain_pitch[P] = saved_gain[PITCH_P];
    atti_pid.gain_pitch[I] = saved_gain[PITCH_I];
    atti_pid.gain_pitch[D] = saved_gain[PITCH_D];
    
    atti_pid.gain_yaw[P] = saved_gain[YAW_P];
    atti_pid.gain_yaw[I] = saved_gain[YAW_I];
    atti_pid.gain_yaw[D] = saved_gain[YAW_D];
    */
    atti_pid.gain_roll[P] = now_gain[ROLL_P];
    atti_pid.gain_roll[I] = now_gain[ROLL_I];
    atti_pid.gain_roll[D] = now_gain[ROLL_D];
    
    atti_pid.gain_pitch[P] = now_gain[PITCH_P];
    atti_pid.gain_pitch[I] = now_gain[PITCH_I];
    atti_pid.gain_pitch[D] = now_gain[PITCH_D];
    
    atti_pid.gain_yaw[P] = now_gain[YAW_P];
    atti_pid.gain_yaw[I] = now_gain[YAW_I];
    atti_pid.gain_yaw[D] = now_gain[YAW_D];
    
    alt_pid.gain[P] = now_gain[ALT_P];
    alt_pid.gain[I] = now_gain[ALT_I];
    alt_pid.gain[D] = now_gain[ALT_D];
  }
  
  printf("initial gain data\n\r");
  for(axis = 0; axis <= 3; axis++)
  {
    for(type = 0; type <= 2; type++)
    {
      if(axis == ROLL)
      {
        printf("%.4f\n\r", atti_pid.gain_roll[type]);
      }
      else if(axis == PITCH)
      {
        printf("%.4f\n\r", atti_pid.gain_pitch[type]);
      }
      else if(axis == YAW)
      {
        printf("%.4f\n\r", atti_pid.gain_yaw[type]);
      }
      else if(axis == ALT)
      {
        printf("%.4f\n\r", alt_pid.gain[type]);
      }
    }
  }
}

/* Start Reading Gain values from Flash */
void Flash_StartRead(void)
{
  HAL_FLASH_Unlock();
  __HAL_FLASH_CLEAR_FLAG(FLASH_FLAG_EOP | FLASH_FLAG_OPERR | FLASH_FLAG_WRPERR | FLASH_FLAG_PGAERR | FLASH_FLAG_PGPERR | FLASH_FLAG_PGSERR);
}

/* Start Writing Gain Values to Flash */
void Flash_StartWrite(void)
{
  HAL_FLASH_Unlock();
  __HAL_FLASH_CLEAR_FLAG(FLASH_FLAG_EOP | FLASH_FLAG_OPERR | FLASH_FLAG_WRPERR | FLASH_FLAG_PGAERR | FLASH_FLAG_PGPERR | FLASH_FLAG_PGSERR);
}

/* Read Data from Flash after performing start reading condition */
uint32_t Flash_ReadData(uint32_t addr)
{
  uint32_t data;
  
  data = *(__IO uint32_t*)addr;
  
  return data;
}

/* Write Data to Flash after performing start writing condition */
void Flash_WriteData(uint32_t addr, uint32_t data)
{
  if(HAL_FLASH_Program(TYPEPROGRAM_WORD, addr, data) != HAL_OK)
  {
    printf("Write Error!\n\r");
  }
}

/* Load Gain Values from Flash */
void get_now_gain()
{
  int idx;
  
  Flash_StartRead();
  for(idx = 0; idx < COUNT_OF_GAIN; idx++)
  {
    gain.int_value = Flash_ReadData(gain_addr[idx]);
    now_gain[idx] = gain.float_value;
  }
  HAL_FLASH_Lock();
}

/* Change Gain Values(call save_gain() function)*/
void change_gain(uint8_t gain_type, float gain_value)
{
  switch(gain_type)
  {
    case ROLL_P :
      atti_pid.gain_roll[P] = gain_value;
      break;
    case ROLL_I :
      atti_pid.gain_roll[I] = gain_value;
      break;
    case ROLL_D :
      atti_pid.gain_roll[D] = gain_value;
      break;
    case PITCH_P :
      atti_pid.gain_pitch[P] = gain_value;
      break;
    case PITCH_I :
      atti_pid.gain_pitch[I] = gain_value;
      break;
    case PITCH_D :
      atti_pid.gain_pitch[D] = gain_value;
      break;
    case YAW_P :
      atti_pid.gain_yaw[P] = gain_value;
      break;
    case YAW_I :
      atti_pid.gain_yaw[I] = gain_value;
      break;
    case YAW_D :
      atti_pid.gain_yaw[D] = gain_value;
      break;
    case ALT_P :
      alt_pid.gain[P] = gain_value;
      break;
    case ALT_I :
      alt_pid.gain[I] = gain_value;
      break;
    case ALT_D :
      alt_pid.gain[D] = gain_value;
      break;
  }
  
  save_gain(gain_type, gain_value);
}

/* Save New Gain Values on Flash */
void save_gain(uint8_t gain_type, float gain_value)
{
  int idx;
  
  get_now_gain();
  
  Flash_StartWrite();
  FLASH_Erase_Sector(FLASH_SECTOR_3, FLASH_VOLTAGE_RANGE_3);
  Flash_WriteData(ADDR_CHECK, (uint32_t)1);
  
  /* Write All Gain Values Because previous datas in Flash were erased */
  for(idx = 0; idx < COUNT_OF_GAIN; idx++)
  {
    if(idx == gain_type)
    {
      gain.float_value = gain_value;
      Flash_WriteData(gain_addr[gain_type], gain.int_value);
    }
    else
    {
      gain.float_value = now_gain[idx];
      Flash_WriteData(gain_addr[idx], gain.int_value);
    }
  }
  HAL_FLASH_Lock();
}

