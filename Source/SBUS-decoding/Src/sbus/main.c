#include "sbus.h"

sbus_pwm_struct sbus_pwm;
sbus_struct sbus;

// void SystemClock_Config(void);

int main(void)
{
  HAL_Init();
  SystemClock_Config();

  MX_GPIO_Init();
  MX_USART1_UART_Init();
  MX_USART2_UART_Init();
  MX_TIM1_Init();

  HAL_UART_Receive_IT(&huart1,sbus.uart_rx_buff,1);
  HAL_TIM_PWM_Start(&htim1, TIM_CHANNEL_1);

  // sbus initializing
  init_sbus(&sbus, &sbus_pwm, &htim1, TIM_CHANNEL_1);

  while (1)
  {   
    if(sbus.rx_flag == 1)
    {
      sbus.rx_flag = 0;
      sbus_data_filtering(&sbus, &sbus_pwm);     
    }
    
  }
}

// SystemClock config code

void HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart)
{ 
  HAL_UART_Receive_IT(&huart1, sbus.uart_rx_receive_buff,1);
  
  if(!sbus.rx_flag)
    sbus.rx_flag = 1;
}

// Else codes...