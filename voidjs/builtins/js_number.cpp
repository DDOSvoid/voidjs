#include "voidjs/builtins/js_number.h"

#include "voidjs/gc/js_handle_scope.h"
#include "voidjs/types/heap_object.h"
#include "voidjs/types/js_value.h"
#include "voidjs/types/object_factory.h"
#include "voidjs/types/lang_types/number.h"
#include "voidjs/interpreter/runtime_call_info.h"

namespace voidjs {
namespace builtins {

JSValue JSNumber::NumberConstructorCall(RuntimeCallInfo* argv) {
  VM* vm = argv->GetVM();
  JSHandleScope handle_scope{vm};
  ObjectFactory* factory = vm->GetObjectFactory();

  return argv->GetArgsNum() == 0 ?
    JSValue{0} : JSValue{JSValue::ToNumber(vm, argv->GetArg(0))};
}

JSValue JSNumber::NumberConstructorConstruct(RuntimeCallInfo* argv) {
  VM* vm = argv->GetVM();
  JSHandleScope handle_scope{vm};
  ObjectFactory* factory = vm->GetObjectFactory();
  
  // The [[Prototype]] internal property of the newly constructed object is
  // set to the original Number prototype object,
  // the one that is the initial value of Number.prototype (15.7.3.1).
  // 
  // The [[Class]] internal property of the newly constructed object is set to "Number".
  // 
  // The [[PrimitiveValue]] internal property of the newly constructed object is set to ToNumber(value)
  // if value was supplied, else to +0.
  // 
  // The [[Extensible]] internal property of the newly constructed object is set to true.
  auto num = factory->NewObject(JSNumber::SIZE, JSType::JS_NUMBER, ObjectClassType::NUMBER,
                                vm->GetNumberPrototype().As<JSValue>(), true, false, false).As<JSNumber>();
  auto val = JSHandle<JSValue>{vm,
    argv->GetArgsNum() == 0 ?
    JSValue{0} : JSValue{JSValue::ToNumber(vm, argv->GetArg(0))}};
  num->SetPrimitiveValue(JSValue{JSValue::ToNumber(vm, val.As<JSValue>())});

  return num.GetJSValue();
}

}  // namespace builtins
}  // namespace voidjs
