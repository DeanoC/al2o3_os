// Full license at end of file
// Summary: Apache - original from The-Forge modified by DeanoC

#pragma once

#include "al2o3_platform/platform.h"
#include "al2o3_os/file.h"
#include "al2o3_tinystl/string.hpp"

namespace Os {

// C++ wrapper for File_*
struct File {

  static File Open(tinystl::string const& filename, const Os_FileMode mode) {
    return {Os_FileOpen(filename.c_str(), mode), filename};
  }

  bool Close() { return Os_FileClose(handle); }
  bool IsOpen() const { return handle != nullptr; }

  tinystl::string_view Name() const { return tinystl::string_view(name.c_str(), name.size()); }

  void Flush() { Os_FileFlush(handle); }
  size_t Read(void *buffer, size_t byteCount) { return Os_FileRead(handle, buffer, byteCount); }
  size_t Write(void const *buffer, size_t byteCount) { return Os_FileWrite(handle, buffer, byteCount); };
  bool Seek(int64_t offset, Os_FileSeekDir origin) { return Os_FileSeek(handle, offset, origin); }
  int64_t Tell() const { return Os_FileTell(handle); };

  Os_FileHandle handle = nullptr;
  tinystl::string name;
};

} // end Os namespace
