#include "gy63.h"

GY63 gy63;
uint8_t send_data;
uint8_t rx_buff[1024];
uint8_t tx_buff[1024];
uint16_t c[8];
uint32_t press_value;
uint32_t temp_value;

void init_gy63()
{
  send_gy63_reset_cmd();
  read_gy63_calibration_data();
}

void send_gy63_reset_cmd()
{
  send_data = CMD_RESET;
  HAL_I2C_Master_Transmit(&hi2c1, GY63_ADDR_W, &(send_data), 1, 100);
  HAL_Delay(3);
  printf("fuck\r\n");
}

void read_gy63_calibration_data()
{
  int i;
  for(i = 0; i < 8; i++)
  {
      send_data = CMD_PROM_READ + (i << 1);      
      HAL_I2C_Master_Transmit(&hi2c1, GY63_ADDR_W, &(send_data), 1, 100);
      while(!(HAL_I2C_GetState(&hi2c1) == HAL_I2C_STATE_READY));
      HAL_I2C_Master_Receive(&hi2c1, GY63_ADDR_R, rx_buff, 2, 100);
      while(!(HAL_I2C_GetState(&hi2c1) == HAL_I2C_STATE_READY));
      c[i] = (rx_buff[0] << 8) | (rx_buff[1]);
      printf("%d : %d\r\n", i, c[i]);
  }
}

void read_gy63_adc(uint8_t cmd)
{
  int i;
  for(i = 0; i < 2; i++)
  {
    if(i == 0)
    {
      send_data = CMD_ADC_D1 + cmd;
      //printf("temp send : %x\r\n", send_data);
      HAL_I2C_Master_Transmit(&hi2c1, GY63_ADDR_W, &send_data, 1, 100);
    }
    else
    {
      send_data = CMD_ADC_D2 + cmd;
      //printf("p temp : %x\r\n", send_data);
      HAL_I2C_Master_Transmit(&hi2c1, GY63_ADDR_W, &send_data, 1, 100);
    }
    
    switch(cmd & 0x0f)
    {
      case CMD_ADC_256: 
        HAL_Delay(1); 
        break;
      case CMD_ADC_512: 
        HAL_Delay(3);
        break;
      case CMD_ADC_1024:
        HAL_Delay(4);
        break;
      case CMD_ADC_2048:
        HAL_Delay(6);
        break;
      case CMD_ADC_4096:
        HAL_Delay(10);
        break;
    }
    
    send_data = CMD_ADC_READ;
    HAL_I2C_Master_Transmit(&hi2c1, GY63_ADDR_W, &send_data, 1, 100);
    while(!(HAL_I2C_GetState(&hi2c1) == HAL_I2C_STATE_READY));
    HAL_I2C_Master_Receive(&hi2c1, GY63_ADDR_R, rx_buff, 3, 100);
    while(!(HAL_I2C_GetState(&hi2c1) == HAL_I2C_STATE_READY));
    
    if(i == 0)
    {
      //rx_buff[1] = (((rx_buff[1]&0x80)>>7) | ((rx_buff[1]&0x40)>>5) | ((rx_buff[1]&0x20)>>3) | ((rx_buff[1]&0x10)>>1) | ((rx_buff[1]&0x08)<<1) | ((rx_buff[1]&0x04)<<3) | ((rx_buff[1]&0x02)<<5) | ((rx_buff[1]&0x01)<<7));
      gy63.digital_p = (rx_buff[0] << 16) | (rx_buff[1] << 8) | (rx_buff[2]);
    }
    else
    {
      //rx_buff[1] = (((rx_buff[1]&0x80)>>7) | ((rx_buff[1]&0x40)>>5) | ((rx_buff[1]&0x20)>>3) | ((rx_buff[1]&0x10)>>1) | ((rx_buff[1]&0x08)<<1) | ((rx_buff[1]&0x04)<<3) | ((rx_buff[1]&0x02)<<5) | ((rx_buff[1]&0x01)<<7));
      gy63.digital_temp = (rx_buff[0] << 16) | (rx_buff[1] << 8) | (rx_buff[2]);
    }
  }
}

