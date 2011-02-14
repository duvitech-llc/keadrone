/*
 * calculations_heading.c
 *
 *  Created on: 13 feb 2011
 *      Author: W. Pietersz
 */
#include "FreeRTOS.h"
#include "task.h"
#include "semphr.h"

#include "LPC17xx.h"
#include "lpc17xx_clkpwr.h"
#include "dAccelerometer_BMA180.h"
#include "dGyro_ITG-3200.h"

xSemaphoreHandle xSemaphore;
sAcc_data accCurrent;
GYRO_S gyroCurrent;
const double PI = 3.141592;

struct
{
	volatile float x;
	volatile float y;
	volatile float z;

} gyro;

#define LONG_TIME 0xffff

static void calculations_heading(void *pvParameters)
{
	UARTInit(3, 115200); /* baud rate setting */
	printf("Maintask: Running\n");

	CLKPWR_ConfigPPWR(CLKPWR_PCONP_PCGPIO, ENABLE);
	spiInit();
	gyroInit();

	for (;;)
	{
		static uint16_t value = 0;
		value += 98;
		if (value >= (781))
			value = 0;
		servoSet(0, value);

		/*Block waiting for the semaphore to become available. */
		if (xSemaphoreTake( xSemaphore, LONG_TIME ) == pdTRUE)
		{
			/* It is time to execute. */

			static sAcc_data acc_copy;
			static GYRO_S gyro_copy;

			acc_copy = accCurrent;
			gyro_copy = gyroCurrent;
			printf("%d,%d,%d,%d,%d,%d,\n", acc_copy.X, acc_copy.Y, acc_copy.Z,
					gyro_copy.x, gyro_copy.y, gyro_copy.z);

			//printf("%f,%f,%f\n", gyro.x, gyro.y, gyro.z);
		}
	}
}

void calculations_heading_init(void)
{
	vSemaphoreCreateBinary( xSemaphore );
	xTaskCreate( calculations_heading, ( signed char * ) "UART", (300), NULL, tskIDLE_PRIORITY+1, NULL );
	// block on error creating semaphore
	while (xSemaphore == NULL)
		;
}
#include "math.h"

void calculations_heading_FromISR(void)
{
	static signed portBASE_TYPE xHigherPriorityTaskWoken;
	static uint16_t counter = 0;

	// Get data from both sensors + do some calculations
	spiGetAccelero(&accCurrent);
	gyroGetDataFromChip(&gyroCurrent);

	// convert to degrees/sec
	static float Gyro[3];
	Gyro[0] += (((float) gyroCurrent.x - gyroCurrent.x_offset) / 14.375) / 2000;
	Gyro[1] += (((float) gyroCurrent.y - gyroCurrent.y_offset) / 14.375) / 2000;
	Gyro[2] += (((float) gyroCurrent.z - gyroCurrent.z_offset) / 14.375) / 2000;

	float AaccXZ;

	if (abs(accCurrent.Z) != 0)
	{
		AaccXZ = atan2(accCurrent.X, accCurrent.Z);
		Gyro[0] = AaccXZ * 180 / PI;
		Gyro[1] = Gyro[1] * 0.96 + Gyro[0] * 0.04;
	}

	if (counter++ == 200)
	{
		counter = 0;
		xHigherPriorityTaskWoken = pdFALSE;
		gyro.x = Gyro[0];
		gyro.y = Gyro[1];
		gyro.z = Gyro[2];
		/* Unblock the task by releasing the semaphore. */
		xSemaphoreGiveFromISR(xSemaphore, &xHigherPriorityTaskWoken);
		portEND_SWITCHING_ISR( xHigherPriorityTaskWoken );
	}
}
