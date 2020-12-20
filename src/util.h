#ifndef _KPMOUSE_UTIL_H_
#define _KPMOUSE_UTIL_H_

////////////////////////////////////////////
// Includes
////////////////////////////////////////////

#include "config.h"
#include <time.h>


////////////////////////////////////////////
// Functions
////////////////////////////////////////////

/**
 * How many milliseconds elapsed since the given ts was taken from
 * CLOCK_MONOTONIC.
 *
 * @return non-negative number of elapsed milliseconds.
 */
long int kpm__ms_elapsed(const struct timespec* ts);

/** Similar to kpm__ms_elapsed(), but will update ts. */
long int kpm__ms_elapsed_upd(struct timespec* ts);

#endif /*_KPMOUSE_UTIL_H_*/

