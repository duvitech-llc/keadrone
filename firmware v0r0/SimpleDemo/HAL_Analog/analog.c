/*
 * analog.c
 *
 *  Created on: 28 jan 2011
 *      Author: Willem (wnpd.nl)
 */
#include "LPC17xx.h"

// ***********
// * Defines related to ADC
// ***********
// PCADC / PCAD
#define ADC_POWERON (1 << 12)
#define PCLK_ADC 24
#define PCLK_ADC_MASK (3 << 24)

// AD0.0 - P0.23, PINSEL1 [15:14] = 01
#define SELECT_ADC0 (0x1<<14)

// ADOCR constants
#define START_ADC (1<<24)
#define OPERATIONAL_ADC (1 << 21)
#define SEL_AD0 (1 <<0)
#define ADC_DONE_BIT	(1 << 31)
//WP BELOW ADD
#define SEL_AD2 (1 <<2)
#define SEL_AD5 (1 <<5)
#define SEL_AD6 (1 <<6)
#define SEL_AD7 (1 <<7)

void analogInit(void)
{
	// Turn on power to ADC block
	LPC_SC->PCONP |= ADC_POWERON;

	// Turn on ADC peripheral clock
	LPC_SC->PCLKSEL0 &= ~(PCLK_ADC_MASK);
	LPC_SC->PCLKSEL0 |= (3 << PCLK_ADC);

	// Set P0.23 to AD0.0 in PINSEL1
	//LPC_PINCON->PINSEL1 |= SELECT_ADC0;
	//P0.3 == AD0.6 == TEMP
	LPC_PINCON->PINSEL0 |= (2 << 6);
	LPC_PINCON->PINMODE0 |= (2 << 6);
	// P0.2 -- AD0.7 == maxsonar
	LPC_PINCON->PINSEL0 |= (2 << 4);
	LPC_PINCON->PINMODE0 |= (2 << 4);
	// P0.25 -- AD0.2 == batt. volt
	LPC_PINCON->PINSEL1 |= (1 << 18);
	LPC_PINCON->PINMODE1 |= (1 << 18);
	// P1.31 -- AD0.4 == batt. volt
	LPC_PINCON->PINSEL3 |= (3 << 30);
	LPC_PINCON->PINMODE3 |= (3 << 30);
	//max clock div.
	LPC_ADC->ADCR |= 0xFF << 8;
}

void analog(void)
{
	int adval, adval_64;

	// Start A/D conversion for on AD0.0
	LPC_ADC->ADCR = START_ADC | OPERATIONAL_ADC | SEL_AD6;

	do
	{
		adval = LPC_ADC->ADGDR; // Read A/D Data Register
	} while ((adval & ADC_DONE_BIT) == 0); // Wait for end of A/D Conversion

	// Stop A/D Conversion
	LPC_ADC->ADCR &= ~(START_ADC | OPERATIONAL_ADC | 0xFF);

	// Extract AD0.X value - 12 bit result in bits [15:4]
	adval = (adval >> 4) & 0x0FFF;

	//Convert integer ADC value to string
	//	sprintf(strbuf, "%04d", adval);
	//	printf(strbuf);
	//	printf("\n");
}
