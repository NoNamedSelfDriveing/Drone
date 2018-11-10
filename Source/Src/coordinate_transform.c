#include "coordinate_transform.h"
#include "mti.h"
#include "math.h"
#include "gps.h"

float dcm_body_to_ned[3][3];
float ned_data[3];

void calc_dcm_body_to_ned(float roll, float pitch, float yaw)
{
  roll = roll * PI / 180.0f;
  pitch = pitch * PI / 180.0f;
  yaw = yaw * PI / 180.0f;
  
  dcm_body_to_ned[0][0] = cos(yaw)*cos(pitch);
  dcm_body_to_ned[0][1] = -sin(yaw)*cos(roll) + cos(yaw)*sin(pitch)*sin(roll);
  dcm_body_to_ned[0][2] = sin(yaw)*sin(roll) + cos(yaw)*sin(pitch)*cos(roll);
  
  dcm_body_to_ned[1][0] = sin(yaw)*cos(pitch);
  dcm_body_to_ned[1][1] = cos(yaw)*cos(roll) + sin(yaw)*sin(pitch)*sin(roll);
  dcm_body_to_ned[1][2] = -cos(yaw)*sin(roll)+sin(yaw)*sin(pitch)*cos(roll);
  
  dcm_body_to_ned[2][0] = -sin(pitch);
  dcm_body_to_ned[2][1] = cos(pitch)*sin(roll);
  dcm_body_to_ned[2][2] = cos(pitch)*cos(roll);
}

void transform_data_body_to_ned(float x, float y, float z)
{
  ned_data[0] = dcm_body_to_ned[0][0]*x + dcm_body_to_ned[0][1]*y + dcm_body_to_ned[0][2]*z;
  ned_data[1] = dcm_body_to_ned[1][0]*x + dcm_body_to_ned[1][1]*y + dcm_body_to_ned[1][2]*z;
  ned_data[2] = dcm_body_to_ned[2][0]*x + dcm_body_to_ned[2][1]*y + dcm_body_to_ned[2][2]*z;
}

float get_ned_acc_z(void)
{
  return ned_data[2];
}

float get_ned_free_acc_z(void)
{
  return -ned_data[2] - EARTH_GRAVITY;
}