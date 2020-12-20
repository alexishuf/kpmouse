#ifndef _KPMOUSE_STATE_H_
#define _KPMOUSE_STATE_H_

#include "config.h"
#include "user_config.h"
#include "errors.h"
#include <time.h>

////////////////////////////////////////////
// Third paty forward declarations
////////////////////////////////////////////

typedef struct xdo xdo_t; //xdo.h

////////////////////////////////////////////
// Types and Constants
////////////////////////////////////////////

/**
 * Movement occurs in 8 possible directions according to a 3-bit number:
 *     (h_bit<<2) | (v_bit<<1) | (cross_bit<<0)
 * - h_bit    : 0 means left and 1 means right
 * - v_bit    : 0 means up and 1 means down
 * - cross_bit: 0 means center on sub-rectangles, 1 means stay on the cross
 *              lines that divide sub-rectangles
 * When cross_bit==0, h_bit (or v_bit) == 1 mean increase x (or y). When
 * cross_bit==1, this equivalence does not hold for the down strech of
 * the cross.
 *
 * Applying a >>1 shift, the h_bit|v_bit portion yields the following numbers:
 *
 *     ++====================++
 *     ||         |          ||
 *     ||    0    0     1    ||
 *     ||         |          ||
 *     ||----2----+-----3----||
 *     ||         |          ||
 *     ||    2    1     3    ||
 *     ||         |          ||
 *     ++====================++
 *
 * Without the shift (including the cross_bit), these are the values:
 *
 *     ++====================++
 *     ||         |          ||
 *     ||    0    1     2    ||
 *     ||         |          ||
 *     ||----5----+-----7----||
 *     ||         |          ||
 *     ||    4    3     6    ||
 *     ||         |          ||
 *     ++====================++
 *
 */
typedef char kpm_move_t;

/* vvvvvvvvvvvvvvvv Constants values for kpm_move_t vvvvvvvvvvvvvvvv */
#define KPM_TL        0 ///<    top-left  rectangle center
#define KPM_TR        2 ///<    top-right rectangle center
#define KPM_BL        4 ///< bottom-left  rectangle center
#define KPM_BR        6 ///< bottom-right rectangle center
#define KPM_CU        1 ///< upper cross segment center
#define KPM_CD        3 ///< lower cross segment center
#define KPM_CL        5 ///< left  cross segment center
#define KPM_CR        7 ///< right cross segment center
#define KPM_NULL_MOVE 8 ///< not a move value
/* ^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^ */

typedef char kpm_button_t;

/* vvvvvvvvvvvvvvvv Constants values for kpm_move_t vvvvvvvvvvvvvvvv */
#define KPM_L           0 ///< left   mouse button
#define KPM_M           1 ///< middle mouse button
#define KPM_R           2 ///< right  mouse button
#define KPM_NULL_BUTTON 3 ///< not a mouse button
/* ^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^ */

/** State object */
typedef struct kpm_st_s {
  /** Current width and height of window (to be split on next move) */
  unsigned int w, h;

  /**
   * How many logarithmic moves where done (each log step splits the movement
   * window in four rectangles). After initialization or reset, w and h cover
   * the whole screen and this field is zero. After some point (KPM_LOG_STEPS)
   * it stops incrementing and moves become linear (see step_x and step_y).
   */
  unsigned char log_steps;

  /**
   * Center points of the last log movement window. If 1 < log_steps <
   * max_log_steps, this represents the pointer position. If no move was done or
   * if in linear stepping, these are not updated and have no relation to the
   * pointer position.
   */
  int log_x, log_y;

  /**
   * stack of kpm_move_t log steps performed. history[log_steps-1] is the most
   * recent move.
   */
  kpm_move_t history[KPM_LOG_STEPS];

  /**
   * Maximum number of logarithmic steps before movement becomes linear
   * @see KPM_LOG_STEPS
   */
  unsigned char max_log_steps;

  /**
   * Once max_log_steps is reached and movement becomes linear, the horizontal
   * and vertical steps (step_x and step_y) are computed by dividing the last
   * log movement window by this number.
   * @see KPM_LINEAR_STEPS
   */
  unsigned char expected_linear_steps;

  /** Linear step size (after maximum log steps performed) */
  short step_x, step_y;

  /** clock timestamp of last move (log or linear).  */
  struct timespec move_ts;

  /**
   * How may milliseconds a move survives inactivity. If current time is more
   * than move_ttl_ms milliseconds from move_ts, then the move operation is
   * expired. Further kpm_st_move() operations will start a new move from
   * scratch.
   */
  unsigned int move_ttl_ms;

  /** move_code[m] is the KeyCode that should trigger the kpm_move_t m */
  KeyCode move_code[8];

  /**
   * button_code[0:2] is the KeyCode that should trigger the left, middle or
   * right mouse button
   */
  KeyCode button_code[6];

  /** libxdo context */
  xdo_t* xdo;
} kpm_st_t;


////////////////////////////////////////////
// Functions
////////////////////////////////////////////

/**
 * Initializaes a kpm_st_t object. Calling kpm_st_reset() is not necessary.
 *
 * @return 0 if successful, or an error code
 */
int kpm_st_init(kpm_st_t* state);

/** Releases all resouces held by state */
void kpm_st_destroy(kpm_st_t* state);

/**
 * Reset the state back to the whole active mouse screen
 *
 * @return 0 if successful, else an KMP_ERR_ code.
 */
int kpm_st_reset(kpm_st_t* state);

/**
 * Do a move operation on state at the given direction (move).
 *
 * @return 0 if successful, else an KMP_ERR_ code.
 */
int kpm_st_move(kpm_st_t* state, kpm_move_t move);

/**
 * Restores the state to what it was before the last logarithmic operation (or
 * undo all linear moves the the last logarithmic move). The operation has no
 * effect if no move has been done or if the whole history has been reversed.
 *
 * @return 0 if successful, else an KMP_ERR_ code.
 */
int kpm_st_unmove(kpm_st_t* state);


#endif /*_KPMOUSE_STATE_H_*/

