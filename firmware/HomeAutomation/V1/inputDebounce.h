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
#define BUTTON_TIME_MS_RELEASED_LONG    BUTTON_TIME_MS_PRESSED_LONG
#define BUTTON_TIME_MS_RELEASED_SHORT   BUTTON_TIME_MS_PRESSED_SHORT

typedef enum {
    BTN_DISABLED_LONG = 0,
    BTN_DISABLED_SHORT,
    BTN_DISABLED,
    BTN_ENABLED,
    BTN_ENABLED_SHORT,
    BTN_ENABLED_LONG
} BTN_EV_STATES;

typedef union {

    struct {
        /** Current state of the button*/
        BTN_EV_STATES debouncedBtnState : 3;
        /** user must update this register according to live button state */
        unsigned rawStatus : 1;
        unsigned bootup : 1;
        /** Debounce times*/
        unsigned timePressed : 7;
        unsigned timeReleased : 5;
    };
    UINT32 value;
} BTN_DEBOUNCE_STRUCT;
BOOL btnDebounce(BTN_DEBOUNCE_STRUCT *p);
/*
 static BUTTON_DEBOUNCE_STRUCT btn1 = {0};
 */
#endif
