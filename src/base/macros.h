#pragma once

#define UNUSED_VARIABLE(x) (void)(x)

#define STATIC_ONLY(Type)                \
  Type() = delete;                       \
  Type(const Type&) = delete;            \
  Type& operator=(const Type&) = delete; \
  void* operator new(size_t) = delete;   \
  void* operator new(size_t, void*) = delete
