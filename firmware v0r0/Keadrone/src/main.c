/* Kernel includes. */
#include "FreeRTOS.h"
#include "task.h"

#include "LPC17xx.h"
#include "calculations_heading.h"

static void mainTask(void *pvParameters);
/*-----------------------------------------------------------*/

int main(void)
{

	initPwmServos();
	//timerInit();

	/* Start the two tasks as described in the accompanying application
	 note. */
	calculations_heading_init();
	//	xTaskCreate( idleTask, ( signed char * ) "TX", (300), NULL, tskIDLE_PRIORITY, NULL );

	/* Start the tasks running. */
	vTaskStartScheduler();

	/* If all is well we will never reach here as the scheduler will now be
	 running.  If we do reach here then it is likely that there was insufficient
	 heap available for the idle task to be created. */
	for (;;)
		;
}

/*-----------------------------------------------------------*/

/* External input interrupt on change handler */
void EINT3_IRQHandler(void)
{
	// only rising edge of Acc. meter is usefull and enabled.
	if (LPC_GPIOINT->IO0IntStatR & (1 << 8)) // GPIO 0.8
	{
		LPC_GPIOINT->IO0IntClr = 1 << 8;
		spiReqNewData_FromISR();
	}
	// gyro data ready interrupt
	else if (LPC_GPIOINT->IO0IntStatR & (1 << 7)) //GPIO 0.7
	{
		LPC_GPIOINT->IO0IntClr = 1 << 7;
		calculations_heading_FromISR();
	}
}
/*-----------------------------------------------------------*/
