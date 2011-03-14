/*
 * gyro-ITG3200.h
 *
 *  Created on: 10 feb 2011
 *      Author: Willem (wnpd.nl)
 */

#ifndef GYROITG3200_H_
#define GYROITG3200_H_

typedef struct
{
	volatile int16_t x;
	volatile int16_t y;
	volatile int16_t z;
	int16_t x_offset;
	int16_t y_offset;
	int16_t z_offset;
	volatile int16_t temp;
} GYRO_S;

xSemaphoreHandle xSemaphore;
GYRO_S gyro;

acc_data accCurrent;
void gyroGetDataFromChip(void);
void gyroInit(void);

#endif /* GYROITG3200_H_ */
