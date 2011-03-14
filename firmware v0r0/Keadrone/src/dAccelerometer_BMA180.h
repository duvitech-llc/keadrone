/*
 * dAccelerometer_BMA180.h
 *
 *  Created on: 13 feb 2011
 *      Author: Willem Pietersz
 */

#ifndef DACCELEROMETER_BMA180_H_
#define DACCELEROMETER_BMA180_H_

typedef struct
{
	int16_t X;
	int16_t Y;
	int16_t Z;
	uint8_t temp;
} sAcc_data;

void spiGetAccelero(sAcc_data *p);
void spiReqNewData_FromISR(void);
void spiInit(void);

/************Address defines for BMA180 *********/
//====================//
//ID and Version Registers
#define ID 0x00
#define Version 0x01
#define ACCXLSB 0x02
#define ACCXMSB 0x03
#define ACCYLSB 0x04
#define ACCYMSB 0x05
#define ACCZLSB 0x06
#define ACCZMSB 0x07
#define TEMPERATURE 0x08
#define STATREG1 0x09
#define STATREG2 0x0A
#define STATREG3 0x0B
#define STATREG4 0x0C
#define CTRLREG0 0x0D
#define CTRLREG1 0x0E
#define CTRLREG2 0x0F

#define BWTCS 0x20
#define CTRLREG3 0x21

#define HILOWNFO 0x25
#define LOWDUR 0x26

#define LOWTH 0x29

#define tco_y 0x2F
#define tco_z 0x30

#define OLSB1 0x35

//====================//
//Range setting
#define RANGESHIFT 1
#define RANGEMASK 0x0E
#define BWMASK 0xF0
#define BWSHIFT 4

// interrupt setting
#define NEW_DATA_INT_SHIFT	1

#endif /* DACCELEROMETER_BMA180_H_ */
