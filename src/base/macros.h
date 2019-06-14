#pragma once

#define UNUSED_VARIABLE(x) (void)(x)

#define STATIC_ONLY(Type)                   \
  Type() = delete;                          \
  Type(const Type&) = delete;               \
  Type& operator=(const Type&) = delete;    \
  void* operator new(std::size_t) = delete; \
  void* operator new(std::size_t, void*) = delete

#define NOTREACHED()                                               \
  DLOG(INFO) << "Should not reach the line " << __LINE__ << " in " \
             << __FUNCTION__ << " of file " << __FILE__
