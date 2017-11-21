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

/* USER CODE BEGIN Includes */
#include <math.h>
/* USER CODE END Includes */

/* Private variables ---------------------------------------------------------*/
I2C_HandleTypeDef hi2c1;

/* USER CODE BEGIN PV */
/* Private variables ---------------------------------------------------------*/
<<<<<<< HEAD:Source/Atmosphere_Sensor decoding/Src/main.c
#define w_address 0xEC  // use when transmit
#define r_address 0xED  // use when read
#define d1_4096  0x48   //get pressure_value command
#define d2_4096  0x58   //get temperature_value command
#define adc_read 0x00   //get adc read command
#define prom_read 0xA0  //get prom read command
#define d_time 9

uint8_t rxbuff[4]; 
uint8_t txbuff[1];
uint16_t c[8] = {0,};  //get C value that listed in datasheet

uint32_t pressure_value;
uint32_t temperature_value;

float dt;
float temp;
float off;
float sens;
float off2;
float sens2;
float temp2;
float p;  
float h_temp;  
float altitude;  
float altitude_meter;
=======
#define address 0x76 << 1 // address in datasheet and shift left

uint8_t buffer[4]; 
uint16_t value; 
>>>>>>> 068126febda50069f3211d2cd21e2696226669b4:Source/atmosphere_sensor/Src/main.c
/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
static void MX_GPIO_Init(void);
static void MX_I2C1_Init(void);

/* USER CODE BEGIN PFP */
/* Private function prototypes -----------------------------------------------*/
void get_value();
void get_alt();
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
  MX_I2C1_Init();

  /* USER CODE BEGIN 2 */
  
  /* USER CODE END 2 */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
  while (1)
  {
  /* USER CODE END WHILE */

  /* USER CODE BEGIN 3 */
<<<<<<< HEAD:Source/Atmosphere_Sensor decoding/Src/main.c
  HAL_Delay(1000);
  get_value();
  get_alt();
=======
  
  buffer[0] = 0xA6; //read memory address
  buffer[1] = 0;
  buffer[2] = 0;
  buffer[3] = 0;
  HAL_I2C_Master_Transmit(&hi2c1, address, buffer , 2 ,100); // 2 = datasize 100 = timeout
  
  //HAL_Delay(30);
  //respond
  HAL_I2C_Master_Receive(&hi2c1, address, buffer , 3 ,100);


  HAL_Delay(400);
  
  buffer[0] = 0x48; //Pressure conversion command
  HAL_I2C_Master_Transmit(&hi2c1, address, buffer , 2 ,100);
  
 
  HAL_Delay(400);
  
  buffer[0] = 0x00; //ADC read command 
  HAL_I2C_Master_Transmit(&hi2c1, address , buffer , 2 ,100);
  
  HAL_Delay(400);
  
  HAL_I2C_Master_Receive(&hi2c1, address , buffer , 4, 100);
  
  
  //HAL_Delay(20);
  
  value = buffer[0] << 8| buffer[1]; // make 16 bit
  HAL_Delay(100);
>>>>>>> 068126febda50069f3211d2cd21e2696226669b4:Source/atmosphere_sensor/Src/main.c
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
  RCC_OscInitStruct.PLL.PLLN = 96;
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
  RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV2;
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV2;
  RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV1;

  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_1) != HAL_OK)
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

/* I2C1 init function */
static void MX_I2C1_Init(void)
{

  hi2c1.Instance = I2C1;
  hi2c1.Init.ClockSpeed = 100000;
  hi2c1.Init.DutyCycle = I2C_DUTYCYCLE_2;
  hi2c1.Init.OwnAddress1 = 0;
  hi2c1.Init.AddressingMode = I2C_ADDRESSINGMODE_7BIT;
  hi2c1.Init.DualAddressMode = I2C_DUALADDRESS_DISABLE;
  hi2c1.Init.OwnAddress2 = 0;
  hi2c1.Init.GeneralCallMode = I2C_GENERALCALL_DISABLE;
  hi2c1.Init.NoStretchMode = I2C_NOSTRETCH_DISABLE;
  if (HAL_I2C_Init(&hi2c1) != HAL_OK)
  {
    _Error_Handler(__FILE__, __LINE__);
  }

}

/** Configure pins as 
        * Analog 
        * Input 
        * Output
        * EVENT_OUT
        * EXTI
*/
static void MX_GPIO_Init(void)
{

  /* GPIO Ports Clock Enable */
  __HAL_RCC_GPIOA_CLK_ENABLE();
  __HAL_RCC_GPIOB_CLK_ENABLE();

}

