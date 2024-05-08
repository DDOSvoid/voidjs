#ifndef VOIDJS_GC_NO_GC_H
#define VOIDJS_GC_NO_GC_H

#include "voidjs/gc/gc_base.h"
#include "voidjs/interpreter/vm.h"
#include "voidjs/utils/helper.h"

namespace voidjs {
namespace gc {

class NoGC {
 public:
  NoGC(VM* vm, std::size_t size) {
    space_ = reinterpret_cast<std::uintptr_t>(new std::byte[size]{});
  }

  ~NoGC() {
    delete[] utils::BitCast<std::byte*>(space_);
  }
  
  std::uintptr_t Allocate(std::size_t size) {
    std::uintptr_t addr = alloc_;
    alloc_ += size;
    return addr;
  }

  void Collect() {
    // do nothing
  }

 private:
  VM* vm_ {nullptr};
  std::uintptr_t space_ {0};
  std::uintptr_t alloc_ {0};
};

}  // namespace gc
}  // namespace voidjs


#endif  // VOIDJS_GC_NO_GC_H
