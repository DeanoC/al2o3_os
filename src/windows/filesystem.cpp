#include "al2o3_platform/platform.h"
#include "al2o3_platform/windows.h"
#include "al2o3_os/file.h"
#include "al2o3_tinystl/string.hpp"
#include "al2o3_os/thread.h"
#include "al2o3_os/filesystem.hpp"
#include <shlobj_core.h>
#include <io.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/stat.h>

// this can produce false negative if give a pure filename, however in this case it
// doesn't matter, tho in may be slightly slower due to a redundent normalised to platform transform
AL2O3_EXTERN_C bool Os_IsNormalisedPath(char const *p) {
  tinystl::string path(p);
  size_t slash = path.find_last('/');
  if (slash == tinystl::string::npos) {
    return false;
  } else {
    return true;
  }
}

AL2O3_EXTERN_C bool Os_GetNormalisedPathFromPlatformPath(char const *path, char *pathOut, size_t maxSize) {
  ASSERT(path);
  ASSERT(pathOut);

  // copy and replace \ with /
  if (strlen(path) >= maxSize) { return false; }
  unsigned int i = 0;
  do {
    if (path[i] == '\\') {
      pathOut[i] = '/';
    } else {
      pathOut[i] = path[i];
    }
    ++i;
  } while (path[i] != 0);

  pathOut[i] = 0;
  return true;
}

AL2O3_EXTERN_C bool Os_GetPlatformPathFromNormalisedPath(char const *path, char *pathOut, size_t maxSize) {
  ASSERT(path);
  ASSERT(pathOut);

  // copy and replace \ with /
  if (strlen(path) >= maxSize) { return false; }
  unsigned int i = 0;
  do {
    if (path[i] == '/') {
      pathOut[i] = '\\';
    } else {
      pathOut[i] = path[i];
    }
    ++i;
  } while (path[i] != 0);

  pathOut[i] = 0;
  return true;
}

AL2O3_EXTERN_C bool Os_GetCurrentDir(char *pathOut, size_t maxSize) {
  ASSERT(MAX_PATH <= maxSize);
  char tmp[MAX_PATH];
  size_t s = GetCurrentDirectoryA(MAX_PATH, tmp);
	if (s >= MAX_PATH) return false;
  tmp[s] = '\\';
  tmp[s + 1] = 0;
  Os_GetNormalisedPathFromPlatformPath(tmp, pathOut, maxSize);
	return true;
}

AL2O3_EXTERN_C bool Os_SetCurrentDir(const char *path) {
  char tmp[2048];
  if (!Os_GetPlatformPathFromNormalisedPath(path, tmp, sizeof(tmp))) { return false; }

  SetCurrentDirectoryA(tmp);
	return true;
}

AL2O3_EXTERN_C bool Os_FileExists(const char *fileName) {
  char tmp[2048];
  if (!Os_GetPlatformPathFromNormalisedPath(fileName, tmp, sizeof(tmp))) { return false; }

#ifdef _DURANGO
  return (fopen(tmp, "rb") != NULL);
#else
  return ((_access(tmp, 0)) != -1);
#endif
}

AL2O3_EXTERN_C bool Os_DirExists(char const *pathName) {
  char tmp[2048];
  if (!Os_GetPlatformPathFromNormalisedPath(pathName, tmp, sizeof(tmp))) { return false; }

  DWORD attributes = GetFileAttributesA(tmp);
  if (attributes == INVALID_FILE_ATTRIBUTES ||
      !(attributes & FILE_ATTRIBUTE_DIRECTORY)) {
    return false;
  } else { return true; }
}

AL2O3_EXTERN_C bool Os_FileCopy(char const *src, char const *dst) {
  char srctmp[2048];
  char dsttmp[2048];
  if (!Os_GetPlatformPathFromNormalisedPath(src, srctmp, sizeof(srctmp))) { return false; }
  if (!Os_GetPlatformPathFromNormalisedPath(dst, dsttmp, sizeof(dsttmp))) { return false; }

  return CopyFileA(srctmp, dsttmp, FALSE) ? true : false;

}

AL2O3_EXTERN_C bool Os_FileDelete(char const *fileName) {
  char tmp[2048];
  if (!Os_GetPlatformPathFromNormalisedPath(fileName, tmp, sizeof(tmp))) { return false; }
  return DeleteFileA(tmp) != 0;
}

AL2O3_EXTERN_C bool Os_GetExePath(char *dirOut, int maxSize) {
  dirOut[0] = 0;
  GetModuleFileNameA(nullptr, dirOut, maxSize);
  return true;
}

