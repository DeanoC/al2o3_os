// Full license at end of file
// Summary: Apache - original from The-Forge modified by DeanoC

#pragma once
#ifndef AL2O3_OS_FILESYSTEM_H
#define AL2O3_OS_FILESYSTEM_H

#include "al2o3_platform/platform.h"

static const size_t FS_npos = (size_t) (-1);

// is the provided path a platform or the a normalised path(they can be the same)
// internal paths are normalized to use forward slash '/' as directory seperator
// platform paths are slashes in the platform preferred direction
AL2O3_EXTERN_C bool Os_IsNormalisedPath(char const *pathName);
AL2O3_EXTERN_C bool Os_GetNormalisedPathFromPlatformPath(char const *pathName, char *dirOut, size_t maxSize);
AL2O3_EXTERN_C bool Os_GetPlatformPathFromNormalisedPath(char const *pathName, char *dirOut, size_t maxSize);

AL2O3_EXTERN_C bool Os_IsAbsolutePath(char const *path);
AL2O3_EXTERN_C bool Os_SplitPath(char const *path, size_t *fileName, size_t *extension);
AL2O3_EXTERN_C bool Os_ReplaceExtension(char const *path, char const *newExtension, char *dirOut, size_t maxSize);
AL2O3_EXTERN_C bool Os_GetParentPath(char const *path, char *dirOut, size_t maxSize);

AL2O3_EXTERN_C bool Os_GetCurrentDir(char *dirOut, size_t maxSize);
AL2O3_EXTERN_C bool Os_SetCurrentDir(char const *dir);

AL2O3_EXTERN_C bool Os_FileExists(char const *fileName);
AL2O3_EXTERN_C bool Os_DirExists(char const *pathName);

AL2O3_EXTERN_C bool Os_FileCopy(char const *src, char const *dst);
AL2O3_EXTERN_C bool Os_FileDelete(char const *fileName);
AL2O3_EXTERN_C bool Os_CreateDir(char const *pathName);
AL2O3_EXTERN_C int Os_SystemRun(char const *fileName, int argc, const char **argv);


AL2O3_EXTERN_C bool Os_GetExePath(char *dirOut, int maxSize);
AL2O3_EXTERN_C bool Os_GetUserDocumentsDir(char *dirOut, int maxSize);
AL2O3_EXTERN_C bool Os_GetAppPrefsDir(char const *org, char const *app, char *dirOut, int maxSize);

AL2O3_EXTERN_C size_t Os_GetLastModifiedTime(char const *fileName);

typedef void* Os_DirectoryEnumeratorHandle;
typedef void (*Os_DirectoryEnumeratorFunc)(Os_DirectoryEnumeratorHandle handle, void* userData, char const* filename);

AL2O3_EXTERN_C Os_DirectoryEnumeratorHandle Os_DirectoryEnumeratorFromPath(char const* path, Os_DirectoryEnumeratorFunc func, void* userData);
AL2O3_EXTERN_C void Os_DirectoryEnumeratorClose(Os_DirectoryEnumeratorHandle handle);

AL2O3_EXTERN_C bool Os_DirectoryEnumeratorAsyncStart(Os_DirectoryEnumeratorHandle handle);
AL2O3_EXTERN_C bool Os_DirectoryEnumeratorAsyncStall(Os_DirectoryEnumeratorHandle handle);

AL2O3_EXTERN_C bool Os_DirectoryEnumeratorSyncStart(Os_DirectoryEnumeratorHandle handle);
AL2O3_EXTERN_C bool Os_DirectoryEnumeratorSyncNext(Os_DirectoryEnumeratorHandle handle);

AL2O3_EXTERN_C bool Os_DirectoryEnumeratorCancel(Os_DirectoryEnumeratorHandle handle);

#endif //AL2O3_OS_FILESYSTEM_H

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
