/* Kernel includes. */
#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"

#include "semphr.h"
#include "bma180.h"
#include "gyro-ITG3200.h"
#include "lpc17xx_clkpwr.h"

/* The rate at which data is sent to the queue, specified in milliseconds. */
#define mainQUEUE_SEND_FREQUENCY_MS			( 100 / portTICK_RATE_MS )

/* The number of items the queue can hold.  This is 1 as the receive task
 will remove items as they are added, meaning the send task should always find
 the queue empty. */
#define mainQUEUE_LENGTH					( 1 )

/*
 * The tasks as described in the accompanying PDF application note.
 */
static void mainTask(void *pvParameters);
static void idleTask(void *pvParameters);

/* The queue used by both tasks. */
static xQueueHandle xQueue = NULL;

xSemaphoreHandle xSemaphore;
/*-----------------------------------------------------------*/

int main(void)
{
	/* Create the queue. */
	//xQueue = xQueueCreate(mainQUEUE_LENGTH, sizeof(unsigned long));xQueue != NULL &&

	vSemaphoreCreateBinary( xSemaphore );

	if (xSemaphore != NULL)
	{
		/* Start the two tasks as described in the accompanying application
		 note. */
		xTaskCreate( mainTask, ( signed char * ) "Rx", (200), NULL, tskIDLE_PRIORITY+1, NULL );
		xTaskCreate( idleTask, ( signed char * ) "TX", (300), NULL, tskIDLE_PRIORITY, NULL );

		/* Start the tasks running. */
		vTaskStartScheduler();
	}

	/* If all is well we will never reach here as the scheduler will now be
	 running.  If we do reach here then it is likely that there was insufficient
	 heap available for the idle task to be created. */
	for (;;)
		;
}
/*-----------------------------------------------------------*/

#define BUFSIZE 10

static void mainTask(void *pvParameters)
{
	portTickType xNextWakeTime;
	const unsigned long ulValueToSend = 100UL;

	/* Initialize xNextWakeTime - this only needs to be done once. */
	xNextWakeTime = xTaskGetTickCount();

	UARTInit(3, 115200); /* baud rate setting */
	printf("Maintask: Running\n");

	CLKPWR_ConfigPPWR(CLKPWR_PCONP_PCGPIO, ENABLE);
	spiInit();
	gyroInit();

	while (1)
	{
		vTaskDelay(1);
	}
}

/*-----------------------------------------------------------*/

static void idleTask(void *pvParameters)
{
	uint32_t ulReceivedValue;

	/* Initialize P1_1 for the LED. */
	LPC_PINCON->PINSEL2 &= (~(3 << 2));
	LPC_GPIO1->FIODIR |= (1 << 1);

	for (;;)
	{
		uint32_t ulLEDState;
#ifdef disabled
		/* Obtain the current P0 state. */
		ulLEDState = LPC_GPIO1->FIOPIN;

		/* Turn the LED off if it was on, and on if it was off. */
		LPC_GPIO1->FIOCLR = ulLEDState & (1 << 1);
		LPC_GPIO1->FIOSET = ((~ulLEDState) & (1 << 1));
#endif
#define LONG_TIME 0xffff
		/*Block waiting for the semaphore to become available. */
		if (xSemaphoreTake( xSemaphore, LONG_TIME ) == pdTRUE)
		{
			/* It is time to execute. */

			static acc_data acc_copy;
			static GYRO_S gyro_copy;

			acc_copy = accCurrent;
			gyro_copy = gyro;
			printf("%d,%d,%d,%d,%d,%d\n", acc_copy.X, acc_copy.Y, acc_copy.Z,
					gyro_copy.x, gyro_copy.y, gyro_copy.z);
			/* We have finished our task.  Return to the top of the loop where
			 we will block on the semaphore until it is time to execute
			 again.  Note when using the semaphore for synchronisation with an
			 ISR in this manner there is no need to 'give' the semaphore back. */
		}

//		vTaskDelay(500);
	}
}
/*-----------------------------------------------------------*/

/* External input interrupt on change handler */
void EINT3_IRQHandler(void)
{
	// only rising edge of Acc. meter is usefull and enabled.
	if (LPC_GPIOINT->IO0IntStatR & (1 << 8)) // GPIO 0.8
	{
		LPC_GPIOINT->IO0IntClr = 1 << 8;
		spiPoll();
	}
	// gyro data ready interrupt
	else if (LPC_GPIOINT->IO0IntStatR & (1 << 7)) //GPIO 0.7
	{
		LPC_GPIOINT->IO0IntClr = 1 << 7;
		gyroGetDataFromChip();
	}
}
/*-----------------------------------------------------------*/
