#include "stdint.h"
#include "calculations.h"

/* ******************************************************* */
/* I2C code for ADXL345 accelerometer                      */
/* and HMC5843 magnetometer                                */
/* ******************************************************* */
//todo: put the init in a #define
static int SENSOR_SIGN[9] =
{ -1, 1, -1, 1, 1, 1, -1, -1, -1 }; //Correct directions x,y,z - gyros, accels, magnetormeter
//blijkbaar AN[0..2] = x,y,z van acc. meter (ruwe, oversampled data) kwam van ADC
static int AN[6]; //array that store the 3 ADC filtered data (gyros)
static int AN_OFFSET[6] =
{ 0, 0, 0, 0, 0, 0 }; //Array that stores the Offset of the sensors
static int ACC[3]; //array that store the accelerometers data


// komt van adc.c file, echter is de AN[0..6] array gecombineerd met acc. en dus hier geplaatst
float read_adc(int select)
{
	if (SENSOR_SIGN[select] < 0)
		return (AN_OFFSET[select] - AN[select]);
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

int AccelAddress = 0x53;

#ifdef INITS_OF_PERIP
void I2C_Init()
{
	Wire.begin();
}

void Accel_Init()
{
	Wire.beginTransmission(AccelAddress);
	Wire.send(0x2D); // power register
	Wire.send(0x08); // measurement mode
	Wire.endTransmission();
	delay(5);
	Wire.beginTransmission(AccelAddress);
	Wire.send(0x31); // Data format register
	Wire.send(0x08); // set to full resolution
	Wire.endTransmission();
	delay(5);
	// Because our main loop runs at 50Hz we adjust the output data rate to 50Hz (25Hz bandwidth)
	Wire.beginTransmission(AccelAddress);
	Wire.send(0x2C); // Rate
	Wire.send(0x09); // set to 50Hz, normal operation
	Wire.endTransmission();
	delay(5);
}
#endif

// Reads x,y and z accelerometer registers
void Read_Accel()
{
	int i = 0;
	uint8_t buff[6];

	ACC[1] = (((int) buff[1]) << 8) | buff[0]; // Y axis (internal sensor x axis)
	ACC[0] = (((int) buff[3]) << 8) | buff[2]; // X axis (internal sensor y axis)
	ACC[2] = (((int) buff[5]) << 8) | buff[4]; // Z axis
	// hieronder wordt de gyro data naar de juiste plekken in AN array gegooit
	AN[3] = ACC[0];
	AN[4] = ACC[1];
	AN[5] = ACC[2];

	uint16_t accelero[3];

	accelero[0] = SENSOR_SIGN[3] * (ACC[0] - AN_OFFSET[3]);
	accelero[1] = SENSOR_SIGN[4] * (ACC[1] - AN_OFFSET[4]);
	accelero[2] = SENSOR_SIGN[5] * (ACC[2] - AN_OFFSET[5]);
	updateAccelero(accelero);

}
