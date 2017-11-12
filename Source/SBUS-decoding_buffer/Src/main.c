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
#include "dma.h"
#include "tim.h"
#include "usart.h"
#include "gpio.h"

/* USER CODE BEGIN Includes */
#include <string.h>
/* USER CODE END Includes */

/* Private variables ---------------------------------------------------------*/

/* USER CODE BEGIN PV */
/* Private variables ---------------------------------------------------------*/

#define COUNTOF(__BUFFER__) (sizeof(__BUFFER__)/sizeof(*(__BUFFER__)))
#define BUFFERSIZE(buffer) (COUNTOF(buffer))
#define IS_STACKING_BUFFER_FULL(sbus) ((sbus).uart_rx_stacking_idx>=25)

#define SBUS_DATA_SIZE 23
#define UART_DATA_SIZE 25
#define STACKING_SIZE 250
#define ROW_SIZE 10
#define START_BYTE 0x0f
#define END_BYTE 0x04

typedef struct _SBUS_PWM
{
  float pwm;
  float min_pwm;
  float max_pwm;
  float min_duty;
  float max_duty;
  
  TIM_HandleTypeDef* htim;
  uint32_t channel;
  
  float F_dutyCycle;
  uint16_t DutyCycle; 
}SBUS_pwm;

typedef struct _SBUS
{
  uint8_t         uart_rx_receive_buff[11];
  uint8_t         uart_rx_stacking_buff[STACKING_SIZE];
  uint8_t		  uart_rx_decoding_buff[STACKING_SIZE];
  uint8_t		  remained_after_decoding[STACKING_SIZE];
  uint8_t         uart_rx_buff[ROW_SIZE][UART_DATA_SIZE];

  uint16_t        data_buff[ROW_SIZE][18];
  
  uint16_t		  uart_rx_data_idx;
  uint16_t		  uart_rx_stacking_idx;
  uint16_t        uart_rx_row_idx;
  uint16_t        sb_index_saver;
  
  uint16_t        idx;
  
  volatile        uint8_t rx_flag;
  volatile        uint8_t start_flag;

  SBUS_pwm sbus_pwm;
}SBUS;

// SBUS
SBUS_pwm sbus_pwm;
SBUS sbus;

/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);

/* USER CODE BEGIN PFP */
/* Private function prototypes -----------------------------------------------*/
void init_sbus_pwm(TIM_HandleTypeDef*, uint32_t);
void init_sbus();

// make next decodeable buffer
void make_next_decodeable_buffer();
  
// check sbus data packet (start byte, end byte) when sbus data stacked 25 bytes
void check_sbus_data_packet();

// decode sbus data
void decode_sbus_data();

/* USER CODE END PFP */

/* USER CODE BEGIN 0 */

/* USER CODE END 0 */

