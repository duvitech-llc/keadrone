/*
 * dServoPwm.c
 *
 *  Created on: 15 feb 2011
 *      Author: Willem (wnpd.nl)
 */

#include "LPC17xx.h"
#include "lpc17xx_clkpwr.h"

void initPwmServos(void)
{
	LPC_PINCON->PINSEL3 |= 0x2 << 8; //PWM1.2 @p1.20
	LPC_PINCON->PINSEL3 |= 0x2 << 14; //PWM1.4 @p1.23-
	LPC_PINCON->PINSEL3 |= 0x2 << 16; //PWM1.5 @p1.24-
	LPC_PINCON->PINSEL3 |= 0x2 << 20; //PWM1.6 @p1.26

	/* Set up clock and power for module */
	CLKPWR_ConfigPPWR(CLKPWR_PCONP_PCPWM1, ENABLE);
	CLKPWR_SetPCLKDiv(CLKPWR_PCONP_PCPWM1, CLKPWR_PCLKSEL_CCLK_DIV_1);

	// ENABLE PWM OUTPUTS
	LPC_PWM1->PCR = (1 << 10) | (1 << 12) | (1 << 13) | (1 << 14);
	LPC_PWM1->MCR = 1 << 1; // CLEAR PWM REG ON MATCH PWM0.0

	// MATCH REGISTERS MR2 == PWM1.2, ETC.
	LPC_PWM1->MR0 = 125000; // CLEARS TIMER, THIS IS THE PWM FREQ.

	LPC_PWM1->MR2 = 25000;//25.000 == 1mS
	LPC_PWM1->MR4 = 25000;
	LPC_PWM1->MR5 = 25000;
	LPC_PWM1->MR6 = 25000;
	// LATCH VALUES FROM MATCH REGISTERS TO PWM REGISTERS
	LPC_PWM1->LER = (1 << 0) | (1 << 2) | (1 << 4) | (1 << 5) | (1 << 6);

	// ENABLE PWM & COUNTERS
	LPC_PWM1->TCR = 1 | (1 << 3);
}

/* 10 bit value */
void servoSet(uint8_t servoNo, uint16_t value)
{
	switch (servoNo)
	{
	case 0:
		LPC_PWM1->MR2 = 25000 + (value * 24);
		// LATCH VALUES FROM MATCH REGISTERS TO PWM REGISTERS
		LPC_PWM1->LER |= (1 << 2);
		break;
	case 1:
		LPC_PWM1->MR4 = 25000 + (value * 24);
		// LATCH VALUES FROM MATCH REGISTERS TO PWM REGISTERS
		LPC_PWM1->LER |= (1 << 4);
		break;
	case 2:
		LPC_PWM1->MR5 = 25000 + (value * 24);
		// LATCH VALUES FROM MATCH REGISTERS TO PWM REGISTERS
		LPC_PWM1->LER |= (1 << 5);
		break;
	case 3:
		LPC_PWM1->MR6 = 25000 + (value * 24);
		// LATCH VALUES FROM MATCH REGISTERS TO PWM REGISTERS
		LPC_PWM1->LER |= (1 << 6);
		break;
	default:
		return;
	}

}
