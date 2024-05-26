#include "voidjs/builtins/js_error.h"

#include "voidjs/types/heap_object.h"
#include "voidjs/types/js_type.h"
#include "voidjs/types/js_value.h"
#include "voidjs/types/object_class_type.h"
#include "voidjs/types/object_factory.h"
#include "voidjs/builtins/builtin.h"
#include "voidjs/gc/js_handle_scope.h"
#include "voidjs/interpreter/runtime_call_info.h"
#include "voidjs/interpreter/global_constants.h"
#include "voidjs/utils/macros.h"

namespace voidjs {
namespace builtins {

// Error(message)
// Defined in ECMAScript 5.1 Chapter 15.11.1.1
JSValue JSError::ErrorConstructorCall(RuntimeCallInfo* argv) {
  return ErrorConstructorConstruct(argv);
}

// new Error(message)
// Defined in ECMAScript 5.1 Chapter 15.11.1.2
JSValue JSError::ErrorConstructorConstruct(RuntimeCallInfo* argv) {
  VM* vm = argv->GetVM();
  JSHandleScope handle_scope{vm};
  JSHandle<JSValue> message = argv->GetArg(0);
  ObjectFactory* factory = vm->GetObjectFactory();

  // The [[Prototype]] internal property of the newly constructed object is
  // set to the original Error prototype object,
  // the one that is the initial value of Error.prototype (15.11.3.1).
  // 
  // The [[Class]] internal property of the newly constructed Error object is set to "Error".
  //
  // The [[Extensible]] internal property of the newly constructed object is set to true.
  //
  // If the argument message is not undefined,
  // the message own property of the newly constructed object is set to ToString(message).
  JSHandle<JSError> error =
    factory->NewObject(JSError::SIZE, JSType::JS_ERROR, ObjectClassType::ERROR,
                       vm->GetErrorPrototype().As<JSValue>(), true, false, false).As<JSError>();

  error->SetErrorType(ErrorType::ERROR);

  if (!message->IsUndefined()) {
    Builtin::SetDataProperty(vm, error, factory->NewString(u"message"),
                             JSValue::ToString(vm, message).As<JSValue>(), true, false, true);
  }

  return error.GetJSValue();
}

// Error.prototype.toString()
JSValue JSError::ToString(RuntimeCallInfo* argv) {
  VM* vm = argv->GetVM();
  JSHandleScope handle_scope{vm};
  JSHandle<JSValue> this_value = argv->GetThis();
  ObjectFactory* factory = vm->GetObjectFactory();

  // 1. Let O be the this value.
  // 2. If Type(O) is not Object, throw a TypeError exception.
  if (!this_value->IsObject()) {
    THROW_TYPE_ERROR_AND_RETURN_VALUE(
      vm, u"this value in Error.prototype.toString() is not an object.", JSValue{});
  }
  JSHandle<types::Object> O = this_value.As<types::Object>();
  
  // 3. Let name be the result of calling the [[Get]] internal method of O with argument "name".
  JSHandle<JSValue> name_prop = types::Object::Get(vm, O, factory->NewString(u"name"));
  
  // 4. If name is undefined, then let name be "Error"; else let name be ToString(name).
  JSHandle<types::String> name = name_prop->IsUndefined() ? 
    factory->NewString(u"Error") : JSValue::ToString(vm, name_prop);
  
  // 5. Let msg be the result of calling the [[Get]] internal method of O with argument "message".
  JSHandle<JSValue> msg_prop = types::Object::Get(vm, O, factory->NewString(u"message"));
  
  // 6. If msg is undefined, then let msg be the empty String; else let msg be ToString(msg).
  JSHandle<types::String> msg = msg_prop->IsUndefined() ?
    vm->GetGlobalConstants()->HandledEmptyString() : JSValue::ToString(vm, msg_prop);
  
  // 7. If name and msg are both the empty String, return "Error".
  if (name->IsEmptyString() && msg->IsEmptyString()) {
    return factory->NewString(u"Error").GetJSValue();
  }
  
  // 8. If name is the empty String, return msg.
  if (name->IsEmptyString()) {
    return msg.GetJSValue();
  }
  
  // 9. If msg is the empty String, return name.
  if (msg->IsEmptyString()) {
    return name.GetJSValue();
  }
  
  // 10. Return the result of concatenating name, ":", a single space character, and msg.
  return types::String::Concat(vm, name, factory->NewString(u": "), msg).GetJSValue();
}

// EvalError(message)
// Defined in ECMAScript 5.1 Chapter 15.11.7.2
JSValue JSError::EvalErrorConstructorCall(RuntimeCallInfo* argv) {
  return EvalErrorConstructorConstruct(argv);
}

// new EvalError(message)
// Defined in ECMAScript 5.1 Chapter 15.11.7.3
JSValue JSError::EvalErrorConstructorConstruct(RuntimeCallInfo* argv) {
  VM* vm = argv->GetVM();
  JSHandleScope handle_scope{vm};
  JSHandle<JSValue> message = argv->GetArg(0);
  ObjectFactory* factory = vm->GetObjectFactory();
  
  // The [[Prototype]] internal property of the newly constructed object is set to the prototype object for this error constructor.
  // 
  // The [[Class]] internal property of the newly constructed object is set to "Error".
  // 
  // The [[Extensible]] internal property of the newly constructed object is set to true.
  // 
  // If the argument message is not undefined,
  // the message own property of the newly constructed object is set to ToString(message).
  JSHandle<JSError> error = factory->NewNativeError(ErrorType::EVAL_ERROR);

  if (!message->IsUndefined()) {
    Builtin::SetDataProperty(vm, error, factory->NewString(u"message"),
                             JSValue::ToString(vm, message).As<JSValue>(), true, false, true);
  }

  return error.GetJSValue();
}

// RangeError(message)
// Defined in ECMAScript 5.1 Chapter 15.11.7.2
JSValue JSError::RangeErrorConstructorCall(RuntimeCallInfo* argv) {
  return EvalErrorConstructorConstruct(argv);
}

// new RangeError(message)
// Defined in ECMAScript 5.1 Chapter 15.11.7.3
JSValue JSError::RangeErrorConstructorConstruct(RuntimeCallInfo* argv) {
  VM* vm = argv->GetVM();
  JSHandleScope handle_scope{vm};
  JSHandle<JSValue> message = argv->GetArg(0);
  ObjectFactory* factory = vm->GetObjectFactory();
  
  // The [[Prototype]] internal property of the newly constructed object is set to the prototype object for this error constructor.
  // 
  // The [[Class]] internal property of the newly constructed object is set to "Error".
  // 
  // The [[Extensible]] internal property of the newly constructed object is set to true.
  // 
  // If the argument message is not undefined,
  // the message own property of the newly constructed object is set to ToString(message).
  JSHandle<JSError> error = factory->NewNativeError(ErrorType::RANGE_ERROR);

  if (!message->IsUndefined()) {
    Builtin::SetDataProperty(vm, error, factory->NewString(u"message"),
                             JSValue::ToString(vm, message).As<JSValue>(), true, false, true);
  }

  return error.GetJSValue();
}

// ReferenceError(message)
// Defined in ECMAScript 5.1 Chapter 15.11.7.2
JSValue JSError::ReferenceErrorConstructorCall(RuntimeCallInfo* argv) {
  return EvalErrorConstructorConstruct(argv);
}

// new ReferenceError(message)
// Defined in ECMAScript 5.1 Chapter 15.11.7.3
JSValue JSError::ReferenceErrorConstructorConstruct(RuntimeCallInfo* argv) {
  VM* vm = argv->GetVM();
  JSHandleScope handle_scope{vm};
  JSHandle<JSValue> message = argv->GetArg(0);
  ObjectFactory* factory = vm->GetObjectFactory();
  
  // The [[Prototype]] internal property of the newly constructed object is set to the prototype object for this error constructor.
  // 
  // The [[Class]] internal property of the newly constructed object is set to "Error".
  // 
  // The [[Extensible]] internal property of the newly constructed object is set to true.
  // 
  // If the argument message is not undefined,
  // the message own property of the newly constructed object is set to ToString(message).
  JSHandle<JSError> error = factory->NewNativeError(ErrorType::REFERENCE_ERROR);

  if (!message->IsUndefined()) {
    Builtin::SetDataProperty(vm, error, factory->NewString(u"message"),
                             JSValue::ToString(vm, message).As<JSValue>(), true, false, true);
  }

  return error.GetJSValue();
}

// SyntaxError(message)
// Defined in ECMAScript 5.1 Chapter 15.11.7.2
JSValue JSError::SyntaxErrorConstructorCall(RuntimeCallInfo* argv) {
  return EvalErrorConstructorConstruct(argv);
}

// new SyntaxError(message)
// Defined in ECMAScript 5.1 Chapter 15.11.7.3
JSValue JSError::SyntaxErrorConstructorConstruct(RuntimeCallInfo* argv) {
  VM* vm = argv->GetVM();
  JSHandleScope handle_scope{vm};
  JSHandle<JSValue> message = argv->GetArg(0);
  ObjectFactory* factory = vm->GetObjectFactory();
  
  // The [[Prototype]] internal property of the newly constructed object is set to the prototype object for this error constructor.
  // 
  // The [[Class]] internal property of the newly constructed object is set to "Error".
  // 
  // The [[Extensible]] internal property of the newly constructed object is set to true.
  // 
  // If the argument message is not undefined,
  // the message own property of the newly constructed object is set to ToString(message).
  JSHandle<JSError> error = factory->NewNativeError(ErrorType::SYNTAX_ERROR);

  if (!message->IsUndefined()) {
    Builtin::SetDataProperty(vm, error, factory->NewString(u"message"),
                             JSValue::ToString(vm, message).As<JSValue>(), true, false, true);
  }

  return error.GetJSValue();
}

// TypeError(message)
// Defined in ECMAScript 5.1 Chapter 15.11.7.2
JSValue JSError::TypeErrorConstructorCall(RuntimeCallInfo* argv) {
  return EvalErrorConstructorConstruct(argv);
}

// new TypeError(message)
// Defined in ECMAScript 5.1 Chapter 15.11.7.3
JSValue JSError::TypeErrorConstructorConstruct(RuntimeCallInfo* argv) {
  VM* vm = argv->GetVM();
  JSHandleScope handle_scope{vm};
  JSHandle<JSValue> message = argv->GetArg(0);
  ObjectFactory* factory = vm->GetObjectFactory();
  
  // The [[Prototype]] internal property of the newly constructed object is set to the prototype object for this error constructor.
  // 
  // The [[Class]] internal property of the newly constructed object is set to "Error".
  // 
  // The [[Extensible]] internal property of the newly constructed object is set to true.
  // 
  // If the argument message is not undefined,
  // the message own property of the newly constructed object is set to ToString(message).
  JSHandle<JSError> error = factory->NewNativeError(ErrorType::TYPE_ERROR);

  if (!message->IsUndefined()) {
    Builtin::SetDataProperty(vm, error, factory->NewString(u"message"),
                             JSValue::ToString(vm, message).As<JSValue>(), true, false, true);
  }

  return error.GetJSValue();
}

// URIError(message)
// Defined in ECMAScript 5.1 Chapter 15.11.7.2
JSValue JSError::URIErrorConstructorCall(RuntimeCallInfo* argv) {
  return EvalErrorConstructorConstruct(argv);
}

// new URIError(message)
// Defined in ECMAScript 5.1 Chapter 15.11.7.3
JSValue JSError::URIErrorConstructorConstruct(RuntimeCallInfo* argv) {
  VM* vm = argv->GetVM();
  JSHandleScope handle_scope{vm};
  JSHandle<JSValue> message = argv->GetArg(0);
  ObjectFactory* factory = vm->GetObjectFactory();
  
  // The [[Prototype]] internal property of the newly constructed object is set to the prototype object for this error constructor.
  // 
  // The [[Class]] internal property of the newly constructed object is set to "Error".
  // 
  // The [[Extensible]] internal property of the newly constructed object is set to true.
  // 
  // If the argument message is not undefined,
  // the message own property of the newly constructed object is set to ToString(message).
  JSHandle<JSError> error = factory->NewNativeError(ErrorType::URI_ERROR);

  if (!message->IsUndefined()) {
    Builtin::SetDataProperty(vm, error, factory->NewString(u"message"),
                             JSValue::ToString(vm, message).As<JSValue>(), true, false, true);
  }

  return error.GetJSValue();
}

}  // namespace builtins
}  // namespace voidjs
