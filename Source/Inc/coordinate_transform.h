#ifndef COORDINATE_TRANSFORM_H
#define COORDINATE_TRANSFORM_H

#define EARTH_GRAVITY 9.80665f

void calc_dcm_body_to_ned(float roll, float pitch, float yaw);
void transform_data_body_to_ned(float x, float y, float z);
float get_ned_acc_z(void);
float get_ned_free_acc_z(void);

#endif