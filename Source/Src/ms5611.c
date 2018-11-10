#include "ms5611.h"
#include "lpf.h"
#include "i2c.h"
#include <math.h>

MS5611State ms5611_state;
MS5611 ms5611;
uint8_t send_cmd;
uint8_t rx_buff[1024];
uint16_t c[8];
uint32_t digital_temp, digital_press;
LPF alt_lpf;

float p;

/* initialze */
void init_ms5611()
{
  ms5611_state.adc_start_flag = 0;
  ms5611_state.adc_finish_flag = 0;
  ms5611_state.adc_time_cnt = 0;
  ms5611_state.adc_type = 0;
  
  ms5611_state.cali_flag = 0;
  ms5611_state.new_hgt_flag = 0;
  ms5611_state.alt_rx_cnt = 0;
  
  ms5611.start_alt = 0.0f;
  
  //init_lpf(&alt_lpf, 5.0f, 0.02f);
  
  reset_ms5611();
  read_ms5611_prom();
}

/* whole ms5611 sequence function*/
void read_ms5611()
{
  ms5611_state.new_hgt_flag = 0;
  /* if converting is started, count time till 10ms(wait for converting)*/
  if(ms5611_state.adc_start_flag)
  {
    ms5611_state.adc_time_cnt++;
  }
  /* if converting is completed(use 10ms), read adc value */
  if(ms5611_state.adc_time_cnt >= ADC_DELAY_TIME)
  {
    read_ms5611_adc(ms5611_state.adc_type);
    ms5611_state.adc_start_flag = 0;
    ms5611_state.adc_time_cnt = 0;
    
    /* if temp, pressure were received, start calculating */
    if(ms5611_state.adc_type != 0)
    {
      calc_press();
      calc_alt();
      
      // Get average altitude(n = 100)
      if(!ms5611_state.cali_flag)
      {
        ms5611.start_alt += ms5611.now_alt;
        ms5611_state.alt_rx_cnt++;
        if(ms5611_state.alt_rx_cnt >= COUNT_OF_AVG_DATA)
        {
          ms5611.start_alt /= (float)COUNT_OF_AVG_DATA;
          ms5611_state.cali_flag = 1;
        }
      }
      
      if(ms5611_state.cali_flag)
      {
        ms5611.hgt = ms5611.now_alt - ms5611.start_alt;
        ms5611_state.new_hgt_flag = 1;
      }
      ms5611_state.adc_finish_flag = 1;
    }
    ms5611_state.adc_type = !(ms5611_state.adc_type);
  }
  /* if reading adc value is completed, start new conversion */
  if(!(ms5611_state.adc_start_flag))
  {
    start_ms5611_adc(ms5611_state.adc_type);
    ms5611_state.adc_start_flag = 1;
  }
}

/* start reset sequence */
void reset_ms5611()
{
  send_cmd = CMD_RESET;
  HAL_I2C_Master_Transmit(&hi2c1, MS5611_ADDR_W, &(send_cmd), 1, 100);
  HAL_Delay(3);
}

/* read coefficient value from prom */
void read_ms5611_prom()
{
  int i;
  for(i = 1; i <= 6; i++)
  {
    send_cmd = CMD_PROM_READ | (i << 1);
    HAL_I2C_Master_Transmit(&hi2c1, MS5611_ADDR_W, &(send_cmd), 1, 100);
    HAL_I2C_Master_Receive(&hi2c1, MS5611_ADDR_R, rx_buff, 2, 100);
    c[i] = (rx_buff[0] << 8) | (rx_buff[1]);
  }
}

/* start adc sequence */
void start_ms5611_adc(uint8_t type)
{
  /* if type is pressure*/
  if(type == 0)
  {
    send_cmd = CMD_ADC_D1;
  }
  
  /* if type is temperature */
  else
  {
    send_cmd = CMD_ADC_D2;
  }
  HAL_I2C_Master_Transmit(&hi2c1, MS5611_ADDR_W, &(send_cmd), 1, 100);
}  

/* read adc(digital temp, pressure) value */
void read_ms5611_adc(uint8_t type)
{
  send_cmd = CMD_ADC_READ;
  HAL_I2C_Master_Transmit(&hi2c1, MS5611_ADDR_W, &(send_cmd), 1, 100);
  
  HAL_I2C_Master_Receive(&hi2c1, MS5611_ADDR_R, rx_buff, 3, 100);
  
  /* if type is pressure */
  if(type == 0)
  {
    digital_press = (rx_buff[0] << 16) | (rx_buff[1] << 8) | (rx_buff[2]);
  }
  
  /* if type is temperature */
  else
  {
    digital_temp = (rx_buff[0] << 16) | (rx_buff[1] << 8) | (rx_buff[2]);
  }
}

/* calculate pressure by using coefficients, several values */
void calc_press()
{
  
  int32_t dt, temp;
  int64_t off, sens;
  
  int32_t temp2 = 0;
  int64_t off2 = 0, sens2 = 0;
  
  dt = digital_temp - (uint32_t)c[5] * 256;
  temp = 2000 + ((int64_t)dt * c[6]) / 8388608;
  
  off = (int64_t)c[2] * 65536 + (int64_t)c[4] * dt / 128;
  sens = (int64_t)c[1] * 32768 + (int64_t)c[3] * dt / 256;
  
  if(temp < 2000)
  {
    temp2 = (dt * dt) / (2 << 30);
    off2 = 5 * ((temp - 2000) * (temp - 2000)) / 2;
    sens2 = 5 * ((temp - 2000) * (temp - 2000)) / 4;
    
    temp = temp - temp2;
    off = off - off2;
    sens = sens - sens2;
  }
  
  else if(temp < -1500)
  {
    off2 = off2 + 7 * ((temp + 1500) * (temp + 1500));
    sens2 = sens2 + 11 * ((temp + 1500) * (temp + 1500)) / 2;
    
    off = off - off2;
    sens = sens - sens2;
  }
  
  ms5611.temp = temp * 0.01f;
  ms5611.p = ((digital_press * sens / 2097152 - off) / 32768) * 0.01f;
}

void calc_alt()
{
  ms5611.now_alt = 44330.0f * (1.0f - pow((ms5611.p/1023.9f), 0.1902949f));
}