#ifndef CONTROL_H
#define CONTROL_H

#include "stm32f4xx_hal.h"

#define dT 0.01f
#define ALT_SET_POINT 0.0f    //5 meter from ground

#define P 0
#define I 1
#define D 2

#define ROLL  0
#define PITCH 1
#define YAW   2
#define ALT   3

typedef struct _attitude_pid
{
  float set_point[3];
  float error[3];
  float gain_roll[3];
  float gain_pitch[3];
  float gain_yaw[3];
  float term_roll[3];
  float term_pitch[3];
  float term_yaw[3];
  float saved_gain_I[3];
  float output[3];
}ATTITUDE_PID;

typedef struct _altitude_pid
{
  float set_point;
  float input;
  float error;
  float output;
  float gain[3];
  float term[3];
}ALTITUDE_PID;

void init_alt_pid_gain();
void make_atti_control_cmd();
void make_alt_control_cmd();
void atti_controller();
void alt_controller();

extern ATTITUDE_PID atti_pid;
extern ALTITUDE_PID alt_pid;
extern uint16_t last_throttle;
extern uint8_t mode_change_flag;

#endif