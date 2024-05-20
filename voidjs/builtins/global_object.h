#ifndef VOIDJS_BUILTINS_GLOBAL_OBJECT_H
#define VOIDJS_BUILTINS_GLOBAL_OBJECT_H

#include "voidjs/types/js_value.h"
#include "voidjs/types/lang_types/object.h"
#include "voidjs/interpreter/runtime_call_info.h"

namespace voidjs {
namespace builtins {

class GlobalObject : public types::Object {
 public:
  static constexpr std::size_t SIZE = 0;
  static constexpr std::size_t END_OFFSET = types::Object::END_OFFSET + SIZE;
  static_assert(SIZE == 0);

  // Host Functions
  static JSValue Print(RuntimeCallInfo* argv); 
};

}  // namespace builtins
}  // namespace voidjs

#endif  // VOIDJS_BUILTINS_GLOBAL_OBJECT_H
