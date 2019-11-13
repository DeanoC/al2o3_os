#include "al2o3_platform/platform.h"
#include "al2o3_os/time.h"
#include <time.h>

int64_t Os_GetUSec() {
  timespec ts;
  clock_gettime(CLOCK_REALTIME, &ts);
  long us = (ts.tv_nsec / 1000);
  us += ts.tv_sec * 1e6;
  return us;
}



