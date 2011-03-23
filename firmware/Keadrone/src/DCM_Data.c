/*
 * calculations.c
 *
 *  Created on: 15 feb 2011
 *      Author: Willem (wnpd.nl)
 */
#include "math.h"
#include "stdint.h"
#include "DCM_Data.h"

GYRO_STRUCT currentHeading;
// this is in the format gyro X,Y,Z accelero X,Y,Z
#define SENSOR_SIGNS 1, -1, -1, 1, -1, -1

static int16_t SENSOR_SIGN[LAST_ELEMENT] =
{ SENSOR_SIGNS}; //Correct directions x,y,z - gyros, accels, magnetormeter
static int32_t AN_OFFSET[LAST_ELEMENT] =
{ 0, 0, 0, 0, 0, 0 }; //Array that stores the Offset of the sensors
static int16_t AN_DATA[LAST_ELEMENT];

// will be called from DCM
void imuHeadingUpdate(float pitch, float roll, float yaw)
{
	currentHeading.pitch = pitch;
	currentHeading.roll = roll;
	currentHeading.yaw = yaw;
}

int16_t imu_read_sensor(SENSOR_DATA select)
{
	return ((AN_DATA[select] - AN_OFFSET[select]));
}

void imuInit_1(void)
{
	int y;
	for (y = 0; y < 6; y++) // Cumulate values
		AN_OFFSET[y] += AN_DATA[y];
}

void imuInit_2(void)
{
	int y;
	for (y = 0; y < 6; y++)
		AN_OFFSET[y] = AN_OFFSET[y] / 32;

	AN_OFFSET[ACC_Z] += GRAVITY_DIV * SENSOR_SIGN[ACC_Z];
}

/* Parameters are given in grad/s */
void imuUpdate(int16_t *data)
{
	AN_DATA[GYRO_X] = SENSOR_SIGN[GYRO_X] * (*(data + GYRO_X));
	AN_DATA[GYRO_Y] = SENSOR_SIGN[GYRO_Y] * (*(data + GYRO_Y));
	AN_DATA[GYRO_Z] = SENSOR_SIGN[GYRO_Z] * (*(data + GYRO_Z));

	AN_DATA[ACC_X] = SENSOR_SIGN[ACC_X] * (*(data + ACC_X));
	AN_DATA[ACC_Y] = SENSOR_SIGN[ACC_Y] * (*(data + ACC_Y));
	AN_DATA[ACC_Z] = SENSOR_SIGN[ACC_Z] * (*(data + ACC_Z));
}

float getHeadingRoll(void)
{
	return ToDeg(currentHeading.roll);
}

float getHeadingPitch(void)
{
	return ToDeg(currentHeading.pitch);
}

float getHeadingYaw(void)
{
	return ToDeg(currentHeading.yaw);
}
