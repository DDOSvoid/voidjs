#ifndef VOIDJS_GC_HEAP_H
#define VOIDJS_GC_HEAP_H

#include <cstdint>

#include "voidjs/gc/gc_base.h"
#include "voidjs/gc/copying_gc.h"
#include "voidjs/gc/no_gc.h"

namespace voidjs {

class Heap {
 public:
  Heap(VM* vm)
    : normal_space_(vm, NORMAL_SPACE_SIZE),
      const_space_(vm, CONST_SPACE_SIZE)
  {}
  
  std::uintptr_t Allocate(std::size_t size) {
    return normal_space_.Allocate(size);
  }

  std::uintptr_t AllocateFromConstSpace(std::size_t size) {
    return const_space_.Allocate(size);
  }
  
 private:
  static constexpr std::size_t NORMAL_SPACE_SIZE = 512 * 1024 * 1024;  // 512MB
  static constexpr std::size_t CONST_SPACE_SIZE  = 10 * 1024 * 1024;   // 10 MB

  VM* vm_;
  gc::CopyingGC normal_space_;
  gc::NoGC const_space_;
}; 

}  // namespace voidjs

#endif // VOIDJS_GC_HEAP_H
