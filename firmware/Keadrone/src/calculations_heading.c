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
#include "dAccelerometer_BMA180.h"
#include "dGyro_ITG-3200.h"

xSemaphoreHandle xSemaphore;
sAcc_data accCurrent;

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
			gyro_copy = gyro;
			printf("%d,%d,%d,%d,%d,%d\n", acc_copy.X, acc_copy.Y, acc_copy.Z,
					gyro_copy.x, gyro_copy.y, gyro_copy.z);
		}
	}
}

void calculations_heading_init(void)
{
	vSemaphoreCreateBinary( xSemaphore );
	xTaskCreate( calculations_heading, ( signed char * ) "UART", (200), NULL, tskIDLE_PRIORITY+1, NULL );
	// block on error creating semaphore
	while (xSemaphore == NULL)
		;
}

void calculations_heading_FromISR(void)
{
	static signed portBASE_TYPE xHigherPriorityTaskWoken;
	static uint16_t counter = 0;

	// Get data from both sensors + do some calculations

	if (counter++ == 100)
	{
		counter = 0;
		xHigherPriorityTaskWoken = pdFALSE;

		/* Unblock the task by releasing the semaphore. */
		xSemaphoreGiveFromISR(xSemaphore, &xHigherPriorityTaskWoken);
		portEND_SWITCHING_ISR( xHigherPriorityTaskWoken );
	}
}
