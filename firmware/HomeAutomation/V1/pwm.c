/*
 * File:   pwm.c
 * Author: Willem
 *
 * Created on January 6, 2012, 6:30 PM
 */

#include <p24Fxxxx.h>
#include <GenericTypeDefs.h>
/**
 * The following code will set the Output Compare 1 module
 * for PWM mode w/o FAULT pin enabled, a 50% duty cycle and a
 * PWM frequency of 52.08 kHz at Fosc = 8 MHz. Timer 2 is selected as
 * the clock for the PWM time base and Timer2 interrupts
 * are enabled.*
 */
#define PWM_BASE_FREQ 4095

void pwmInit(void)
{
    OC2CON1 = 0x0000; // Turn off Output Compare 1 Module
    OC2CON2 = 0;
    OC2R = 0; // Initialize Compare Register1 with 0x0026
    OC2RS = PWM_BASE_FREQ; // Initialize Secondary Compare Register1 with
    OC2CON1 = 7174; // perifferal clock and edge-pwm
    OC2CON2 = 0b11111; // use our own sync source

    OC3CON1 = 0x0000; // Turn off Output Compare 1 Module
    OC3CON2 = 0;
    OC3R = 0; // Initialize Compare Register1 with 0x0026
    OC3RS = PWM_BASE_FREQ; // Initialize Secondary Compare Register1 with
    OC3CON1 = 7174; // perifferal clock and edge-pwm
    OC3CON2 = 0b11111; // use our own sync source
}

#include <math.h>

/**
 * (255/(exp(255*0.014)-1)) (exp(x*0.014)-1)
 * @param value between 0 and 255
 */
UINT16 valueToPwm(UINT16 value)
{
#define MAX_STEPS_VALUE 255
#define STEEPNESS_EXP   0.012
    float output;
    output = (MAX_STEPS_VALUE / (expf((float) MAX_STEPS_VALUE * STEEPNESS_EXP) - 1));
    output = output * (expf((float) value * STEEPNESS_EXP) - 1);
    return (UINT16) output;
}

void pwmSetA(UINT16 value)
{
    UINT16 value2;
    value2 = valueToPwm(value);
    value2 *= (PWM_BASE_FREQ / 255);
    OC2R = value2;
}

void pwmSetB(UINT16 value)
{
    UINT16 value2;
    value2 = valueToPwm(value);
    value2 *= (PWM_BASE_FREQ / 255);
    OC3R = value2;
}
