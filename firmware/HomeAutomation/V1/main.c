/*
 * File:   main.c
 * Author: admin
 *
 * Created on 25 oktober 2011, 8:23
 */
#include "GenericTypeDefs.h"
#include "hardware.h"
#include "timer1.h"
#include <p24Fxxxx.h>


_FOSCSEL(FNOSC_FRCPLL)
_FICD(ICS_PGx3)
_FOSC(POSCMOD_NONE & OSCIOFNC_OFF)

PRIVATE void oscConfig(void);
PRIVATE void clearIntrflags(void);
#ifdef WARNING
todo;

adc priority error -> should be the calling timer
#endif


typedef struct
{

    union
    {

        struct
        {
            unsigned enabled : 1;
            unsigned enabledMirror : 1;
            unsigned morningWakeLight : 1;
            unsigned dimloop : 1;
            unsigned softstart : 1;
            unsigned softOff : 1;
            unsigned dimloopToggle : 1;
        };
        UINT16 unionValue; // value of union
    };
    UINT16 tmrMorningWakeLights;
    UINT16 dimvalue; // actual value of the dimming
    UINT16 tmrAutoShutdownLights;
} LIGHT_DATA;

LIGHT_DATA badkamer = {0};
LIGHT_DATA wc = {0};
#define DIMVALUE_MAX    0x3FF
#define DIMVALUE_DIMMED    150

struct
{
    BOOL enabled;
    UINT16 naloopTimer;
} fan;

UINT16 lightController(LIGHT_DATA * p)
{
    if (p->morningWakeLight == TRUE && p->dimvalue < DIMVALUE_MAX && p->enabled == TRUE) {
        p->softstart = FALSE; //make sure soft start is disabled
        p->dimloop = FALSE;

        if (p->tmrMorningWakeLights++ == 700) {
            p->dimvalue++;
            p->tmrMorningWakeLights = 0;
        }
        return p->dimvalue;
    }
    else {
        p->morningWakeLight = FALSE;
    }

    if (p->enabled == TRUE) {
        if (p->enabledMirror == FALSE && p->dimloop == FALSE) {
            // POWER ON - short button press soft-start light to max.
            p->softstart = 1;
        }
        else if (p->enabledMirror == FALSE && p->dimloop == TRUE) {
            p->dimvalue = DIMVALUE_DIMMED; // initial value
        }
        p->enabledMirror = TRUE; // mark that we are online now

        if (p->softstart == TRUE && p->dimloop == FALSE) {
            // increase output to max
            p->dimvalue++;
            if (DIMVALUE_MAX == p->dimvalue)
                p->softstart = 0;
        }

        if (p->dimloop == TRUE) {
            p->softstart = 0; //make sure soft start is disabled

            switch (p->dimloopToggle) {
            case TRUE:
                p->dimvalue++;
                if (DIMVALUE_MAX <= p->dimvalue)
                    p->dimloopToggle ^= 1;
                break;
            case FALSE:
                p->dimvalue--;
                if (p->dimvalue < DIMVALUE_DIMMED)
                    p->dimloopToggle ^= 1;
                break;
            }
        }
    }
    else if (p->enabled == FALSE) {

        if (p->enabledMirror == TRUE) {
            p->softOff = TRUE;
            p->enabledMirror = FALSE;
        }

        if (p->softOff == TRUE) {
            // power down softly
            //p->dimvalue = 0; //overide
            if (p->dimvalue-- == 0) {
                p->softOff = FALSE;
                p->dimvalue = 0;
                p->unionValue = 0;
            }
        }
    }
    return p->dimvalue;
}

#include "inputDebounce.h"

