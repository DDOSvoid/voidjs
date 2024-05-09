#ifndef VOIDJS_INTERPRETER_GLOBAL_CONSTANTS_H
#define VOIDJS_INTERPRETER_GLOBAL_CONSTANTS_H

#include <cstdint>

#include "voidjs/types/js_value.h"
#include "voidjs/gc/js_handle.h"

#define DECLARE_GET_METHOD_FOR_JSVALUE(name)    \
  JSValue name() const;                         \
  JSHandle<JSValue> Handled##name() const;      \

#define DECLARE_GET_METHOD_FOR_HEAP_OBJECT(name, type)  \
  type* name() const;                                   \
  JSHandle<type> Handled##name() const;                 \

namespace voidjs {
class GlobalConstants {
 public:
  GlobalConstants(VM* vm)
    : vm_(vm) {}

  void Initialize();
  
  DECLARE_GET_METHOD_FOR_JSVALUE(Undefined)
  DECLARE_GET_METHOD_FOR_JSVALUE(Null)
  DECLARE_GET_METHOD_FOR_JSVALUE(False)
  DECLARE_GET_METHOD_FOR_JSVALUE(True)
  
  DECLARE_GET_METHOD_FOR_HEAP_OBJECT(EmptyString, types::String)
  
  DECLARE_GET_METHOD_FOR_HEAP_OBJECT(LengthString, types::String)
  
  DECLARE_GET_METHOD_FOR_HEAP_OBJECT(UndefinedString, types::String)
  DECLARE_GET_METHOD_FOR_HEAP_OBJECT(NullString, types::String)
  DECLARE_GET_METHOD_FOR_HEAP_OBJECT(FalseString, types::String)
  DECLARE_GET_METHOD_FOR_HEAP_OBJECT(TrueString, types::String)
  
  DECLARE_GET_METHOD_FOR_HEAP_OBJECT(ZeroString, types::String)
  DECLARE_GET_METHOD_FOR_HEAP_OBJECT(NANString, types::String)
  DECLARE_GET_METHOD_FOR_HEAP_OBJECT(PositiveInfinityString, types::String)
  DECLARE_GET_METHOD_FOR_HEAP_OBJECT(NegativeInfinityString, types::String)
  
  DECLARE_GET_METHOD_FOR_HEAP_OBJECT(ToStringString, types::String)
  DECLARE_GET_METHOD_FOR_HEAP_OBJECT(ValueOfString, types::String)

  DECLARE_GET_METHOD_FOR_HEAP_OBJECT(ValueString, types::String)
  DECLARE_GET_METHOD_FOR_HEAP_OBJECT(WritableString, types::String)
  DECLARE_GET_METHOD_FOR_HEAP_OBJECT(GetString, types::String)
  DECLARE_GET_METHOD_FOR_HEAP_OBJECT(SetString, types::String)
  DECLARE_GET_METHOD_FOR_HEAP_OBJECT(EnumerableString, types::String)
  DECLARE_GET_METHOD_FOR_HEAP_OBJECT(ConfigurableString, types::String)
  
  DECLARE_GET_METHOD_FOR_HEAP_OBJECT(ConstructorString, types::String)
  DECLARE_GET_METHOD_FOR_HEAP_OBJECT(PrototypeString, types::String)

  DECLARE_GET_METHOD_FOR_HEAP_OBJECT(ObjectString, types::String)
  DECLARE_GET_METHOD_FOR_HEAP_OBJECT(FunctionString, types::String)
  DECLARE_GET_METHOD_FOR_HEAP_OBJECT(ArrayString, types::String)
  DECLARE_GET_METHOD_FOR_HEAP_OBJECT(StringString, types::String)
  DECLARE_GET_METHOD_FOR_HEAP_OBJECT(BooleanString, types::String)
  DECLARE_GET_METHOD_FOR_HEAP_OBJECT(NumberString, types::String)
  DECLARE_GET_METHOD_FOR_HEAP_OBJECT(DateString, types::String)
  DECLARE_GET_METHOD_FOR_HEAP_OBJECT(RegExpString, types::String)
  DECLARE_GET_METHOD_FOR_HEAP_OBJECT(ErrorString, types::String)
  DECLARE_GET_METHOD_FOR_HEAP_OBJECT(EvalErrorString, types::String)
  DECLARE_GET_METHOD_FOR_HEAP_OBJECT(RangeErrorString, types::String)
  DECLARE_GET_METHOD_FOR_HEAP_OBJECT(ReferenceErrorString, types::String)
  DECLARE_GET_METHOD_FOR_HEAP_OBJECT(SyntaxErrorString, types::String)
  DECLARE_GET_METHOD_FOR_HEAP_OBJECT(TypeErrorString, types::String)
  DECLARE_GET_METHOD_FOR_HEAP_OBJECT(URIErrorString, types::String)
  
 private:
  static constexpr std::size_t GLOBAL_CONSTANTS_NUM = 100;
  JSValue constants_[GLOBAL_CONSTANTS_NUM];
  VM* vm_;
};

#undef DECLARE_GET_METHOD
}  // namespace voidjs

#endif  // VOIDJS_INTERPRETER_GLOBAL_CONSTANTS_H
