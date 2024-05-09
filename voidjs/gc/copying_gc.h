#ifndef VOIDJS_GC_COPYING_GC_H
#define VOIDJS_GC_COPYING_GC_H

#include <iostream>
#include <cstdint>
#include <cstddef>
#include <cstring>

#include "voidjs/types/js_value.h"
#include "voidjs/gc/gc_base.h"
#include "voidjs/interpreter/vm.h"
#include "voidjs/utils/helper.h"

namespace voidjs {
namespace gc {

class CopyingGC {
 public:
  CopyingGC(VM* vm, std::size_t size)
    : vm_(vm) {
    auto space = reinterpret_cast<std::uintptr_t>(new std::byte[size]{});
    fromspace_ = space;
    tospace_ = space + size / 2;
    extent_ = size / 2; 
    alloc_ = fromspace_;
  }

  ~CopyingGC() {
    delete[] utils::BitCast<std::byte*>(std::min(fromspace_, tospace_));
  }
  
  std::uintptr_t Allocate(std::size_t size) {
    if (size & 0x7) {
      size += 0x8 - (size & 0x7);
    }
    if (alloc_ + size > fromspace_ + extent_) {
      Collect();
    }
    std::uintptr_t ret = alloc_;
    alloc_ += size;
    return ret;
  }

  void Collect() {
    forward_addr_map_.clear();
    
    std::swap(fromspace_, tospace_);
    alloc_ = fromspace_;
    scan_ = fromspace_;

    std::vector<JSHandle<JSValue>> handles = vm_->GetRoots();
    for (auto handle : handles) {
      Copy(handle);
    }

    while (scan_ < alloc_) {
      auto value = JSValue{scan_};
      std::vector<JSHandle<JSValue>> handles = HeapObject::GetValues(value);
      for (auto handle : handles) {
        Copy(handle);
      }
      scan_ += HeapObject::GetSize(value);
    }
  }

  void Copy(JSHandle<JSValue> handle) {
    if (!handle->IsHeapObject()) {
      return;
    }

    if (!InHeapSpace(handle.GetJSValue().GetRawData())) {
      return;
    }

    if (auto it = forward_addr_map_.find(handle.GetJSValue().GetRawData());
        it != forward_addr_map_.end()) {
      std::uintptr_t addr = it->second;
      *reinterpret_cast<JSValue*>(handle.GetAddress()) = JSValue{addr};
      return ;
    }
    
    std::uintptr_t addr = alloc_;
    std::size_t size = HeapObject::GetSize(handle);
    alloc_ += size;

    std::memcpy((void*)addr, (void*)handle.GetJSValue().GetRawData(), size);

    *reinterpret_cast<JSValue*>(handle.GetAddress()) = JSValue{addr};
  }

 private:
  bool InHeapSpace(std::uintptr_t addr) {
    auto [min_addr, max_addr] = std::minmax(fromspace_, tospace_);
    return addr < min_addr || addr > max_addr;
  }

 private:
  VM* vm_ {nullptr};
  std::uintptr_t fromspace_ {0};
  std::uintptr_t tospace_ {0};
  std::uintptr_t extent_ {0};
  std::uintptr_t alloc_ {0};
  std::uintptr_t scan_ {0};
  
  std::unordered_map<std::uintptr_t, std::uintptr_t> forward_addr_map_;
};

}  // namespace gc
}  // namespace voidjs

#endif // VOIDJS_GC_COPYING_GC_H
