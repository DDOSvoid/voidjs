#include "voidjs/interpreter/global_constants.h"

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


DEFINE_GET_METHOD_FOR_HEAP_OBJECT(LengthString, String, types::String, 4)

#undef DEFINE_GET_METHOD_FOR_JSVALUE
#undef DEFINE_GET_METHOD_FOR_HEAP_OBJECT

}  // namespace voidjs