AL2O3_EXTERN_C bool Os_GetUserDocumentsDir(char *dirOut, int maxSize) {
  ASSERT(maxSize >= MAX_PATH);
  dirOut[0] = 0;
  SHGetSpecialFolderPathA(nullptr, dirOut, CSIDL_PERSONAL, 0);
  return true;
}

AL2O3_EXTERN_C bool Os_GetAppPrefsDir(const char *org, const char *app, char *dirOut, int maxSize) {
  /*
  * Vista and later has a new API for this, but SHGetFolderPath works there,
  *  and apparently just wraps the new API. This is the new way to do it:
  *
  *	SHGetKnownFolderPath(FOLDERID_RoamingAppData, KF_FLAG_CREATE,
  *						 NULL, &wszPath);
  */

  ASSERT(maxSize >= MAX_PATH);
  size_t new_wpath_len = 0;

  if (!SUCCEEDED(SHGetFolderPathA(nullptr, CSIDL_APPDATA | CSIDL_FLAG_CREATE, nullptr, 0, dirOut))) {
    return false;
  }

  new_wpath_len = strlen(org) + strlen(app) + strlen(dirOut) + 3;

  if ((new_wpath_len + 1) > MAX_PATH) {
    return false;
  }

  strcat(dirOut, "\\");
  strcat(dirOut, org);

  if (CreateDirectoryA(dirOut, nullptr) == FALSE) {
    if (GetLastError() != ERROR_ALREADY_EXISTS) {
      return NULL;
    }
  }

  strcat(dirOut, "\\");
  strcat(dirOut, app);

  if (CreateDirectoryA(dirOut, nullptr) == FALSE) {
    if (GetLastError() != ERROR_ALREADY_EXISTS) {
      return false;
    }
  }

  strcat(dirOut, "\\");
  return true;
}

AL2O3_EXTERN_C size_t Os_GetLastModifiedTime(const char *_fileName) {
  struct stat fileInfo{};

  if (!stat(_fileName, &fileInfo)) {
    return (size_t) fileInfo.st_mtime;
  } else {
    // return an impossible large mod time as the file doesn't exist
    return ~0;
  }
}
AL2O3_EXTERN_C int Os_SystemRun(char const* fileName, int argc, const char** argv) {
	char path[2048];

	size_t fileNamePos;
	size_t extPos;
	Os_GetPlatformPathFromNormalisedPath(fileName, path, sizeof(path));
	Os_SplitPath(path, &fileNamePos, &extPos);


	// TODO replace memory allocating vis string ops
	tinystl::string pathStr;

	if (extPos == FS_npos) {
		pathStr = "\"" + tinystl::string(path) + ".exe\"";
	}
	else {
		pathStr = "\"" + tinystl::string(path) + "\"";
	}

	tinystl::string commandLine = pathStr;

	for (int i = 0; i < argc; ++i) {
		commandLine += " " + tinystl::string(argv[i]);
	}

	STARTUPINFOA        startupInfo;
	PROCESS_INFORMATION processInfo;
	memset(&startupInfo, 0, sizeof startupInfo);
	memset(&processInfo, 0, sizeof processInfo);
	startupInfo.cb = sizeof(STARTUPINFO);
	startupInfo.dwFlags |= STARTF_USESTDHANDLES;
	startupInfo.hStdOutput = GetStdHandle(STD_OUTPUT_HANDLE);
	startupInfo.hStdError = GetStdHandle(STD_ERROR_HANDLE);

	if (!CreateProcessA(
		NULL, (LPSTR)commandLine.c_str(), NULL, NULL, TRUE, CREATE_NO_WINDOW, NULL, NULL, &startupInfo, &processInfo))
		return -1;

	WaitForSingleObject(processInfo.hProcess, INFINITE);
	DWORD exitCode;
	GetExitCodeProcess(processInfo.hProcess, &exitCode);

	CloseHandle(processInfo.hProcess);
	CloseHandle(processInfo.hThread);

	return exitCode;
}

struct Os_WinDirectoryEnumerator {
	char path[2048];
	HANDLE findHandle;
	Os_DirectoryEnumeratorFunc func;
	void* userData;
};

