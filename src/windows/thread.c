#include "al2o3_platform/platform.h"
#include "al2o3_platform/windows.h"
#include "al2o3_os/thread.h"
#include <stdlib.h>
#include "al2o3_memory/memory.h"

static_assert(sizeof(CRITICAL_SECTION) == sizeof(Os_Mutex_t), "Mutex size failure in windows/thread.c");
static_assert(sizeof(CONDITION_VARIABLE) == sizeof(Os_ConditionalVariable_t), "Condition Varialbe size failure in windows/thread.c");

AL2O3_EXTERN_C bool Os_MutexCreate(Os_Mutex_t *mutex) {
  InitializeCriticalSection((CRITICAL_SECTION *) mutex);
  return true;
}
AL2O3_EXTERN_C void Os_MutexDestroy(Os_Mutex_t *mutex) {
  DeleteCriticalSection((CRITICAL_SECTION *) mutex);
}

AL2O3_EXTERN_C void Os_MutexAcquire(Os_Mutex_t *mutex) {
  EnterCriticalSection((CRITICAL_SECTION *) mutex);
}
AL2O3_EXTERN_C void Os_MutexRelease(Os_Mutex_t *mutex) {
  LeaveCriticalSection((CRITICAL_SECTION *) mutex);
}

AL2O3_EXTERN_C bool Os_ConditionalVariableCreate(Os_ConditionalVariable_t *cv) {
  InitializeConditionVariable((CONDITION_VARIABLE *) cv);
  return true;
}

AL2O3_EXTERN_C void Os_ConditionalVariableDestroy(Os_ConditionalVariable_t *cv) {
}

AL2O3_EXTERN_C void Os_ConditionalVariableWait(Os_ConditionalVariable_t *cv, Os_Mutex_t *mutex, uint64_t waitms) {
  SleepConditionVariableCS((CONDITION_VARIABLE *) cv, (CRITICAL_SECTION *) mutex, (DWORD) waitms);
}
AL2O3_EXTERN_C void Os_ConditionalVariableSet(Os_ConditionalVariable_t *cv) {
  WakeConditionVariable((CONDITION_VARIABLE *) cv);
}

struct TrampParam {
  Os_JobFunction_t func;
  void *param;
};

static DWORD WINAPI FuncTrampoline(void *param) {
  struct TrampParam *tp = (struct TrampParam *) param;
  tp->func(tp->param);
  MEMORY_FREE(tp);
  return 0;
}

AL2O3_EXTERN_C bool Os_ThreadCreate(Os_Thread_t *thread, Os_JobFunction_t func, void *data) {
  ASSERT(thread);
  struct TrampParam *tp = (struct TrampParam *) MEMORY_MALLOC(sizeof(struct TrampParam));
  tp->func = func;
  tp->param = data;
  *thread = CreateThread(0, 0, &FuncTrampoline, tp, 0, 0);
  
  return (*thread != NULL);
}

AL2O3_EXTERN_C void Os_ThreadDestroy(Os_Thread_t *thread) {
  ASSERT(thread);
  WaitForSingleObject((HANDLE) *thread, INFINITE);
  CloseHandle((HANDLE) *thread);
}
AL2O3_EXTERN_C void Os_ThreadJoin(Os_Thread_t *thread) {
  WaitForSingleObject((HANDLE) *thread, INFINITE);
}

static bool s_isMainThreadIDSet = false;
static Os_ThreadID_t s_mainThreadID;

AL2O3_EXTERN_C Os_ThreadID_t Os_GetCurrentThreadID(void) {
  return GetCurrentThreadId();
}
AL2O3_EXTERN_C void Os_SetMainThread(void) {
  ASSERT(s_isMainThreadIDSet == false);
  s_mainThreadID = GetCurrentThreadId();
  s_isMainThreadIDSet = true;
}

AL2O3_EXTERN_C bool Os_IsMainThread(void) {
  ASSERT(s_isMainThreadIDSet == true);
  return Os_GetCurrentThreadID() == s_mainThreadID;
}

AL2O3_EXTERN_C void Os_Sleep(uint64_t waitms) {
  Sleep((DWORD) waitms);
}
AL2O3_EXTERN_C uint32_t Os_CPUCoreCount(void) {
  SYSTEM_INFO systemInfo;
  GetSystemInfo(&systemInfo);
  return systemInfo.dwNumberOfProcessors;
}

/*
 * Copyright (c) 2018-2019 Confetti Interactive Inc.
 *
 * This file is part of The-Forge
 * (see https://github.com/ConfettiFX/The-Forge).
 *
 * Licensed to the Apache Software Foundation (ASF) under one
 * or more contributor license agreements.  See the NOTICE file
 * distributed with this work for additional information
 * regarding copyright ownership.  The ASF licenses this file
 * to you under the Apache License, Version 2.0 (the
 * "License"); you may not use this file except in compliance
 * with the License.  You may obtain a copy of the License at
 *
 *   http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing,
 * software distributed under the License is distributed on an
 * "AS IS" BASIS, WITHOUT WARRANTIES OR CONDITIONS OF ANY
 * KIND, either express or implied.  See the License for the
 * specific language governing permissions and limitations
 * under the License.
*/
