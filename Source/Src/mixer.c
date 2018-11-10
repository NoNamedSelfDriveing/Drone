#include "mixer.h"
#include "control.h"
#include "sbus.h"
#include "tim.h"

int16_t motor_output[4];

/* Control motor outputs along motors' position */
//
//void mixer()
//{
//  const float min_pulse = 4598.0;
//  const float max_pulse = 8126.0;
//  const float max_pid_output = 1696.0;
//  const float min_pid_output = 352.0;
//  static uint16_t prev_arm_data = 352, curr_arm_data = 352;
//  static uint8_t arming_flag = 0;
//  int i;
//  
//  /* if throttle is very low, it must be stop*/
//  if(pid_output[3]<=360)
//  {
//    for(i = 0 ; i < 4; i++) 
//      motor_output[i] = 352;
//  }
//  /* sum and subtract PID outputs along motors' position*/
//  else
//  {
//    motor_output[0] = (int16_t)(-pid_output[0] + pid_output[1] + pid_output[2] + pid_output[3]);
//    motor_output[1] = (int16_t)(pid_output[0] + pid_output[1] - pid_output[2] + pid_output[3]);
//    motor_output[2] = (int16_t)(pid_output[0] - pid_output[1] + pid_output[2] + pid_output[3]);
//    motor_output[3] = (int16_t)(-pid_output[0] - pid_output[1] - pid_output[2] + pid_output[3]);
//  }
//  
//  //printf("%d %d %d %d\n\r", motor_output[0], motor_output[1], motor_output[2], motor_output[3]);
//  
//  for(i = 0; i < 4; i++)
//  {
//    if(motor_output[i] >= 1696)
//    {
//      motor_output[i] = 1696;
//    }
//    else if(motor_output[i] <= 352)
//    {
//      motor_output[i] = 352;
//    }
//  }
//  
//  //printf("%d %d %d %d\n\r", motor_output[0], motor_output[1], motor_output[2], motor_output[3]);
//  
//  curr_arm_data = sbus_data_buff[CH12];
//  
//  /* if CH12 Switch went up and down, turn on arming or turn off arming */
//  if((prev_arm_data == 352) && (curr_arm_data == 1696))
//  {
//    /* turn off motor */
//    if(arming_flag == 1)
//    {
//      arming_flag = 0;
//    }
//    /* turn on motor */
//    else
//    {
//      arming_flag = 1;
//    }
//  }
//  
//  /* if it is not armed now, no motor output for safety */
//  if(arming_flag == 0)
//  {
//    for(i = 0; i < 4; i++)
//      motor_output[i] = 352;
//  }
// 
//  /* convert output value to 'pulse' value to change real PWM duty cycle */
//  htim1.Instance -> CCR1 = (uint16_t)(motor_output[0] / ((max_pid_output - min_pid_output) / (max_pulse - min_pulse)) + 3696);
//  htim1.Instance -> CCR2 = (uint16_t)(motor_output[1] / ((max_pid_output - min_pid_output) / (max_pulse - min_pulse)) + 3696);
//  htim1.Instance -> CCR3 = (uint16_t)(motor_output[2] / ((max_pid_output - min_pid_output) / (max_pulse - min_pulse)) + 3696);
//  htim1.Instance -> CCR4 = (uint16_t)(motor_output[3] / ((max_pid_output - min_pid_output) / (max_pulse - min_pulse)) + 3696);
//  
//  prev_arm_data = curr_arm_data;
//}

