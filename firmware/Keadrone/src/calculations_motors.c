/*
 * calculations_motors.c
 *
 *  Created on: 14 mrt 2011
 *      Author: Willem (wnpd.nl)
 */
#include "FreeRTOS.h"
#include "task.h"
#include "semphr.h"

#include "DCM_Data.h"

/* PID calculations ********************************************/

typedef struct
{
	int32_t pTerm;
	int32_t iTerm;
	int32_t dTerm;

	float currError; // error from current sensor data minus set point

	int32_t setpoint; // requested set point
	int32_t pwrOffset; // will be added to output
	float output;
} PID_PARAM;

#define dErrorLimit 120
int32_t pidCalc(PID_PARAM *p)
{
	/****Proportional**************/
	p->output = (float) p->currError * p->pTerm;

	/*****Derivative **************/
	int32_t derivative_term;
	static int32_t en0, en1, en2; // old errors
	en0 = p->currError;
	//Calculate the differential term
	derivative_term = en0 - en2;
	if (derivative_term > dErrorLimit)
		derivative_term = dErrorLimit;
	if (derivative_term < -dErrorLimit)
		derivative_term = -dErrorLimit;
	derivative_term = derivative_term * p->dTerm;
	//	derivative_term = derivative_term >> 5;
	//divide by 32

	if (derivative_term > dErrorLimit)
		derivative_term = dErrorLimit;
	if (derivative_term < -dErrorLimit)
		derivative_term = -dErrorLimit;

	// save the new error value
	en2 = en1;
	en1 = en0;

	return ((int16_t) p->output);
}

void servoCalc(uint16_t *servoA, uint16_t *servoB, PID_PARAM *p)
{
	int32_t calc;
	calc = p->pwrOffset - p->output;
	if (calc < 0)
		calc = 0;
	else if (calc > 380)
		calc = 380; // limit output power
	*servoA = (uint16_t) calc;

	calc = p->pwrOffset + p->output;
	if (calc < 0)
		calc = 0;
	else if (calc > 380)
		calc = 380;
	*servoB = (uint16_t) calc;

}

/**********************************/

PID_PARAM axisX;
PID_PARAM axisY;
PID_PARAM axisZ;
static uint16_t currentPower = 330;

void initCalculations(void)
{
	axisX.pTerm = -60;
	axisX.iTerm = 0;
	axisX.dTerm = 6;

	axisY.pTerm = -60;
	axisY.iTerm = 0;
	axisY.dTerm = 6;

	axisZ.pTerm = -60;
	axisZ.iTerm = 0;
	axisZ.dTerm = 6;

}

void calculations_motor(void)
{
	static int16_t initWarmup = 1000;
	if (initWarmup != 0)
	{
		initWarmup--;
		if (initWarmup == 0)
			initCalculations();
		return;
	}

	uint16_t servoA, servoB;

	axisZ.currError = getHeadingYaw() - axisZ.setpoint;
	pidCalc(&axisZ);
	// limit yaw
	if (axisZ.output > 50)
		axisZ.output = 50;
	else if (axisZ.output < -50)
		axisZ.output = -50;

	// get the current power offset for all motors
	axisY.pwrOffset = axisX.pwrOffset = currentPower;

	// adjust X/Y engines to correct the YAW
	axisX.pwrOffset -= axisZ.output;
	axisY.pwrOffset += axisZ.output;

	// Y horizontal calculations
	axisY.currError = getHeadingPitch() - axisY.setpoint;
	pidCalc(&axisY);
	servoCalc(&servoA, &servoB, &axisY);
	servoSet(3, servoA);
	servoSet(1, servoB);

	// Z horizontal calculations
	axisX.currError = getHeadingRoll() - axisX.setpoint;
	pidCalc(&axisX);
	servoCalc(&servoA, &servoB, &axisX);
	servoSet(0, servoA);
	servoSet(2, servoB);

}
