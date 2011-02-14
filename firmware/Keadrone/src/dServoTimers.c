/*
 * dServoTimers.c
 *
 *  Created on: 14 feb 2011
 *      Author: Willem (wnpd.nl)
 */

#include "LPC17xx.h"
#include "lpc17xx_clkpwr.h"

struct
{
	volatile uint32_t output1; // value between 100.0000 and 200.000
	volatile uint32_t output2; // value between 100.0000 and 200.000
	volatile uint32_t output3; // value between 100.0000 and 200.000
	volatile uint32_t output4; // value between 100.0000 and 200.000
	volatile Bool update_tmr0;
	volatile Bool update_tmr1;
} SERVO_TIMERS;

/* 10 bit value */
void servoSet(uint8_t servoNo, uint16_t value)
{
	switch (servoNo)
	{
	case 0:
		SERVO_TIMERS.output1 = 100000 + (value * 128);
		SERVO_TIMERS.update_tmr0 = TRUE;
		break;
	case 1:
		SERVO_TIMERS.output2 = 100000 + (value * 128);
		SERVO_TIMERS.update_tmr0 = TRUE;
		break;
	case 2:
		SERVO_TIMERS.output3 = 100000 + (value * 128);
		SERVO_TIMERS.update_tmr1 = TRUE;
		break;
	case 3:
		SERVO_TIMERS.output4 = 100000 + (value * 128);
		SERVO_TIMERS.update_tmr1 = TRUE;
		break;
	default:
		return;
	}

}

void timerInit(void)
{
	LPC_PINCON->PINSEL3 |= 0x3 << 24; //MAT0.0 @p0.28
	LPC_PINCON->PINSEL3 |= 0x3 << 26; //MAT0.1 @p0.29

	/* Set up clock and power for module */
	CLKPWR_ConfigPPWR(CLKPWR_PCONP_PCTIM0, ENABLE);
	CLKPWR_SetPCLKDiv(CLKPWR_PCLKSEL_TIMER0, CLKPWR_PCLKSEL_CCLK_DIV_1);

	/* Clock / desired ticks
	 * eg. 1mS pulse @100MHz clock = 100kHz ticks
	 * Match register:*/
	LPC_TIM0->MR0 = 100000; // 1mS
	LPC_TIM0->MR1 = 100000; // 1mS
	LPC_TIM0->MR2 = 500000; // 5mS (resets counter + interrupt)

	LPC_TIM0->MCR = (1 << 6) | (1 << 7); // interrupt + reset TMR0.2 on match

	LPC_TIM0->EMR |= 1 << 4; // EMx clear output on match
	LPC_TIM0->EMR |= 1 << 6; // EMx clear output on match

	NVIC_SetPriority(TIMER0_IRQn, 1);
	NVIC_EnableIRQ(TIMER0_IRQn);
	LPC_TIM0->TCR = 1;
}

/* External input interrupt on change handler */
void TIMER0_IRQHandler(void)
{
	LPC_TIM0->EMR |= 1 << 0; // EMx Enable high
	LPC_TIM0->EMR |= 1 << 1; // EMx Enable high

	if (SERVO_TIMERS.update_tmr0)
	{
		SERVO_TIMERS.update_tmr0 = FALSE;
		LPC_TIM0->MR0 = SERVO_TIMERS.output1; // 1mS
		LPC_TIM0->MR1 = SERVO_TIMERS.output2; // 1mS
	}

	// clear all timer type interrupts
	LPC_TIM0->IR = 0x1F;
}