/* USER CODE BEGIN 4 */
void get_value() // get value in sensor
{ 
  txbuff[0] = 0x1E;
  
  HAL_I2C_Master_Transmit(&hi2c1, w_address, txbuff, 1, 100);
  HAL_Delay(d_time);
  
  // Get SENS_T1
  txbuff[0] = prom_read | 0x02;
  
  HAL_I2C_Master_Transmit(&hi2c1, w_address, txbuff, 1, 100);
  HAL_Delay(d_time);
  
  HAL_I2C_Master_Receive(&hi2c1, r_address, rxbuff, 2, 100);
  
  c[1] = (rxbuff[0] << 8) | (rxbuff[1]);
  
  
  // Get OFF_T1
  txbuff[0] = prom_read | 0x04;
  
  HAL_I2C_Master_Transmit(&hi2c1, w_address, txbuff , 1 ,100);
  HAL_Delay(d_time);
  
  HAL_I2C_Master_Receive(&hi2c1, r_address , rxbuff , 2, 100);
  
  c[2] = ((rxbuff[0] << 8) | rxbuff[1]);
  
  
  //Get TCS
  txbuff[0] = prom_read | 0x06; 
  
  HAL_I2C_Master_Transmit(&hi2c1, w_address, txbuff , 1 ,100);
  HAL_Delay(d_time);
  
  HAL_I2C_Master_Receive(&hi2c1, r_address , rxbuff , 2, 100);
  
  c[3] = ((rxbuff[0] << 8) | rxbuff[1]);
  
  //Get TCO     
  txbuff[0] = prom_read | 0x08; 
  
  HAL_I2C_Master_Transmit(&hi2c1, w_address, txbuff , 1 ,100);
  HAL_Delay(d_time);
  
  HAL_I2C_Master_Receive(&hi2c1, r_address , rxbuff , 2, 100);
  
  c[4] = ((rxbuff[0] << 8) | rxbuff[1]);
 
  //Get T_REF
  txbuff[0] = prom_read | 0x0a;
  
  HAL_I2C_Master_Transmit(&hi2c1, w_address, txbuff , 1 ,100);
  HAL_Delay(d_time);
  
  HAL_I2C_Master_Receive(&hi2c1, r_address , rxbuff , 2, 100);
  
  c[5] = ((rxbuff[0] << 8) | rxbuff[1]);
  

  //Get TEMPSENS
  txbuff[0] = prom_read | 0x0c; 
  
  HAL_I2C_Master_Transmit(&hi2c1, w_address, txbuff , 1 ,100);
  HAL_Delay(d_time);
  
  HAL_I2C_Master_Receive(&hi2c1, r_address , rxbuff , 2, 100);
  
  c[6] = ((rxbuff[0] << 8) | rxbuff[1]);
  
  //Get CRC
  txbuff[0] = prom_read | 0x0e;
  
  HAL_I2C_Master_Transmit(&hi2c1, w_address, txbuff, 1, 100);
  HAL_Delay(d_time);
  
  HAL_I2C_Master_Receive(&hi2c1, r_address, rxbuff, 2, 100);
  
  c[7] = (rxbuff[0] << 8) | (rxbuff[1]);
  
  // Get Pressure_value
  txbuff[0] = d1_4096; 
  
  HAL_I2C_Master_Transmit(&hi2c1, w_address, txbuff , 1 ,100);
  HAL_Delay(d_time);
  
  txbuff[0] = adc_read;
  
  HAL_I2C_Master_Transmit(&hi2c1, w_address, txbuff , 1 ,100);
  
  HAL_Delay(d_time);
  
  HAL_I2C_Master_Receive(&hi2c1, r_address , rxbuff , 3, 100);
  
  pressure_value = (rxbuff[0] << 16) | (rxbuff[1] << 8) | (rxbuff[2]);
  
  //Get Temperature_value
  txbuff[0] = d2_4096; 
  
  HAL_I2C_Master_Transmit(&hi2c1, w_address, txbuff , 1 ,100);
  
  HAL_Delay(d_time);
  
  txbuff[0] = adc_read;
  
  HAL_I2C_Master_Transmit(&hi2c1, w_address, txbuff , 1 ,100);
  
  HAL_Delay(d_time);
  
  HAL_I2C_Master_Receive(&hi2c1, r_address , rxbuff , 3, 100);
  
  temperature_value = (rxbuff[0] << 16) | (rxbuff[1] << 8) | (rxbuff[2]);
}

void get_alt()  // get altitude
{
  dt = temperature_value - c[5] * pow(2,8);
  temp = ((2000 + (dt*c[5]) / pow(2,23)) / 100);
  
  if(temp < 20)
  {                // if temperature is lower than 20, this equation must add.
    temp2 = pow(dt, 2) / pow(2, 31);
    off2 = 5 * pow((temp - 2000),2) / pow(2,1);
    sens2 = 5 * pow((temp-2000),2) / pow(2,2);
  }
  else
  {     
    temp2 = 0;
    off2 = 0;
    sens2 = 0;
  }
  
  off = c[2] * pow(2,16) + (c[4] * dt) / pow(2,7);
  sens = c[1] * pow(2,15) + (c[3] * dt) / pow(2,8);
  
  temp = temp - temp2;
  off = off - off2;
  sens = sens - sens2;
  
  p = (((pressure_value * sens / pow(2,21)) - off) / pow(2,15)) / 100;
  
  h_temp = p/1013.25;
  altitude = (1-pow(h_temp,0.190284)) * 145366.45;
  altitude_meter = 0.3048 * altitude;
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
