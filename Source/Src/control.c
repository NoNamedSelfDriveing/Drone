#include "control.h"

float sbus_cmd[3];
float pid_output[4];
float gain_roll[3], gain_pitch[3], gain_yaw[3];

/* make command from sbus receiver about each axis*/
void control_cmd()
{
   /* each converted sbus roll, pitch, yaw data */
   sbus_cmd[ROLL] = ((sbus_data_buff[CH1] - 1024) / 672.0f) * 30.0f;    // max min +-30 deg
   sbus_cmd[PITCH] = ((sbus_data_buff[CH2] - 1024) / 672.0f) * 30.0f;    // max min +-30 deg
   sbus_cmd[YAW] = ((sbus_data_buff[CH4] - 1024) / 672.0f) * 150.0f;   // max min +-150 deg
}

/* make control output by using PID controller */
void controller()
{
  static float set_point[3] = {0.0f, };
  static float error_roll[3] = {0.0f, }, error_pitch[3] = {0.0, }, error_yaw[3] = {0.0, };
  float dt = 0.001;
  int i;

  /* if throttle is very low, Integral term must be 0 for safety */
  if(sbus_data_buff[CH3] <= 360)
  {
     gain_roll[I] = 0.0f;
     gain_pitch[I] = 0.0f;
     gain_yaw[I] = 0.0f;
  }
  
  /* PID Controller */
  for(i = 0; i < 3; i++)
  {
     set_point[i] = sbus_cmd[i];
     
     /* Roll Axis PID Controller */
     if(i == 0)
     {
       error_roll[P] = set_point[i] - mti.euler[0];
       error_roll[I] = error_roll[1] + (error_roll[P] * dt);
       error_roll[D] = -mti.pqr[0];
       pid_output[i] = (gain_roll[P] * error_roll[P]) + (gain_roll[I] * error_roll[I]) + (gain_roll[D] * error_roll[D]);
     }
     /* Pitch Axis PID Controller */
     else if(i == 1)
     {
       error_pitch[P] = set_point[i] - mti.euler[1];
       error_pitch[I] = error_pitch[1] + (error_pitch[P] * dt);
       error_pitch[D] = -mti.pqr[1];
       pid_output[i] = (gain_pitch[P] * error_pitch[P]) + (gain_pitch[I] * error_pitch[I]) + (gain_pitch[D] * error_pitch[D]);
     }
     /* Yaw Axis PID Controller */
     else if(i == 2)
     {
       error_yaw[P] = set_point[i] - mti.pqr[2];
       error_yaw[I] = error_yaw[1] + (error_yaw[P] * dt);
       error_yaw[D] = -mti.pqr[2];
       pid_output[i] = (gain_yaw[P] * error_yaw[P]) + (gain_yaw[I] * error_yaw[I]) + (gain_yaw[D] * error_yaw[D]);
     }
  }
  /* Just Throttle Value(not using PID Controller) */
  pid_output[3] = sbus_data_buff[CH3];
}