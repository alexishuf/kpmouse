#include "user_config.h"
#include <X11/Xutil.h>

KeySym kpm_move_sym[8] = {
  XK_KP_Home,      //KPM_TL
  XK_KP_Up,        //KPM_CU
  XK_KP_Page_Up,   //KPM_TR
  XK_KP_Down,      //KPM_CD
  XK_KP_End,       //KPM_BL
  XK_KP_Left,      //KPM_CL
  XK_KP_Page_Down, //KPM_BR
  XK_KP_Right      //KPM_CR
};

KeySym kpm_button_sym[3] = {
  XK_KP_Divide,
  XK_KP_Multiply,
  XK_KP_Subtract
};

