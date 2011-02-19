/*
 * calculations.c
 *
 *  Created on: 15 feb 2011
 *      Author: Willem (wnpd.nl)
 */
#include "math.h"

#include "stdint.h"
#include "calculations.h"
#include "DCM.h"

/* ******************************************************* */
/* I2C code for ADXL345 accelerometer                      */
/* and HMC5843 magnetometer                                */
/* ******************************************************* */
//todo: put the init in a #define
static int SENSOR_SIGN[9] =
{
		1, -1, -1, -1, 1, 1, -1, -1, -1 }; //Correct directions x,y,z - gyros, accels, magnetormeter
//blijkbaar AN[0..2] = x,y,z van acc. meter (ruwe, oversampled data) kwam van ADC
static int AN[6]; //array that store the 3 ADC filtered data (gyros)
static int AN_OFFSET[6] =
{ 0, 0, 0, 0, 0, 0 }; //Array that stores the Offset of the sensors
static int ACC[3]; //array that store the accelerometers data


// komt van adc.c file, echter is de AN[0..6] array gecombineerd met acc. en dus hier geplaatst
float read_adc(int select)
{
	if (SENSOR_SIGN[select] < 0)
		return (-(AN[select] - AN_OFFSET[select]));
	else
		return (AN[select] - AN_OFFSET[select]);
}

void initi2c1(void)
{
	int y;
	for (y = 0; y < 6; y++) // Cumulate values
		AN_OFFSET[y] += AN[y];
}

void initi2c2(void)
{
	int y;
	for (y = 0; y < 6; y++)
		AN_OFFSET[y] = AN_OFFSET[y] / 32;

	AN_OFFSET[5] -= GRAVITY_DIV * SENSOR_SIGN[5];

#ifdef NOT_USED_WP
	//Serial.println("Offset:");
	for (y = 0; y < 6; y++)
	Serial.println(AN_OFFSET[y]);
#endif
}

//expect 6 elements, starting with xyz gyro and then xyz accelero
void update_sensor_data(int16_t *data)
{
	// hieronder wordt de gyro data naar de juiste plekken in AN array gegooit
	AN[0] = *(data + 0);
	AN[1] = *(data + 1);
	AN[2] = *(data + 2);

	ACC[0] = *(data + 3);
	ACC[1] = *(data + 4);
	ACC[2] = *(data + 5);
	AN[3] = ACC[0];
	AN[4] = ACC[1];
	AN[5] = ACC[2];

	int16_t accelero[3];
	accelero[0] = SENSOR_SIGN[3] * (ACC[0] - AN_OFFSET[3]);
	accelero[1] = SENSOR_SIGN[4] * (ACC[1] - AN_OFFSET[4]);
	accelero[2] = SENSOR_SIGN[5] * (ACC[2] - AN_OFFSET[5]);
	updateAcc(accelero);

}

