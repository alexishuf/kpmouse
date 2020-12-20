
#include "state.h"
#include "util.h"
#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <string.h>
#include <xdo.h>
#include <X11/Xlib.h>

// KPM_LOG_STEPS must fit in a char
extern int ASSERT_KPM_LOG_STEPS_max[KPM_LOG_STEPS >= 256 ? -1 : 1];
// KPM_LOG_STEPS can be 0, but cannot be negative
extern int ASSERT_KPM_LOG_STEPS_min[KPM_LOG_STEPS <  0   ? -1 : 1];

// KPM_LINEAR_STEPS cannot be <= 0
extern int ASSERT_KPM_LINEAR_STEPS_min[KPM_LOG_STEPS <=  0  ? -1 : 1];

#define MOVE_MOUSE(...) \
  KPM_CHK2(KPM_ERR_XDO_MOVE_MOUSE, xdo_move_mouse, __VA_ARGS__)


////////////////////////////////////
// private functions
////////////////////////////////////

static int kpm_st_reset2(kpm_st_t* st, int screen) {
  KPM_RET2(KPM_ERR_XDO_VIEWPORT, xdo_get_viewport_dimensions,
           st->xdo, &st->w, &st->h, screen)
    st->log_steps = 0;
  return KPM_SUCCESS;
}

static int kpm_st_get_screen(kpm_st_t* st) {
  int x, y, s;
  return KPM_CHK(xdo_get_mouse_location, st->xdo, &x, &y, &s) ? 0 : s;
}

static void kpm_add_move(int* x, int* y, int step_x, int step_y,
                         kpm_move_t move, char reverse) {
  assert(step_x >= 0);
  assert(step_y >= 0);
  assert(step_x != 0);
  assert(step_y != 0);
  assert(!(move & ~0x7));
  move &= 0x7; // ignore invalid bits

  int negative = -1, positive = 1;
  if (move & 0x1) { //move over cross
    switch(move>>1) {
    case 0:
      step_y *= negative;
    case 1:
      step_y *= positive;
      step_x  = 0;
      break;
    case 2:
      step_x *= negative;
    case 3:
      step_x *= positive;
      step_y  = 0;
      break;
    }
  } else { // move to rectangle center
    step_y *= (move & 4) ? positive : negative;
    step_x *= (move & 2) ? positive : negative;
  }

#ifndef NDEBUG
  printf("kpm_add_move(%d, %d, %d, %d, %d, %d) -> (%d, %d)\n",
         *x, *y, step_x, step_y, move, reverse, *x+step_x, *y+step_y);
#endif /*NDEBUG*/
  *x += step_x;
  *y += step_y;
}

/** Sets st->move_ts and returns non-zero iff the move in st was not expired */
static int kpm_set_move_ts(kpm_st_t* st) {
  long int age = kpm__ms_elapsed_upd(&st->move_ts);
  return age < st->move_ttl_ms;
}


////////////////////////////////////
// public functions
////////////////////////////////////

int kpm_st_init(kpm_st_t* st) {
  memset(st, 0, sizeof(kpm_st_t));
  st->xdo = xdo_new(NULL);
  if (!st->xdo) {
    fprintf(stderr, "xdo_new(NULL) failed");
    return KPM_ERR_XDO_NEW;
  }
  st->max_log_steps = KPM_LOG_STEPS;
  st->expected_linear_steps = KPM_LINEAR_STEPS;
  st->move_ttl_ms = KPM_MOVE_TTL_MS;
  KPM_RET2(KPM_ERR_GETTIME, clock_gettime, CLOCK_MONOTONIC, &st->move_ts);
  KPM_RET(kpm_st_reset, st);
  st->step_x = st->w/(1<<st->max_log_steps)/st->expected_linear_steps;
  st->step_y = st->h/(1<<st->max_log_steps)/st->expected_linear_steps;
  for (int i = 0; i < 8; ++i) {
    st->move_code[i] = XKeysymToKeycode(st->xdo->xdpy, kpm_move_sym[i]);
    if (!st->move_code[i]) {
      fprintf(stderr, "No KeyCode for KeySym %lx of move %d\n",
              kpm_move_sym[i], i);
      return KPM_ERR_NO_KEYCODE;
    }
  }
  for (int i = 0; i < 3; ++i) {
    st->button_code[i] = XKeysymToKeycode(st->xdo->xdpy, kpm_button_sym[i]);
    if (!st->button_code[i]) {
      fprintf(stderr, "No KeyCode for KeySym %lx of mouse button %d\n",
              kpm_move_sym[i], i);
      return KPM_ERR_NO_KEYCODE;
    }
  }
#ifdef NDEBUG
  printf("kpm_st_init(%p) {\n"
         "w = %d,\n"
         "h = %d,\n"
         "log_steps = %d,\n"
         "log_x = %d,\n"
         "log_y = %d,\n"
         "max_log_steps = %d,\n"
         "expected_linear_steps = %d,\n"
         "step_x = %d,\n"
         "step_y = %d,\n"
         "move_code = {%d, %d, %d, %d, %d, %d, %d, %d}\n"
         "move_button = {%d, %d, %d}\n"
         "}\n",
         st, st->w, st->h, st->log_steps,
         st->log_x, st->log_y, st->max_log_steps,
         st->expected_linear_steps, st->step_x, st->step_y,
         st->move_code[0], st->move_code[1], st->move_code[2],
         st->move_code[3], st->move_code[4], st->move_code[5],
         st->move_code[6], st->move_code[7],
         st->button_code[0], st->button_code[1], st->button_code[2]);
#endif /*NDEBUG*/
  return KPM_SUCCESS;
}

void kpm_st_destroy(kpm_st_t* st) {
  xdo_free(st->xdo);
  st->xdo = NULL;
}


int kpm_st_reset(kpm_st_t* st) {
  return kpm_st_reset2(st, kpm_st_get_screen(st));
}


int kpm_st_move(kpm_st_t* st, kpm_move_t move) {
  int x, y, screen;
  KPM_RET2(KPM_ERR_XDO_GET_MOUSE, xdo_get_mouse_location,
           st->xdo, &x, &y, &screen);
  if (!kpm_set_move_ts(st))
    st->log_steps = 0; //expired move
  if (st->log_steps == 0 && st->max_log_steps > 0) {
    kpm_st_reset2(st, screen);
    x = st->w/2;
    y = st->h/2;
  }
  if (st->log_steps < st->max_log_steps) {
    kpm_add_move(&x, &y, st->w/4, st->h/4, move, 0);
    st->log_x = x;
    st->log_y = y;
    st->history[st->log_steps++] = move;
    st->w /= 2;
    st->h /= 2;
  } else {
    kpm_add_move(&x, &y, st->step_x, st->step_y, move, 0);
  }
  return MOVE_MOUSE(st->xdo, x, y, screen);
}

int kpm_st_unmove(kpm_st_t* st) {
  if (!kpm_set_move_ts(st))
    st->log_steps = 0; //expired move
  if (!st->log_steps)
    return KPM_SUCCESS;

  int screen = kpm_st_get_screen(st);
  if (st->log_steps >= st->max_log_steps) { //undo all linear steps
    --st->log_steps;
    return MOVE_MOUSE(st->xdo, st->log_x, st->log_y, screen);
  } // else: undo a log step

  kpm_add_move(&st->log_x, &st->log_y, st->w/2, st->h/2,
               st->history[st->log_steps], 1);
  st->w *= 2;
  st->h *= 2;
  --st->log_steps;

  return MOVE_MOUSE(st->xdo, st->log_x, st->log_y, screen);
}

