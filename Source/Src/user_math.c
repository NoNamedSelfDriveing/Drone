#include "user_math.h"
#include "stm32f4xx_hal.h"

/* vector plus vector */
/* Begin */
void vec_2_plus_vec_2(float vec1[][1], float vec2[][1], float out[][1])
{
  uint8_t row;
  
  for(row = 0; row < 2; row++)
    out[row][0] = vec1[row][0] + vec2[row][0];
}
/* End */


/* vector substract vector */
/* Begin */
void vec_1_substract_vec_1(float vec1[][1], float vec2[][1], float out[][1])
{
  out[0][0] = vec1[0][0] - vec2[0][0];
}
/* End */


/* matrix multiply vector */
/* Begin */
void mat_2x1_multiply_vec_1(float A[][1], float vec[][1], float out[][1])
{
  uint8_t row;
  
  for(row = 0; row < 2; row++)
  {
    out[row][0] = A[row][0] * vec[0][0];
  }
}

void mat_2x2_multiply_vec_2(float A[][2], float vec[][1], float out[][1])
{
  uint8_t row;
  uint8_t i;
  
  for(row = 0; row < 2; row++)
  {
    for(i = 0; i < 2; i++)
      out[row][0] += A[row][i] * vec[i][0];
  }
}

void mat_1x2_multiply_vec_2(float A[][2], float vec[][1], float out[][1])
{
  uint8_t i;
  
  for(i = 0; i < 2; i++)
    out[0][0] += A[0][i] * vec[i][0];
}
/* End */


/* matrix plus matrix */
/* Begin */
void mat_2x2_plus_mat_2x2(float A[][2], float B[][2], float out[][2])
{
  uint8_t row, col;
  
  for(row = 0; row < 2; row++)
  {
    for(col = 0; col < 2; col++)
      out[row][col] = A[row][col] + B[row][col];
  }
}

void mat_1x1_plus_mat_1x1(float A[][1], float B[][1], float out[][1])
{
  out[0][0] = A[0][0] + B[0][0];
}
/* End */


/* matrix substract matrix */
/* Begin */
void mat_2x2_substract_mat_2x2(float A[][2], float B[][2], float out[][2])
{
  uint8_t row, col;
  
  for(row = 0; row < 2; row++)
  {
    for(col = 0; col < 2; col++)
      out[row][col] = A[row][col] - B[row][col];
  }
}
/* End */


/* matrix multiply matrix*/
/* Begin */
void mat_2x2_multiply_mat_2x2(float A[][2], float B[][2], float out[][2])
{
  uint8_t row, col;
  uint8_t i;
  
  for(row = 0; row < 2; row++)
  {
    for(col = 0; col < 2; col++)
    {
      for(i = 0; i < 2; i++)
        out[row][col] += A[row][i] * B[i][col];
    }
  }
}

void mat_2x2_multiply_mat_2x1(float A[][2], float B[][1], float out[][1])
{
  uint8_t row, i;
  
  for(row = 0; row < 2; row++)
  {
    for(i = 0; i < 2; i++)
      out[row][0] += A[row][i] * B[i][0];
  }
}

void mat_1x2_multiply_mat_2x2(float A[][2], float B[][2], float out[][2])
{
  uint8_t i, col;
  
  for(col = 0; col < 2; col++)
  {
    for(i = 0; i < 2; i++)
      out[0][col] += A[0][i] * B[i][col];
  }
}

void mat_1x2_multiply_mat_2x1(float A[][2], float B[][1], float out[][1])
{
  uint8_t i;
  
  for(i = 0; i < 2; i++)
    out[0][0] += A[0][i] * B[i][0];
}

void mat_2x1_multiply_mat_1x1(float A[][1], float B[][1], float out[][1])
{
  uint8_t i;
  
  for(i = 0; i < 2; i++)
    out[i][0] = A[i][0] * B[0][0];
}

void mat_2x1_multiply_mat_1x2(float A[][1], float B[][2], float out[][2])
{
  uint8_t row, i;
  
  for(row = 0; row < 2; row++)
  {
    for(i = 0; i < 2; i++)
      out[row][i] = A[row][0] * B[0][i];
  }
}
/* End */


/* matrix */
/* Begin */
void mat_2x2_transpose(float A[][2], float out[][2])
{
  uint8_t row, col;
    
  for(row = 0; row < 2; row++)
  {
    for(col = 0; col < 2; col++)
      out[row][col] = A[col][row];
  }
}

void mat_1x1_inverse(float A[][1], float out[][1])
{
  out[0][0] = 1.0f / A[0][0];
}
/* End */