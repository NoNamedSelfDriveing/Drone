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
#define GPS_BUFF_SIZE 256
#define NAV_POSLLH_SIZE 36
#define NAV_POSLLH_LENGTH 28

/* USER CODE END Includes */

/* Private variables ---------------------------------------------------------*/

/* USER CODE BEGIN PV */
/* Private variables ---------------------------------------------------------*/
union GPS_POSLLH_DATA
{
  uint8_t buff[4];
  float data;
  int data_int;
}posllh;

int data = 0;

uint8_t gps_data_temp_buff[2];
uint8_t gps_data_buff[NAV_POSLLH_SIZE];
uint8_t gps_rx_buff[GPS_BUFF_SIZE];

int gps_posllh_data_buff[7];

uint8_t CK_A = 0X00;
uint8_t CK_B = 0x00;

int gps_flag = 0;
int gps_start_flag = 0;
int idx = 0;
int count = 0;

/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);

/* USER CODE BEGIN PFP */
/* Private function prototypes -----------------------------------------------*/
void gps_data_filtering();
void gps_data_decoding();

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
  HAL_UART_Receive_IT(&huart1, gps_data_temp_buff, 1);
  /* USER CODE END 2 */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
  while (1)
  {
    if(gps_flag)
    {
      //gps_flag = 0;
      gps_data_filtering();
    }
    
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
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSE;
  RCC_OscInitStruct.HSEState = RCC_HSE_ON;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
  RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSE;
  RCC_OscInitStruct.PLL.PLLM = 4;
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
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV8;
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

void gps_data_decoding()
{
  int i = 0;
  int j = 0;
  
  int buff_idx = 6; //6~34
  int bit = 24;
    
  /*
  gps_posllh_data_buff[0] = (gps_data_buff[6] + (gps_data_buff[7] << 8) + (gps_data_buff[8] << 16) + (gps_data_buff[9] << 24));
  gps_posllh_data_buff[1] = (gps_data_buff[10] + (gps_data_buff[11] << 8) + (gps_data_buff[12] << 16) + (gps_data_buff[13] << 24));
  gps_posllh_data_buff[2] = (gps_data_buff[14] + (gps_data_buff[15] << 8) + (gps_data_buff[16] << 16) + (gps_data_buff[17] << 24));
  gps_posllh_data_buff[3] = (gps_data_buff[18] + (gps_data_buff[19] << 8) + (gps_data_buff[20] << 16) + (gps_data_buff[21] << 24));
  gps_posllh_data_buff[4] = (gps_data_buff[22] + (gps_data_buff[23] << 8) + (gps_data_buff[24] << 16) + (gps_data_buff[25] << 24));
  
  for(i=0; i<5; i++)
    printf("%4d ", gps_posllh_data_buff[i]);

  printf("\r\n");
  
  */
  
  for(i=0; i<7; i++)
  {
    for(j=0; j<4; j++)
    {
      posllh.buff[j] = gps_data_buff[buff_idx++];
    }
    gps_posllh_data_buff[i] = posllh.data_int;
  }
  
  for(i=0; i<7; i++)
    printf("%4d ", gps_posllh_data_buff[i]);
  
  printf("\r\n");
  
  /*
  posllh.buff[0] = gps_data_buff[6];
  posllh.buff[1] = gps_data_buff[7];
  posllh.buff[2] = gps_data_buff[8];
  posllh.buff[3] = gps_data_buff[9];

 for(i=0; i<4; i++)
    printf("%4X ", posllh.buff[i]);

 printf("%f %d",(float)data, posllh.data_int);
  
  
  
  
 
  
  for(i=0; i<4; i++)
    posllh.buff[i] = 0x00;
  
  posllh.data = 0.0;
  posllh.data_int = 0;
  /*
  printf("\r\n");
  printf("%f\r\n", posllh.data); 
    */

  
  /*
  for(i=6; i>=0; i--)
  {
    bit = 24;
    
    gps_posllh_data_buff[i] = 0;
                            
    for(j=0; j<4; j++)
    {
      gps_posllh_data_buff[i] += (gps_data_buff[buff_idx--]<< bit);
      bit -= 8;
    }
    
  }
  */

  
  printf("\r\n");
  
}

void gps_data_filtering()
{
  int i = 0;
  int j = 0;
  
  for(i=0; i<GPS_BUFF_SIZE; i++)
  {
    if(gps_rx_buff[i] == 0xB5 && gps_rx_buff[i+1] == 0x62)
    {
      if(gps_rx_buff[i+2] == 0x01 && gps_rx_buff[i+3] == 0x02)
      {
        for(j=0; j<NAV_POSLLH_SIZE ; j++)
        {
          gps_data_buff[j] = gps_rx_buff[i++];
        }
        
        CK_A = 0;
        CK_B = 0;
        
        for(j=2; j<NAV_POSLLH_SIZE-2; j++)
        {
          CK_A = CK_A + gps_data_buff[j];
          CK_B = CK_B + CK_A;
        }
        
        if(CK_A != gps_data_buff[NAV_POSLLH_SIZE-2] || CK_B != gps_data_buff[NAV_POSLLH_SIZE-1])
          continue;
        else
          for(j=0; j<NAV_POSLLH_SIZE ; j++)
            printf("%4X", gps_data_buff[j]);
                 
        printf("%4X %4X", CK_A, CK_B);
        printf("\r\n");
        
        gps_data_decoding();
      }
    }
  }
  
  gps_flag = 0;
  count = 1;
  HAL_UART_Receive_IT(&huart1,gps_data_temp_buff,1);

}

void HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart)
{ 
  if(!gps_flag)
    HAL_UART_Receive_IT(&huart1,gps_data_temp_buff,1);
  
  gps_rx_buff[count++] = gps_data_temp_buff[0];
 
  if(count == GPS_BUFF_SIZE)
  {
    count = 0;
    gps_flag = 1;
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
  HAL_UART_Transmit(&huart2, (uint8_t *)&ch, 1, 0xFFFF); 

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