void calculate_gy63_altitude()
{
   static uint8_t home_flag = 0;
   static double home_alt = 0.0;
   static double filtered_alt = 0.0;
   double temp2, off2, sens2;
  
  gy63.dt = gy63.digital_temp - c[5] * pow(2, 8);
  //gy63.temp = (float)((2000 + (gy63.dt * c[5]) / pow(2, 23)) / 100);
  gy63.temp = 2000 + ((gy63.dt * c[6]) / pow(2, 23));
  //printf("dt : %f\r\n", gy63.dt);
  //printf("temp : %f\r\n", gy63.temp);
  
  gy63.off = (c[2] * pow(2, 16) + (c[4] * gy63.dt) / pow(2, 7));
  gy63.sens = (c[1] * pow(2, 15) + (c[3] * gy63.dt) / pow(2, 8));
  
  if((float)(gy63.temp/100.0) < -20.0)
  {
    temp2 = pow(gy63.dt, 2) / pow(2, 31);
    off2 = (5 * pow((gy63.temp - 2000), 2)) / pow(2, 1);
    sens2 = (5 * pow((gy63.temp - 2000), 2)) / pow(2, 2);
  }
  else
  {
    temp2 = 0;
    off2 = 0;
    sens2 = 0;
  }
  
  
  
  gy63.temp = (gy63.temp - temp2);
  gy63.off = (gy63.off - off2);
  gy63.sens = (gy63.sens - sens2);
  
  //gy63.p = ((((gy63.digital_p * gy63.sens / pow(2, 21)) - gy63.off) / pow(2, 15)) / 100);
  gy63.p = (((gy63.digital_p * gy63.sens) / pow(2, 21) - gy63.off) / pow(2, 15)) / 100;
  //printf("p : %f\r\n", gy63.p);
  //gy63.h_temp = gy63.p*100 / 101325;
  //printf("h_temp : %f\r\n", gy63.h_temp);
  //gy63.altitude = (double)44330 * (1 - pow(gy63.h_temp, 0.190295));// * 145366.45);
  //printf("altitude : %f\r\n", gy63.altitude);
  //gy63.altitude_meter = 0.3048 * gy63.altitude;
  gy63.altitude = 0.3048 * ((1 - pow((gy63.p / 1013.25), 0.190284)) * 145366.45);
  if(home_flag == 0){
    home_alt = gy63.altitude;
    filtered_alt = gy63.altitude;
    home_flag = 1;
    return;
  }
  //gy63.altitude = 0.96 * gy63.altitude - home_alt;
  filtered_alt = 0.96 * filtered_alt + (1 - 0.96) * gy63.altitude;
  //printf("%f %f %f\r\n",(gy63.altitude - home_alt),gy63.p,gy63.temp);
  printf("%f %f %f\r\n",(filtered_alt - home_alt),gy63.p,gy63.temp);
  //printf("dt : %d\r\n", gy63.dt);
  //printf("temp : %f\r\n", gy63.temp);
  //printf("altitude_meter : %f\r\n", gy63.altitude_meter);
}

