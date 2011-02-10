/*
 * gyro-ITG3200.c
 *
 *  Created on: 7 feb 2011
 *      Author: Willem (wnpd.nl)
 */
#include "i2c.h"
#include "LPC17xx.h"

volatile uint8_t i2cBuffer[10];
I2C_DATA i2c;
typedef struct
{
	volatile int16_t x;
	volatile int16_t y;
	volatile int16_t z;
	volatile int16_t temp;
} GYRO_S;

int16_t x_offset;
int16_t y_offset;
int16_t z_offset;

GYRO_S GYROA[64];
GYRO_S GYROB[64];
static void gpioIntInit(void)
{
	//Enable rising edge interrupt for P0.7
	LPC_GPIOINT->IO0IntEnR |= 1 << 7;//| 1<<9;
	NVIC_EnableIRQ(EINT3_IRQn);
}
void gyroCalculateOffset(void);
void gyroInit(void)
{
	I2CInit();
#define ITG3200 0xD0

	/* Config of ITG-3200 registers */
	i2c.address = ITG3200;
	i2c.buffer = i2cBuffer;

	do
	{
		i2c.slaveRegister = 62;
		i2c.readData = FALSE;
		i2c.bufLength = 1;
		// hard reset gyro
		i2cBuffer[0] = 0x80; // new value reg. 62 (power management)
		I2CEnginePolling(&i2c);
	} while (I2CEnginePolling(&i2c) == FALSE);
	vTaskDelay(1);

	do
	{
		i2c.slaveRegister = 0;// who am i register
		i2c.readData = TRUE;
		i2c.bufLength = 2;
		I2CEnginePolling(&i2c);
		// Who Am I register is always 0x34
	} while (((i2cBuffer[0] >> 1) & 0x3F) != 0x34);
	printf("Gyro connected...");

	do
	{
		i2c.slaveRegister = 62;
		i2c.readData = FALSE;
		i2c.bufLength = 1;
		i2cBuffer[0] = 0x1; // new value reg. 62 (power management)
		I2CEnginePolling(&i2c);
	} while (I2CEnginePolling(&i2c) == FALSE);
	// PLL needs to reach a steady mode, thats why we wait.
	vTaskDelay(10);

	do
	{
		i2c.readData = FALSE;
		i2c.slaveRegister = 21;
		i2c.bufLength = 1;
		i2cBuffer[0] = 0x3; // sample rate divider
	} while (I2CEnginePolling(&i2c) == FALSE);

	do
	{
		i2c.readData = FALSE;
		i2c.slaveRegister = 22;
		i2c.bufLength = 1;
		i2cBuffer[0] = 0x18; // new value reg. 22
	} while (I2CEnginePolling(&i2c) == FALSE);

	do
	{
		i2c.readData = FALSE;
		i2c.slaveRegister = 23;
		i2c.bufLength = 1;
		i2cBuffer[0] = 0x31;//0x11; // new value reg. 23 (interrupts)
	} while (I2CEnginePolling(&i2c) == FALSE);
//	printf("Gyro config complete...");
	vTaskDelay(10);
	// assemble our first request
	i2c.address = ITG3200;
	i2c.slaveRegister = 27;
	i2c.readData = TRUE;
	i2c.bufLength = 8;
	i2c.buffer = i2cBuffer;
	I2CEngine_FromISR(&i2c);

	gyroCalculateOffset();
	gpioIntInit();
}

void gyroFirAverage(GYRO_S *p)
{
	uint16_t counter;
	int32_t bigSum = 0;

	for (counter = 0; counter < 128; counter++)
	{
		bigSum += p[counter].x;
	}
	bigSum /= 128;
	x_offset = bigSum;

}

void gyroCalculateOffset(void)
{
	for (;;)
	{
		// block for new data
		while (!(LPC_GPIO0->FIOPIN & (1 << 7)))
			;

		i2c.bufLength = 8;
		i2c.buffer = i2cBuffer;
		I2CEngine_FromISR(&i2c);

		static uint16_t counter = 0;

		GYROA[counter].temp = ((int16_t) i2cBuffer[0] << 8) + i2cBuffer[1];
		GYROA[counter].x = ((int16_t) i2cBuffer[2] << 8) + i2cBuffer[3];
		GYROA[counter].y = ((int16_t) i2cBuffer[4] << 8) + i2cBuffer[5];
		GYROA[counter].z = ((int16_t) i2cBuffer[6] << 8) + i2cBuffer[7];

		counter++;
		if (counter == 64)
		{
			counter = 0;

			//gyroFirAverage(GYROA);
			break;
		}
	}
}
static volatile Bool bufferType = 0;
static volatile Bool bufferFull = 0;
void gyroGetDataFromChip(void)
{
	// Request all data from gyro (temp + gZ,gX,gY)
	//	i2c.address = ITG3200;
	//i2c.slaveRegister = 27;
	//i2c.readData = TRUE;
	i2c.bufLength = 8;
	i2c.buffer = i2cBuffer;
	I2CEngine_FromISR(&i2c);

	static uint16_t counter = 0;
	if (bufferType == 0)
	{
		GYROA[counter].temp = ((int16_t) i2cBuffer[0] << 8) + i2cBuffer[1];
		GYROA[counter].x = ((int16_t) i2cBuffer[2] << 8) + i2cBuffer[3];
		GYROA[counter].y = ((int16_t) i2cBuffer[4] << 8) + i2cBuffer[5];
		GYROA[counter].z = ((int16_t) i2cBuffer[6] << 8) + i2cBuffer[7];
	}
	else
	{
		GYROB[counter].temp = ((int16_t) i2cBuffer[0] << 8) + i2cBuffer[1];
		GYROB[counter].x = ((int16_t) i2cBuffer[2] << 8) + i2cBuffer[3];
		GYROB[counter].y = ((int16_t) i2cBuffer[4] << 8) + i2cBuffer[5];
		GYROB[counter].z = ((int16_t) i2cBuffer[6] << 8) + i2cBuffer[7];
	}
	counter++;
	if (counter == 64)
	{
		counter = 0;
		if (bufferFull == FALSE)
		{
			bufferFull = TRUE;
			// change the buffer
			bufferType ^= 1;
		}
	}

	/*

	 while (!(LPC_GPIO0->FIOPIN & (1 << 7)))
	 ;
	 */
}

void gyroGetData(void)
{
	static uint16_t counter = 0;
	GYRO_S * GYRO;
	if (bufferFull)
	{
		if (bufferType == 1)
			GYRO = GYROA;
		else
			GYRO = GYROB;

		for (counter = 0; counter < 64; counter++)
		{
			printf("%d,", GYRO[counter].x);
			printf("%d,", GYRO[counter].y);
			printf("%d,", GYRO[counter].z);
			printf("\n");
		}
		bufferFull = FALSE;
		printf(",,,EndBuffer\n");

	}
}