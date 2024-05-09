#ifndef VOIDJS_GC_HEAP_H
#define VOIDJS_GC_HEAP_H

#include <cstdint>

#include "voidjs/gc/gc_base.h"
#include "voidjs/gc/copying_gc.h"
#include "voidjs/gc/no_gc.h"

namespace voidjs {

enum class GCFlag : std::uint8_t {
  NORMAL,
  CONST,
};

class Heap {
 public:
  Heap(VM* vm)
    : normal_space_(vm, NORMAL_SPACE_SIZE),
      const_space_(vm, CONST_SPACE_SIZE)
  {}

  template <GCFlag flag> 
  std::uintptr_t Allocate(std::size_t size) {
    if constexpr (flag == GCFlag::NORMAL) {
      return normal_space_.Allocate(size);
    } else {
      return const_space_.Allocate(size);
    }
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
