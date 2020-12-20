#include "util.h"
#include "errors.h"
#include "limits.h"

long int kpm__ms_elapsed(const struct timespec* ts) {
  struct timespec copy = *ts;
  return kpm__ms_elapsed_upd(&copy);
}

long int kpm__ms_elapsed_upd(struct timespec* ts) {
  struct timespec now;
  if (KPM_CHK(clock_gettime, CLOCK_MONOTONIC, &now))
    return INT_MAX;
  int age = (now.tv_sec - ts->tv_sec)*1000;
  if (age == 0)
    age += (now.tv_nsec - ts->tv_nsec)/1000000L;
  else
    age += 1000 - ts->tv_nsec/1000000L + now.tv_nsec/1000000L;
  *ts = now;
  return age;
}
