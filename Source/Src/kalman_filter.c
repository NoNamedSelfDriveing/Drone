#include "kalman_filter.h"
#include "user_math.h"

ALT_KF alt_kf;
ALT_KF_TEMP alt_kf_temp;

/* Initialize KF arguments */
void init_alt_kf()
{
  /* init x(state variables) */
  // [ h ]
  // [ v ]
  alt_kf.x[0] = 0.0f;
  alt_kf.x[1] = 0.0f;

  /* init P(state variables covariance) */
  alt_kf.p[0][0] = 2.0f;
  alt_kf.p[0][1] = 0.0f;
  alt_kf.p[1][0] = 0.0f;
  alt_kf.p[1][1] = 2.0f;

  /* init Q */
  alt_kf.Q[0][0] = 1.0f;
  alt_kf.Q[0][1] = 0.0f;
  alt_kf.Q[1][0] = 0.0f;
  alt_kf.Q[1][1] = 1.0f;
  
  /* init R */
  alt_kf.R = 3.0f;
  
  /* init A */
  // [1 dt]
  // [0 1]
  alt_kf.A[0][0] = 1.0f;
  alt_kf.A[0][1] = dt;
  alt_kf.A[1][0] = 0.0f;
  alt_kf.A[1][1] = 1.0f;

  /* init Transpose A */
  alt_kf.At[0][0] = 1.0f;
  alt_kf.At[0][1] = 0.0f;
  alt_kf.At[1][0] = dt;
  alt_kf.At[1][1] = 1.0f;
  
  /* init B */
  // [1/2*dt^2]
  // [   dt   ]  
  alt_kf.B[0][0] = 0.5*dt*dt;
  alt_kf.B[1][0] = dt;

  /* init H */
  // [1 0]  
  alt_kf.H[0] = 1.0f;
  alt_kf.H[1] = 0.0f;
  
  /* init Transpose H */
  alt_kf.Ht[0] = 0.0f;
  alt_kf.Ht[1] = 1.0f;
}

/* Predict state variables */
//void alt_kf_predict(float acc_z)
//{
//  alt_kf.u[0][0] = acc_z;
//  
//  /* x = Ax + Bu */
//  // A * x
//  mat_2x2_multiply_vec_2(alt_kf.A, alt_kf.x, alt_kf_temp.Ax);
//  
//  // B * u
//  mat_2x1_multiply_vec_1(alt_kf.B, alt_kf.u, alt_kf_temp.Bu);
//  
//  // x = Ax + Bu
//  vec_2_plus_vec_2(alt_kf_temp.Ax, alt_kf_temp.Bu, alt_kf.x);
//  
//  
//  /* P = APA' + Q */
//  // A * P
//  mat_2x2_multiply_mat_2x2(alt_kf.A, alt_kf.p, alt_kf_temp.AP);
//  
//  // AP * A'
//  mat_2x2_multiply_mat_2x2(alt_kf_temp.AP, alt_kf.At, alt_kf_temp.APAt);
//  
//  // P = APA' + Q
//  mat_2x2_plus_mat_2x2(alt_kf_temp.APAt, alt_kf.Q, alt_kf.p);
//}

