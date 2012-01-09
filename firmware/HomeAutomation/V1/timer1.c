/*
 * File:   timer1.c
 * Author: admin
 *
 * Created on 25 oktober 2011, 14:04
 */
#include "GenericTypeDefs.h"
#include "timer1.h"
#include <p24Fxxxx.h>

BOOL tmrExpired500mS = FALSE;
BOOL tmrExpired1mS = TRUE;
BOOL tmrExpired1minute = FALSE;

/* timer init defines */

#define STOP_TIMER_IN_IDLE_MODE     0x2000
#define TIMER_SOURCE_INTERNAL       0x0000
#define TIMER_ON                    0x8000
#define GATED_TIME_DISABLED         0x0000
#define TIMER_16BIT_MODE            0x0000
#define TIMER_PRESCALER_1           0x0000
#define TIMER_PRESCALER_8           0x0010
#define TIMER_PRESCALER_64          0x0020
#define TIMER_PRESCALER_256         0x0030
#define TIMER_INTERRUPT_PRIORITY    0x1000

#define TIMER_PRESCALER		TIMER_PRESCALER_64	//divide by 8
#define TIMER_PERIOD		250;// 250@64prescaler = 1mS

void __attribute__((interrupt, auto_psv)) _T3Interrupt(void)
{
    // Clear flag
    _T3IF = 0;

    tmrExpired1mS = TRUE;
    //#include "hardware.h"
    //LED_RED ^= TRUE;
    static int counter = 0;
    if (++counter == 500) {
        counter = 0;
        tmrExpired500mS = TRUE;

        static int counter1minute = 0;
        if (++counter1minute == 20*6)
            tmrExpired1minute = TRUE;
    }
}

void tmrInit1(void)
{

    //@ 8MHz clock with 4x PLL = 32MHz
    // 16MHz instruction cycle 1/16000000 * 8x prescale * 2000 = 1mS
    //@pr120 1000 == 2mS

    TMR3 = 0;
    PR3 = TIMER_PERIOD;

    T3CON = TIMER_ON | TIMER_SOURCE_INTERNAL | GATED_TIME_DISABLED
            | TIMER_16BIT_MODE | TIMER_PRESCALER;

    _T3IP = 1;
    _T3IF = 0; //Clear flag
    _T3IE = 1; //Enable interrupt
    T3CONbits.TON = 1; //Run timer
}
