/*
 * calculations.h
 *
 *  Created on: 15 feb 2011
 *      Author: Willem (wnpd.nl)
 */

#ifndef CALCULATIONS_H_
#define CALCULATIONS_H_
typedef enum
{
	GYRO_X = 0,
	GYRO_Y = 1,
	GYRO_Z = 2,
	ACC_X = 3,
	ACC_Y = 4,
	ACC_Z = 5,
	LAST_ELEMENT
} SENSOR_DATA;

// ADXL345 Sensitivity(from datasheet) => 4mg/LSB   1G => 1000mg/4mg = 256 steps
// Tested value : 248
#define GRAVITY_DIV  4096 //this equivalent to 1G in the raw data coming from the accelerometer
#define Accel_Scale(x) x*(GRAVITY_DIV/9.81)//Scaling the raw data of the accel to actual acceleration in meters for seconds square
#define GYRO_DIV 14.375
/********************************/
/********************************/
#define ToRad(x) (x*0.01745329252)  // *pi/180
#define ToDeg(x) (x*57.2957795131)  // *180/pi
/********************************/
/********************************/
#define Kp_ROLLPITCH 0.0002
#define Ki_ROLLPITCH 0.00000
#define Kp_YAW 1.2
#define Ki_YAW 0.00002

/*For debugging purposes*/
//OUTPUTMODE=1 will print the corrected data,
//OUTPUTMODE=0 will print uncorrected data of the gyros (with drift)
#define OUTPUTMODE 1

typedef struct
{
	float roll; //x
	float pitch; // y
	float yaw;//z
} GYRO_STRUCT;

void imuHeadingUpdate(float pitch, float roll, float yaw);
float getHeadingRoll(void);
float getHeadingPitch(void);
float getHeadingYaw(void);

#endif /* CALCULATIONS_H_ */