/* Update state variables */
//void alt_kf_update(float alt)
//{
//  alt_kf.z[0][0] = alt;
//  
//  /* K = PH' * inv(HPH' + R)*/
//  // P * H'
//  mat_2x2_multiply_mat_2x1(alt_kf.p, alt_kf.Ht, alt_kf_temp.PHt);
//  
//  // H * P
//  mat_1x2_multiply_mat_2x2(alt_kf.H, alt_kf.p, alt_kf_temp.HP);
//  
//  // HP * H'
//  mat_1x2_multiply_mat_2x1(alt_kf_temp.HP, alt_kf.Ht, alt_kf_temp.HPHt);
//  
//  // HPH' + R
//  mat_1x1_plus_mat_1x1(alt_kf_temp.HPHt, alt_kf.R, alt_kf_temp.HPHtPlusR);
//  
//  // inv(HPH' + R)
//  mat_1x1_inverse(alt_kf_temp.HPHtPlusR, alt_kf_temp.InvHPHtPlusR);
//  
//  // K = PH' * inv(HPH' + R)
//  mat_2x1_multiply_mat_1x1(alt_kf_temp.PHt, alt_kf_temp.InvHPHtPlusR, alt_kf.K);
//  
//  
//  /* x = x + K(z - H*x) */
//  // H * x
//  mat_2x2_multiply_vec_2(alt_kf.H, alt_kf.x, alt_kf_temp.Hx);
//  
//  // z - Hx
//  vec_1_substract_vec_1(alt_kf.z, alt_kf_temp.Hx, alt_kf_temp.ZSubHx);
//  
//  // K(z -Hx)
//  mat_2x1_multiply_vec_1(alt_kf.K, alt_kf_temp.ZSubHx, alt_kf_temp.KMulZSubHx);
//  
//  // x + K(z - Hx)
//  vec_2_plus_vec_2(alt_kf.x, alt_kf_temp.KMulZSubHx, alt_kf.x);
//  
//  
//  /* P = P - KHP */
//  // K * H
//  mat_2x1_multiply_mat_1x2(alt_kf.K, alt_kf.H, alt_kf_temp.KH);
//  
//  // KH * P
//  mat_2x2_multiply_mat_2x2(alt_kf_temp.KH, alt_kf.p, alt_kf_temp.KHP);
//  
//  // P = P - KHP
//  mat_2x2_substract_mat_2x2(alt_kf.p, alt_kf_temp.KHP, alt_kf.p);
//}

void alt_kf_predict(float acc_z)
{
  alt_kf.u = acc_z;
  
  // x_k' = Ax_k-1 + Bu_k
  // [ h_k' ] = [ 1 dt ] * [ h_k-1 ] + [ 0.5dt^2 ] * u_k
  // [ v_k' ]   [ 0 1  ]   [ v_k-1 ]   [   dt    ]
  //
  // [ h_k' ] = [ h_k-1 + dt*v_k-1 + 0.5*dt*dt * u_k ]
  // [ v_k' ] = [          v_k-1 + dt*u_k                ] 
  alt_kf.x[0] = alt_kf.x[0] + dt*alt_kf.x[1] + 0.5*dt*dt*alt_kf.u;
  alt_kf.x[1] = alt_kf.x[1] + dt*alt_kf.u;
  
  // P_k' = AP_k-1At + Q
  // AP_k-1 = [ 1 dt ] * [ P_00 P_01 ] = [ P_00 + dt*P_10   P_01 + dt*P_11 ]
  //          [ 0 1  ]   [ P_10 P_11 ]   [     P_10               P_11     ]
  //
  // AP_k-1At = AP_k-1 * At = [ P_00 + dt*P_10   P_01 + dt*P_11 ] * [ 1 0  ] = [ (P_00 + dt*P_10) + dt*(P_01 + dt*P_11)   P_01 + dt*P_11 ] = [ P_00 + dt*(P_10 + P[0][1] + dt*P[1][1])   P_01 + dt*P_11 ]
  //                          [     P_10               P_11     ]   [ dt 1 ]   [          P_10 + dt*P_11                      P_11       ]   [               P_10 + dt*P_11                   P_11      ]
  //
  // P_k' = AP_k-1At + Q = [ P_00 + dt*(P_10 + P[0][1] + dt*P[1][1])   P_01 + dt*P_11 ] + [ Q_00 Q_01 ] = [ P_00 + dt*(P_10 + P[0][1] + dt*P[1][1]) + Q_00  P_01 + dt*P_11 + Q_01 ]
  //                       [               P_10 + dt*P_11                   P_11      ]   { Q_10 Q_11 ]   [            P_10 + dt*P_11 + Q_10                    P_11 + Q_11       ]
  alt_kf.p[0][0] = alt_kf.p[0][0] + dt*(alt_kf.p[1][0] + alt_kf.p[0][1] + dt*alt_kf.p[1][1]) + alt_kf.Q[0][0];
  alt_kf.p[0][1] = alt_kf.p[0][1] + dt*alt_kf.p[1][1] + alt_kf.Q[0][1];
  alt_kf.p[1][0] = alt_kf.p[1][0] + dt*alt_kf.p[1][1] + alt_kf.Q[1][0];
  alt_kf.p[1][1] = alt_kf.p[1][1] + alt_kf.Q[1][1];
}

