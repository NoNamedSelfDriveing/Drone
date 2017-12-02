#ifndef CONTROL_H
#define CONTROL_H

#include "stm32f4xx_hal.h"
#include "mti.h"
#include "sbus.h"

void init_gain();
void control_cmd();
void controller();
void pid();

extern float pid_output[4];
#endif