void serviceUserEvents(void)
{
    static BTN_DEBOUNCE_STRUCT btn1 = {0};
    static BTN_DEBOUNCE_STRUCT btn2 = {0};
    BTN_DEBOUNCE_STRUCT *btnX;

    btnX = &btn1;
    btnX->rawStatus = !SW_BADKAMER;
    static int shortPressedTimes = 0;
    if (btnDebounce(btnX)) {
        switch (btnX->debouncedBtnState) {
        case BTN_DISABLED_LONG:
            badkamer.enabled = FALSE;
            break;
        case BTN_DISABLED_SHORT:
            shortPressedTimes++;
            if (shortPressedTimes == 1) {
                //if (badkamer.softstart == TRUE)
                badkamer.softstart = FALSE;
                //else {
                //badkamer.dimloop ^= TRUE;
                badkamer.morningWakeLight = FALSE;
                badkamer.dimvalue = 500;
                //}
            }
            else if (shortPressedTimes == 2) {
                //badkamer.morningWakeLight = TRUE;
                badkamer.dimvalue = 250;
            }
            else if (shortPressedTimes == 2) {
                badkamer.morningWakeLight = TRUE;
                badkamer.dimvalue = 250;
            }

            break;
        case BTN_ENABLED:
            if (badkamer.enabled == FALSE) {
                badkamer.enabled = 1;
                badkamer.dimloop = FALSE;
                badkamer.dimloopToggle = 1;
            }
            break;

        case BTN_ENABLED_LONG:
            shortPressedTimes = 0;
            break;

        }
    }

    static int shortPressedTimes2 = 0;

    btnX = &btn2;
    btnX->rawStatus = !SW_WC;
    if (btnDebounce(btnX)) {
        switch (btnX->debouncedBtnState) {
        case BTN_DISABLED_LONG:
            wc.dimloop = FALSE;
            wc.enabled = 0;
            break;
        case BTN_DISABLED_SHORT:
            shortPressedTimes2++;
            if (shortPressedTimes2 == 1) {
                //if (wc.softstart == TRUE)
                wc.softstart = FALSE;
                //else
                //wc.dimloop ^= TRUE;
                wc.dimvalue = 500;
            }
            if (shortPressedTimes2 == 2) {
                wc.softstart = FALSE;
                wc.dimvalue = 250;
            }
            break;
        case BTN_ENABLED:
            wc.enabled = 1;
            wc.dimloopToggle = 1;
            break;

        case BTN_ENABLED_LONG:
            shortPressedTimes2 = 0;
            break;
        }
    }
}
#define AUTO_SHUTDOWN_IN_MINUTES 120
#define AUTO_SHUTDOWN_IN_MINUTES_WC 20

/**
 * Call this every 1second
 */
void lightsAutoOfftmr(void)
{
    if (badkamer.enabled) {
        if (badkamer.tmrAutoShutdownLights++ == AUTO_SHUTDOWN_IN_MINUTES) {
            badkamer.enabled = FALSE;
        }
    }
    else
        badkamer.tmrAutoShutdownLights = 0;

    if (wc.enabled) {
        if (wc.tmrAutoShutdownLights++ == AUTO_SHUTDOWN_IN_MINUTES_WC) {
            wc.enabled = FALSE;
        }
    }
    else
        wc.tmrAutoShutdownLights = 0;

}

void systemAutoShutdownWhenIdle(void)
{
    static UINT16 tmrShutdown = 0;

    if (wc.enabled == FALSE && badkamer.enabled == FALSE && fan.enabled == FALSE) {
        // begin countdown to shutdown system
        if (tmrShutdown == 10);
        else if (++tmrShutdown == 10) {
            RELAY_1_TRANSFORMER = 0; // shutdown system
        }
    }
    else {
        tmrShutdown = 0; // clear the shutdown timer
        RELAY_1_TRANSFORMER = 1; // make sure power system is enabled
    }
}

/**
 *  Call this every 1 sec.
 */
void sensorOperation(void)
{
    // only execute hygrometer if bathroom light is enabled
    // or if the fan is executing nalooptijd
    if (badkamer.enabled == FALSE && fan.enabled == FALSE)
        return;
    switch (sensor_getValue()) {
    case 0:
        if (fan.enabled == FALSE)
            pwmSetFan(0);
        else if (fan.naloopTimer != 0) // start countdown
            fan.naloopTimer--;
        else // disable fan when countdown expired
            fan.enabled = FALSE;

        break;
    case 1:
        pwmSetFan(75); // geruisloos maar draait
        fan.enabled = 1;
        fan.naloopTimer = 60 * 5;
        break;
    case 2:
        pwmSetFan(80);
        break;
    case 3:
        pwmSetFan(90);
        break;
    case 4:
        pwmSetFan(100);
        break;
    }
}

