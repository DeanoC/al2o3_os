#include "al2o3_platform/platform.h"
#include "al2o3_catch2/catch2.hpp"
#include "al2o3_os/thread.h"

void SetVarTo42(void *data) {
  REQUIRE(data);
  REQUIRE(*((int *) data) == 0);
  *(int *) data = 42;
}

TEST_CASE("Thread Create and Destroy (C)", "[OS File]") {

  int t0Data = 0;
  Os_Thread_t t0;
  bool t0Okay = Os_ThreadCreate(&t0, &SetVarTo42, &t0Data);
  REQUIRE(t0Okay);
  Os_ThreadJoin(&t0);
  REQUIRE(t0Data == 42);
  Os_ThreadDestroy(&t0);
}


