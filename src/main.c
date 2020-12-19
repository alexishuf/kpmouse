#include <string.h>
#include <stdio.h>
#include "state.h"
#include "event_loop.h"
#include <X11/Xlib.h>

#define G_ERR_BUF_LEN 1024
static char g_err_buf[G_ERR_BUF_LEN];

static int err_handler(Display* dsp, XErrorEvent* evt) {
  memset(g_err_buf, 0, G_ERR_BUF_LEN);
  XGetErrorText(dsp, evt->error_code, g_err_buf, G_ERR_BUF_LEN);
  fprintf(stderr, "Failed X11 operation (code %u): %s. request=%u.%u,"
          "serial=%lu, resource=%lu\n", evt->error_code, g_err_buf,
          evt->request_code, evt->minor_code, evt->serial, evt->resourceid);
  return 0;
}

int main(int argc, char** argv) {
  kpm_st_t st;
  kpm_el_t el;
  int err;

  XSetErrorHandler(&err_handler);

  KPM_RET(kpm_st_init, &st);
  if (!(err = KPM_CHK(kpm_el_init, &el, &st)))
    err = KPM_CHK(kpm_el_run, &el);
  kpm_el_destroy(&el);
  kpm_st_destroy(&st);

  return err;
}


