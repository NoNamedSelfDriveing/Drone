#ifndef MIXER_H
#define MIXER_H

#include "stm32f4xx_hal.h"


#define    M1    0
#define    M2    1
#define    M3    2
#define    M4    3

#define    MIN_PULSE         4598.0f
#define    MAX_PULSE         8126.0f
#define    MIN_PID_OUTPUT    352.0f
#define    MAX_PID_OUTPUT    1696.0f

#define    GAIN              ((MAX_PULSE - MIN_PULSE) / (MAX_PID_OUTPUT - MIN_PID_OUTPUT))
#define    OFFSET            3696

void atti_mixer();
void altitude_mixer();

#endif