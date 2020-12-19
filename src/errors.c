#include "errors.h"
#include <stdio.h>

int kpm__report(int is_bool, int overide_err, int err,
                const char* fn_name, const char* args,
                const char* file, int line, const char* caller) {
  if (is_bool) {
    if (!err) {
      fprintf(stderr, "%s(%s) return false at %s:%d (%s)\n",
              fn_name, args, file, line, caller);
      return overide_err;
    }
  } else if (err) {
    fprintf(stderr, "%s(%s) failed with code %d at %s:%d (%s)\n",
            fn_name, args, err, file, line, caller);
    return overide_err ? overide_err : err;
  }
  return KPM_SUCCESS;
}
