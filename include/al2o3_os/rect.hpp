#pragma once
#ifndef AL2O3_OS_RECT_HPP
#define AL2O#_OS_RECT_HPP

#include "al2o3_platform/platform.h"
#include "al2o3_os/rect.h"

namespace Os {

using Rect = Os_Rect;

inline int32_t Width(const Rect& rect) {
  return rect.right - rect.left;
}

inline int32_t Height(const Rect& rect) {
  return rect.bottom - rect.top;
}

inline int32_t Width(const Rect* rect) {
  return rect->right - rect->left;
}

inline int32_t Height(const Rect* rect) {
  return rect->bottom - rect->top;
}

} // end namespace Os

#endif //WYRD_RECT_HPP
