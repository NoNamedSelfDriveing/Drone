#include "mixer.h"

int16_t motor_output[4];

/* Divide Motor Outpus along Motors' Position */
void mixer()
{
  const float min_duty = 4598.0;
  const float max_duty = 8126.0;
  const float max_pwm = 1696.0;
  const float min_pwm = 352.0;
  static uint16_t prev_arm_data = 352, curr_arm_data = 352;
  static uint8_t arming_flag = 0;
  int i;
 
  /* if throttle is very low, it must be stop*/
  if(pid_output[3]<=360)
  {
    for(i = 0 ; i < 4; i++) 
      motor_output[i] = 352;
  }
  
  /* sum and subtract PID outputs along motors' position*/
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
  
  curr_arm_data = sbus_data_buff[CH12];
  
  /* if CH12 Switch went up and down, turn on arming or turn off arming */
  if((prev_arm_data == 352) && (curr_arm_data == 1696))
  {
    if(arming_flag == 1)
    {
      //printf("arm disable\r\n");
      arming_flag = 0;
    }
    else
    {
      //printf("arm able\r\n");
      arming_flag = 1;
    }
  }
  
  /* if it is not armed now, no motor output for safety */
  if(arming_flag == 0)
  {
    for(i = 0; i < 4; i++)
      motor_output[i] = 352;
  }
  
  /* convert output value to 'pulse' value to change real PWM duty cycle */
  htim1.Instance -> CCR1 = (uint16_t)(motor_output[0] / ((max_pwm - min_pwm) / (max_duty - min_duty)) + 3696);
  htim1.Instance -> CCR2 = (uint16_t)(motor_output[1] / ((max_pwm - min_pwm) / (max_duty - min_duty)) + 3696);
  htim1.Instance -> CCR3 = (uint16_t)(motor_output[2] / ((max_pwm - min_pwm) / (max_duty - min_duty)) + 3696);
  htim1.Instance -> CCR4 = (uint16_t)(motor_output[3] / ((max_pwm - min_pwm) / (max_duty - min_duty)) + 3696);
  
  prev_arm_data = curr_arm_data;
}