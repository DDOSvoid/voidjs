#include "voidjs/builtins/js_number.h"

#include "voidjs/gc/js_handle_scope.h"
#include "voidjs/types/heap_object.h"
#include "voidjs/types/js_value.h"
#include "voidjs/types/object_factory.h"
#include "voidjs/types/lang_types/number.h"
#include "voidjs/types/lang_types/object.h"
#include "voidjs/builtins/js_function.h"
#include "voidjs/interpreter/runtime_call_info.h"
#include "voidjs/interpreter/global_constants.h"
#include "voidjs/utils/macros.h"

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

// Number.prototype.toString([radix])
// Defined in ECMAScript 5.1 Chapter 15.7.4.2
// todo
JSValue JSNumber::ToString(RuntimeCallInfo* argv) {
  VM* vm = argv->GetVM();
  JSHandleScope handle_scope{vm};
  JSHandle<JSValue> this_value = argv->GetThis();
  JSHandle<JSValue> radix = argv->GetArg(0);
  ObjectFactory* factory = vm->GetObjectFactory();

  JSHandle<JSNumber> number;
  if (this_value->IsNumber()) {
    number = types::Object::Construct(
      vm, vm->GetNumberConstructor(), vm->GetGlobalConstants()->HandledUndefined(), {this_value}).As<JSNumber>();
  } else if (this_value->IsObject() && this_value->GetHeapObject()->IsJSNumber()) {
    number = this_value.As<JSNumber>();
  } else {
    THROW_TYPE_ERROR_AND_RETURN_VALUE(
      vm, u"this value is not a Number or a Number Object in Number.prototype.toString().", JSValue{});
  }

  if (radix->IsUndefined() || JSValue::ToInteger(vm, radix).GetNumber() == 10) {
    return JSValue::ToString(vm, JSHandle<JSValue>{vm, number->GetPrimitiveValue()}).GetJSValue();
  }
}

// Number.prototype.valueOf()
// Defined in ECMAScript 5.1 Chapter 15.7.4.4
// todo
JSValue JSNumber::ValueOf(RuntimeCallInfo* argv) {
  VM* vm = argv->GetVM();
  JSHandleScope handle_scope{vm};
  JSHandle<JSValue> this_value = argv->GetThis();
  ObjectFactory* factory = vm->GetObjectFactory();

  if (!this_value->IsObject() || !this_value->IsNumber() && !this_value->GetHeapObject()->IsJSNumber()) {
    THROW_TYPE_ERROR_AND_RETURN_VALUE(
      vm, u"this value is not a Number or a Number Object in Number.prototype.valueOf().", JSValue{});
  }

  return this_value.GetJSValue();
}

// Number.prototype.toFixed(fractionDigits)
// Defined in ECMAScript 5.1 Chapter 15.7.4.5
JSValue JSNumber::ToFixed(RuntimeCallInfo* argv) {
  VM* vm = argv->GetVM();
  JSHandleScope handle_scope{vm};
  JSHandle<JSValue> this_value = argv->GetThis();
  JSHandle<JSValue> fraction_digits = argv->GetArg(0);
  ObjectFactory* factory = vm->GetObjectFactory();

  // // 1. Let f be ToInteger(fractionDigits). (If fractionDigits is undefined, this step produces the value 0).
  // types::Number f = JSValue::ToInteger(vm, fraction_digits);
  
  // // 2. If f < 0 or f > 20, throw a RangeError exception.
  // if (f.GetNumber() < 0 && f.GetNumber() > 20) {
  //   THROW_RANGE_ERROR_AND_RETURN_VALUE(
  //     vm, u"fractionDigits exceeded [0, 20] in Number.prototype.toFixed(fractionDigits).", JSValue{});
  // }
  
  // // 3. Let x be this Number value.
  // JSHandle<JSValue> x = this_value;
  
  // // 4. If x is NaN, return the String "NaN".
  // // 5. Let s be the empty String.
  // // 6. If x < 0, then
}

}  // namespace builtins
}  // namespace voidjs
