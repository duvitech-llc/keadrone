/*
 * filterIIR.c
 *
 *  Created on: 14 mrt 2011
 *      Author: Willem (wnpd.nl)
 */
#include "stdint.h"
#include "filterIIR.h"


uint16_t IIR_Average(IIR_DATA *p)
{
	// on boot-up, our value is never 0xFFFFFFFF
	if (p->IIR_Sum == 0xFFFFFFFF)
		p->IIR_Sum = (int32_t) p->currReading * p->IIR_HISTORY;

	p->IIR_Sum -= (int32_t)(p->IIR_Sum / p->IIR_HISTORY);
	p->IIR_Sum += p->currReading;

	// note, this filter has an gain of HISTORY
	// therefore we must divide the average value
	// with HISTORY to get our current average
	return (uint16_t)(p->IIR_Sum / p->IIR_HISTORY);
}