int main(void)
{

  /* USER CODE BEGIN 1 */
 
  init_sbus();
  
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
  MX_DMA_Init();
  MX_USART1_UART_Init();
  MX_USART2_UART_Init();
  MX_TIM1_Init();

  /* USER CODE BEGIN 2 */
  HAL_UART_Receive_DMA(&huart1, sbus.uart_rx_receive_buff, 5);
  HAL_TIM_PWM_Start(&htim1, TIM_CHANNEL_1);
  
  init_sbus_pwm(&htim1, TIM_CHANNEL_1);
  init_sbus();  
  /* USER CODE END 2 */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
  
  while (1)
  {
	if( IS_STACKING_BUFFER_FULL(sbus) )
    {
	  make_next_decodeable_buffer();
      check_sbus_data_packet();
      decode_sbus_data();
    }
  }
  /* USER CODE END WHILE */

  /* USER CODE BEGIN 3 */
    
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

void init_sbus_pwm(TIM_HandleTypeDef * htim, uint32_t channel){
    sbus_pwm.min_duty = 4598;
    sbus_pwm.max_duty = 8126;
    sbus_pwm.max_pwm = 1696;
    sbus_pwm.min_pwm = 352;
    sbus_pwm.pwm = 0;

    sbus_pwm.htim = htim;
    sbus_pwm.channel = channel;
}

void init_sbus(){
    sbus.sbus_pwm = sbus_pwm;

    sbus.uart_rx_data_idx = 0;
    sbus.uart_rx_stacking_idx = 0;
    sbus.sb_index_saver = 0;
    sbus.rx_flag = 0;
    sbus.start_flag = 0;
}

void make_next_decodeable_buffer()
{  
  sbus.uart_rx_stacking_idx = 0;
  memcpy(sbus.uart_rx_decoding_buff, sbus.remained_after_decoding, sizeof(uint8_t) * sbus.sb_index_saver);
  memcpy(sbus.uart_rx_decoding_buff + sbus.sb_index_saver, sbus.uart_rx_stacking_buff, sizeof(uint8_t) * (STACKING_SIZE - sbus.sb_index_saver) );
}

void check_sbus_data_packet()
{ 
  uint8_t sb_index = 0;        // Start byte index
  uint8_t is_index_over = 0;
  uint8_t uart_rx_row_index = 0;
  
  while(sb_index < STACKING_SIZE){

    // Increase index until find Start byte
    if(sbus.uart_rx_decoding_buff[sb_index] != START_BYTE && sbus.start_flag == 0)
    {
	  sbus.start_flag = 0;
	  sb_index++;
	  continue;
    }

    if(sbus.uart_rx_decoding_buff[sb_index] == START_BYTE && sbus.start_flag == 0)
    {
      // Search to full packet
      while( !(sbus.uart_rx_decoding_buff[sb_index] == START_BYTE && sbus.uart_rx_decoding_buff[sb_index + (UART_DATA_SIZE-1)]%16 == END_BYTE) )
      {
        sb_index++;
		if(sb_index + (UART_DATA_SIZE-1) > STACKING_SIZE-1){
		  is_index_over = 1;
		  break;
		}
      }

      if(is_index_over)
        break;
      
      sbus.start_flag = 1;
    }
    
    // If start flag is on
    if(sbus.start_flag)
    {
	  memcpy((sbus.uart_rx_buff[uart_rx_row_index]), (sbus.uart_rx_decoding_buff+sb_index), sizeof(uint8_t)*UART_DATA_SIZE);
	  uart_rx_row_index++;

      // location of next packet's start byte
      sb_index++;
	  sbus.start_flag = 0;
    }
  }

  memcpy(sbus.remained_after_decoding, sbus.uart_rx_decoding_buff + sb_index, sizeof(uint8_t) * (STACKING_SIZE - sb_index)); 
  
  memset(sbus.uart_rx_decoding_buff, 0, sizeof(uint8_t)*STACKING_SIZE);
  
  sbus.uart_rx_row_idx = uart_rx_row_index;
  sbus.sb_index_saver = (STACKING_SIZE - sb_index);  // 다음 버퍼에서 뒤에 이어붙이기 시작할 인덱스
}

void decode_sbus_data()
{ 
  uint8_t row_index = 0;
  for(row_index = 0 ; row_index < sbus.uart_rx_row_idx ; row_index++)
  {
    sbus.data_buff[row_index][0] = sbus.uart_rx_buff[row_index][1] + (uint16_t)((sbus.uart_rx_buff[row_index][2]&0x07)<<8);
    sbus.data_buff[row_index][1] = (uint16_t)((sbus.uart_rx_buff[row_index][2]&0xf8)>>3) + (uint16_t)((sbus.uart_rx_buff[row_index][3]&0x3f)<<5);
    sbus.data_buff[row_index][2] = (uint16_t)((sbus.uart_rx_buff[row_index][3]&0xc0)>>6) + (uint16_t)(sbus.uart_rx_buff[row_index][4]<<2) + (uint16_t)((sbus.uart_rx_buff[row_index][5]&0x01)<<10);
    sbus.data_buff[row_index][3] = (uint16_t)((sbus.uart_rx_buff[row_index][5]&0xfe)>>1) + (uint16_t)((sbus.uart_rx_buff[row_index][6]&0x0f)<<7);
    sbus.data_buff[row_index][4] = (uint16_t)((sbus.uart_rx_buff[row_index][6]&0xf0)>>4) + (uint16_t)((sbus.uart_rx_buff[row_index][7]&0x7f)<<4);
    sbus.data_buff[row_index][5] = (uint16_t)((sbus.uart_rx_buff[row_index][7]&0x80)>>7) + (uint16_t)(sbus.uart_rx_buff[row_index][8]<<1) + (uint16_t)((sbus.uart_rx_buff[row_index][9]&0x03)<<9);
    sbus.data_buff[row_index][6] = (uint16_t)((sbus.uart_rx_buff[row_index][9]&0xfc)>>2) + (uint16_t)((sbus.uart_rx_buff[row_index][10]&0x1f)<<6);
  	printf("%.4d\r\n", sbus.data_buff[row_index][2]);	// Throttle of Altitude (CH.2) 
  }
}

/*

void sbus_pwm_make(uint16_t F_value){
   //F_dutyCycle : (((value - 352)/ 1344) * 100 * 0.05) + 7
   uint16_t pulse;
   
   sbus_pwm.F_dutyCycle = ((F_value - 352) * 5.5 / 1344) +7;
   pulse = (((64499 + 1) * sbus_pwm.F_dutyCycle) / 100) - 1;
   
   sbus_pwm.DutyCycle = pulse;
   __HAL_TIM_SetCompare(&htim1, TIM_CHANNEL_1, sbus_pwm.DutyCycle);
    
   htim1.Instance -> CCR1 = sbus_pwm.DutyCycle;
   
   printf("%.4d %.4d %.3d \r\n", F_value, sbus_pwm.DutyCycle, sbus_pwm.F_dutyCycle);
}
*/

void HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart)
{ 
  if(huart == &huart1)
  {
	memcpy(sbus.uart_rx_stacking_buff+sbus.uart_rx_stacking_idx, sbus.uart_rx_receive_buff, sizeof(uint8_t)*5);
	sbus.uart_rx_stacking_idx += 5;
	
	//printf("%d\n\r", sbus.uart_rx_stacking_idx);
	//sbus.uart_rx_stacking_buff[ sbus.uart_rx_stacking_idx++ ] = sbus.uart_rx_receive_buff[0];
  	//HAL_UART_Receive_DMA(&huart1,sbus.uart_rx_receive_buff,8);
  }
}

#ifdef __GNUC__
/* With GCC, small printf (option LD Linker->Libraries->Small printf
set to 'Yes') calls __io_putchar() */
#define PUTCHAR_PROTOTYPE int __io_putchar(int ch)
#else
#define PUTCHAR_PROTOTYPE int fputc(int ch, FILE *f)
#endif /* __GNUC__ */
PUTCHAR_PROTOTYPE
{
  /* Place your implementation of fputc here */
  /* e.g. write a character to the EVAL_COM1 and Loop until the end of transmission */
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
  /* User can add his own implementation to rep nort the file name and line number,
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
