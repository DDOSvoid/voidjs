#include "voidjs/interpreter/global_constants.h"

#include "voidjs/types/js_value.h"
#include "voidjs/types/lang_types/string.h"

namespace voidjs {

#define DEFINE_GET_METHOD_FOR_JSVALUE(name, index)              \
  JSValue GlobalConstants::name() const  {                      \
    return constants_[index];                                   \
  }                                                             \
  JSHandle<JSValue> GlobalConstants::Handled##name() const  {   \
    return JSHandle<JSValue>{                                   \
      reinterpret_cast<uintptr_t>(&constants_[index])};         \
  }                                                             \

#define DEFINE_GET_METHOD_FOR_HEAP_OBJECT(name, castname, type, index)  \
  type* GlobalConstants::name() const  {                                \
    return constants_[index].GetHeapObject()->As##castname();           \
  }                                                                     \
  JSHandle<type> GlobalConstants::Handled##name() const  {              \
    return JSHandle<type>{                                              \
      reinterpret_cast<uintptr_t>(&constants_[index])};                 \
  }                                                                     \

DEFINE_GET_METHOD_FOR_JSVALUE(Undefined, 0)
DEFINE_GET_METHOD_FOR_JSVALUE(Null, 1)
DEFINE_GET_METHOD_FOR_JSVALUE(False, 2)
DEFINE_GET_METHOD_FOR_JSVALUE(True, 3)

DEFINE_GET_METHOD_FOR_HEAP_OBJECT(EmptyString, String, types::String, 4)

DEFINE_GET_METHOD_FOR_HEAP_OBJECT(LengthString, String, types::String, 5)

DEFINE_GET_METHOD_FOR_HEAP_OBJECT(UndefinedString, String, types::String, 6)
DEFINE_GET_METHOD_FOR_HEAP_OBJECT(NullString, String, types::String, 7)
DEFINE_GET_METHOD_FOR_HEAP_OBJECT(FalseString, String, types::String, 8)
DEFINE_GET_METHOD_FOR_HEAP_OBJECT(TrueString, String, types::String, 9)

DEFINE_GET_METHOD_FOR_HEAP_OBJECT(ZeroString, String, types::String, 10)
DEFINE_GET_METHOD_FOR_HEAP_OBJECT(NANString, String, types::String, 11)
DEFINE_GET_METHOD_FOR_HEAP_OBJECT(PositiveInfinityString, String, types::String, 12)
DEFINE_GET_METHOD_FOR_HEAP_OBJECT(NegativeInfinityString, String, types::String, 13)

DEFINE_GET_METHOD_FOR_HEAP_OBJECT(ToStringString, String, types::String, 14)
DEFINE_GET_METHOD_FOR_HEAP_OBJECT(ValueOfString, String, types::String, 15)

#define SET_CONSTANT(name, value, index)         \
  constants_[index] = value       \


void GlobalConstants::Initialize() {
  auto factory = vm_->GetObjectFactory();
    
  SET_CONSTANT(Undefined, JSValue::Undefined(), 0);
  SET_CONSTANT(Null, JSValue::Null(), 1);
  SET_CONSTANT(Null, JSValue::False(), 2);
  SET_CONSTANT(Null, JSValue::True(), 3);

  SET_CONSTANT(EmptyString, factory->NewString(u"").GetJSValue(), 4);
  
  SET_CONSTANT(LengthString, factory->NewString(u"length").GetJSValue(), 5);
  
  SET_CONSTANT(UndefinedString, factory->NewString(u"undefined").GetJSValue(), 6);
  SET_CONSTANT(NullString, factory->NewString(u"null").GetJSValue(), 7);
  SET_CONSTANT(FalseString, factory->NewString(u"false").GetJSValue(), 8);
  SET_CONSTANT(TrueString, factory->NewString(u"true").GetJSValue(), 9);
  
  SET_CONSTANT(ZeroString, factory->NewString(u"0").GetJSValue(), 10);
  SET_CONSTANT(NANString, factory->NewString(u"NAN").GetJSValue(), 11);
  SET_CONSTANT(PositiveInfinityString, factory->NewString(u"Infinity").GetJSValue(), 12);
  SET_CONSTANT(NegativeInfinityString, factory->NewString(u"-Infinity").GetJSValue(), 13);

  
  SET_CONSTANT(ToStringString, factory->NewString(u"toString").GetJSValue(), 14);
  SET_CONSTANT(ValueOfString, factory->NewString(u"valueOf").GetJSValue(), 15);
}

#undef DEFINE_GET_METHOD_FOR_JSVALUE
#undef DEFINE_GET_METHOD_FOR_HEAP_OBJECT

}  // namespace voidjs