/* Control motor outputs along motors' position */
//void mixer()
//{
//  //const float min_pulse = 4598.0;
//  //const float max_pulse = 8126.0;
//  //const float max_pid_output = 1696.0;
//  //const float min_pid_output = 352.0;
//  static uint16_t prev_arm_data = 352, curr_arm_data = 352;
//  static uint8_t arming_flag = 0;
//  uint16_t throttle;
//  int idx;
//  
//  throttle = sbus_data_buff[CH3];
//  
//  /* if throttle is very low, it must be stop*/
//  if(throttle <= 360)
//  {
//    for(idx = 0; idx <= 3; idx++) 
//      motor_output[idx] = 352;
//  }
//  /* sum and subtract PID outputs along motors' position*/
//  else
//  {
//      motor_output[M1] = (int16_t)(-atti_pid.output[ROLL] + atti_pid.output[PITCH] + atti_pid.output[YAW] + alt_pid.output) + throttle;
//      motor_output[M2] = (int16_t)(atti_pid.output[ROLL] + atti_pid.output[PITCH] - atti_pid.output[YAW] + alt_pid.output) + throttle;
//      motor_output[M3] = (int16_t)(atti_pid.output[ROLL] - atti_pid.output[PITCH] + atti_pid.output[YAW] + alt_pid.output) + throttle;
//      motor_output[M4] = (int16_t)(-atti_pid.output[ROLL] - atti_pid.output[PITCH] - atti_pid.output[YAW] + alt_pid.output) + throttle;
////    motor_output[0] = (int16_t)(-pid_output[0] + pid_output[1] + pid_output[2] + pid_output[3] + alt_pid.output);
////    motor_output[1] = (int16_t)(pid_output[0] + pid_output[1] - pid_output[2] + pid_output[3] + alt_pid.output);
////    motor_output[2] = (int16_t)(pid_output[0] - pid_output[1] + pid_output[2] + pid_output[3] + alt_pid.output);
////    motor_output[3] = (int16_t)(-pid_output[0] - pid_output[1] - pid_output[2] + pid_output[3] + alt_pid.output);
//  }
//  
//  //printf("%d %d %d %d\n\r", motor_output[0], motor_output[1], motor_output[2], motor_output[3]);
//
//  /* Prevent so big/small motor output values */
//  for(idx = 0; idx <= 3; idx++)
//  {
//    if(motor_output[idx] >= 1696)
//      motor_output[idx] = 1696;
//    else if(motor_output[idx] <= 352)
//      motor_output[idx] = 352;
//  }
//  
//  //printf("%d %d %d %d\n\r", motor_output[0], motor_output[1], motor_output[2], motor_output[3]);
//  
//  curr_arm_data = sbus_data_buff[CH12];
//  
//  /* if CH12 Switch went up and down, turn on arming or turn off arming */
//  if((prev_arm_data == 352) && (curr_arm_data == 1696))
//  {
//    /* turn off motor */
//    if(arming_flag == 1)
//    {
//      arming_flag = 0;
//    }
//    /* turn on motor */
//    else
//    {
//      arming_flag = 1;
//    }
//  }
//  
//  /* if it is not armed now, no motor output for safety */
//  if(arming_flag == 0)
//  {
//    for(idx = 0; idx <= 3; idx++)
//      motor_output[idx] = 352;
//  }
// 
//  /* convert output value to 'pulse' value to change real PWM duty cycle */
//  htim1.Instance->CCR1 = (uint16_t)(GAIN * motor_output[M1] + OFFSET);
//  htim1.Instance->CCR2 = (uint16_t)(GAIN * motor_output[M2] + OFFSET);
//  htim1.Instance->CCR3 = (uint16_t)(GAIN * motor_output[M3] + OFFSET);
//  htim1.Instance->CCR4 = (uint16_t)(GAIN * motor_output[M4] + OFFSET);
//  
//  //htim1.Instance -> CCR1 = (uint16_t)(motor_output[0] / ((max_pid_output - min_pid_output) / (max_pulse - min_pulse)) + 3696);
////  htim1.Instance -> CCR2 = (uint16_t)(motor_output[1] / ((max_pid_output - min_pid_output) / (max_pulse - min_pulse)) + 3696);
////  htim1.Instance -> CCR3 = (uint16_t)(motor_output[2] / ((max_pid_output - min_pid_output) / (max_pulse - min_pulse)) + 3696);
////  htim1.Instance -> CCR4 = (uint16_t)(motor_output[3] / ((max_pid_output - min_pid_output) / (max_pulse - min_pulse)) + 3696);
//  
//  prev_arm_data = curr_arm_data;
//}

