#include "voidjs/builtins/js_boolean.h"

#include "voidjs/types/heap_object.h"
#include "voidjs/types/js_value.h"
#include "voidjs/types/object_factory.h"
#include "voidjs/gc/js_handle_scope.h"
#include "voidjs/interpreter/global_constants.h"
#include "voidjs/interpreter/runtime_call_info.h"
#include "voidjs/utils/macros.h"

namespace voidjs {
namespace builtins {

// Boolean(value)
// Defined in ECMAScript 5.1 Chapter 15.6.1.1
JSValue JSBoolean::BooleanConstructorCall(RuntimeCallInfo* argv) {
  VM* vm = argv->GetVM();
  JSHandleScope handle_scope{vm};
  JSHandle<JSValue> value = argv->GetArg(0);
  
  // Returns a Boolean value (not a Boolean object) computed by ToBoolean(value).
  return JSValue{JSValue::ToBoolean(vm, value)};
}

// new Boolean(value)
// Defined in ECMAScript 5.1 Chapter 15.6.2.1
JSValue JSBoolean::BooleanConstructorConstruct(RuntimeCallInfo* argv) {
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

// Boolean.prototype.toString()
// Defined in ECMAScript 5.1 Chapter 15.6.4.2
JSValue JSBoolean::ToString(RuntimeCallInfo* argv) {
  VM* vm = argv->GetVM();
  JSHandleScope handle_scope{vm};
  JSHandle<JSValue> this_value = argv->GetThis();
  ObjectFactory* factory = vm->GetObjectFactory();

  // 1. Let B be the this value.
  JSHandle<JSValue> B = this_value;

  // 2. If Type(B) is Boolean, then let b be B.
  JSHandle<JSValue> b;
  if (B->IsBoolean()) {
    b = B;
  }
  // 3. Else if Type(B) is Object and the value of the [[Class]] internal property of B is "Boolean",
  //    then let b be the value of the [[PrimitiveValue]] internal property of B.
  else if (B->IsObject() && B->GetHeapObject()->IsJSBoolean()) {
    b = JSHandle<JSValue>{vm, B.As<JSBoolean>()->GetPrimitiveValue()};
  }
  // 4. Else throw a TypeError exception.
  else {
    THROW_TYPE_ERROR_AND_RETURN_VALUE(
      vm, u"this value is not Boolean or Boolean Object when calling Boolean.prototype.toString()", JSValue{});
  }
  
  // 5. If b is true, then return "true"; else return "false".
  return b->GetBoolean() ?
    vm->GetGlobalConstants()->HandledTrueString().GetJSValue() :
    vm->GetGlobalConstants()->HandledFalseString().GetJSValue();
}

// Boolean.prototype.valueOf()
// Defined in ECMAScript 5.1 Chapter 15.6.4.3
JSValue JSBoolean::ValueOf(RuntimeCallInfo* argv) {
  VM* vm = argv->GetVM();
  JSHandleScope handle_scope{vm};
  JSHandle<JSValue> this_value = argv->GetThis();
  ObjectFactory* factory = vm->GetObjectFactory();

  // 1. Let B be the this value.
  JSHandle<JSValue> B = this_value;

  // 2. If Type(B) is Boolean, then let b be B.
  JSHandle<JSValue> b;
  if (B->IsBoolean()) {
    b = B;
  }
  // 3. Else if Type(B) is Object and the value of the [[Class]] internal property of B is "Boolean",
  //    then let b be the value of the [[PrimitiveValue]] internal property of B.
  else if (B->IsObject() && B->GetHeapObject()->IsJSBoolean()) {
    b = JSHandle<JSValue>{vm, B.As<JSBoolean>()->GetPrimitiveValue()};
  }
  // 4. Else throw a TypeError exception.
  else {
    THROW_TYPE_ERROR_AND_RETURN_VALUE(
      vm, u"this value is not Boolean or Boolean Object when calling Boolean.prototype.toString()", JSValue{});
  }
  
  // 5. Return b.
  return b.GetJSValue();
}

}  // namespace builtins
}  // namespace voidjs
