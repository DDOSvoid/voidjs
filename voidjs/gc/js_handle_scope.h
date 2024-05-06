#ifndef VOIDJS_GC_JS_HANDLE_SCOPE_H
#define VOIDJS_GC_JS_HANDLE_SCOPE_H

#include "voidjs/types/js_value.h"
#include "voidjs/interpreter/vm.h"

namespace voidjs {

class JSHandleScope {
 public:
  explicit JSHandleScope(VM* vm);
  ~JSHandleScope();
  
  static std::uintptr_t NewHandle(VM* vm, JSValueType value);

 private:
  VM* vm_ {nullptr};
  JSValueType* prev_pos_ {nullptr};
  JSValueType* prev_end_ {nullptr};
  std::int32_t prev_index_ {-1};
};

}  // namespace voidjs

#endif  // VOIDJS_GC_JS_HANDLE_SCOPE_H
