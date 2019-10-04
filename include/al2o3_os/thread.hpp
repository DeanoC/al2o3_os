// Full license at end of file
// Summary: MIT

#pragma once
#include "al2o3_platform/platform.h"
#include "al2o3_os/thread.h"

namespace Os {

struct Mutex {
  Mutex() { Os_MutexCreate(&handle); };
  ~Mutex() { Os_MutexDestroy(&handle); };

  // take ownership of a C level mutex
  explicit Mutex(Os_Mutex_t mutie) : handle(mutie) {};

  void Acquire() { Os_MutexAcquire(&handle); };
  void Release() { Os_MutexRelease(&handle); };

  Os_Mutex_t handle;
};

struct ConditionalVariable {
  ConditionalVariable() { Os_ConditionalVariableCreate(&handle); };
  ~ConditionalVariable() { Os_ConditionalVariableDestroy(&handle); };

  void Wait(Mutex& mutex, uint64_t waitms) {
    Os_ConditionalVariableWait(&handle, &mutex.handle, waitms);
  }
  void Set() { Os_ConditionalVariableSet(&handle); };

  Os_ConditionalVariable_t handle;
};

struct MutexLock {
  MutexLock(Mutex& mutex) : mMutex(&mutex.handle) { Os_MutexAcquire(mMutex); };
  MutexLock(Os_Mutex_t *mutex) : mMutex(mutex) { Os_MutexAcquire(mMutex); };
  ~MutexLock() { Os_MutexRelease(mMutex); };

  /// Prevent copy construction.
  MutexLock(const MutexLock& rhs) = delete;
  /// Prevent assignment.
  MutexLock& operator=(const MutexLock& rhs) = delete;

  Os_Mutex_t *mMutex;
};

struct Thread {
  Thread(Os_JobFunction_t function, void *data) { Os_ThreadCreate(&handle, function, data); }
  ~Thread() { Os_ThreadDestroy(&handle); }

  void Join() { Os_ThreadJoin(&handle); }

  static void SetMainThread() { Os_SetMainThread(); };
  static Os_ThreadID_t GetCurrentThreadID() { return Os_GetCurrentThreadID(); };
  static bool IsMainThread() { return Os_IsMainThread(); }
  static void Sleep(uint64_t waitms) { Os_Sleep(waitms); }
  static uint32_t GetNumCPUCores(void) { return Os_CPUCoreCount(); };

  Os_Thread_t handle;
};

}; // end Os namesapce