void get_gy63_calibration_data()
{
  /* RESET */
  
  tx_buff[0] = 0x1e;
  HAL_I2C_Master_Transmit(&hi2c1, GY63_ADDR, tx_buff, 1, 100);
  //while(!(HAL_I2C_GetState(&hi2c1) == HAL_I2C_STATE_READY));
  HAL_Delay(D_TIME);
  
  /* GET SENS_T1 */
  tx_buff[0] = PROM_READ | 0x02;
  HAL_I2C_Master_Transmit(&hi2c1, GY63_ADDR, tx_buff, 1, 100);
  //while(!(HAL_I2C_GetState(&hi2c1) == HAL_I2C_STATE_READY));
  HAL_Delay(D_TIME);
  
  HAL_I2C_Master_Receive(&hi2c1, (GY63_ADDR | 0x01), rx_buff, 2, 100);
  while(!(HAL_I2C_GetState(&hi2c1) == HAL_I2C_STATE_READY));
  c[0] = (rx_buff[0] << 8) | rx_buff[1];
  //printf("%4d\r\n", c[0]);
  
  /* GET OFF_T1 */
  tx_buff[0] = PROM_READ | 0x04;
  HAL_I2C_Master_Transmit(&hi2c1, GY63_ADDR, tx_buff, 1, 100);
  //while(!(HAL_I2C_GetState(&hi2c1) == HAL_I2C_STATE_READY));
  HAL_Delay(D_TIME);
  
  HAL_I2C_Master_Receive(&hi2c1, (GY63_ADDR | 0x01), rx_buff, 2, 100);
  while(!(HAL_I2C_GetState(&hi2c1) == HAL_I2C_STATE_READY));
  c[1] = (rx_buff[0] << 8) | rx_buff[1];
  //printf("%4d\r\n", c[1]);
  
  /* GET TCS */
  tx_buff[0] = PROM_READ | 0x06;
  HAL_I2C_Master_Transmit(&hi2c1, GY63_ADDR, tx_buff, 1, 100);
  //while(!(HAL_I2C_GetState(&hi2c1) == HAL_I2C_STATE_READY));
  HAL_Delay(D_TIME);
  
  HAL_I2C_Master_Receive(&hi2c1, (GY63_ADDR | 0x01), rx_buff, 2, 100);
  while(!(HAL_I2C_GetState(&hi2c1) == HAL_I2C_STATE_READY));
  c[2] = (rx_buff[0] << 8) | rx_buff[1];
  //printf("%4d\r\n", c[2]);
  
  /* GET TCO */
  tx_buff[0] = PROM_READ | 0x08;
  HAL_I2C_Master_Transmit(&hi2c1, GY63_ADDR, tx_buff, 1, 100);
  //while(!(HAL_I2C_GetState(&hi2c1) == HAL_I2C_STATE_READY));
  HAL_Delay(D_TIME);
  
  HAL_I2C_Master_Receive(&hi2c1, (GY63_ADDR | 0x01), rx_buff, 2, 100);
  while(!(HAL_I2C_GetState(&hi2c1) == HAL_I2C_STATE_READY));
  c[3] = (rx_buff[0] << 8) | rx_buff[1];
  //printf("%4d\r\n", c[3]);
  
  /* GET T_REF */
  tx_buff[0] = PROM_READ | 0x0a;
  HAL_I2C_Master_Transmit(&hi2c1, GY63_ADDR, tx_buff, 1, 100);
  //while(!(HAL_I2C_GetState(&hi2c1) == HAL_I2C_STATE_READY));
  HAL_Delay(D_TIME);
  
  HAL_I2C_Master_Receive(&hi2c1, (GY63_ADDR | 0x01), rx_buff, 2, 100);
  while(!(HAL_I2C_GetState(&hi2c1) == HAL_I2C_STATE_READY));
  c[4] = (rx_buff[0] << 8) | rx_buff[1];
  //printf("%4d\r\n", c[4]);
  
  /* GET TEMPSENS */
  tx_buff[0] = PROM_READ | 0x0c;
  HAL_I2C_Master_Transmit(&hi2c1, GY63_ADDR, tx_buff, 1, 100);
  //while(!(HAL_I2C_GetState(&hi2c1) == HAL_I2C_STATE_READY));
  HAL_Delay(D_TIME);
  
  HAL_I2C_Master_Receive(&hi2c1, (GY63_ADDR | 0x01), rx_buff, 2, 100);
  while(!(HAL_I2C_GetState(&hi2c1) == HAL_I2C_STATE_READY));
  c[5] = (rx_buff[0] << 8) | rx_buff[1];
  //printf("%4d\r\n", c[5]);
  
  /* GET CRC */
  tx_buff[0] = PROM_READ | 0x0E;
  HAL_I2C_Master_Transmit(&hi2c1, GY63_ADDR, tx_buff, 1, 100);
  //while(!(HAL_I2C_GetState(&hi2c1) == HAL_I2C_STATE_READY));
  HAL_Delay(D_TIME);
  
  HAL_I2C_Master_Receive(&hi2c1, (GY63_ADDR | 0x01), rx_buff, 2, 100);
  while(!(HAL_I2C_GetState(&hi2c1) == HAL_I2C_STATE_READY));
  c[6] = (rx_buff[0] << 8) | rx_buff[1];
  //printf("%4d\r\n", c[6]);
  
  printf("SENS : %d\r\n", c[0]);
  printf("OFF : %d\r\n", c[1]);
  printf("TCS : %d\r\n", c[2]);
  printf("TCO : %d\r\n", c[3]);
  printf("T : %d\r\n", c[4]);
  printf("TEMPSENS : %d\r\n", c[5]);
  printf("CRC : %d\r\n", c[6]);
}

