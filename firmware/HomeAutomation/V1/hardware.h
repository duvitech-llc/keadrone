/*
 * File:   hardware.h
 * Author: admin
 *
 * Created on 25 oktober 2011, 8:57
 */

#ifndef hardware_H
#define hardware_H

// #define demo
//#define demo_voor_bert

/* logica board defines */
#define OUTPUT  0
#define INPUT   1
#define LED_COMMON_TRIS _TRISB8
#define LED_RED_TRIS    _TRISB7
#define LED_RED         _LATB7

#define ROTARY_1_PU       _CN29PUE
#define ROTARY_2_PU       _CN6PUE
#define ROTARY_4_PU       _CN30PUE
#define ROTARY_8_PU       _CN7PUE

#define ROTARY_1        _RA3
#define ROTARY_2        _RB2
#define ROTARY_4        _RA2
#define ROTARY_8        _RB3
/* end - logica board defines */

#define RELAY_1_TRIS    _TRISB13
#define RELAY_1_TRANSFORMER _LATB13

#define RELAY_2_TRIS    _TRISA1
#define RELAY_2_FAN     _LATA1

#define LED_1_TRIS      _TRISB10
/** OC3 for PWM */
#define LED_1_BADKAMER  _LATB10

#define LED_2_TRIS  _TRISB11
/** OC2 for PWM */
#define LED_2_WC    _LATB10

#define FAN_LV_TRIS _TRISB5
#define FAN_LV      _LATB5

#define SW_WC      _RB14
#define SW_BADKAMER _RB15

void pwmSetA(UINT16 value);
void pwmSetB(UINT16 value);
void pwmInit(void);


#endif
