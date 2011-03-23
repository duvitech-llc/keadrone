/* Kernel includes. */
#include "FreeRTOS.h"
#include "task.h"

#include "LPC17xx.h"
#include "calculations_heading.h"

static void mainTask(void *pvParameters);
/*-----------------------------------------------------------*/
void initExternalInt(void);

int main(void)
{

	initExternalInt();
	while (1)
		;

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
#include "queue.h"
#include "lpc17xx_clkpwr.h"

static xQueueHandle xQueueCaptureTimes;
typedef struct
{
	uint32_t values[4];
} CAPTURE_STRUCT;

CAPTURE_STRUCT volatile capture;

void createQueue(void)
{
	// Create a queue capable of containing 10 unsigned long values.
	xQueueCaptureTimes = xQueueCreate(1, sizeof(CAPTURE_STRUCT));
	while (xQueueCaptureTimes == 0)
	{
		// Queue was not created and must not be used.
	}
}

void initExternalInt(void)
{
	CLKPWR_ConfigPPWR(CLKPWR_PCONP_PCGPIO, ENABLE);
	//Enable rising edge interrupt for P2.[0..3]
	uint32_t mask;
	mask = (1 << 0) + (1 << 1) + (1 << 2) + (1 << 3);
	LPC_GPIOINT->IO2IntEnR |= mask;
	//Enable FALLING edge interrupt for P2.[0..3]
	LPC_GPIOINT->IO2IntEnF |= mask;

	//init timer3
	// enable tmr3
	CLKPWR_ConfigPPWR(CLKPWR_PCONP_PCTIM3, ENABLE);
	// set clock to tmr3
	CLKPWR_SetPCLKDiv(CLKPWR_PCLKSEL_TIMER3, CLKPWR_PCLKSEL_CCLK_DIV_1);

	/* speed of tmr3:
	 * CLK / 4 = 100MHz
	 * 32bit / 100MHz = 42.75Seconds
	 * measurement is between 1 to 2ms. I need at least 1024 steps
	 * Dt = 1mS / 1024 = ~1MHz but I take 4mhz resolution:
	 *
	 * 100MHz / 4 = 25prescaler
	 * 25 * 42.75 = 17.8125 minutes
	 */
	LPC_TIM3->PR = 25 - 1; // prescaler
	//LPC_TIM3->TCR |= 2;// reset counter
	LPC_TIM3->TCR |= 1;// enable counter

	LPC_TIM3->TC;// tmr3 counter value
	// timer is free running, no interrupts and/or resets.

	//createQueue();

	// enable interrupts, they will fire right away
	NVIC_SetPriority(EINT3_IRQn, 30);
	NVIC_EnableIRQ(EINT3_IRQn);

}

uint32_t timerGetCurrentTime(void)
{
	uint32_t value;
	value = LPC_TIM3->TC;
	return value;
}

void capturePostData_FromISR(CAPTURE_STRUCT *p)
{
	portBASE_TYPE xHigherPriorityTaskWoken = pdFALSE;

	/* Post the struct */
	xQueueSendFromISR(xQueueCaptureTimes, p, &xHigherPriorityTaskWoken);
	if (xHigherPriorityTaskWoken)
	{
		/* Actual macro used here is port specific. */
		taskYIELD_FROM_ISR();
	}
}

/* Blocks max. 100mS. If returns FALSE no data was captured anymore for more than 100mS*/
Bool captureGetData(CAPTURE_STRUCT *p)
{
	if (xQueueReceive(xQueueCaptureTimes, p, (portTickType) 100))
	{
		return TRUE;
	}
	else
		return FALSE;
}

/* Sets current time in the register specified
 * and clears the input bit that is checked*/
uint16_t rfTriggerBitTest(uint16_t *input, uint32_t *array)
{
	if (*input & 1)
	{
		*input &= ~1;
		uint32_t value;
		value = timerGetCurrentTime();
		*array = timerGetCurrentTime();
		return 1;
	}
	else if (*input & 2)
	{
		*input &= ~2;
		*(array + 1) = timerGetCurrentTime();
		return 2;
	}
	else if (*input & 4)
	{
		*input &= ~4;
		*(array + 2) = timerGetCurrentTime();
		return 3;
	}
	else if (*input & 8)
	{
		*input &= ~8;
		*(array + 3) = timerGetCurrentTime();
		return 4;
	}
	return 0;
}

/* Called each time a capture inputs changes from state */
void rfTriggerInput(uint16_t inputsF, uint16_t inputsR)
{
	static uint32_t extCaptureTimeR[4] =
	{ 0 };
	static uint32_t extCaptureTimeF[4] =
	{ 0 };
	static Bool inputUpdated[4] =
	{ 0 };

	if (inputsR)
	//rising edges
	{
		rfTriggerBitTest(&inputsR, extCaptureTimeR);
	}

	if (inputsF)
	// falling edges
	{
		uint16_t channel;
		channel = rfTriggerBitTest(&inputsF, extCaptureTimeF);

		// calculate new times

		// falling time should always be larger than rising time
		if (extCaptureTimeF > extCaptureTimeR && channel != 0)
		{
			uint32_t value;
			value = extCaptureTimeF - extCaptureTimeR;
			capture.values[channel] = value; // store the new time
			inputUpdated[channel] = TRUE;

			// test if we have new data for all 4 channels
			for (value = 0; value < 4; value++)
			{
				if (inputUpdated == FALSE)
					break;
				else if (value == 3)
				// all values are new! Post it.
				{
					//capturePostData_FromISR(&capture);
					// clear all events
					memset(inputUpdated, 0, sizeof(inputUpdated));
				}
			}
		}
	}
}

/* External input interrupt on change handler */
void EINT3_IRQHandler(void)
{
	static uint16_t bitTestF, bitTestR;
	uint32_t mask;
	mask = (1 << 0) + (1 << 1) + (1 << 2) + (1 << 3);
	bitTestR = LPC_GPIOINT->IO2IntStatR & mask;
	bitTestF = LPC_GPIOINT->IO2IntStatF & mask;
	// rf servo input checked first due time measurement
	if (bitTestF || bitTestR)
	{
		rfTriggerInput(bitTestF, bitTestR);
	}
	// only rising edge of Acc. meter is usefull and enabled.
	else if (LPC_GPIOINT->IO0IntStatR & (1 << 8)) // GPIO 0.8
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
