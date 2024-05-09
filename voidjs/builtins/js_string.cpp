#include "voidjs/builtins/js_string.h"

#include "voidjs/types/heap_object.h"
#include "voidjs/types/js_value.h"
#include "voidjs/types/object_class_type.h"
#include "voidjs/types/object_factory.h"
#include "voidjs/types/lang_types/number.h"
#include "voidjs/types/lang_types/object.h"
#include "voidjs/types/lang_types/string.h"
#include "voidjs/interpreter/runtime_call_info.h"
#include "voidjs/interpreter/global_constants.h"
#include "voidjs/interpreter/vm.h"
#include "voidjs/utils/macros.h"

namespace voidjs {
namespace builtins {

// String([value])
// Defined in ECMAScript 5.1 Chapter 15.5.1.1
JSValue JSString::Call(RuntimeCallInfo* argv) {
  VM* vm = argv->GetVM();
  JSHandleScope handle_scope{vm};
  ObjectFactory* factory = vm->GetObjectFactory();
  
  // Returns a String value (not a String object) computed by ToString(value).
  // If value is not supplied, the empty String "" is returned.
  auto ret = argv->GetArgsNum() == 0 ?
    vm->GetGlobalConstants()->HandledEmptyString() : JSValue::ToString(vm, argv->GetArg(0));

  return ret.GetJSValue();
}

// new String([value])
JSValue JSString::Construct(RuntimeCallInfo* argv) {
  VM* vm = argv->GetVM();
  JSHandleScope handle_scope{vm};
  ObjectFactory* factory = vm->GetObjectFactory();
  
  // The [[Prototype]] internal property of the newly constructed object is
  // set to the standard built-in String prototype object that is
  // the initial value of String.prototype (15.5.3.1).
  // 
  // The [[Class]] internal property of the newly constructed object is set to "String".
  // 
  // The [[Extensible]] internal property of the newly constructed object is set to true.
  //
  // The [[PrimitiveValue]] internal property of the newly constructed object is
  // set to ToString(value), or to the empty String if value is not supplied.
  auto str = factory->NewObject(JSString::SIZE, JSType::JS_STRING, ObjectClassType::STRING,
                                vm->GetStringPrototype().As<JSValue>(), true, false, false)->AsJSString();
  JSHandle<types::String> val = argv->GetArgsNum() == 0 ?
    vm->GetGlobalConstants()->HandledEmptyString() : JSValue::ToString(vm, argv->GetArg(0));
  str->SetPrimitiveValue(val.As<JSValue>());

  return JSValue{str};
}


// String.prototype.charAt(pos)
// Defined in ECMAScript 5.1 Chapter 15.5.4.4
JSValue JSString::CharAt(RuntimeCallInfo* argv) {
  VM* vm = argv->GetVM();
  JSHandleScope handle_scope{vm};
  JSHandle<JSValue> this_value = argv->GetThis();
  JSHandle<JSValue> pos = argv->GetArg(0);
  ObjectFactory* factory = vm->GetObjectFactory();
  
  // 1. Call CheckObjectCoercible passing the this value as its argument.
  JSValue::CheckObjectCoercible(vm, this_value);
  RETURN_VALUE_IF_HAS_EXCEPTION(vm, JSValue{});
  
  // 2. Let S be the result of calling ToString, giving it the this value as its argument.
  auto S = JSValue::ToString(vm, this_value);
  
  // 3. Let position be ToInteger(pos).
  auto position = JSValue::ToInteger(vm, pos).GetNumber();
  
  // 4. Let size be the number of characters in S.
  auto size = S->GetLength();
  
  // 5. If position < 0 or position ≥ size, return the empty String.
  if (position < 0 || position >= size) {
    return JSValue{vm->GetGlobalConstants()->EmptyString()};
  }
  auto idx = static_cast<int>(position);
  
  // 6. Return a String of length 1, containing one character from S,
  //    namely the character at position position,
  //    where the first (leftmost) character in S is considered to be at position 0,
  //    the next one at position 1, and so on.
  return factory->NewString(std::u16string_view{S->GetData() + idx, 1}).GetJSValue();
}

// String.prototype.concat([string1[,string2[,...]]]
// Defined in ECMAScript 5.1 Chapter 15.5.4.6
JSValue JSString::Concat(RuntimeCallInfo* argv) {
  auto vm = argv->GetVM();
  auto this_value = argv->GetThis();
  
  // 1. Call CheckObjectCoercible passing the this value as its argument.
  JSValue::CheckObjectCoercible(vm, this_value);
  RETURN_VALUE_IF_HAS_EXCEPTION(vm, JSValue{});
  
  // 2. Let S be the result of calling ToString, giving it the this value as its argument.
  auto S = JSValue::ToString(vm, this_value);
  
  // 3. Let args be an internal list that is a copy of the argument list passed to this function.
  
  // 4. Let R be S.
  auto R = S;
  
  // 5. Repeat, while args is not empty
  auto args_num = argv->GetArgsNum();
  for (std::size_t idx = 0; idx < args_num; ++idx) {
    // a. Remove the first element from args and let next be the value of that element.
    auto next = argv->GetArg(idx);
    
    // b. Let R be the String value consisting of the characters in the previous value of R followed by the characters of ToString(next).
    R = types::String::Concat(vm, R, JSValue::ToString(vm, next));
  }
  
  // 6. Return R.
  return R.GetJSValue();
}

// String.prototype.indexOf(searchString, position)
// Defined in ECMAScript 5.1 Chapter 15.5.4.7
JSValue JSString::IndexOf(RuntimeCallInfo* argv) {
  auto vm = argv->GetVM();
  auto this_value = argv->GetThis();
  auto search_string = argv->GetArg(0);
  auto position = argv->GetArg(1);
  
  // 1. Call CheckObjectCoercible passing the this value as its argument.
  JSValue::CheckObjectCoercible(vm, this_value);
  RETURN_VALUE_IF_HAS_EXCEPTION(vm, JSValue{});
  
  // 2. Let S be the result of calling ToString, giving it the this value as its argument.
  auto S = JSValue::ToString(vm, this_value);
  // 3. Let searchStr be ToString(searchString).
  auto search_str = JSValue::ToString(vm, search_string);
  
  // 4. Let pos be ToInteger(position). (If position is undefined, this step produces the value 0).
  auto pos = position->IsUndefined() ? JSValue{0} : JSValue::ToInteger(vm, position);
  
  // 5. Let len be the number of characters in S.
  auto len = S->GetLength();
  
  // 6. Let start be min(max(pos, 0), len).
  auto start = static_cast<std::size_t>(std::min(std::max(pos.GetNumber(), 0.0), static_cast<double>(len)));
  
  // 7. Let searchLen be the number of characters in searchStr.
  auto search_len = search_str->GetLength();
  
  // 8. Return the smallest possible integer k not smaller than start such that k + searchLen is not greater than len,
  //    and for all nonnegative integers j less than searchLen,
  //    the character at position k + j of S is the same as the character at position j of searchStr);
  //    but if there is no such integer k, then return the value -1.
  auto find_pos = S->GetString().find(search_str->GetString(), start);
  if (find_pos == std::u16string_view::npos) {
    return JSValue{-1};
  } else {
    return JSValue{static_cast<std::int32_t>(find_pos)};
  }
}

}  // namespace voidjs
}  // namespace builtins
