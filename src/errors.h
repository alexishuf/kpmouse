#ifndef _KPMOUSE_ERRORS_H_
#define _KPMOUSE_ERRORS_H_

////////////////////////////////////////////
// Constants
////////////////////////////////////////////

/* vvvvvvvvvvvvvvvvvvvvvvvvvv Error codes vvvvvvvvvvvvvvvvvvvvvvvvvv */
#define KPM_SUCCESS            0
#define KPM_ERR_XDO_NEW        1
#define KPM_ERR_XDO_VIEWPORT   2
#define KPM_ERR_NO_KEYCODE     3
#define KPM_ERR_XDO_GET_MOUSE  4
#define KPM_ERR_XDO_MOVE_MOUSE 5
#define KPM_ERR_XDO_MOUSE_DOWN 6
#define KPM_ERR_XDO_MOUSE_UP   7
#define KPM_ERR_X_GRAB         8
#define KPM_ERR_X_SEL_INPUT    9
#define KPM_ERR_X_NEXT_EVT     10
#define KPM_ERR_GETTIME        11
/* ^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^ */

////////////////////////////////////////////
// Functions
////////////////////////////////////////////

/**
 * If err != 0 print a log message and return overide_err (or err if
 * overide_err==0). Else return 0.
 *
 * If is_bool is non-zero, then override_err will be returned if err==0
 */
int kpm__report(int is_bool, int overide_err, int err,
                const char* fn_name, const char* args,
                const char* file, int line, const char* caller);

////////////////////////////////////////////
// Macros
////////////////////////////////////////////

/**
 * Run fn(...) and if result is non-zero, evaluate to err, else evaluate to 0.
 *
 * For non-zero results from fn(...) a log message "fn(...) failed with
 * code n at file:line (caller_function)".
 */
#define KPM_CHK2(err, fn, ...) \
  kpm__report(0, err, fn(__VA_ARGS__), #fn, #__VA_ARGS__,  \
             __FILE__, __LINE__, __func__)
/** Same as KPM_CHK2(), but returns the exact non-zero return of fn */
#define KPM_CHK(fn, ...) KPM_CHK2(0, fn, __VA_ARGS__)

/** Run fn(...) and return err if it returns 0 or 0 if it returns non-zero. */
#define KPM_BCHK(err, fn, ...)                          \
  kpm__report(1, err, fn(__VA_ARGS__), #fn, #__VA_ARGS__, \
              __FILE__, __LINE__, __func__)

/**
 * Similar to KPM_CHK2, but does not evaluate to anything. If fn returns
 * non-zero the message will be logged and the caller will return err.
 */
#define KPM_RET2(err, fn, ...)                        \
  {                                                   \
    int kpm_ret_err = KPM_CHK2(err, fn, __VA_ARGS__); \
    if (kpm_ret_err) return kpm_ret_err;              \
  }

/** Version of KPM_RET2 that does not replace the non-zero return of fn.*/
#define KPM_RET(fn, ...) KPM_RET2(0, fn, __VA_ARGS__)

/** If fn(...) returns zero, make the caller return with err. */
#define KPM_BRET(err, fn, ...)                        \
  {                                                   \
    int kpm_ret_err = KPM_BCHK(err, fn, __VA_ARGS__); \
    if (kpm_ret_err) return kpm_ret_err;              \
  }

#endif /*_KPMOUSE_ERRORS_H_*/

