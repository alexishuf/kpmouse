#ifndef _KPMOUSE_USERCONFIG_H_
#define _KPMOUSE_USERCONFIG_H_

////////////////////////////////////////////
// Includes
////////////////////////////////////////////

#include <X11/X.h>

////////////////////////////////////////////
// Types and Constants
////////////////////////////////////////////

/**
 * How many steps should be done in logarithmic mode (splitting the movement
 * window into four rectangles) before movement becomes linear (advancing a
 * fixed ammount of vertical and horizontal pixels).
 */
#define KPM_LOG_STEPS 4

/**
 * Once after KPM_LOG_STEPS logarithmic movements, movement becomes linear. The
 * horizontal and vertical steps (in pixels) is determined by divding the width
 * and height of the movement window resulted from the last logarithmic
 * movement. In linear mode, the window size is used only to determine step
 * size. Thus, movement can go beyond the movement window borders.
 */
#define KPM_LINEAR_STEPS 5

/**
 * How many millisconds of inactivity cancel a movement.
 */
#define KPM_MOVE_TTL_MS 4000

/**
 * Array with a KeySym (see X11/keysymdef.h) for each kpm_move_t constant
 */
extern KeySym kpm_move_sym[8];

/**
 * Array with a KeySym (see X11/keysymdef.h) for:
 * kpm_button_sym[0]: left mouse button
 * kpm_button_sym[1]: middle mouse button
 * kpm_button_sym[2]: right mouse button
 */
extern KeySym kpm_button_sym[3];

#endif /*_KPMOUSE_USERCONFIG_H_*/

