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
#include "DCM_Data.h"

static xSemaphoreHandle xSemaphore, xSemaphoreTerminal;
static sAcc_data accCurrent;
static GYRO_S gyroCurrent;

static void terminal(void *pvParameters)
{
	for (;;)
	{
		/*Block waiting for the semaphore to become available. */
		if (xSemaphoreTake( xSemaphoreTerminal, 0xffff ) == pdTRUE)
		{

			static sAcc_data acc_copy;
			static GYRO_S gyro_copy;

			acc_copy = accCurrent;
			gyro_copy = gyroCurrent;

			float roll, pitch;
			roll = ToDeg(currentHeading.roll);
			pitch = ToDeg(currentHeading.pitch);
			printf("%d,%d,%d,%d,%d,%d,", acc_copy.X, acc_copy.Y, acc_copy.Z,
					gyro_copy.x, gyro_copy.y, gyro_copy.z);

			/* Printout section */
			printf("%4.2f,", roll );
			printf("%4.2f\n", pitch);

		}
	}
}

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
		if (xSemaphoreTake( xSemaphore, 0xffff ) == pdTRUE)
		{
			/* It is time to execute. */;

			/* Turn the LED off if it was on, and on if it was off. */
			LPC_GPIO1->FIOSET = (1 << 1);

			static Bool initDone = TRUE;
			if (initDone)
			{
				static uint16_t initIteration = 0;
				if (initIteration++ == 32)
				{
					initDone = FALSE;
					imuInit_2();
				}
				else
				{
					imuInit_1();
				}
			}
			else
			{

				Matrix_update();
				Normalize();
				Drift_correction();
				Euler_angles();

				static int32_t valueY = 0;
				static int32_t powerVal = 300;
				static int32_t pTerm = 8;
				static int32_t setpoint = 0;
				static int32_t error = 0;

				error = ToDeg(currentHeading.roll) - setpoint;
				valueY = error * pTerm;

				int32_t calc;

				calc = powerVal - valueY;
				if (calc < 0)
					calc = 0;
				else if (calc > 600)
					calc = 600;
				servoSet(3, calc);

				calc = powerVal + valueY;
				if (calc < 0)
					calc = 0;
				else if (calc > 600)
					calc = 600;
				servoSet(1, calc);

				static uint16_t counter = 0;
				counter++;
				if (counter == 4)
				{
					counter = 0;
					xSemaphoreGive(xSemaphoreTerminal);
				}

			}

			LPC_GPIO1->FIOCLR = (1 << 1);
		}
	}
}

void calculations_heading_init(void)
{
	/* Initialize P1_1 for the LED. */
	LPC_PINCON->PINSEL2 &= (~(3 << 2));
	LPC_GPIO1->FIODIR |= (1 << 1);

	vSemaphoreCreateBinary( xSemaphore );
	vSemaphoreCreateBinary( xSemaphoreTerminal );

	xTaskCreate( terminal, ( signed char * ) "terminal", (300), NULL, tskIDLE_PRIORITY+1, NULL );
	xTaskCreate( calculations_heading, ( signed char * ) "UART", (200), NULL, tskIDLE_PRIORITY+2, NULL );
	// block on error creating semaphore
	while (xSemaphore == NULL)
		;
}

/* Gets called every 2000Hz*/
void calculations_heading_FromISR(void)
{
	static signed portBASE_TYPE xHigherPriorityTaskWoken;
	static uint16_t counter = 0;

	// Get data from both sensors + do some calculations
	spiGetAccelero(&accCurrent);
	gyroGetDataFromChip(&gyroCurrent);

	static int16_t array[6];
	static int32_t bigArray[6];
	static int16_t iterations = 0;

	bigArray[ACC_X] += ((accCurrent.X));
	bigArray[ACC_Y] += ((accCurrent.Y));
	bigArray[ACC_Z] += ((accCurrent.Z));

	bigArray[GYRO_X] += ((gyroCurrent.x));
	bigArray[GYRO_Y] += ((gyroCurrent.y));
	bigArray[GYRO_Z] += ((gyroCurrent.z));

	iterations++;
	if (iterations == 8)
	{
		for (iterations = 0; iterations < 6; iterations++)
		{
			//average the data to a single sample
			array[iterations] = bigArray[iterations] / 8;
		}
		/* Run DCM Calculations */

		/*updates values in DCM_DATA */
		imuUpdate((int16_t*) &array[0]);

		/* Unblock the task by releasing the semaphore. */
		xHigherPriorityTaskWoken = pdFALSE;
		xSemaphoreGiveFromISR(xSemaphore, &xHigherPriorityTaskWoken);
		portEND_SWITCHING_ISR( xHigherPriorityTaskWoken );

		for (iterations = 0; iterations < 6; iterations++)
		{
			//reset sample average
			bigArray[iterations] = 0;
		}
		iterations = 0;
	}

}
