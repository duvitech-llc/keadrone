/*
 * calculations.c
 *
 *  Created on: 15 feb 2011
 *      Author: Willem (wnpd.nl)
 */
#include "math.h"
#include "stdint.h"
#include "DCM_Data.h"

static DCM_DATA gyro;
static DCM_DATA_OFFSETS gyroOffset;
static DCM_DATA gyroSensorSign;
static DCM_DATA accelero;
static DCM_DATA_OFFSETS acceleroOffset;
static DCM_DATA acceleroSign;

// Posts new heading in grad/s format (not rad/s!)
void imuHeadingUpdate(float pitch, float roll, float yaw)
{
	currentHeading.pitch = pitch;
	currentHeading.roll = roll;
	currentHeading.yaw = yaw;
}

/******************GETTERS************************************/

// OUTPUT SCALED TO GRAD/S
float getGyroX(void)
{
	return ((float) (gyro.X - gyroOffset.X) / 14.375);
}
// OUTPUT SCALED TO GRAD/S
float getGyroY(void)
{
	return ((float) (gyro.Y - gyroOffset.Y) / 14.375);
}
// OUTPUT SCALED TO GRAD/S
float getGyroZ(void)
{
	return ((float) (gyro.Z - gyroOffset.Z) / 14.375);
}

// OUTPUT ACELERO NOT SCALED
//(REMEMBER TO SPECIFY GRAVITY DIVIDER IN DCM_DATA.H)
int16_t getAcceleroX(void)
{
	return (accelero.X - acceleroOffset.X);
}
int16_t getAcceleroY(void)
{
	return (accelero.Y - acceleroOffset.Y);
}
int16_t getAcceleroZ(void)
{
	return (accelero.Z - acceleroOffset.Z);
}
/**********************************************************************/

// call this function on init 32 times with valid, new data from all sensors
void imuInit_1(void)
{
	acceleroSign.X = 1;
	acceleroSign.Y = -1;
	acceleroSign.Z = -1;

	gyroSensorSign.X = 1;
	gyroSensorSign.Y = -1;
	gyroSensorSign.Z = -1;

	acceleroOffset.X += accelero.X;
	acceleroOffset.Y += accelero.Y;
	acceleroOffset.Z += accelero.Z;

	gyroOffset.X += gyro.X;
	gyroOffset.Y += gyro.Y;
	gyroOffset.Z += gyro.Z;

}

//Simply divides all the offset values
void imuInit_2(void)
{
	acceleroOffset.X /= 32;
	acceleroOffset.Y /= 32;
	acceleroOffset.Z /= 32;

	gyroOffset.X /= 32;
	gyroOffset.Y /= 32;
	gyroOffset.Z /= 32;

	acceleroOffset.Z -= GRAVITY_DIV * acceleroSign.Z;
}

/* Parameters are given in grad/s */
void imuUpdate(DCM_DATA *pGyro, DCM_DATA *pAccelero)
{
	gyro.X = gyroSensorSign.X * pGyro->X;
	gyro.Y = gyroSensorSign.Y * pGyro->Y;
	gyro.Z = gyroSensorSign.Z * pGyro->Z;

	accelero.X = acceleroSign.X * pAccelero->X;
	accelero.Y = acceleroSign.Y * pAccelero->Y;
	accelero.Z = acceleroSign.Z * pAccelero->Z;
}
