#include "al2o3_platform/platform.h"
#include "al2o3_os/thread.h"
//#include "../Interfaces/IMemoryManager.h"

#include <unistd.h>
#include <sys/sysctl.h>

AL2O3_EXTERN_C bool Os_MutexCreate(Os_Mutex_t *mutex) {
  ASSERT(mutex);
  return pthread_mutex_init(mutex, NULL) == 0;
}

AL2O3_EXTERN_C void Os_MutexDestroy(Os_Mutex_t *mutex) {
  ASSERT(mutex);
  pthread_mutex_destroy(mutex);
}

AL2O3_EXTERN_C void Os_MutexAcquire(Os_Mutex_t *mutex) {
  ASSERT(mutex);
  pthread_mutex_lock(mutex);

}

AL2O3_EXTERN_C void Os_MutexRelease(Os_Mutex_t *mutex) {
  ASSERT(mutex);
  pthread_mutex_unlock(mutex);
}

AL2O3_EXTERN_C bool Os_ConditionalVariableCreate(Os_ConditionalVariable_t *cv) {
  ASSERT(cv);
  return pthread_cond_init(cv, NULL) == 0;
}

AL2O3_EXTERN_C void Os_ConditionalVariableDestroy(Os_ConditionalVariable_t *cv) {
  ASSERT(cv);
  pthread_cond_destroy(cv);
}

AL2O3_EXTERN_C void Os_ConditionalVariableWait(Os_ConditionalVariable_t *cv, Os_Mutex_t *mutex, uint64_t waitms) {
  ASSERT(cv);
  ASSERT(mutex);

  struct timespec ts;
  ts.tv_sec = 0;
  ts.tv_nsec = (long) waitms * 1000;

  pthread_mutex_t *mutexHandle = mutex;
  pthread_cond_timedwait(cv, mutexHandle, &ts);

}

AL2O3_EXTERN_C void Os_ConditionalVariableSet(Os_ConditionalVariable_t *cv) {
  ASSERT(cv);
  pthread_cond_signal(cv);
}

struct TrampParam {
  Os_JobFunction_t func;
  void *param;
};

static void *FuncTrampoline(void *param) {
  struct TrampParam *tp = (struct TrampParam *) param;
  tp->func(tp->param);
  MEMORY_FREE(tp);

  return NULL;
}

AL2O3_EXTERN_C bool Os_ThreadCreate(Os_Thread_t *thread, Os_JobFunction_t func, void *data) {
  ASSERT(thread);
  struct TrampParam *tp = (struct TrampParam *) MEMORY_MALLOC(sizeof(struct TrampParam));
  tp->func = func;
  tp->param = data;

  return pthread_create(thread, NULL, &FuncTrampoline, tp) == 0;
}

AL2O3_EXTERN_C void Os_ThreadDestroy(Os_Thread_t *thread) {
  ASSERT(thread);

  // should this be detach rather than join?
  pthread_join(*thread, NULL);
}

AL2O3_EXTERN_C void Os_ThreadJoin(Os_Thread_t *thread) {
  ASSERT(thread);
  pthread_join(*thread, NULL);
}

AL2O3_EXTERN_C void Os_Sleep(uint64_t waitms) {
  usleep((useconds_t) waitms * 1000);
}

AL2O3_EXTERN_C uint32_t Os_CPUCoreCount(void) {
  size_t len;
  unsigned int ncpu;
  len = sizeof(ncpu);
  sysctlbyname("hw.ncpu", &ncpu, &len, NULL, 0);
  return (uint32_t) ncpu;
}

static bool s_isMainThreadIDSet = false;
static Os_ThreadID_t s_mainThreadID;

AL2O3_EXTERN_C void Os_SetMainThread(void) {
  ASSERT(s_isMainThreadIDSet == false);
  s_mainThreadID = Os_GetCurrentThreadID();
  s_isMainThreadIDSet = true;
}

AL2O3_EXTERN_C Os_ThreadID_t Os_GetCurrentThreadID(void) {
  return (Os_ThreadID_t) pthread_self();
}

AL2O3_EXTERN_C bool Os_IsMainThread(void) {
  ASSERT(s_isMainThreadIDSet);
  return Os_GetCurrentThreadID() == s_mainThreadID;
}