AL2O3_EXTERN_C Os_DirectoryEnumeratorHandle Os_DirectoryEnumeratorFromPath(char const* cpath, Os_DirectoryEnumeratorFunc func, void* userData) {
	ASSERT(cpath);
	ASSERT(func);

	Os_WinDirectoryEnumerator* enumerator = (Os_WinDirectoryEnumerator*)malloc(sizeof(*enumerator));
	if (enumerator == nullptr) return nullptr;

	Os_GetPlatformPathFromNormalisedPath(cpath, enumerator->path, sizeof(enumerator->path));
	size_t s = strlen(enumerator->path);
	if (s + 1 < sizeof(enumerator->path)) {
		enumerator->path[s] = '*';
		enumerator->path[s + 1] = 0;
		s = s + 1;
	}
	else {
		free(enumerator);
		return nullptr;
	}
	enumerator->findHandle = INVALID_HANDLE_VALUE;
	enumerator->func = func;
	enumerator->userData = userData;
	return (Os_DirectoryEnumeratorHandle)enumerator;
}

AL2O3_EXTERN_C void Os_DirectoryEnumeratorClose(Os_DirectoryEnumeratorHandle handle) {
	ASSERT(handle != nullptr);
	Os_WinDirectoryEnumerator* enumerator = (Os_WinDirectoryEnumerator*)handle;
	if(enumerator->findHandle != INVALID_HANDLE_VALUE) FindClose(enumerator->findHandle);
	free(enumerator);
}

AL2O3_EXTERN_C bool Os_DirectoryEnumeratorAsyncStart(Os_DirectoryEnumeratorHandle handle) {
	LOGINFOF("Os_DirectoryEnumeratorAsyncStart isn't async on windows yet, will be a busy sync for now");

	// TODO implement Async for now its just sync in a loop :(
	if (Os_DirectoryEnumeratorSyncStart(handle) == false) return false;
	while (Os_DirectoryEnumeratorSyncNext(handle)) {
		// empty loop action happens in sync next
	}
}

AL2O3_EXTERN_C bool Os_DirectoryEnumeratorSyncStart(Os_DirectoryEnumeratorHandle handle) {
	ASSERT(handle != nullptr);
	Os_WinDirectoryEnumerator * enumerator = (Os_WinDirectoryEnumerator*)handle;

	// check it hasn't already started
	if (enumerator->findHandle != INVALID_HANDLE_VALUE) {
		return false;
	}

	WIN32_FIND_DATAA findData;
	enumerator->findHandle = FindFirstFileA(enumerator->path, &findData);
	if (enumerator->findHandle == INVALID_HANDLE_VALUE)
	{
		return false;
	}
	enumerator->func(handle, enumerator->userData, findData.cFileName);
	return true;
}

AL2O3_EXTERN_C bool Os_DirectoryEnumeratorSyncNext(Os_DirectoryEnumeratorHandle handle) {
	ASSERT(handle != nullptr);
	Os_WinDirectoryEnumerator* enumerator = (Os_WinDirectoryEnumerator*)handle;
	ASSERT(enumerator->findHandle != INVALID_HANDLE_VALUE);
	WIN32_FIND_DATAA findData;
	bool ndone = FindNextFileA(enumerator->findHandle, &findData) != 0;
	if (ndone) {
		enumerator->func(handle, enumerator->userData, findData.cFileName);
	}

	return ndone;
}

AL2O3_EXTERN_C bool Os_DirectoryEnumeratorCancel(Os_DirectoryEnumeratorHandle handle) {
	// TODO no async means this doesn't do anything
	return true;
}
AL2O3_EXTERN_C bool Os_DirectoryEnumeratorStallForAll(Os_DirectoryEnumeratorHandle handle) {
	// TODO no async means this doesn't do anything
	return true;
}