void alt_kf_update(float hgt)
{
  alt_kf.z = hgt;
  
  // K_k = P_k'Ht * inv(HP_k'Ht + R)
  // P_k'Ht = [ P_00 P_01 ] * [ 1 ] = [ P_00 ]
  //          [ P_10 P_11 ]   [ 0 ]   [ P_10 ]
  //
  // HP_k' = [ 1 0 ] * [ P_00 P_01 ] = [ P_00 P_01 ] 
  //                   [ P_10 P_11 ]
  //
  // HP_K'Ht = HP_K' * Ht = [ P_00 P_01 ] * [ 1 ] = [ P_00 ]
  //                                        [ 0 ]
  // 
  // inv(HP_K'Ht + R) = [ 1 / (P_00 + R) ]
  // K_k = P_K'Ht * inv(HP_k'Ht + R) = [ P_00 ] * [ 1/(P_00 + R)] = [ P_00 * (1 / (P_00 + R)) ] = [ P_00 / (P_00 + R) ]
  //                                   [ P_10 ]                     [ P_10 * (1 / (P_00 + R)) ]   [ P_10 / (P_00 + R) ] 
  alt_kf.K[0] = alt_kf.p[0][0] / (alt_kf.p[0][0] + alt_kf.R);
  alt_kf.K[1] = alt_kf.p[1][0] / (alt_kf.p[0][0] + alt_kf.R);
  
  // x_k = x_k' + K_k(z_k - Hx_k')
  // K_k(z_k - Hx_k') = [ K_0 ] * (z - [ 1 0 ] * [ h_k' ]) = [ K_0 ] * (z - h_k') = [ K_0 * (z - h_k') ]
  //                    [ K_1 ]   (              [ v_k' ])   [ K_1 ]                [ K_1 * (z - h_k') ] 
  //
  // x_k = x_k' + K_k(z_k - Hx_k') = [ h_k' ] + [ K_0 * (z - h_k') ] = [ h_k' + K_0 * (z - h_k') ] 
  //                                 [ v_k' ]   [ K_1 * (z - h_k') ]   [ v_k' + K_1 * (z - h_k') ] 
  alt_kf.x[0] = alt_kf.x[0] + alt_kf.K[0] * (alt_kf.z - alt_kf.x[0]);
  alt_kf.x[1] = alt_kf.x[1] + alt_kf.K[1] * (alt_kf.z - alt_kf.x[0]);
  
  // P_k = (I - K_kH)P_k'
  // (I - K_k * H) = [ 1 0 ] - [ K_0 ] * [ 1 0 ] = [ 1-K_0  0 ]
  //                 [ 0 1 ]   [ K_1 ]             [ -K_1   1 ]
  //
  // P_k = (I - K_kH)P_k' = [ 1-K_0  0 ] * [ P_00 P_01 ] = [ (1-K_0) * P_00        (1-K_0) * P_01       ]
  //                        [ -K_1   1 ]   [ P_10 P_11 ]   [ (-K_1) * P_00 + P_10  (-K_1) * P_01 + P_11 ]
  alt_kf.p[0][0] = alt_kf.p[0][0] - alt_kf.K[0]*alt_kf.p[0][0];
  alt_kf.p[0][1] = alt_kf.p[0][1] - alt_kf.K[0]*alt_kf.p[0][1];
  alt_kf.p[1][0] = alt_kf.p[1][0] - alt_kf.K[1]*alt_kf.p[0][0];
  alt_kf.p[1][1] = alt_kf.p[1][1] - alt_kf.K[1]*alt_kf.p[0][1];
}