/* 
 * File:   analog.c
 * Author: Willem
 *
 * Created on October 13, 2012, 7:17 PM
 */

#include "GenericTypeDefs.h"
#include "hardware.h"
#include "timer1.h"
#include <p24Fxxxx.h>

#define WINDOW_SIZE 31

typedef struct
{
    /** FIR filter history array with samples */
    volatile UINT32 history[WINDOW_SIZE];
    /** FIR filter average index */
    volatile UINT8 index;
    UINT FILTER_HISTORY;
} FIR_DATA_STRUCT;

UINT32 FIR_Average(FIR_DATA_STRUCT *p, INT32 value)
{
    // static to reduce interrupt stack
    static UINT8 i;
    static UINT32 average;
    average = 0;

    if (WINDOW_SIZE - 1 < p->FILTER_HISTORY)
        while (1);

    // Replace oldest reading
    p->history[p->index] = value;
    // Sum all reading values
    for (i = 0; i < p->FILTER_HISTORY; i++) {
        average += p->history[i];
    }
    average = (UINT32) (average / p->FILTER_HISTORY);
    // Update index for next function call
    p->index++;
    if (p->index > (p->FILTER_HISTORY - 1)) {
        p->index = 0;
    }
    return average;
}

static FIR_DATA_STRUCT sensor1 = {0};

UINT16 adc_getReading(void)
{
    // select channel AN0
    _CH0SA = 0;
    int tmr;
    for (tmr = 0; tmr < 15; tmr++) Nop();
    // clear flag so ADC can set them in hardware
    _AD1IF = 0;
    // Stop sampling, conversion will start
    _SAMP = 0;
    // wait for conversion to be done
    while (_AD1IF == FALSE)
        ;
    return (ADC1BUF0);
}

/* Returns a value between 0 and 4. Where 0 means Humidity OK, and
 * 4 means Humidity very wet */
UINT16 sensor_getValue(void)
{

    /* Get samples and average them */
    UINT32 value = 0;
    value += adc_getReading();
    value += adc_getReading();
    value += adc_getReading();
    value += adc_getReading();
    value /= 4;
    /* put them in an average filter*/
    UINT16 average;
    sensor1.FILTER_HISTORY = 30;
    average = (UINT16) FIR_Average(&sensor1, value);

    /* implement some hysteresis */
    static UINT16 avgHistory = 0;
    if (avgHistory < (average - 50) || (average + 50) < avgHistory)
        avgHistory = average;
    else average = avgHistory;

    /* return the value based on humidity */
    if (average < 2300) // 2300 equals 70%
        return 0;
    else if (average < 2500) // 75
        return 1;
    else if (average < 2700) // 80
        return 2;
    else if (average < 2900) // 90
        return 3;
    else
        return 4;
}

void adc_init(void)
{
    AD1CON1 = 0;
    AD1CON2 = 0;
    AD1CON3 = 0;

    AD1CON5 = 0;

    AD1CON3bits.ADRC = 1; //1 = ADC Internal RC Clock (= 250nS = 4Mhz)
    AD1CON3bits.ADCS = 0; //63; // ADC Conversion Clock

    AD1CON2bits.SMPI = 0; // x+1 ADC buffers will fill, when done-> interrupt
    /* dont enable the external Vref since they exceed the silicon specs!!! */
    //_PVCFG = 0b01; // external Vref+
    //_NVCFG = 0b01; // external Vref-

    IFS0bits.AD1IF = 0; // Clear the A/D interrupt flag bit
    IEC0bits.AD1IE = 0; // Enable A/D interrupt
    AD1CON1bits.ASAM = 1; // 1 = ADC Sample Control: Sampling begins immediately after conversion

    AD1CON1 |= (1 << 10); // enable 12bit mode

    AD1CON1bits.ADON = 1; // Turn on the A/D converter

    // takes 20uS to stabilize from off->on ADC
    while (AD1CON1bits.SAMP == FALSE)
        ;
}