int main(void)
{
    clearIntrflags();
    oscConfig();
    tmrInit1();
    pwmInit();
    adc_init();

    // all ports digital
    ANSA = 1; // an1 = analog
    ANSB = 0;

    //LED_COMMON_TRIS = 0; not used since this port is now used as PWM output for FAN
    LED_RED_TRIS = 0;
    LED_RED = 0;

    RELAY_1_TRIS = 0;
    RELAY_2_TRIS = 0;
    RELAY_1_TRANSFORMER = 1;

    while (1) {
        ClrWdt();
        if (tmrExpired1mS) {
            tmrExpired1mS = 0;

            static UINT tmrExpired10mS = 9;
            if (++tmrExpired10mS == 10) {
                tmrExpired10mS = 0;
                serviceUserEvents();
            }

            static UINT tmrExpired2mS = 1;
            if (++tmrExpired2mS == 2) {
                tmrExpired2mS = 0;

            }
            pwmSetA(lightController(&wc));
            pwmSetB(lightController(&badkamer));
        }

        if (tmrExpired500mS) {
            tmrExpired500mS = FALSE;
            systemAutoShutdownWhenIdle();

            static UINT tmrExpired1000mS = 0;
            if (++tmrExpired1000mS == 2) {
                tmrExpired1000mS = 0;
                sensorOperation();
            }

        }

        if (tmrExpired1minute) {

            tmrExpired1minute = FALSE;
            lightsAutoOfftmr();
        }
    }

    return 0;
}

PRIVATE void oscConfig(void)
{

    /*  Configure Oscillator to operate the device at 40Mhz
    Fosc= Fin*M/(N1*N2), Fcy=Fosc/2
    Fosc= 8M*40/(2*2)=80Mhz for 8M input clock */

    OSCTUN = 23; /* Tune FRC oscillator, if FRC is used */
    _RCDIV = 0;
    while (OSCCONbits.LOCK != 1) {
    };
}

PRIVATE void clearIntrflags(void)
{

    /* Clear Interrupt Flags */
    IFS0 = 0;
    IFS1 = 0;
    //    IFS3 = 0;
    IFS4 = 0;
}

#ifdef UNUSED

PRIVATE void ioPortsInit(void)
{


    LED_COMMON_TRIS = OUTPUT;
    LED_RED_TRIS = OUTPUT;
    LED_RED = TRUE;

    ROTARY_1_PU = TRUE;
    ROTARY_2_PU = TRUE;
    ROTARY_4_PU = TRUE;
    ROTARY_8_PU = TRUE;

    UINT8 slaveAddres = 1;
    if (!ROTARY_1) slaveAddres += 1;
    if (!ROTARY_2) slaveAddres += 2;
    if (!ROTARY_4) slaveAddres += 4;

    if (!ROTARY_8) slaveAddres += 8;
}
#endif
#ifdef __DEBUG
#define _trapISR __attribute__((interrupt,no_auto_psv))

/* ****************************************************************
 * Standard Exception Vector handlers if ALTIVT (INTCON2<15>) = 0  *
 *                                                                 *
 * Not required for labs but good to always include                *
 ******************************************************************/
void _trapISR
_OscillatorFail(void)
{

    INTCON1bits.OSCFAIL = 0;

    while (1);
}

void _trapISR
_AddressError(void)
{

    INTCON1bits.ADDRERR = 0;

    while (1);
}

void _trapISR
_StackError(void)
{

    INTCON1bits.STKERR = 0;

    while (1);
}

void _trapISR
_MathError(void)
{

    INTCON1bits.MATHERR = 0;

    while (1);
}

/* ****************************************************************
 * Alternate Exception Vector handlers if ALTIVT (INTCON2<15>) = 1 *
 *                                                                 *
 * Not required for labs but good to always include                *
 ******************************************************************/
void _trapISR
_AltOscillatorFail(void)
{

    INTCON1bits.OSCFAIL = 0;

    while (1);
}

void _trapISR
_AltAddressError(void)
{

    INTCON1bits.ADDRERR = 0;

    while (1);
}

void _trapISR
_AltStackError(void)
{

    INTCON1bits.STKERR = 0;

    while (1);
}

void _trapISR
_AltMathError(void)
{

    INTCON1bits.MATHERR = 0;
    while (1);
}

#endif
