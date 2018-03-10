#ifndef CONTROL_H
#define CONTROL_H

#include "stm32f4xx_hal.h"
#include "mti.h"
#include "sbus.h"

#define P 0
#define I 1
#define D 2

#define ROLL  0
#define PITCH 1
#define YAW   2

void init_gain();
void control_cmd();
void controller();
void pid();

extern float pid_output[4];
extern float gain_roll[3], gain_pitch[3], gain_yaw[3];
#endif