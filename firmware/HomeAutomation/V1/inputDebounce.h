/*
 * File:   inputDebounce.h
 * Author: Willem
 *
 * Created on January 7, 2012, 10:18 PM
 */

#ifndef inputDebounce_H
#define inputDebounce_H

/** MaxTime == 2550 */
#define BUTTON_TIME_MS_PRESSED_SHORT  20
#define BUTTON_TIME_MS_PRESSED_LONG  400
/** MaxTime == 310 */
#define BUTTON_TIME_MS_RELEASED 50

typedef enum {
    BTN_PRESSED_LONG = 0,
    BTN_PRESSED_SHORT,
    BTN_RELEASED,
    BTN_RELEASED_LONG
} BTN_EV_STATES;

typedef union {

    struct {
        /** Current state of the button*/
        BTN_EV_STATES debouncedBtnState : 2;
        /** user must update this register according to live button state */
        unsigned rawStatus : 1;
        unsigned bootup : 1;
        /** Debounce times*/
        unsigned timePressed : 7;
        unsigned timeReleased : 5;
    };
    UINT16 value;
} BTN_DEBOUNCE_STRUCT;
BOOL btnDebounce(BTN_DEBOUNCE_STRUCT *p);
/*
 static BUTTON_DEBOUNCE_STRUCT btn1 = {0};
 */
#endif
