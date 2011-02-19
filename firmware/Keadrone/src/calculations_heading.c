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
#include "DCM.h"
#include "calculations.h"

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
		/*Block waiting for the semaphore to become available. */
		if (xSemaphoreTake( xSemaphore, LONG_TIME ) == pdTRUE)
		{
			/* It is time to execute. */

			static sAcc_data acc_copy;
			static GYRO_S gyro_copy;

			acc_copy = accCurrent;
			gyro_copy = gyroCurrent;

			static Bool initDone = TRUE;
			if (initDone)
			{
				static uint16_t initIteration = 0;
				if (initIteration++ == 32)
				{
					initDone = FALSE;
					initi2c2();
				}
				else
				{
					initi2c1();
				}
			}
			else
			{
				static MATRIX_UPDATE_STRUCT data;
				data.gyro_x = ToRad(((float)read_adc(0) / 14.375));
				data.gyro_y = ToRad(((float)read_adc(1) / 14.375));
				data.gyro_z = ToRad(((float)read_adc(2) / 14.375));

				Matrix_update(&data);
				Normalize();
				Drift_correction();
				Euler_angles();
				float temp;
				temp = (ToDeg(gyro_true.pitch) * 22);
				servoSet(1, ((uint16_t) temp));
			}

			printf("%4.2f,%4.2f,%4.2f,", ToDeg(gyro_true.roll),
					ToDeg(gyro_true.pitch), ToDeg(gyro_true.yaw));

			printf("%4.2f,%4.2f,%4.2f,", gyro.x, gyro.y, gyro.z);
			printf("%d,%d,%d,%d,%d,%d,\n", acc_copy.X, acc_copy.Y, acc_copy.Z,
					gyro_copy.x, gyro_copy.y, gyro_copy.z);

			//	printf("\n");
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

void calculations_heading_FromISR(void)
{
	static signed portBASE_TYPE xHigherPriorityTaskWoken;
	static uint16_t counter = 0;

	// Get data from both sensors + do some calculations
	spiGetAccelero(&accCurrent);
	gyroGetDataFromChip(&gyroCurrent);

#define FILTER 1
	static int16_t array[6];
	array[3] += ((accCurrent.X >> 2) - (array[3] >> 2)) >> FILTER;
	array[4] += ((accCurrent.Y >> 2) - (array[4] >> 2)) >> FILTER;
	array[5] += ((accCurrent.Z >> 2) - (array[5] >> 2)) >> FILTER;

	array[0] += ((gyroCurrent.x >> 2) - (array[0] >> 2)) >> FILTER;
	array[1] += ((gyroCurrent.y >> 2) - (array[1] >> 2)) >> FILTER;
	array[2] += ((gyroCurrent.z >> 2) - (array[2] >> 2)) >> FILTER;

	/* convert to degrees/sec*/
	static float Gyro[3];
	Gyro[0] += (((float) gyroCurrent.x - gyroCurrent.x_offset) / 14.375) / 2000;
	Gyro[1] += (((float) gyroCurrent.y - gyroCurrent.y_offset) / 14.375) / 2000;
	Gyro[2] += (((float) gyroCurrent.z - gyroCurrent.z_offset) / 14.375) / 2000;

	if (counter++ == 40)//50hz
	{
		counter = 0;
		update_sensor_data(array);
		xHigherPriorityTaskWoken = pdFALSE;

		gyro.x = Gyro[0];
		gyro.y = Gyro[1];
		gyro.z = Gyro[2];
		/* Unblock the task by releasing the semaphore. */
		xSemaphoreGiveFromISR(xSemaphore, &xHigherPriorityTaskWoken);
		portEND_SWITCHING_ISR( xHigherPriorityTaskWoken );
	}
}
