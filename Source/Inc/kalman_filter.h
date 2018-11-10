#ifndef KALMAN_FILTER_H
#define KALMAN_FILTER_H

#define dt 0.01f

typedef struct _ALT_KF{
  float x[2];
  float p[2][2];
  
  float u;
  float z;
  
  float A[2][2];
  float B[2][1];
  
  float H[2];
  float K[2];
  
  float Q[2][2];
  float R;
  
  float At[2][2];
  float Ht[2];
}ALT_KF;

typedef struct _ALT_KF_TEMP{
  float Ax[2][1];
  float Bu[2][1];
  
  float AP[2][2];
  float APAt[2][2];
  
  float PHt[2][1];
  float HP[1][2];
  float HPHt[1][1];
  float HPHtPlusR[1][1];
  float InvHPHtPlusR[1][1];
  
  float Hx[1][1];
  float ZSubHx[1][1];
  float KMulZSubHx[2][1];
  
  float KH[2][2];
  float KHP[2][2];
}ALT_KF_TEMP;

void init_alt_kf();
void alt_kf_predict(float acc_z);
void alt_kf_update(float hgt);

extern ALT_KF alt_kf;
extern ALT_KF_TEMP alt_kf_temp;

#endif