#import <Foundation/Foundation.h>
#include <AppKit/NSOpenPanel.h>

#include "al2o3_platform/platform.h"
#include "al2o3_platform/utf8.h"
#include "al2o3_os/filesystem.hpp"

AL2O3_EXTERN_C bool Os_IsAbsolutePath(char const *fileFullPath) {
	return [NSString stringWithUTF8String:fileFullPath].absolutePath;
}

AL2O3_EXTERN_C bool Os_GetExePath(char *dirOut, int maxSize) {
  const char *exePath =
      [[[[NSBundle mainBundle] bundlePath] stringByStandardizingPath] cStringUsingEncoding:NSUTF8StringEncoding];
  if (exePath == NULL) { return false; }
  if (utf8size(exePath) >= maxSize) { return false; }

  utf8ncpy(dirOut, exePath, maxSize);
  return true;
}

AL2O3_EXTERN_C bool Os_GetUserDocumentsDir(char *dirOut, int maxSize) {
  const char *rawUserPath = [[[[NSFileManager defaultManager] homeDirectoryForCurrentUser] absoluteString] UTF8String];
  if (rawUserPath == NULL) { return false; }

  const char *path;
  path = strstr(rawUserPath, "/Users/");
  if (utf8size(path) >= maxSize) { return false; }
  utf8cpy(dirOut, path);
  return true;
}

AL2O3_EXTERN_C bool Os_GetAppPrefsDir(char const *org, char const *app, char *dirOut, int maxSize) {
  const char *rawUserPath = [[[[NSFileManager defaultManager] homeDirectoryForCurrentUser] absoluteString] UTF8String];
	if (rawUserPath == nullptr) {
		return false;
	}
  char const * path;
  path  = (char const *) utf8str(rawUserPath, "/Users/");

  tinystl::string out = tinystl::string(path) +
      tinystl::string("Library/") +
      tinystl::string(org) +
      tinystl::string("/") +
      tinystl::string(app);

  if (out.size() >= maxSize) { return false; }

  utf8cpy(dirOut, out.c_str());
  return true;
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