/* Control motor outputs along motors' position */
void attitude_mixer()
{
  static uint16_t prev_arm_data = 352, curr_arm_data = 352;
  static uint8_t arming_flag = 0;
  int idx;
  
  /* if throttle is very low, it must be stop*/
  if(sbus.throttle <= 360)
  {
    for(idx = 0; idx <= 3; idx++) 
      motor_output[idx] = 352;
  }
  /* sum and subtract PID outputs along motors' position*/
  else
  {
    motor_output[M1] = (int16_t)(-atti_pid.output[ROLL] + atti_pid.output[PITCH] + atti_pid.output[YAW]) + sbus.throttle;
    motor_output[M2] = (int16_t)(atti_pid.output[ROLL] + atti_pid.output[PITCH] - atti_pid.output[YAW]) + sbus.throttle;
    motor_output[M3] = (int16_t)(atti_pid.output[ROLL] - atti_pid.output[PITCH] + atti_pid.output[YAW]) + sbus.throttle;
    motor_output[M4] = (int16_t)(-atti_pid.output[ROLL] - atti_pid.output[PITCH] - atti_pid.output[YAW]) + sbus.throttle;
  }
  
  /* Prevent so big/small motor output values */
  for(idx = 0; idx <= 3; idx++)
  {
    if(motor_output[idx] >= 1696)
      motor_output[idx] = 1696;
    else if(motor_output[idx] <= 352)
      motor_output[idx] = 352;
  }
  
  curr_arm_data = sbus_data_buff[CH12];
  
//  /* if CH12 Switch went up and down, turn on arming or turn off arming */
//  if((prev_arm_data == 352) && (curr_arm_data == 1696))
//  {
//    /* turn off motor */
//    if(arming_flag == 1)
//    {
//      arming_flag = 0;
//    }
//    /* turn on motor */
//    else
//    {
//      arming_flag = 1;
//    }
//  }
  
  if(sbus_data_buff[CH8] == 352)
    arming_flag = 1;
  else
    arming_flag = 0;
  
  /* if it is not armed now, no motor output for safety */
  if(arming_flag == 0)
  {
    for(idx = 0; idx <= 3; idx++)
      motor_output[idx] = 352;
  }
 
  //if(sbus_data_buff[CH7] == 1696)
  //{
    /* convert output value to 'pulse' value to change real PWM duty cycle */
    htim1.Instance->CCR1 = (uint16_t)(GAIN * motor_output[M1] + OFFSET);
    htim1.Instance->CCR2 = (uint16_t)(GAIN * motor_output[M2] + OFFSET);
    htim1.Instance->CCR3 = (uint16_t)(GAIN * motor_output[M3] + OFFSET);
    htim1.Instance->CCR4 = (uint16_t)(GAIN * motor_output[M4] + OFFSET);
  //}
  
  prev_arm_data = curr_arm_data;
}

void altitude_mixer()
{
  int idx;
  
  for(idx = 0; idx <= 3; idx++)
    motor_output[idx] += alt_pid.output;
  
  /* Prevent so big/small motor output values */
  for(idx = 0; idx <= 3; idx++)
  {
    if(motor_output[idx] >= 1696)
      motor_output[idx] = 1696;
    else if(motor_output[idx] <= 352)
      motor_output[idx] = 352;
  }

  /* convert output value to 'pulse' value to change real PWM duty cycle */
  htim1.Instance->CCR1 = (uint16_t)(GAIN * motor_output[M1] + OFFSET);
  htim1.Instance->CCR2 = (uint16_t)(GAIN * motor_output[M2] + OFFSET);
  htim1.Instance->CCR3 = (uint16_t)(GAIN * motor_output[M3] + OFFSET);
  htim1.Instance->CCR4 = (uint16_t)(GAIN * motor_output[M4] + OFFSET);
}