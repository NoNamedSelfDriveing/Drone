#include "mixer.h"

int16_t motor_output[4];

void mixer()
{
  const float min_duty = 4598.0;
  const float max_duty = 8126.0;
  const float max_pwm = 1696.0;
  const float min_pwm = 352.0;
  static uint16_t prev_arm_data = 352, curr_arm_data = 352;
  static uint8_t arming_flag = 0;
  int i;
  
 
  printf("curr_arm_data new\r\n");
  /* if throttle is very low == stop*/
  if(pid_output[3]<=360)
  {
    for(i = 0 ; i < 4; i++) 
      motor_output[i] = 352;
  }
 
  else
  {
    motor_output[0] = (int16_t)(-pid_output[0] + pid_output[1] + pid_output[2] + pid_output[3]);
    motor_output[1] = (int16_t)(pid_output[0] + pid_output[1] - pid_output[2] + pid_output[3]);
    motor_output[2] = (int16_t)(pid_output[0] - pid_output[1] + pid_output[2] + pid_output[3]);
    motor_output[3] = (int16_t)(-pid_output[0] - pid_output[1] - pid_output[2] + pid_output[3]);
  }
  
  for(i = 0; i < 4; i++)
  {
    if(motor_output[i] >= 1696)
    {
      motor_output[i] = 1696;
    }
    else if(motor_output[i] <= 352)
    {
      motor_output[i] = 352;
    }
  }
  
  curr_arm_data = sbus_data_buff[11];
  
  if((prev_arm_data == 352) && (curr_arm_data == 1696))
  {
    if(arming_flag == 1)
    {
      printf("arm disable\r\n");
      arming_flag = 0;
    }
    else if(arming_flag == 0)
    {
      printf("arm able\r\n");
      arming_flag = 1;
    }
  }
  
  if(arming_flag == 0)
  {
    //printf("fuck\r\n");
    for(i = 0; i < 4; i++)
      motor_output[i] = 352;
  }
  
    
  
  htim1.Instance -> CCR1 = (uint16_t)(motor_output[0] / ((max_pwm - min_pwm) / (max_duty - min_duty)) + 3696);
  htim1.Instance -> CCR2 = (uint16_t)(motor_output[1] / ((max_pwm - min_pwm) / (max_duty - min_duty)) + 3696);
  htim1.Instance -> CCR3 = (uint16_t)(motor_output[2] / ((max_pwm - min_pwm) / (max_duty - min_duty)) + 3696);
  htim1.Instance -> CCR4 = (uint16_t)(motor_output[3] / ((max_pwm - min_pwm) / (max_duty - min_duty)) + 3696);
  
  prev_arm_data = curr_arm_data;
}