void get_altitude()
{
  uint32_t press_value, temp_value;
  float temp, off, sens;

  //for(int i = 0; i < 6; i++)
  //  printf("%d : %4d ", i, c[i]);
  //printf("\r\n");
  
  /* GET Pressure Value */
  tx_buff[0] = D1_4096;
  HAL_I2C_Master_Transmit(&hi2c1, GY63_ADDR, tx_buff, 1, 100);
  //while(!(HAL_I2C_GetState(&hi2c1) == HAL_I2C_STATE_READY));
  HAL_Delay(D_TIME);
  
  tx_buff[0] = ADC_READ;
  HAL_I2C_Master_Transmit(&hi2c1, GY63_ADDR, tx_buff, 1, 100);
  //while(!(HAL_I2C_GetState(&hi2c1) == HAL_I2C_STATE_READY));
  HAL_Delay(D_TIME);
  
  HAL_I2C_Master_Receive(&hi2c1, (GY63_ADDR | 0x01), rx_buff, 3, 100);
  while(!(HAL_I2C_GetState(&hi2c1) == HAL_I2C_STATE_READY));
  press_value = (rx_buff[0] << 16) | (rx_buff[1] << 8) | (rx_buff[2]);
  //printf("%4d\r\n", press_value);
  
  
  /* GET Temperature Value */
  tx_buff[0] = D2_4096;
  HAL_I2C_Master_Transmit(&hi2c1, GY63_ADDR, tx_buff, 1, 100);
  //while(!(HAL_I2C_GetState(&hi2c1) == HAL_I2C_STATE_READY));
  HAL_Delay(D_TIME);
  
  tx_buff[0] = ADC_READ;
  HAL_I2C_Master_Transmit(&hi2c1, GY63_ADDR, tx_buff, 1, 100);
  //while(!(HAL_I2C_GetState(&hi2c1) == HAL_I2C_STATE_READY));
  HAL_Delay(D_TIME);
  
  HAL_I2C_Master_Receive(&hi2c1, (GY63_ADDR | 0x01), rx_buff, 3, 100);
  while(!(HAL_I2C_GetState(&hi2c1) == HAL_I2C_STATE_READY));
  temp_value = (rx_buff[0] << 16) | (rx_buff[1] << 8) | (rx_buff[2]);
  //printf("%4d\r\n", temp_value);
  
  gy63.dt = temp_value - c[4] * pow(2, 8);
  //printf("dt : %f\r\n", gy63.dt); 
  gy63.temp = ((2000 + (gy63.dt*c[5]) / pow(2,23)) / 100);
  //printf("temp : %f\r\n", gy63.temp);
  
  // if temperature is lower than 20, this equation must add.
  if(gy63.temp < 20.0)
  {     
    temp = pow(gy63.dt, 2) / pow(2, 31);
    off = 5 * pow((gy63.temp - 2000),2) / pow(2,1);
    sens = 5 * pow((gy63.temp - 2000),2) / pow(2,2);
  }
  
  
  else
  {
    temp = 0;
    off = 0;
    sens = 0;
  }
  
  gy63.off = c[1] * pow(2, 16) + (c[3] * gy63.dt) / pow(2, 7);
  gy63.sens = c[0] * pow(2, 15) + (c[2] * gy63.dt) / pow(2, 8);
  
  gy63.temp = gy63.temp - temp;
  gy63.off = gy63.off - off;
  gy63.sens = gy63.sens - sens;
  
  gy63.p = (((press_value * gy63.sens / pow(2,21)) - gy63.off) / pow(2,15)) / 100;
  
  //printf("temp : %f\r\n", gy63.temp);
  //printf("off : %f\r\n", gy63.off);
  //printf("sens : %f\r\n", gy63.sens);
  //printf("p : %f\r\n", gy63.p);
  
  gy63.h_temp = gy63.p / 1013.25;       
  //printf("h_temp : %f\r\n", gy63.h_temp);
  gy63.altitude = (1 - pow(gy63.h_temp, 0.190284)) * 145366.45;
  //gy63.altitude = (1 - pow(gy63.h_temp, 0.190295)) * 145366.45;
  //printf("altitude : %f\r\n", gy63.altitude);
  gy63.altitude_meter = 0.3048 * gy63.altitude;
  //gy63.altitude_meter = (float)44330 * (1 - pow(gy63.h_temp, 0.190295));
  //printf("temp : %f\r\n", gy63.temp);
  printf("altitude_meter : %f\r\n", gy63.altitude_meter);
}