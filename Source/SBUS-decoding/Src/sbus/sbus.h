#include "main.h"
#include "stm32f4xx_hal.h"
#include "tim.h"
#include "usart.h"
#include "gpio.h"

#define COUNTOF(__BUFFER__) (sizeof(__BUFFER__)/sizeof(*(__BUFFER__)))
#define BUFFERSIZE(buffer) (COUNTOF(buffer))

#define SBUS_DATA_SIZE 23
#define UART_DATA_SIZE 25
#define STACKING_SIZE 256
#define START_BYTE 0x0f
#define END_BYTE 0x04

typedef struct _SBUS_PWM
{
  float pwm;
    
  float min_pwm;
  float max_pwm;
  float min_duty;
  float max_duty;
    
  TIM_HandleTypeDef * htim;
  uint32_t channel;
}sbus_pwm_struct;

typedef struct _SBUS
{
  uint8_t         uart_rx_receive_buff[2];
  uint8_t         uart_rx_stacking_buff[STACKING_SIZE];
  uint8_t         uart_rx_buff[UART_DATA_SIZE];
  uint16_t        data_buff[18];

  // check_sbus_data_packet에서 uart_rx_buff에 데이터 적재할때 쓰이는 인덱스
  uint16_t        uart_rx_data_idx;
  // stack_sbus_data에서 uart_rx_stacking_buff에 데이터 저장할때 쓰이는 인덱스
  uint16_t        uart_rx_stacking_idx;
  
  volatile        uint8_t rx_flag;
  volatile        uint8_t start_flag;

  sbus_pwm_struct sbus_pwm;
}sbus_struct;

// Initialize sbus pwm maker struct
// Usage : init_sbus(&sbus, init_sbus_pwm(htim1, TIM_CHANNEL_1))
sbus_pwm_struct init_sbus_pwm(TIM_HandleTypeDef *, uint32_t);

// Initialize sbus struct
// Usage : init_sbus(&sbus, init_sbus_pwm(htim1, TIM_CHANNEL_1))
void init_sbus(sbus_struct *, sbus_pwm_struct);

// Stack sbus datas
// Usage : stack_sbus_data(&sbus)
void stack_sbus_data(sbus_struct *);

// check sbus data packet (start byte, end byte) when sbus data stacked 255 bytes
// Usage : check_sbus_data_packet(&sbus)
void check_sbus_data_packet(sbus_struct *);

// decode sbus data
// Usage : sbus_data_decoding(&sbus)
void decode_sbus_data(sbus_struct *);

// make pwm with sbus.data_buff[0]
// Usage : sbus_pwm_make(&sbus)
void make_sbus_pwm(sbus_struct *);

// make pwm only with sbus struct
// Usage : sbus_pwm_make_with_value(&sbus, data_value)
void make_sbus_pwm_with_value(sbus_struct *, uint16_t);

#include "sbus.c"