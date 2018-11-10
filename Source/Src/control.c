#include "control.h"
#include "ms5611.h"
#include "mti.h"
#include "sbus.h"

ALTITUDE_PID alt_pid;
ATTITUDE_PID atti_pid;
uint16_t last_throttle;
uint8_t mode_change_flag;

void init_altitude_pid_gain()
{
  alt_pid.gain[P] = 30.0f;
  alt_pid.gain[I] = 15.0f;
  alt_pid.gain[D] = 0.4f;
}

/* make attitude control command from sbus receiver about each axis*/
void make_attitude_control_cmd()
{
  /* each converted sbus roll, pitch, yaw data */
  sbus.cmd[ROLL] = ((sbus_data_buff[CH1] - 1024) / 672.0f) * 30.0f;    // max min +-30 deg
  sbus.cmd[PITCH] = ((sbus_data_buff[CH2] - 1024) / 672.0f) * 30.0f;    // max min +-30 deg
  sbus.cmd[YAW] = ((sbus_data_buff[CH4] - 1024) / 672.0f) * 150.0f;   // max min +-150 deg
}

/* make control output by using PID controller */
//void controller()
//{
//  static uint8_t start_flag = 1;
//  static float set_point[3] = {0.0f, };
//  static float error_roll[3] = {0.0f, }, error_pitch[3] = {0.0, }, error_yaw[3] = {0.0, };
//  static float saved_gain_I[3] = {0.0f, };
// 
//  float dt = 0.01;
//  int axis;
//  
//  /* save initial gains of I term */
//  if(start_flag)
//  {
//    saved_gain_I[ROLL] = gain_roll[I];
//    saved_gain_I[PITCH] = gain_pitch[I];
//    saved_gain_I[YAW] = gain_yaw[I];
//    
//    start_flag = 0;
//  }
//
//  /* if throttle is very low, Integral term must be 0 for safety */
//  if(sbus_data_buff[CH3] <= 360)
//  {
//     gain_roll[I] = 0.0f;
//     gain_pitch[I] = 0.0f;
//     gain_yaw[I] = 0.0f;
//  }
//  else
//  {
//     gain_roll[I] = saved_gain_I[ROLL];
//     gain_pitch[I] = saved_gain_I[PITCH];
//     gain_yaw[I] = saved_gain_I[YAW];
//  }
//  
//  /* PID Controller */
//  for(axis = 0; axis < 3; axis++)
//  {
//     set_point[axis] = sbus_cmd[axis];
//     
//     /* Roll Axis PID Controller */
//     if(axis == ROLL)
//     {
//       error_roll[P] = set_point[axis] - mti.euler[ROLL];
//       error_roll[I] = error_roll[I] + (error_roll[P] * dt);
//       error_roll[D] = -mti.pqr[0];
//       pid_output[axis] = (gain_roll[P] * error_roll[P]) + (gain_roll[I] * error_roll[I]) + (gain_roll[D] * error_roll[D]);
//     }
//     /* Pitch Axis PID Controller */
//     else if(axis == PITCH)
//     {
//       error_pitch[P] = set_point[axis] - mti.euler[PITCH];
//       error_pitch[I] = error_pitch[I] + (error_pitch[P] * dt);
//       error_pitch[D] = -mti.pqr[1];
//       pid_output[axis] = (gain_pitch[P] * error_pitch[P]) + (gain_pitch[I] * error_pitch[I]) + (gain_pitch[D] * error_pitch[D]);
//     }
//     /* Yaw Axis PID Controller */
//     else if(axis == YAW)
//     {
//       error_yaw[P] = set_point[axis] - mti.pqr[YAW];
//       error_yaw[I] = error_yaw[I] + (error_yaw[P] * dt);
//       error_yaw[D] = -mti.pqr[2];
//       pid_output[axis] = (gain_yaw[P] * error_yaw[P]) + (gain_yaw[I] * error_yaw[I]) + (gain_yaw[D] * error_yaw[D]);
//     }
//  }
//  /* Just Throttle Value(not using PID Controller) */
//  pid_output[3] = sbus_data_buff[CH3];
//}

