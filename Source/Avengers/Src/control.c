#include "control.h"

float sbus_cmd[3];
float pid_output[4];
float k_roll[3], k_pitch[3], k_yaw[3];

void init_gain()
{
    k_roll[0] = 6;
    k_roll[2] = 1.5;
    k_pitch[0] = 6;
    k_pitch[2] = 1.5;
    k_yaw[0] = 2;
    k_yaw[2] = 4;
    //k_yaw[0] = 10;
}

void control_cmd()
{
   /* each converted sbus roll, pitch, yaw data */
   sbus_cmd[0] = ((sbus_data_buff[0] - 1024) / 672.0f) * 30.0f;    // max min +-30 deg
   sbus_cmd[1] = ((sbus_data_buff[1] - 1024) / 672.0f) * 30.0f;    // max min +-30 deg
   sbus_cmd[2] = ((sbus_data_buff[3] - 1024) / 672.0f) * 150.0f;   // max min +-150 deg
}

void controller()
{
  static float set_point[3] = {0.0, };
  static float error_roll[3] = {0.0, }, error_pitch[3] = {0.0, }, error_yaw[3] = {0.0, };
  float dt = 0.001;
  int i;

  /* pid controller */
  for(i = 0; i < 4; i++)
  {
     set_point[i] = sbus_cmd[i];
     if(i == 0)
     {
       error_roll[0] = set_point[i] - mti.euler[0];
       error_roll[1] = error_roll[1] + (error_roll[0] * dt);
       error_roll[2] = -mti.pqr[0];
       pid_output[i] = (k_roll[0] * error_roll[0]) + (k_roll[1] * error_roll[1]) + (k_roll[2] * error_roll[2]);
     }
     else if(i == 1)
     {
       error_pitch[0] = set_point[i] - mti.euler[1];
       error_pitch[1] = error_pitch[1] + (error_pitch[0] * dt);
       error_pitch[2] = -mti.pqr[1];
       pid_output[i] = (k_pitch[0] * error_pitch[0]) + (k_pitch[1] * error_pitch[1]) + (k_pitch[2] * error_pitch[2]);
     }
     else if(i == 2)
     {
       error_yaw[0] = set_point[i] - mti.pqr[2];
       error_yaw[1] = error_yaw[1] + (error_yaw[0] * dt);
       //error_yaw[2] = 0;
       error_yaw[2] = -mti.pqr[2];
       pid_output[i] = (k_yaw[0] * error_yaw[0]) + (k_yaw[1] * error_yaw[1]) + (k_yaw[2] * error_yaw[2]);
     }
     else
     {
       pid_output[i] = sbus_data_buff[2];
     }
  }
}


void pid()
{
  static float set_point[3] = {0.0, };
  static float error_roll[3] = {0.0, }, error_pitch[3] = {0.0, }, error_yaw[3] = {0.0, };
  float dt = 0.001;
  int i;

  /* pid controller */
  for(i = 0; i < 4; i++)
  {
     set_point[i] = sbus_cmd[i];
     if(i == 0)
     {
       error_roll[0] = set_point[i] - mti.euler[0];
       error_roll[1] = error_roll[1] + (error_roll[0] * dt);
       error_roll[2] = -mti.pqr[0];
       pid_output[i] = (k_roll[0] * error_roll[0]) + (k_roll[1] * error_roll[1]) + (k_roll[2] * error_roll[2]);
     }
     else if(i == 1)
     {
       error_pitch[0] = set_point[i] - mti.euler[1];
       error_pitch[1] = error_pitch[1] + (error_pitch[0] * dt);
       error_pitch[2] = -mti.pqr[1];
       pid_output[i] = (k_pitch[0] * error_pitch[0]) + (k_pitch[1] * error_pitch[1]) + (k_pitch[2] * error_pitch[2]);
     }
     else if(i == 2)
     {
       error_yaw[0] = set_point[i] - mti.pqr[2];
       error_yaw[1] = error_yaw[1] + (error_yaw[0] * dt);
       error_yaw[2] = 0;
       pid_output[i] = (k_yaw[0] * error_yaw[0]) + (k_yaw[1] * error_yaw[1]) + (k_yaw[2] * error_yaw[2]);
     }
     else
     {
       pid_output[i] = sbus_data_buff[2];
     }
  }
}