/*
void get_files_with_extension(const char* dir, const char* ext, tinystl::vector<tinystl::string>& filesOut)
{
	tinystl::string  path = FileSystem::GetNativePath(FileSystem::AddTrailingSlash(dir));
	WIN32_FIND_DATAA fd;
	HANDLE           hFind = ::FindFirstFileA(path + "*" + ext, &fd);
	uint32_t         fileIndex = (uint32_t)filesOut.size();
	if (hFind != INVALID_HANDLE_VALUE)
	{
		do
		{
			filesOut.resize(fileIndex + 1);
			//copy the strings to avoid the memory being cleaned up by windows.
			filesOut[fileIndex] = "";
			filesOut[fileIndex++] = path + fd.cFileName;
		} while (::FindNextFileA(hFind, &fd));
		::FindClose(hFind);
	}
}

void get_sub_directories(const char* dir, tinystl::vector<tinystl::string>& subDirectoriesOut)
{
	tinystl::string  path = FileSystem::GetNativePath(FileSystem::AddTrailingSlash(dir));
	WIN32_FIND_DATAA fd;
	HANDLE           hFind = ::FindFirstFileA(path + "*", &fd);
	uint32_t         fileIndex = (uint32_t)subDirectoriesOut.size();
	if (hFind != INVALID_HANDLE_VALUE)
	{
		do
		{
			// skip files, ./ and ../
			if (!strchr(fd.cFileName, '.'))
			{
				subDirectoriesOut.resize(fileIndex + 1);
				//copy the strings to avoid the memory being cleaned up by windows.
				subDirectoriesOut[fileIndex] = "";
				subDirectoriesOut[fileIndex++] = path + fd.cFileName;
			}
		} while (::FindNextFileA(hFind, &fd));
		::FindClose(hFind);
	}
}


static void
	FormatFileExtensionsFilter(const char* fileDesc, const tinystl::vector<tinystl::string>& extFiltersIn, tinystl::string& extFiltersOut)
{
	extFiltersOut = fileDesc;
	extFiltersOut.push_back('\0');
	for (size_t i = 0; i < extFiltersIn.size(); ++i)
	{
		tinystl::string ext = extFiltersIn[i];
		if (ext.size() && ext[0] == '.')
			ext = (ext.begin() + 1);
		extFiltersOut += "*.";
		extFiltersOut += ext;
		if (i != extFiltersIn.size() - 1)
			extFiltersOut += ";";
		else
			extFiltersOut.push_back('\0');
	}
}

void open_file_dialog(
	const char* title, const char* dir, FileDialogCallbackFn callback, void* userData, const char* fileDesc,
	const tinystl::vector<tinystl::string>& fileExtensions)
{
	tinystl::string extFilter;
	FormatFileExtensionsFilter(fileDesc, fileExtensions, extFilter);
	OPENFILENAMEA ofn;                 // common dialog box structure
	char          szFile[MAX_PATH];    // buffer for file name
									   // Initialize OPENFILENAME
	ZeroMemory(&ofn, sizeof(ofn));
	ofn.lpstrTitle = title;
	ofn.lStructSize = sizeof(ofn);
	ofn.hwndOwner = NULL;
	ofn.lpstrFile = szFile;
	// Set lpstrFile[0] to '\0' so that GetOpenFileName does not
	// use the contents of szFile to initialize itself.
	ofn.lpstrFile[0] = '\0';
	ofn.nMaxFile = sizeof(szFile);
	ofn.lpstrFilter = extFilter;
	ofn.nFilterIndex = 1;
	ofn.lpstrFileTitle = NULL;
	ofn.nMaxFileTitle = 0;
	ofn.lpstrInitialDir = dir;
	ofn.Flags = OFN_EXPLORER | OFN_PATHMUSTEXIST | OFN_FILEMUSTEXIST | OFN_NOCHANGEDIR;

	if (::GetOpenFileNameA(&ofn) == TRUE)
	{
		callback(szFile, userData);
	}
}

void save_file_dialog(
	const char* title, const char* dir, FileDialogCallbackFn callback, void* userData, const char* fileDesc,
	const tinystl::vector<tinystl::string>& fileExtensions)
{
	tinystl::string extFilter;
	FormatFileExtensionsFilter(fileDesc, fileExtensions, extFilter);
	OPENFILENAMEA ofn;
	// buffer for file name
	char szFile[MAX_PATH];
	// Initialize OPENFILENAME
	ZeroMemory(&ofn, sizeof(ofn));
	ofn.lpstrTitle = title;
	ofn.lStructSize = sizeof(ofn);
	ofn.hwndOwner = NULL;
	ofn.lpstrFile = szFile;
	// Set lpstrFile[0] to '\0' so that GetOpenFileName does not
	// use the contents of szFile to initialize itself.
	ofn.lpstrFile[0] = '\0';
	ofn.nMaxFile = sizeof(szFile);
	ofn.lpstrFilter = extFilter;
	ofn.nFilterIndex = 1;
	ofn.lpstrFileTitle = NULL;
	ofn.nMaxFileTitle = 0;
	ofn.lpstrInitialDir = dir;
	ofn.Flags = OFN_EXPLORER | OFN_NOCHANGEDIR;

	if (::GetSaveFileNameA(&ofn) == TRUE)
	{
		callback(szFile, userData);
	}
}

 */

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
