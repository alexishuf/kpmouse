#include "event_loop.h"
#include "state.h"
#include "util.h"
#include <X11/Xlib.h>
#include <xdo.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <assert.h>

static const int N_MOD_MASKS = 1<<(Mod5MapIndex+1);

////////////////////////////////////
// private functions
////////////////////////////////////

static kpm_move_t to_move(kpm_el_t* el, KeyCode code) {
  for (int i = 0; i < 8; ++i)
    if (el->st->move_code[i] == code)
      return i;
  return KPM_NULL_MOVE;
}

static kpm_button_t to_button(kpm_el_t* el, KeyCode code) {
  for (int i = 0; i < 3; ++i)
    if (el->st->button_code[i] == code)
      return i;
  return KPM_NULL_BUTTON;
}

static int send_mouse(kpm_el_t* el, kpm_button_t button, char down) {
#ifndef NDEBUG
  printf("send_mouse(%d, %s)\n", button, down ? "DOWN" : "UP");
#endif
  if (down) {

    return KPM_CHK2(KPM_ERR_XDO_MOUSE_DOWN, xdo_mouse_down,
                    el->st->xdo, CURRENTWINDOW, button+1);
  } else {
    return KPM_CHK2(KPM_ERR_XDO_MOUSE_DOWN, xdo_mouse_up,
                    el->st->xdo, CURRENTWINDOW, button+1);
  }
}

static int handle_button(kpm_el_t* el, kpm_button_t button, int press) {
  if (press) {
    if (el->pressed_button == KPM_NULL_BUTTON) {
      el->pressed_button = button;
      KPM_CHK2(KPM_ERR_GETTIME, clock_gettime,
               CLOCK_MONOTONIC, &el->press_ts);
      KPM_RET(send_mouse, el, button, 1); //send "down"
    } else if (kpm__ms_elapsed(&el->press_ts) > el->long_press_ms) {
      KPM_RET(send_mouse, el, el->pressed_button, 0); // long press, send "up"
      el->pressed_button = KPM_NULL_BUTTON;
    } // else: ignore second button in double press
  } else if (el->pressed_button != KPM_NULL_BUTTON) {
    if (kpm__ms_elapsed(&el->press_ts) < el->long_press_ms) {
      KPM_RET(send_mouse, el, el->pressed_button, 0); //clicked, send "up"
      el->pressed_button = KPM_NULL_BUTTON;
    } // else: started a long press, do not send up
  } //else: ignore orphan release
  return KPM_SUCCESS;
}


////////////////////////////////////
// public functions
////////////////////////////////////

int kpm_el_init(kpm_el_t* el, kpm_st_t* st) {
  memset(el, 0, sizeof(kpm_el_t));
  el->st = st;
  el->pressed_button = KPM_NULL_BUTTON;
  el->long_press_ms = KPM_LONG_PRESS_MS;
  int n_screens = ScreenCount(st->xdo->xdpy);
  for (int screen = 0; screen < n_screens; ++screen) {
    Window root          = RootWindow(st->xdo->xdpy, screen);
    Bool   owner_events  = False;
    int    pointer_mode  = GrabModeAsync;
    int    keyboard_mode = GrabModeAsync;

    for (int modMask = 0; modMask < N_MOD_MASKS; ++modMask) {
      if (modMask & Mod2Mask)
        continue; // skip masks with NumLock
      for (int i = 0; i < 8; ++i) {
        KPM_BRET(KPM_ERR_X_GRAB, XGrabKey, st->xdo->xdpy,
                 st->move_code[i], modMask, root,
                 owner_events, pointer_mode, keyboard_mode);
      }
      for (int i = 0; i < 3; ++i) {
        KPM_BRET(KPM_ERR_X_GRAB, XGrabKey, st->xdo->xdpy,
                 st->button_code[i], modMask, root,
                 owner_events, pointer_mode, keyboard_mode);
      }
    }
    KPM_BRET(KPM_ERR_X_SEL_INPUT, XSelectInput, st->xdo->xdpy,
             root, KeyPressMask|KeyReleaseMask);
  }
  return KPM_SUCCESS;
}

void kpm_el_destroy(kpm_el_t* el) {
  int n_screens = ScreenCount(el->st->xdo->xdpy);
  for (int screen = 0; screen < n_screens; ++screen) {
    Window root = RootWindow(el->st->xdo->xdpy, screen);
    for (int modMask = 0; modMask < N_MOD_MASKS; ++modMask) {
      if (modMask & Mod2Mask)
        continue; //skip masks with NumLock
      for (int i = 0; i < 8; ++i) {
        KPM_BCHK(KPM_ERR_X_GRAB, XUngrabKey, el->st->xdo->xdpy,
                 el->st->move_code[i], modMask, root);
      }
      for (int i = 0; i < 3; ++i) {
        KPM_BCHK(KPM_ERR_X_GRAB, XUngrabKey, el->st->xdo->xdpy,
                 el->st->button_code[i], modMask, root);
      }
    }
  }
}

int kpm_el_step(kpm_el_t* el) {
  XEvent ev = {0};
  KPM_RET2(KPM_ERR_X_NEXT_EVT, XNextEvent, el->st->xdo->xdpy, &ev);
  assert(ev.type == KeyPress || ev.type == KeyRelease);
  if (ev.type != KeyPress && ev.type != KeyRelease) {
    fprintf(stderr, "kp_el_step() ignoring unexpected ev.type %d\n", ev.type);
    return KPM_SUCCESS; //not a fatal error
  }
  kpm_move_t move = to_move(el, ev.xkey.keycode);
  if (move != KPM_NULL_MOVE) {
    if (ev.type == KeyPress)
      KPM_RET(kpm_st_move, el->st, move);
    //else: ignore the release event
  } else {
    kpm_button_t button = to_button(el, ev.xkey.keycode);
    if (button != KPM_NULL_BUTTON) {
      KPM_RET(handle_button, el, button, ev.type == KeyPress);
    } else {
      const char* ev_type = ev.type==KeyPress ? "press" : "release";
      fprintf(stderr, "kpm_el_step() ignoring unexpected %s on keycode %d,"
              "mask %x.\n", ev_type , ev.xkey.keycode, ev.xkey.state);
    }
  }
  return KPM_SUCCESS;
}

int kpm_el_run(kpm_el_t* el) {
  int err;
  while (!(err = kpm_el_step(el))) ;
  return err;
}
