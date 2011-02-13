/*
 * dGyro_ITG-3200.c
 *
 *  Created on: 7 feb 2011
 *      Author: Willem (wnpd.nl)
 */
#include "LPC17xx.h"
#include "i2c.h"
#include "dGyro_ITG-3200.h"

/* Kernel includes. */
#include "task.h"

// rx/tx buffer used for i2c
static volatile uint8_t i2cBuffer[10];
// i2c config struct init
static I2C_DATA i2c;

// the ITG i2c address
#define ITG3200 0xD0

static GYRO_S gyro =
{ 0 };

#ifdef UNUSED
#define HISTORY_IIR    32 //'L' average point (baseline updated @100mS)
typedef struct
{
	uint32_t IIR_Sum;
	uint16_t currReading;
}FIR_FILTER;

uint16_t IIR_Average(FIR_FILTER *p)
{
	// on boot-up our value is never 0
	if (p->IIR_Sum == 0)
	p->IIR_Sum = (UINT32) p->currReading * HISTORY_IIR;

	p->IIR_Sum -= (UINT32)(p->IIR_Sum / HISTORY_IIR);
	p->IIR_Sum += p->currReading;

	// note, this filter has an gain of HISTERY
	// therefore we must devide the average value
	// with HISTORY to get our current average
	return (UINT16)(p->IIR_Sum / HISTORY_IIR);
}

#define HISTORY_IIR    32 //'L' average point (baseline updated @100mS)
typedef struct
{
	uint32_t IIR_Sum;
	uint16_t currReading;
}FIR_FILTER;

uint16_t IIR_Average(FIR_FILTER *p)
{
	// on boot-up our value is never 0
	if (p->IIR_Sum == 0)
	p->IIR_Sum = (UINT32) p->currReading * HISTORY_IIR;

	p->IIR_Sum -= (UINT32)(p->IIR_Sum / HISTORY_IIR);
	p->IIR_Sum += p->currReading;

	// note, this filter has an gain of HISTERY
	// therefore we must devide the average value
	// with HISTORY to get our current average
	return (UINT16)(p->IIR_Sum / HISTORY_IIR);
}

#endif

static int16_t slewRateLimit(int16_t newValue, int16_t * rawValue)
{
	// Start slew-rate limiter
	if (*rawValue == 0)
		*rawValue = newValue;

	if (*rawValue < newValue)
		(*rawValue)++;
	else
		(*rawValue)--;

	return (*rawValue);
}

/* Takes X samples and averages them. This is the
 *  new offset that will be used
 */
static void gyroCalculateOffset(void)
{
	//FIR_FILTER x, y, z;
	int16_t newValueX, newValueY, newValueZ;

	int32_t sumX = 0, sumY = 0, sumZ = 0;
	int16_t rawX = 0, rawY = 0, rawZ = 0;
	uint16_t counter = 0;

	for (;;)
	{
		// block for new data
		while (!(LPC_GPIO0->FIOPIN & (1 << 7)))
			;

		do
		{
			// assemble polling request
			i2c.address = ITG3200;
			i2c.slaveRegister = 27;
			i2c.readData = TRUE;
			i2c.bufLength = 8;
			i2c.buffer = i2cBuffer;
		} while (I2CEnginePolling(&i2c) == FALSE);

		newValueX = ((int16_t) i2cBuffer[2] << 8) + i2cBuffer[3];
		newValueY = ((int16_t) i2cBuffer[4] << 8) + i2cBuffer[5];
		newValueZ = ((int16_t) i2cBuffer[6] << 8) + i2cBuffer[7];

		//gyro[counter].temp = ((int16_t) i2cBuffer[0] << 8) + i2cBuffer[1];
		//gyro[counter].x = ((int16_t) i2cBuffer[2] << 8) + i2cBuffer[3];
		//gyro[counter].y = ((int16_t) i2cBuffer[4] << 8) + i2cBuffer[5];
		//gyro[counter].z = ((int16_t) i2cBuffer[6] << 8) + i2cBuffer[7];

		/*
		 x.currReading = ((int16_t) i2cBuffer[2] << 8) + i2cBuffer[3];
		 gyro.x_offset = IIR_Average(x);

		 y.currReading = ((int16_t) i2cBuffer[4] << 8) + i2cBuffer[5];
		 gyro.y_offset = IIR_Average(x);

		 z.currReading = ((int16_t) i2cBuffer[6] << 8) + i2cBuffer[7];
		 gyro.z_offset = IIR_Average(x);
		 */

		sumX += (int32_t) slewRateLimit(newValueX, &rawX);
		sumY += (int32_t) slewRateLimit(newValueY, &rawY);
		sumZ += (int32_t) slewRateLimit(newValueZ, &rawZ);

		counter++;
		if (counter == 2046)
		{
			gyro.x_offset = sumX / 2047;
			gyro.y_offset = sumY / 2047;
			gyro.z_offset = sumZ / 2047;
			break;
		}

	}
}

static void gpioIntEnable(void)
{
	//Enable rising edge interrupt for P0.7
	LPC_GPIOINT->IO0IntEnR |= 1 << 7;//| 1<<9;
	NVIC_EnableIRQ(EINT3_IRQn);
	NVIC_SetPriority(EINT3_IRQn, 30);
}

void gyroInit(void)
{
	// init the i2c bus
	I2CInit();

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
	printf("Gyro config complete...");

	/* Let the internal low pass filter of the gyro fill */
	vTaskDelay(100);

	gyroCalculateOffset();

	do
	{
		// assemble our first request
		i2c.address = ITG3200;
		i2c.slaveRegister = 27;
		i2c.readData = TRUE;
		i2c.bufLength = 8;
		i2c.buffer = i2cBuffer;
	} while (I2CEnginePolling(&i2c) == FALSE);

	// load initial values
	gyro.x = ((int16_t) i2cBuffer[2] << 8) + i2cBuffer[3];
	gyro.y = ((int16_t) i2cBuffer[4] << 8) + i2cBuffer[5];
	gyro.z = ((int16_t) i2cBuffer[6] << 8) + i2cBuffer[7];

	/* Enable interrupt for gyro */
	gpioIntEnable();
}

void gyroGetDataFromChip(GYRO_S *p)
{
	// static to reduce interrupt stack
	static int16_t newValueX, newValueY, newValueZ;

	/* Request new data */
	i2c.bufLength = 8;
	I2CEngine_FromISR(&i2c);

	/* Get previous data */
	newValueX = ((int16_t) i2cBuffer[2] << 8) + i2cBuffer[3];
	newValueY = ((int16_t) i2cBuffer[4] << 8) + i2cBuffer[5];
	newValueZ = ((int16_t) i2cBuffer[6] << 8) + i2cBuffer[7];

#define FILTERSHIFT 1

	// perform just a little bit of filtering to improve signal to noise
	gyro.x += (((newValueX / 2) - (gyro.x / 2)) >> FILTERSHIFT);
	gyro.y += (((newValueY / 2) - (gyro.y / 2)) >> FILTERSHIFT);
	gyro.z += (((newValueZ / 2) - (gyro.z / 2)) >> FILTERSHIFT);

	*p = gyro;

}
