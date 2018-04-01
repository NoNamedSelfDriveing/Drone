#include "ms5611.h"
#include "i2c.h"
#include <math.h>
#include "gps.h"

MS5611State ms5611_state;
LPF LPF_barometer;
uint8_t send_cmd;
uint8_t rx_buff[1024];
uint16_t c[8];
uint32_t digital_temp, digital_press;
int32_t p;

/* initialze */
void init_ms5611()
{
  ms5611_state.adc_start_flag = 0;
  ms5611_state.adc_finish_flag = 0;
  ms5611_state.adc_time_cnt = 0;
  ms5611_state.adc_type = 0;
  
  LPF_barometer.fc = 5;
  LPF_barometer.dt = 0.02f;
  LPF_barometer.lambda = 2 * PI * LPF_barometer.fc * LPF_barometer.dt;
  LPF_barometer.x = 0.0f;
  LPF_barometer.filtered_x = 0.0f;
  LPF_barometer.prev_filtered_x = 0.0f;
  
  reset_ms5611();
  read_ms5611_prom();
}

/* whole ms5611 sequence function*/
void read_ms5611()
{
  /* if converting is started, count time till 10ms(wait for converting)*/
  if(ms5611_state.adc_start_flag)
  {
    ms5611_state.adc_time_cnt++;
  }
  /* if converting is completed(use 10ms), read adc value */
  if(ms5611_state.adc_time_cnt >= 10)
  {
    read_ms5611_adc(ms5611_state.adc_type);
    ms5611_state.adc_start_flag = 0;
    ms5611_state.adc_time_cnt = 0;
    /* if temp, pressure were received, start calculating */
    if(ms5611_state.adc_type != 0)
    {
      calculate_pressure();
      low_pass_filter();
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
  int i = 0;
  for(i = 1; i <= 6; i++)
  {
    send_cmd = CMD_PROM_READ | (i << 1);
    HAL_I2C_Master_Transmit(&hi2c1, MS5611_ADDR_W, &(send_cmd), 1, 100);
    HAL_I2C_Master_Receive(&hi2c1, MS5611_ADDR_R, rx_buff, 2, 100);
    c[i] = (rx_buff[0] << 8) | (rx_buff[1]);
    printf("%d\n\r", c[i]);
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
void calculate_pressure()
{
  int32_t dt, temp;
  int64_t off, sens;
  
  int32_t temp2 = 0;
  int64_t off2 = 0, sens2 = 0;
  
  dt = digital_temp - c[5] * pow(2, 8);
  temp = 2000 + dt * c[6] / pow(2, 23);
  
  off = c[2] * pow(2, 16) + (c[4] * dt) / pow(2, 7);
  sens = c[1] * pow(2, 15) + (c[3] * dt) / pow(2, 8);
  
  if((temp / 100) < 20)
  {
    temp2 = pow(dt, 2) / pow(2, 31);    
    off2 = 5 * pow((temp - 2000), 2) / pow(2, 1);
    sens2 = 5 * pow((temp - 2000), 2) / pow(2, 2);
  }
  
  temp = temp - temp2;
  off = off - off2;
  sens = sens - sens2;
  
  p = (digital_press * sens / pow(2, 21) - off) / pow(2, 15);
}

/* barometer low pass filter function */
void low_pass_filter()
{
  float altitude;
  
  LPF_barometer.x = p;
  LPF_barometer.filtered_x = ((LPF_barometer.lambda / (1 + LPF_barometer.lambda)) * LPF_barometer.x) \
                              + ((1 / (1 + LPF_barometer.lambda)) * LPF_barometer.prev_filtered_x);
  LPF_barometer.prev_filtered_x = LPF_barometer.filtered_x;
  
  altitude = 44330.0 * (1 - pow(((LPF_barometer.filtered_x/100.0) / 1026.6), (1/5.255)));
  //printf("%f\n\r", altitude);  
}

