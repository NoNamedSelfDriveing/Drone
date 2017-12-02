#include "mixer.h"

int16_t motor_output[4];

void mixer()
{
  const float min_duty = 4598.0;
  const float max_duty = 8126.0;
  const float max_pwm = 1696.0;
  const float min_pwm = 352.0;
  int i;
 
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
    
  htim1.Instance -> CCR1 = (uint16_t)(motor_output[0] / ((max_pwm - min_pwm) / (max_duty - min_duty)) + 3696);
  htim1.Instance -> CCR2 = (uint16_t)(motor_output[1] / ((max_pwm - min_pwm) / (max_duty - min_duty)) + 3696);
  htim1.Instance -> CCR3 = (uint16_t)(motor_output[2] / ((max_pwm - min_pwm) / (max_duty - min_duty)) + 3696);
  htim1.Instance -> CCR4 = (uint16_t)(motor_output[3] / ((max_pwm - min_pwm) / (max_duty - min_duty)) + 3696);
  
}