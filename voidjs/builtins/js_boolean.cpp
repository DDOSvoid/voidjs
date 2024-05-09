#include "voidjs/builtins/js_boolean.h"

#include "voidjs/types/heap_object.h"
#include "voidjs/types/js_value.h"
#include "voidjs/types/object_factory.h"
#include "voidjs/interpreter/runtime_call_info.h"

namespace voidjs {
namespace builtins {

// Boolean(value)
// Defined in ECMAScript 5.1 Chapter 15.6.1.1
JSValue JSBoolean::Call(RuntimeCallInfo* argv) {
  auto vm = argv->GetVM();
  auto value = argv->GetArg(0);
  
  // Returns a Boolean value (not a Boolean object) computed by ToBoolean(value).
  return JSValue{JSValue::ToBoolean(vm, value)};
}

// new Boolean(value)
// Defined in ECMAScript 5.1 Chapter 15.6.2.1
JSValue JSBoolean::Construct(RuntimeCallInfo* argv) {
  VM* vm = argv->GetVM();
  JSHandleScope handle_scope{vm};
  JSHandle<JSValue> value = argv->GetArg(0);
  ObjectFactory* factory = vm->GetObjectFactory();
  
  // The [[Prototype]] internal property of the newly constructed object is set to the original Boolean prototype object,
  // the one that is the initial value of Boolean.prototype (15.6.3.1).
  // 
  // The [[Class]] internal property of the newly constructed Boolean object is set to "Boolean".
  //
  // The [[PrimitiveValue]] internal property of the newly constructed Boolean object is set to ToBoolean(value).
  //
  // The [[Extensible]] internal property of the newly constructed object is set to true.
  auto boolean = factory->NewObject(JSBoolean::SIZE, JSType::JS_BOOLEAN, ObjectClassType::BOOLEAN,
                                    vm->GetBooleanPrototype().As<JSValue>(), true, false, false)->AsJSBoolean();
  boolean->SetPrimitiveValue(JSValue{JSValue::ToBoolean(vm, value)});

  return JSValue{boolean};
}

}  // namespace builtins
}  // namespace voidjs
