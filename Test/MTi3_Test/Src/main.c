/**
  ******************************************************************************
  * File Name          : main.c
  * Description        : Main program body
  ******************************************************************************
  ** This notice applies to any and all portions of this file
  * that are not between comment pairs USER CODE BEGIN and
  * USER CODE END. Other portions of this file, whether 
  * inserted by the user or by software development tools
  * are owned by their respective copyright owners.
  *
  * COPYRIGHT(c) 2017 STMicroelectronics
  *
  * Redistribution and use in source and binary forms, with or without modification,
  * are permitted provided that the following conditions are met:
  *   1. Redistributions of source code must retain the above copyright notice,
  *      this list of conditions and the following disclaimer.
  *   2. Redistributions in binary form must reproduce the above copyright notice,
  *      this list of conditions and the following disclaimer in the documentation
  *      and/or other materials provided with the distribution.
  *   3. Neither the name of STMicroelectronics nor the names of its contributors
  *      may be used to endorse or promote products derived from this software
  *      without specific prior written permission.
  *
  * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
  * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
  * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
  * DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE
  * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
  * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
  * SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
  * CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
  * OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
  * OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
  *
  ******************************************************************************
  */
/* Includes ------------------------------------------------------------------*/
#include "main.h"
#include "stm32f4xx_hal.h"
#include "usart.h"
#include "gpio.h"


/* USER CODE BEGIN Includes */
#include "mti.h"
/* USER CODE END Includes */

/* Private variables ---------------------------------------------------------*/

/* USER CODE BEGIN PV */
/* Private variables ---------------------------------------------------------*/\
extern volatile uint8_t mti_rx_flag;
extern volatile uint8_t mti_checksum_flag;
extern uint16_t mti_msg_len;
extern uint8_t mti_it_buff[1024];
extern uint8_t mti_buff[1024];
/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);

/* USER CODE BEGIN PFP */
/* Private function prototypes -----------------------------------------------*/

/* USER CODE END PFP */

/* USER CODE BEGIN 0 */

/* USER CODE END 0 */

int main(void)
{

  /* USER CODE BEGIN 1 */
  
  /* USER CODE END 1 */

  /* MCU Configuration----------------------------------------------------------*/

  /* Reset of all peripherals, Initializes the Flash interface and the Systick. */
  HAL_Init();

  /* USER CODE BEGIN Init */

  /* USER CODE END Init */

  /* Configure the system clock */
  SystemClock_Config();

  /* USER CODE BEGIN SysInit */

  /* USER CODE END SysInit */

  /* Initialize all configured peripherals */
  MX_GPIO_Init();
  MX_USART1_UART_Init();
  MX_USART2_UART_Init();

  /* USER CODE BEGIN 2 */
  HAL_UART_Receive_IT(&huart2, mti_it_buff, 1);
  
  /* 이부분에서 init_mti()를 호출합니다!! */
  init_mti();
  /* USER CODE END 2 */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
  while (1)
  {
    //printf("%d\r\n", mti_rx_flag);
    //HAL_UART_Receive(&huart2, mti_buff, 100, 0xffff);
    //mti_rx_flag = 1;
    if(mti_rx_flag == 1)
    {
      mti_rx_flag = 0;
      mti_checksum_flag = filter_mti();
      if(mti_checksum_flag == 1)
      {
        //mti_checksum_flag = 0;
        decode_mti(ACCELERATION);
      }
    }
    /*if(mti_rx_flag == 1)
    {
      mti_rx_flag = 0;
      mti_checksum_flag = filter_mti();
      if(mti_checksum_flag)
        decode_mti(EULER_ANGLES);
    }*/
  /* USER CODE END WHILE */

  /* USER CODE BEGIN 3 */

  }
  /* USER CODE END 3 */

}

/** System Clock Configuration
*/
void SystemClock_Config(void)
{

  RCC_OscInitTypeDef RCC_OscInitStruct;
  RCC_ClkInitTypeDef RCC_ClkInitStruct;

    /**Configure the main internal regulator output voltage 
    */
  __HAL_RCC_PWR_CLK_ENABLE();

  __HAL_PWR_VOLTAGESCALING_CONFIG(PWR_REGULATOR_VOLTAGE_SCALE1);

    /**Initializes the CPU, AHB and APB busses clocks 
    */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSI;
  RCC_OscInitStruct.HSIState = RCC_HSI_ON;
  RCC_OscInitStruct.HSICalibrationValue = 16;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
  RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSI;
  RCC_OscInitStruct.PLL.PLLM = 8;
  RCC_OscInitStruct.PLL.PLLN = 168;
  RCC_OscInitStruct.PLL.PLLP = RCC_PLLP_DIV2;
  RCC_OscInitStruct.PLL.PLLQ = 4;
  if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
  {
    _Error_Handler(__FILE__, __LINE__);
  }

    /**Initializes the CPU, AHB and APB busses clocks 
    */
  RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK|RCC_CLOCKTYPE_SYSCLK
                              |RCC_CLOCKTYPE_PCLK1|RCC_CLOCKTYPE_PCLK2;
  RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
  RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV4;
  RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV2;

  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_5) != HAL_OK)
  {
    _Error_Handler(__FILE__, __LINE__);
  }

    /**Configure the Systick interrupt time 
    */
  HAL_SYSTICK_Config(HAL_RCC_GetHCLKFreq()/1000);

    /**Configure the Systick 
    */
  HAL_SYSTICK_CLKSourceConfig(SYSTICK_CLKSOURCE_HCLK);

  /* SysTick_IRQn interrupt configuration */
  HAL_NVIC_SetPriority(SysTick_IRQn, 0, 0);
}

