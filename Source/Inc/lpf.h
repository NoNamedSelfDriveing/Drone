#ifndef LPF_H
#define LPF_H

#define PI 3.14159265359f

typedef struct lpf{
  float fc;
  float x;
  float filtered_x;
  float prev_filtered_x;
  float lambda;
  float dt;
  float alpha;
}LPF;

void init_lpf(LPF *target, float alpha);
float do_lpf(LPF *target, float x);

#endif