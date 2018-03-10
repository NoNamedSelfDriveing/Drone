#include "control.h"

float sbus_cmd[3];
float pid_output[4];
float gain_roll[3], gain_pitch[3], gain_yaw[3];

void init_gain()
{
    gain_roll[0] = 6;
    gain_roll[2] = 1.5;
    gain_pitch[0] = 6;
    gain_pitch[2] = 1.5;
    gain_yaw[0] = 2;
    gain_yaw[2] = 4;
    //k_yaw[0] = 10;
}

void control_cmd()
{
   /* each converted sbus roll, pitch, yaw data */
   sbus_cmd[ROLL] = ((sbus_data_buff[CH1] - 1024) / 672.0f) * 30.0f;    // max min +-30 deg
   sbus_cmd[PITCH] = ((sbus_data_buff[CH2] - 1024) / 672.0f) * 30.0f;    // max min +-30 deg
   sbus_cmd[YAW] = ((sbus_data_buff[CH4] - 1024) / 672.0f) * 150.0f;   // max min +-150 deg
}

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
  
  /* pid controller */
  for(i = 0; i < 4; i++)
  {
     set_point[i] = sbus_cmd[i];
     
     if(i == 0)
     {
       error_roll[P] = set_point[i] - mti.euler[0];
       error_roll[I] = error_roll[1] + (error_roll[P] * dt);
       error_roll[D] = -mti.pqr[0];
       pid_output[i] = (gain_roll[P] * error_roll[P]) + (gain_roll[I] * error_roll[I]) + (gain_roll[D] * error_roll[D]);
     }
     else if(i == 1)
     {
       error_pitch[P] = set_point[i] - mti.euler[1];
       error_pitch[I] = error_pitch[1] + (error_pitch[P] * dt);
       error_pitch[D] = -mti.pqr[1];
       pid_output[i] = (gain_pitch[P] * error_pitch[P]) + (gain_pitch[I] * error_pitch[I]) + (gain_pitch[D] * error_pitch[D]);
     }
     else if(i == 2)
     {
       error_yaw[P] = set_point[i] - mti.pqr[2];
       error_yaw[I] = error_yaw[1] + (error_yaw[P] * dt);
       //error_yaw[2] = 0;
       error_yaw[D] = -mti.pqr[2];
       pid_output[i] = (gain_yaw[P] * error_yaw[P]) + (gain_yaw[I] * error_yaw[I]) + (gain_yaw[D] * error_yaw[D]);
     }
     else
     {
       pid_output[i] = sbus_data_buff[CH3];
     }
  }
}

//void pid()
//{
//  static float set_point[3] = {0.0, };
//  static float error_roll[3] = {0.0, }, error_pitch[3] = {0.0, }, error_yaw[3] = {0.0, };
//  float dt = 0.001;
//  int i;
//
//  /* pid controller */
//  for(i = 0; i < 4; i++)
//  {
//     set_point[i] = sbus_cmd[i];
//     if(i == 0)
//     {
//       error_roll[0] = set_point[i] - mti.euler[0];
//       error_roll[1] = error_roll[1] + (error_roll[0] * dt);
//       error_roll[2] = -mti.pqr[0];
//       pid_output[i] = (k_roll[0] * error_roll[0]) + (k_roll[1] * error_roll[1]) + (k_roll[2] * error_roll[2]);
//     }
//     else if(i == 1)
//     {
//       error_pitch[0] = set_point[i] - mti.euler[1];
//       error_pitch[1] = error_pitch[1] + (error_pitch[0] * dt);
//       error_pitch[2] = -mti.pqr[1];
//       pid_output[i] = (k_pitch[0] * error_pitch[0]) + (k_pitch[1] * error_pitch[1]) + (k_pitch[2] * error_pitch[2]);
//     }
//     else if(i == 2)
//     {
//       error_yaw[0] = set_point[i] - mti.pqr[2];
//       error_yaw[1] = error_yaw[1] + (error_yaw[0] * dt);
//       error_yaw[2] = 0;
//       pid_output[i] = (k_yaw[0] * error_yaw[0]) + (k_yaw[1] * error_yaw[1]) + (k_yaw[2] * error_yaw[2]);
//     }
//     else
//     {
//       pid_output[i] = sbus_data_buff[2];
//     }
//  }
//}

