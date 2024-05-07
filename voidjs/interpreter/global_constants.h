#ifndef VOIDJS_INTERPRETER_GLOBAL_CONSTANTS_H
#define VOIDJS_INTERPRETER_GLOBAL_CONSTANTS_H

#include <cstdint>

#include "voidjs/types/js_value.h"
#include "voidjs/gc/js_handle.h"

#define DECLARE_GET_METHOD_FOR_JSVALUE(name, index) \
  JSValue name() const;                             \
  JSHandle<JSValue> Handled##name() const;          \

#define DECLARE_GET_METHOD_FOR_HEAP_OBJECT(name, type, index)   \
  type* name() const;                                           \
  JSHandle<type> Handled##name() const;                         \

namespace voidjs {
class GlobalConstants {
 public:
  DECLARE_GET_METHOD_FOR_JSVALUE(Undefined, 0)
  DECLARE_GET_METHOD_FOR_JSVALUE(Null, 1)
  DECLARE_GET_METHOD_FOR_JSVALUE(False, 2)
  DECLARE_GET_METHOD_FOR_JSVALUE(True, 2)
  
  DECLARE_GET_METHOD_FOR_HEAP_OBJECT(LengthString, types::String, index)
  
 private:
  static constexpr std::size_t GLOBAL_CONSTANTS_NUM = 10;
  JSValue constants_[GLOBAL_CONSTANTS_NUM];
};

#undef DECLARE_GET_METHOD
}  // namespace voidjs

#endif  // VOIDJS_INTERPRETER_GLOBAL_CONSTANTS_H
