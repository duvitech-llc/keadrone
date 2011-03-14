/*
 * dServoPwm.h
 *
 *  Created on: 15 feb 2011
 *      Author: Willem (wnpd.nl)
 */

#ifndef DSERVOPWM_H_
#define DSERVOPWM_H_

/* 10 bit value */
void servoSet(uint8_t servoNo, uint16_t value);
void initPwmServos(void);

#endif /* DSERVOPWM_H_ */
