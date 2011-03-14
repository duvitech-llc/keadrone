/*
 * filterIIR.h
 *
 *  Created on: 14 mrt 2011
 *      Author: Willem (wnpd.nl)
 */

#ifndef FILTERIIR_H_
#define FILTERIIR_H_

typedef struct
{
	int32_t IIR_Sum; //bigsum for IIR
	int32_t currReading; // current value
	int32_t IIR_HISTORY; // number of history samples
} IIR_DATA;

#endif /* FILTERIIR_H_ */
