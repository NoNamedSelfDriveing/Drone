#include "control.h"
#include "ms5611.h"
#include "mti.h"
#include "sbus.h"
#include "kalman_filter.h"

ALTITUDE_PID alt_pid;
ATTITUDE_PID atti_pid;
uint16_t last_throttle;
uint8_t mode_change_flag;

void init_alt_pid_gain()
{
  alt_pid.gain[P] = 30.0f;
  alt_pid.gain[I] = 5.0f;
  alt_pid.gain[D] = 0.0f;
}

/* make attitude control command from sbus receiver about each axis*/
void make_atti_control_cmd()
{
  /* each converted sbus roll, pitch, yaw data */
  sbus.cmd[ROLL] = ((sbus_data_buff[CH1] - 1024) / 672.0f) * 30.0f;    // max min +-30 deg
  sbus.cmd[PITCH] = ((sbus_data_buff[CH2] - 1024) / 672.0f) * 30.0f;    // max min +-30 deg
  sbus.cmd[YAW] = ((sbus_data_buff[CH4] - 1024) / 672.0f) * 150.0f;   // max min +-150 deg
}

/* make altitude control command from sbus receiver */
void make_alt_control_cmd()
{
  sbus.cmd[ALT] = ((sbus_data_buff[CH3] - 1024) / 672.0f) * 3.0f;       // max min +-3m/s
}

/* PID controller for attitude */
void atti_controller()
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
void alt_controller()
{ 
  alt_pid.set_point = sbus.cmd[ALT];
  alt_pid.input = alt_kf.x[VELOCITY];
  
  alt_pid.error = alt_pid.set_point - alt_pid.input;
  
  alt_pid.term[P] = alt_pid.gain[P] * alt_pid.error;
  alt_pid.term[I] += alt_pid.error * dT;
  alt_pid.term[D] = alt_pid.gain[D] * (-alt_pid.input / dT);
  
  alt_pid.output = alt_pid.term[P] + (alt_pid.gain[I] * alt_pid.term[I]) + alt_pid.term[D];
  
  printf("%.2f  %.4f  %.4f\n\r", alt_pid.output, alt_pid.set_point, alt_pid.input);
}