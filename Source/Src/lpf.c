#include "lpf.h"

void init_lpf(LPF *target, float alpha)
{
  //target->fc = fc;
  //target->dt = dt;
  //target->lambda = 2 * PI * target->fc * target->dt;
  target->x = 0.0f;
  //target->prev_filtered_x = 0.0f;
  target->filtered_x = 0.0f;
  //target->alpha = 1.0f / (1 + target->lambda);
  target->alpha = alpha;
}

float do_lpf(LPF *target, float x)
{
  target->filtered_x = (target->alpha * target->filtered_x) + \
                       ((1.0f - target->alpha) * x);
  
  return target->filtered_x;
}