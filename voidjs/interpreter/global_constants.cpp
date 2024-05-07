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

#define SET_CONSTANT(name, value, index)         \
  constants_[index] = value       \


void GlobalConstants::Initialize() {
  auto factory = vm_->GetObjectFactory();
    
  SET_CONSTANT(Undefined, JSValue::Undefined(), 0);
  SET_CONSTANT(Null, JSValue::Null(), 1);
  SET_CONSTANT(Null, JSValue::False(), 2);
  SET_CONSTANT(Null, JSValue::True(), 3);

  SET_CONSTANT(LengthString, factory->GetStringFromTable(u"length").GetJSValue(), 4);
}

#undef DEFINE_GET_METHOD_FOR_JSVALUE
#undef DEFINE_GET_METHOD_FOR_HEAP_OBJECT

}  // namespace voidjs
