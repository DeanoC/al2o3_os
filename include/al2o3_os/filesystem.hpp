#pragma once
#ifndef WYRD_OS_FILESYSTEM_HPP
#define WYRD_OS_FILESYSTEM_HPP

#include "al2o3_platform/platform.h"
#include "al2o3_os/filesystem.h"
#include "al2o3_tinystl/string.hpp"
#include "al2o3_tinystl/vector.hpp"

namespace Os {
namespace FileSystem {

typedef void (*FileDialogCallbackFn)(tinystl::string url, void *userData);

inline bool IsNormalisedPath(tinystl::string const& path) {
  return Os_IsNormalisedPath(path.c_str());
}

inline tinystl::string GetNormalisedPathFromPlatformPath(tinystl::string const& path) {
  char tmp[2048];
  if (Os_GetNormalisedPathFromPlatformPath(path.c_str(), tmp, 2048)) {
    return tinystl::string(tmp);
  } else {
    return tinystl::string();
  }
}

inline tinystl::string GetPlatformPathFromNormalisedPath(tinystl::string const& path) {
  char tmp[2048];
  if (Os_GetPlatformPathFromNormalisedPath(path.c_str(), tmp, sizeof(tmp))) {
    return tinystl::string(tmp);
  } else {
    return tinystl::string();
  }
}

inline bool IsAbsolutePath(tinystl::string const& path) {
  return Os_IsAbsolutePath(path.c_str());
}

bool SplitPath(tinystl::string const& fullPath, tinystl::string_view& fileName,
               tinystl::string_view& extension);
inline bool SplitPath(tinystl::string const& fullPath, tinystl::string& fileName,
               tinystl::string& extension) {
  tinystl::string_view fn, ext;
  bool result = SplitPath(fullPath, fn, ext);
  fileName = fn;
  extension = ext;
  return result;
}

inline tinystl::string ReplaceExtension(tinystl::string const& path,
                                        tinystl::string const& newExtension) {
  char tmp[2048];
  bool okayReplaceExt = Os_ReplaceExtension(path.c_str(), newExtension.c_str(), tmp, sizeof(2048));
  if (okayReplaceExt) { return tinystl::string(tmp); }
  else { return {}; }
}

inline tinystl::string GetParentPath(tinystl::string const& path) {
  char tmp[2048];
  bool okayParentPath = Os_GetParentPath(path.c_str(), tmp, sizeof(2048));
  if (okayParentPath) { return tinystl::string(tmp); }
  else { return {}; }
}

inline tinystl::string GetCurrentDir() {
  char tmp[2048];
  if (Os_GetCurrentDir(tmp, sizeof(2048))) {
    return tinystl::string(tmp);
  } else {
    return tinystl::string();
  }
}

inline bool SetCurrentDir(tinystl::string const& path) {
  return Os_SetCurrentDir(path.c_str());
}

inline bool FileExists(tinystl::string const& path) {
  return Os_FileExists(path.c_str());
}

inline bool DirExists(tinystl::string const& path) {
  return Os_DirExists(path.c_str());
}

inline bool FileCopy(tinystl::string const& src, tinystl::string const& dst) {
  return Os_FileCopy(src.c_str(), dst.c_str());
}

inline bool FileDelete(tinystl::string const& src) {
  return Os_FileDelete(src.c_str());
}

inline bool DirCreate(tinystl::string const& dir) {
  return Os_DirCreate(dir.c_str());
}

inline tinystl::string GetFileName(tinystl::string const& path) {
  tinystl::string_view fileName;
  tinystl::string_view extension;
  bool splitOk = FileSystem::SplitPath(path, fileName, extension);
  if (splitOk) { return fileName; }
  else { return {}; }
}

inline tinystl::string GetExtension(tinystl::string const& path) {
  tinystl::string_view fileName;
  tinystl::string_view extension;
  bool splitOk = FileSystem::SplitPath(path, fileName, extension);
  if (splitOk) { return extension; }
  else { return {}; }
}

inline tinystl::string GetExePath() {
  char tmp[2048];
  if (Os_GetExePath(tmp, sizeof(tmp))) {
    return tinystl::string(tmp);
  } else { return {}; }
}

inline tinystl::string GetUserDocumentsDir() {
  char tmp[2048];
  if (Os_GetUserDocumentsDir(tmp, sizeof(tmp))) {
    return tinystl::string(tmp);
  } else { return {}; }
}

inline tinystl::string GetAppPrefsDir(tinystl::string const& org, tinystl::string const& app) {
  char tmp[2048];
  if (Os_GetAppPrefsDir(org.c_str(), app.c_str(), tmp, sizeof(tmp))) {
    return tinystl::string(tmp);
  } else { return {}; }
}

inline int SystemRun(tinystl::string const& fileName, tinystl::vector<tinystl::string> const& args) {
  char const* cargs[100];
  ASSERT(args.size() < 100);
  for (auto i = 0u; i < args.size(); ++i) {
    cargs[i] = args[i].c_str();
  }
  return Os_SystemRun(fileName.c_str(), (int)args.size(), cargs);
}

inline size_t GetLastModifiedTime(tinystl::string const& fileName) {
  return Os_GetLastModifiedTime(fileName.c_str());
}

} // namespace FileSystem
} // end namespace Os

#endif // WYRD_OS_FILESYSTEM_HPP
