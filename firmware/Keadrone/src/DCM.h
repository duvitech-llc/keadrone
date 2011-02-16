/*
 * DCM.h
 *
 *  Created on: 15 feb 2011
 *      Author: Willem (wnpd.nl)
 */

#ifndef DCM_H_
#define DCM_H_

typedef struct
{
	int accel_x; // gecompenseerd met off-set
	int accel_y;// gecompenseerd met off-set
	int accel_z;// gecompenseerd met off-set
	float gyro_x;// raw data of the gyro in radians per second, use:
	float gyro_y;//  #define ToRad(x) (x*0.01745329252)  // *pi/180
	float gyro_z;// for this calculation
} MATRIX_UPDATE_STRUCT;

typedef struct
{
	float roll;
	float pitch;
	float yaw;
} GYRO_STRUCT;

GYRO_STRUCT gyro_true;
#endif /* DCM_H_ */
