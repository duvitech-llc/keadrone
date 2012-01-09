/*
 * File:   inputDebounce.c
 * Author: Willem
 *
 * Created on January 7, 2012, 10:18 PM
 */
#include <GenericTypeDefs.h>
#include "inputDebounce.h"

#define PRESSED     1
#define RELEASED    0

/**
 * Must be called every 10mS for each button
 * @param p
 * @return True if event happened
 */
BOOL btnDebounce(BTN_DEBOUNCE_STRUCT *p)
{
    // initial states
    if (p->bootup == FALSE) {
        p->bootup = TRUE;
        switch (p->rawStatus) {
            case PRESSED:
                p->timePressed = BUTTON_TIME_MS_PRESSED_LONG / 10;
                break;
            case RELEASED:
                p->timeReleased++ == BUTTON_TIME_MS_RELEASED / 10;
                break;
        }
    }
    switch (p->rawStatus) {
        case PRESSED:
            if (BUTTON_TIME_MS_PRESSED_LONG / 10 < p->timePressed)
                ;
            else if (p->timePressed++ == BUTTON_TIME_MS_PRESSED_LONG / 10) {
                p->debouncedBtnState = BTN_DISABLD_LONG;
                return TRUE; // return that there was an event
            }
            p->timeReleased = 0;
            break;

        case RELEASED:
            if (p->timePressed < BUTTON_TIME_MS_PRESSED_LONG / 10) {
                if (BUTTON_TIME_MS_PRESSED_SHORT / 10 < p->timePressed) {
                    if (p->debouncedBtnState != BTN_DISABLED_SHORT) {
                        p->debouncedBtnState = BTN_DISABLED_SHORT;
                        return TRUE; // return that there was an event
                    }
                }
            }
            if (BUTTON_TIME_MS_RELEASED / 10 < p->timeReleased)
                ;
            else if (p->timeReleased++ == BUTTON_TIME_MS_RELEASED / 10) {
                if (p->debouncedBtnState != BTN_ENABLED) {
                    p->debouncedBtnState = BTN_ENABLED;
                    return TRUE; // return that there was an event
                }
            }
            p->timePressed = 0;
            break;
    }
    return FALSE;
}

