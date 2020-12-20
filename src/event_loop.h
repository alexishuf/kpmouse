#ifndef _KPMOUSE_EVENT_LOOP_H_
#define _KPMOUSE_EVENT_LOOP_H_

////////////////////////////////////////////
// Includes
////////////////////////////////////////////

#include "config.h"

////////////////////////////////////////////
// Forward declarations
////////////////////////////////////////////

typedef struct kpm_st_s kpm_st_t;

typedef struct kpm_el_s {
  kpm_st_t* st;
} kpm_el_t;

////////////////////////////////////////////
// Functions
////////////////////////////////////////////

/**
 * Create a event loop ans setup events from X11 to listen for
 * @returns new kpm_el_t object, or NULL if an error occurred
 */
int kpm_el_init(kpm_el_t* el, kpm_st_t* st);

/**
 * Release resources held by the event loop object.
 * Note: does not destroy the kpm_st_t, since its ownership is not transfered
 */
void kpm_el_destroy(kpm_el_t* el);

/**
 * Wait and process a single event.
 * @returns 0 if event was processed without errors, error code otherwise
 */
int kpm_el_step(kpm_el_t* el);

/**
 * Run an event loop forever (until el_step returns an error)
 * @returns Error code of failed kpm_el_step() call
 */
int kpm_el_run(kpm_el_t* st);

#endif /*_KPMOUSE_EVENT_LOOP_H_*/