/* USER CODE BEGIN 4 */

// mti 한 패킷 필터링 함수
/*uint8_t mti_filtering()
{
  uint8_t idx, checksum_flag = 0;
  uint16_t result = 0;
  uint16_t checksum;
  
  //무결성 확인을 위한 preamble을 제외한 나머지 데이터 모두 합하기 
  for(idx = 1; idx < len; idx++)
  {
    result = result + mti_buff[idx];
  }
  checksum = result & 0x00ff;
  
  // preamble 제외한 모든 데이터 더한 결과의 8개 비트가 0x00이면(체크섬이 맞으면)
  if(checksum == 0x0000)
  {
    //for(idx = 0; idx < len; idx++)
    //{
      //printf("%4x", mti_buff[idx]); 
    //}
    //printf("\r\n");
    
    checksum_flag = 1;
  }
  
  return checksum_flag;
}*/

//수신한 데이터로 deltaV, deltaQ, magnetic_field decode하는 함수
/*void mti_decoding()
{
  uint8_t idx;
  
  //EulerAngles 데이터 시작 인덱스 구하기
  for(idx = 0; idx < len-1; idx++)
  {
    if((mti_buff[idx] == 0x20) && (mti_buff[idx+1] == 0x30))
    {
      idx = idx + 3;
      printf("%x %x %x %x\r\n", mti_buff[idx], mti_buff[idx+1], mti_buff[idx+2], mti_buff[idx+3]);
      break;
    }
  }
  
  //EulerAngles
  mti.euler_angles[0].euler_angles_buff[3] = mti_buff[idx];
  mti.euler_angles[0].euler_angles_buff[2] = mti_buff[idx+1];
  mti.euler_angles[0].euler_angles_buff[1] = mti_buff[idx+2];
  mti.euler_angles[0].euler_angles_buff[0] = mti_buff[idx+3];
  
  for(idx = 0; idx < 4; idx++)
    printf("%x ", mti.euler_angles[0].euler_angles_buff[idx]);
  printf("\r\n");
      
  printf("angles : %f\r\n\r\n", mti.euler_angles[0].euler_angles_data);
  
  mti.delta_v[0].delta_v_buff[2] = mti_buff[idx+1];
  mti.delta_v[0].delta_v_buff[1] = mti_buff[idx+2];
  mti.delta_v[0].delta_v_buff[0] = mti_buff[idx+3];
  mti.delta_v[1].delta_v_buff[3] = mti_buff[idx+4];
  mti.delta_v[1].delta_v_buff[2] = mti_buff[idx+5];
  mti.delta_v[1].delta_v_buff[1] = mti_buff[idx+6];
  mti.delta_v[1].delta_v_buff[0] = mti_buff[idx+7];
  mti.delta_v[2].delta_v_buff[3] = mti_buff[idx+8];
  mti.delta_v[2].delta_v_buff[2] = mti_buff[idx+9];
  mti.delta_v[2].delta_v_buff[1] = mti_buff[idx+10];
  mti.delta_v[2].delta_v_buff[0] = mti_buff[idx+11];
  
  printf("\r\n");
  for(idx = 0; idx < 4; idx++)
    printf("%x ", mti.delta_v[1].delta_v_buff[idx]);
  printf("\r\n");
  for(idx = 0; idx < 4; idx++)
    printf("%x ", mti.delta_v[2].delta_v_buff[idx]);
  printf("\r\n");
      
  printf("deltaV : %3f %3f %3f\r\n\r\n", mti.delta_v[0].delta_v_data, mti.delta_v[1].delta_v_data, mti.delta_v[2].delta_v_data);
  
 }*/ 


void HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart)
{ 
  static int idx;
  HAL_UART_Receive_IT(&huart2, mti_it_buff, 1);
  
  if(mti_it_buff[0] == 0xfa)
  {
    idx = 0;
  }
  
  mti_buff[idx] = mti_it_buff[0];
  idx++;
  
  if((idx >= mti_msg_len))
  {
    mti_rx_flag = 1;
    idx = 0;
  }
}

#ifdef __GNUC__
  /* With GCC/RAISONANCE, small printf (option LD Linker->Libraries->Small printf
     set to 'Yes') calls __io_putchar() */
  #define PUTCHAR_PROTOTYPE int __io_putchar(int ch)
#else
  #define PUTCHAR_PROTOTYPE int fputc(int ch, FILE *f)
#endif /* __GNUC__ */
/**
  * @brief  Retargets the C library printf function to the USART.
  * @param  None
  * @retval None
  */
PUTCHAR_PROTOTYPE
{
  /* write a character to the uart1 and Loop until the end of transmission */
  HAL_UART_Transmit(&huart1, (uint8_t *)&ch, 1, 0xFFFF); 

  return ch;
}
/* USER CODE END 4 */

/**
  * @brief  This function is executed in case of error occurrence.
  * @param  None
  * @retval None
  */
void _Error_Handler(char * file, int line)
{
  /* USER CODE BEGIN Error_Handler_Debug */
  /* User can add his own implementation to report the HAL error return state */
  while(1) 
  {
  }
  /* USER CODE END Error_Handler_Debug */ 
}

#ifdef USE_FULL_ASSERT

/**
   * @brief Reports the name of the source file and the source line number
   * where the assert_param error has occurred.
   * @param file: pointer to the source file name
   * @param line: assert_param error line source number
   * @retval None
   */
void assert_failed(uint8_t* file, uint32_t line)
{
  /* USER CODE BEGIN 6 */
  /* User can add his own implementation to report the file name and line number,
    ex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */
  /* USER CODE END 6 */

}

#endif

/**
  * @}
  */ 

/**
  * @}
*/ 

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
