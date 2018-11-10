#ifndef USER_MATH_H
#define USER_MATH_H

/* vector plus vector */
void vec_2_plus_vec_2(float vec1[][1], float vec2[][1], float out[][1]);

/* vector substract vector */
void vec_1_substract_vec_1(float vec1[][1], float vec2[][1], float out[][1]);

/* matrix multiply vector */
void mat_2x1_multiply_vec_1(float A[][1], float vec[][1], float out[][1]);
void mat_2x2_multiply_vec_2(float A[][2], float vec[][1], float out[][1]);
void mat_1x2_multiply_vec_2(float A[][2], float vec[][1], float out[][1]);

/* matrix plus matrix */
void mat_2x2_plus_mat_2x2(float A[][2], float B[][2], float out[][2]);
void mat_1x1_plus_mat_1x1(float A[][1], float B[][1], float out[][1]);

/* matrix substract matrix */
void mat_2x2_substract_mat_2x2(float A[][2], float B[][2], float out[][2]);

/* matrix multiply matrix */
void mat_2x2_multiply_mat_2x2(float A[][2], float B[][2], float out[][2]);
void mat_2x2_multiply_mat_2x1(float A[][2], float B[][1], float out[][1]);
void mat_1x2_multiply_mat_2x2(float A[][2], float B[][2], float out[][2]);
void mat_1x2_multiply_mat_2x1(float A[][2], float B[][1], float out[][1]);
void mat_2x1_multiply_mat_1x1(float A[][1], float B[][1], float out[][1]);
void mat_2x1_multiply_mat_1x2(float A[][1], float B[][2], float out[][2]);

/* matrix */
void mat_2x2_transpose(float A[][2], float out[][2]);
void mat_1x1_inverse(float A[][1], float out[][1]);

#endif