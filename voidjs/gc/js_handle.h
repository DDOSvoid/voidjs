#ifndef VOIDJS_GC_JS_HANDLE_H
#define VOIDJS_GC_JS_HANDLE_H

#include <cstdint>

#include "voidjs/types/js_value.h"
#include "voidjs/types/heap_object.h"
#include "voidjs/gc/js_handle_scope.h"
#include "voidjs/interpreter/vm.h"

namespace voidjs {

template <typename T>
class JSHandle {
 public:
  JSHandle()
    : address_(reinterpret_cast<std::uintptr_t>(nullptr))
  {}

  JSHandle(VM* vm, JSValue value)
    : address_(JSHandleScope::NewHandle(vm, value.GetRawData()))
  {}

  JSHandle(VM* vm, HeapObject* object)
    : address_(JSHandleScope::NewHandle(vm, reinterpret_cast<JSValueType>(object)))
  {}

  std::uintptr_t GetAddress() const { return address_; }

  JSValue GetJSValue() const { return JSValue{address_}; }

  T* operator*() const {
    return GetJSValue().GetHeapObject()->template As<T>();
  }

  T* operator->() const {
    return GetJSValue().GetHeapObject()->template As<T>();
  }

  bool operator==(const JSHandle<T>& other) const {
    return address_ == other.address_;
  }

  bool operator!=(const JSHandle<T>& other) const {
    return address_ != other.address_;
  }

  bool IsEmpty() const { return address_ != 0; } 

 private:
  std::uintptr_t address_;
};

}  // namespace voidjs

#endif  // VOIDJS_GC_JS_HANDLE_H
