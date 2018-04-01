#include "user_flash.h"

GainData gain;
float gain_buff[COUNT_OF_GAIN];
uint32_t gain_addr[COUNT_OF_GAIN] = {ADDR_ROLL_P, ADDR_ROLL_I, ADDR_ROLL_D, ADDR_PITCH_P, \
                         ADDR_PITCH_I, ADDR_PITCH_D, ADDR_YAW_P, ADDR_YAW_I, ADDR_YAW_D};

/* get last gain values from flash
   if gain values were not saved, set gain value */
void init_flash(void)
{
  float* saved_gain;
  uint32_t check = 100; 
  
  Flash_StartRead();
  check = Flash_ReadData(ADDR_CHECK);
  HAL_FLASH_Lock();
  
  /* if gain values were not saved(flash have not been used) */
  if(check != 1)
  {
    printf("1\n\r");
    Flash_StartWrite();
    
    FLASH_Erase_Sector(FLASH_SECTOR_3, VOLTAGE_RANGE_3);
    Flash_WriteData(ADDR_CHECK, (uint32_t)1);
       
    gain_roll[P] = 6.0f;
    gain_roll[I] = 0.0f;
    gain_roll[D] = 1.5f;
    
    gain_pitch[P] = 6.0f;
    gain_pitch[I] = 0.0f;
    gain_pitch[D] = 1.5f;
    
    gain_yaw[P] = 2.0f;
    gain_yaw[I] = 0.0f;
    gain_yaw[D] = 4.0f;
      
    /* write initial gain */
    for(int axis = 0; axis < 3; axis++)
    {
      for(int type = 0; type < 3; type++)
      {
        if(axis == ROLL)
        {
          gain.float_value = gain_roll[type];
          Flash_WriteData(gain_addr[type], gain.int_value);
        }
        else if(axis == PITCH)
        {
          gain.float_value = gain_pitch[type];
          Flash_WriteData(gain_addr[type+3], gain.int_value);
        }
        else if(axis == YAW)
        {
          gain.float_value = gain_yaw[type];
          Flash_WriteData(gain_addr[type+6], gain.int_value);
        }
      }
    }
    HAL_FLASH_Lock();
  }
  
  /* load last gain values from flash */
  else
  {    
    printf("2\n\r");
    
    saved_gain = get_gain();
    
    gain_roll[P] = saved_gain[ROLL_P];
    gain_roll[I] = saved_gain[ROLL_I];
    gain_roll[D] = saved_gain[ROLL_D];
    
    gain_pitch[P] = saved_gain[PITCH_P];
    gain_pitch[I] = saved_gain[PITCH_I];
    gain_pitch[D] = saved_gain[PITCH_D];
    
    gain_yaw[P] = saved_gain[YAW_P];
    gain_yaw[I] = saved_gain[YAW_I];
    gain_yaw[D] = saved_gain[YAW_D];
  }
  
  printf("initial gain data\n\r");
  for(int axis = 0; axis < 3; axis++)
  {
    for(int type = 0; type < 3; type++)
    {
      if(axis == ROLL)
      {
        printf("%.4f\n\r", gain_roll[type]);
      }
      else if(axis == PITCH)
      {
        printf("%.4f\n\r", gain_pitch[type]);
      }
      else if(axis == YAW)
      {
        printf("%.4f\n\r", gain_yaw[type]);
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
float* get_gain()
{
  static float curr_gain[COUNT_OF_GAIN];
  
  Flash_StartRead();
  for(int i = 0; i < 9; i++)
  {
    gain.int_value = Flash_ReadData(gain_addr[i]);
    curr_gain[i] = gain.float_value;
    //printf("curr_gain[i] : %.4f\n\r", gain.float_value);
  }
  HAL_FLASH_Lock();
  
  return curr_gain;
}

/* Change Gain Values(call save_gain() function)*/
void change_gain(uint8_t gain_type, float gain_value)
{
  switch(gain_type)
  {
    case ROLL_P :
      gain_roll[P] = gain_value;
      break;
    case ROLL_I :
      gain_roll[I] = gain_value;
      break;
    case ROLL_D :
      gain_roll[D] = gain_value;
      break;
    case PITCH_P :
      gain_pitch[P] = gain_value;
      break;
    case PITCH_I :
      gain_pitch[I] = gain_value;
      break;
    case PITCH_D :
      gain_pitch[D] = gain_value;
      break;
    case YAW_P :
      gain_yaw[P] = gain_value;
      break;
    case YAW_I :
      gain_yaw[I] = gain_value;
      break;
    case YAW_D :
      gain_yaw[D] = gain_value;
      break;
    default :
      //printf("Wrong gain type!\n\r");
      return;
  }
  
  save_gain(gain_type, gain_value);
}

/* Save New Gain Values on Flash */
void save_gain(uint8_t gain_type, float gain_value)
{
  float* prev_gain;
  
  prev_gain = get_gain();
  
  Flash_StartWrite();
  FLASH_Erase_Sector(FLASH_SECTOR_3, FLASH_VOLTAGE_RANGE_3);
  Flash_WriteData(ADDR_CHECK, (uint32_t)1);
  
  /* Write All Gain Values Because previous datas in Flash were erased */
  for(int i = 0; i < 9; i++)
  {
    if(i == gain_type)
    {
      gain.float_value = gain_value;
      Flash_WriteData(gain_addr[gain_type], gain.int_value);
      //printf("%.4f\n\r", gain.float_value);
    }
    else
    {
      gain.float_value = prev_gain[i];
      Flash_WriteData(gain_addr[i], gain.int_value);
      //printf("%.4f\n\r", prev_gain[i]);
    }
  }
  HAL_FLASH_Lock();
}

