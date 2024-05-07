#ifndef VOIDJS_GC_JS_HANDLE_H
#define VOIDJS_GC_JS_HANDLE_H

#include <cstdint>
#include <type_traits>

#include "voidjs/types/js_value.h"
#include "voidjs/types/heap_object.h"
#include "voidjs/gc/js_handle_scope.h"

namespace voidjs {

class VM;

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

  // use this constructor
  explicit JSHandle(std::uintptr_t slot)
    : address_(slot)
  {}

  template <typename U>
  JSHandle(JSHandle<U> handle)
    : address_(handle.GetAddress()) {
    static_assert(std::is_convertible_v<std::remove_cv_t<U>, std::remove_cv_t<T>>);
  }

  template <typename U>
  JSHandle<U> As() {
    static_assert(std::is_convertible_v<std::remove_cv_t<U>, std::remove_cv_t<T>> ||
                  std::is_convertible_v<std::remove_cv_t<T>, std::remove_cv_t<U>> ||
                  std::is_same_v<JSValue, std::remove_cv_t<T>>                    ||
                  std::is_same_v<JSValue, std::remove_cv_t<U>>);
    return JSHandle<U>{address_};
  }

  std::uintptr_t GetAddress() const { return address_; }

  JSValue GetJSValue() const {
    if (IsEmpty()) {
      return JSValue::Hole();
    }
    return *reinterpret_cast<JSValue*>(address_);
  }

  T* GetObject() const {
    static_assert(!std::is_same_v<JSValue, std::remove_cv_t<T>>);
    return reinterpret_cast<T*>(GetJSValue().GetHeapObject());
  }

  T* operator*() const {
    return reinterpret_cast<T*>(GetJSValue().GetHeapObject());
  }

  T* operator->() const {
    return reinterpret_cast<T*>(GetJSValue().GetHeapObject());
  }

  bool operator==(const JSHandle<T>& other) const {
    return address_ == other.address_;
  }

  bool operator!=(const JSHandle<T>& other) const {
    return address_ != other.address_;
  }

  bool IsEmpty() const { return address_ == 0; }

 private:
  JSHandle(const T* const slot)
    : address_(slot)
  {}

 private:
  std::uintptr_t address_;
};

template <>
inline JSValue* JSHandle<JSValue>::operator*() const {
  return reinterpret_cast<JSValue*>(address_);
}

template <>
inline JSValue* JSHandle<JSValue>::operator->() const {
  return reinterpret_cast<JSValue*>(address_);
}

}  // namespace voidjs

#endif  // VOIDJS_GC_JS_HANDLE_H