/* PID controller for attitude */
void attitude_controller()
{
  static uint8_t start_flag = 1;
  uint16_t throttle;
  
  throttle = sbus_data_buff[CH3];
  
  /* save initial gains of I term */
  if(start_flag)
  {
    atti_pid.saved_gain_I[ROLL] = atti_pid.gain_roll[I];
    atti_pid.saved_gain_I[PITCH] = atti_pid.gain_pitch[I];
    atti_pid.saved_gain_I[YAW] = atti_pid.gain_yaw[I];
    
    start_flag = 0;
  }

  /* if throttle is very low, Integral term must be 0 for safety */
  if(throttle <= 360)
  {
     atti_pid.gain_roll[I] = 0.0f;
     atti_pid.gain_pitch[I] = 0.0f;
     atti_pid.gain_yaw[I] = 0.0f;
  }
  else
  {
     atti_pid.gain_roll[I] = atti_pid.saved_gain_I[ROLL];
     atti_pid.gain_pitch[I] = atti_pid.saved_gain_I[PITCH];
     atti_pid.gain_yaw[I] = atti_pid.saved_gain_I[YAW];
  }
  
  /* setting set point of each axis */
  atti_pid.set_point[ROLL] = sbus.cmd[ROLL];
  atti_pid.set_point[PITCH] = sbus.cmd[PITCH];
  atti_pid.set_point[YAW] = sbus.cmd[YAW];
  
  
  /* PID Controller */
  /* Roll Axis PID Controller */
  atti_pid.error[ROLL] = atti_pid.set_point[ROLL] - mti.euler[ROLL];

  atti_pid.term_roll[P] = atti_pid.gain_roll[P] * atti_pid.error[ROLL];
  atti_pid.term_roll[I] += atti_pid.error[ROLL] * dT;
  atti_pid.term_roll[D] = atti_pid.gain_roll[D] * -mti.pqr[ROLL];

  atti_pid.output[ROLL] = atti_pid.term_roll[P] + (atti_pid.gain_roll[I] * atti_pid.term_roll[I]) + atti_pid.term_roll[D];
     
  /* Pitch Axis PID Controller */
  atti_pid.error[PITCH] = atti_pid.set_point[PITCH] - mti.euler[PITCH];

  atti_pid.term_pitch[P] = atti_pid.gain_pitch[P] * atti_pid.error[PITCH];
  atti_pid.term_pitch[I] += atti_pid.error[PITCH] * dT;
  atti_pid.term_pitch[D] = atti_pid.gain_pitch[D] * -mti.pqr[PITCH];

  atti_pid.output[PITCH] = atti_pid.term_pitch[P] + (atti_pid.gain_pitch[I] * atti_pid.term_pitch[I]) + atti_pid.term_pitch[D];
     
  /* Yaw Axis PID Controller */
  atti_pid.error[YAW] = atti_pid.set_point[YAW] - mti.pqr[YAW];

  atti_pid.term_yaw[P] = atti_pid.gain_yaw[P] * atti_pid.error[YAW];
  atti_pid.term_yaw[I] += atti_pid.error[YAW] * dT;
  atti_pid.term_yaw[D] = atti_pid.gain_yaw[D] * -mti.pqr[YAW];
   
  atti_pid.output[YAW] = atti_pid.term_yaw[P] + (atti_pid.gain_yaw[I] * atti_pid.term_yaw[I]) + atti_pid.term_yaw[D];
}

/* PID controller for altitude */
void altitude_controller()
{
  if(mode_change_flag)
  {
    last_throttle = sbus.throttle;
    mode_change_flag = 0;
  }
  
  /* Get difference of altitude for PID controller input data */
  ms5611.d_alt = (ms5611.now_alt - ms5611.prev_alt) / dT;
  
  alt_pid.set_point = ALT_SET_POINT;
  alt_pid.input = ms5611.d_alt;
  alt_pid.error = alt_pid.set_point - alt_pid.input;
  
  alt_pid.term[P] = alt_pid.gain[P] * alt_pid.error;
  alt_pid.term[I] += alt_pid.error * dT;
  alt_pid.term[D] = alt_pid.gain[D] * (-alt_pid.input / dT);
  
  alt_pid.output = alt_pid.term[P] + (alt_pid.gain[I] * alt_pid.term[I]) + alt_pid.term[D];
  
}