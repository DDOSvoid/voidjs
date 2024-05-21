#include "voidjs/builtins/js_string.h"

#include <string>

#include "voidjs/gc/js_handle_scope.h"
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
#include "voidjs/types/spec_types/property_descriptor.h"
#include "voidjs/utils/macros.h"

namespace voidjs {
namespace builtins {

// String([value])
// Defined in ECMAScript 5.1 Chapter 15.5.1.1
JSValue JSString::StringConstructorCall(RuntimeCallInfo* argv) {
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
JSValue JSString::StringConstructorConstruct(RuntimeCallInfo* argv) {
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
  JSHandle<JSString> str = factory->NewObject(JSString::SIZE, JSType::JS_STRING, ObjectClassType::STRING,
                                vm->GetStringPrototype().As<JSValue>(), true, false, false).As<JSString>();
  JSHandle<types::String> val = argv->GetArgsNum() == 0 ?
    vm->GetGlobalConstants()->HandledEmptyString() : JSValue::ToString(vm, argv->GetArg(0));
  str->SetPrimitiveValue(val.As<JSValue>());

  return str.GetJSValue();
}

// Defined in ECMAScript 5.1 Chapter 15.5.5.2
types::PropertyDescriptor JSString::GetOwnProperty(VM* vm, JSHandle<JSString> S, JSHandle<types::String> P) {
  // 1. Let desc be the result of calling the default [[GetOwnProperty]] internal method (8.12.1) on S with argument P.
  types::PropertyDescriptor desc = types::Object::GetOwnPropertyDefault(vm, S, P);
  
  // 2. If desc is not undefined return desc.
  if (!desc.IsEmpty()) {
    return desc;
  }
  
  // 3. If ToString(abs(ToInteger(P))) is not the same value as P, return undefined.
  if (!JSValue::ToString(vm, JSHandle<JSValue>{vm, types::Number::Abs(JSValue::ToInteger(vm, P.As<JSValue>()))})->Equal(P)) {
    return {};
  }
  
  // 4. Let str be the String value of the [[PrimitiveValue]] internal property of S.
  auto str = JSHandle<types::String>{vm, S->GetPrimitiveValue()};
  
  // 5. Let index be ToInteger(P).
  types::Number index = JSValue::ToInteger(vm, P.As<JSValue>());
  
  // 6. Let len be the number of characters in str.
  std::size_t len = str->GetLength();
  
  // 7. If len ≤ index, return undefined.
  if (len <= index.GetNumber()) {
    return {};
  }
  
  // 8. Let resultStr be a String of length 1, containing one character from str,
  //    specifically the character at position index, where the first (leftmost) character in str is considered to be at position 0,
  //    the next one at position 1, and so on.
  JSHandle<types::String> result_str = types::String::CharAt(vm, str, index.GetNumber());
  
  // 9. Return a Property Descriptor { [[Value]]: resultStr, [[Enumerable]]: true, [[Writable]]: false, [[Configurable]]: false }
  return types::PropertyDescriptor{vm, result_str.As<JSValue>(), true, false, false};
}

// String.prototype.toString()
// Defined in ECMAScript 5.1 Chapter 15.5.4.2
JSValue JSString::ToString(RuntimeCallInfo* argv) {
  VM* vm = argv->GetVM();
  JSHandleScope handle_scope{vm};
  JSHandle<JSValue> this_value = argv->GetThis();
  ObjectFactory* factory = vm->GetObjectFactory();

  // Returns this String value. (Note that, for a String object, the toString method happens to return the same thing as the valueOf method.)

  // The toString function is not generic;
  // it throws a TypeError exception if its this value is not a String or a String object.
  // Therefore, it cannot be transferred to other kinds of objects for use as a method.

  if (!this_value->IsObject() || !this_value->GetHeapObject()->IsString() && this_value->GetHeapObject()->IsString()) {
    THROW_TYPE_ERROR_AND_RETURN_VALUE(
      vm, u"this value is not a String or String object when calling String.prototype.toString()", JSValue{});
  }

  if (this_value->GetHeapObject()->IsString()) {
    return this_value.GetJSValue();
  } else {
    // this_value must be JSString
    return this_value.As<JSString>()->GetPrimitiveValue();
  }
}

// String.prototype.valueOf()
// Defined in ECMAScript 5.1 Chapter 15.5.4.3
JSValue JSString::ValueOf(RuntimeCallInfo* argv) {
  VM* vm = argv->GetVM();
  JSHandleScope handle_scope{vm};
  JSHandle<JSValue> this_value = argv->GetThis();
  ObjectFactory* factory = vm->GetObjectFactory();

  // Returns this String value. (Note that, for a String object, the toString method happens to return the same thing as the valueOf method.)

  // The valueOf function is not generic;
  // it throws a TypeError exception if its this value is not a String or a String object.
  // Therefore, it cannot be transferred to other kinds of objects for use as a method.

  if (!this_value->IsObject() || !this_value->GetHeapObject()->IsString() && this_value->GetHeapObject()->IsString()) {
    THROW_TYPE_ERROR_AND_RETURN_VALUE(
      vm, u"this value is not a String or String object when calling String.prototype.valueOf()", JSValue{});
  }

  if (this_value->GetHeapObject()->IsString()) {
    return this_value.GetJSValue();
  } else {
    // this_value must be JSString
    return this_value.As<JSString>()->GetPrimitiveValue();
  }
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
  VM* vm = argv->GetVM();
  JSHandleScope handle_scope{vm};
  JSHandle<JSValue> this_value = argv->GetThis();
  
  // 1. Call CheckObjectCoercible passing the this value as its argument.
  JSValue::CheckObjectCoercible(vm, this_value);
  RETURN_VALUE_IF_HAS_EXCEPTION(vm, JSValue{});
  
  // 2. Let S be the result of calling ToString, giving it the this value as its argument.
  JSHandle<types::String> S = JSValue::ToString(vm, this_value);
  
  // 3. Let args be an internal list that is a copy of the argument list passed to this function.
  
  // 4. Let R be S.
  JSHandle<types::String> R = S;
  
  // 5. Repeat, while args is not empty
  std::size_t args_num = argv->GetArgsNum();
  for (std::size_t idx = 0; idx < args_num; ++idx) {
    // a. Remove the first element from args and let next be the value of that element.
    JSHandle<JSValue> next = argv->GetArg(idx);
    
    // b. Let R be the String value consisting of the characters in the previous value of R followed by the characters of ToString(next).
    R = types::String::Concat(vm, R, JSValue::ToString(vm, next));
  }
  
  // 6. Return R.
  return R.GetJSValue();
}

// String.prototype.indexOf(searchString, position)
// Defined in ECMAScript 5.1 Chapter 15.5.4.7
JSValue JSString::IndexOf(RuntimeCallInfo* argv) {
  VM* vm = argv->GetVM();
  JSHandleScope handle_scope{vm};
  JSHandle<JSValue> this_value = argv->GetThis();
  JSHandle<JSValue> search_string = argv->GetArg(0);
  JSHandle<JSValue> position = argv->GetArg(1);
  
  // 1. Call CheckObjectCoercible passing the this value as its argument.
  JSValue::CheckObjectCoercible(vm, this_value);
  RETURN_VALUE_IF_HAS_EXCEPTION(vm, JSValue{});
  
  // 2. Let S be the result of calling ToString, giving it the this value as its argument.
  JSHandle<types::String> S = JSValue::ToString(vm, this_value);
  
  // 3. Let searchStr be ToString(searchString).
  JSHandle<types::String> search_str = JSValue::ToString(vm, search_string);
  
  // 4. Let pos be ToInteger(position). (If position is undefined, this step produces the value 0).
  JSValue pos = position->IsUndefined() ? JSValue{0} : JSValue::ToInteger(vm, position);
  
  // 5. Let len be the number of characters in S.
  std::size_t len = S->GetLength();
  
  // 6. Let start be min(max(pos, 0), len).
  auto start = static_cast<std::size_t>(std::min(std::max(pos.GetNumber(), 0.0), static_cast<double>(len)));
  
  // 7. Let searchLen be the number of characters in searchStr.
  std::size_t search_len = search_str->GetLength();
  
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

// String.prototype.lastIndexOf(searchString, position)
// Defined in ECMAScript 5.1 Chapter 15.5.4.8
JSValue JSString::LastIndexOf(RuntimeCallInfo* argv) {
  VM* vm = argv->GetVM();
  JSHandleScope handle_scope{vm};
  JSHandle<JSValue> this_value = argv->GetThis();
  JSHandle<JSValue> search_string = argv->GetArg(0);
  JSHandle<JSValue> position = argv->GetArg(1);
  
  // 1. Call CheckObjectCoercible passing the this value as its argument.
  JSValue::CheckObjectCoercible(vm, this_value);
  RETURN_VALUE_IF_HAS_EXCEPTION(vm, JSValue{});
  
  // 2. Let S be the result of calling ToString, giving it the this value as its argument.
  JSHandle<types::String> S = JSValue::ToString(vm, this_value);
  
  // 3. Let searchStr be ToString(searchString).
  JSHandle<types::String> search_str = JSValue::ToString(vm, search_string);
  
  // 4. Let numPos be ToNumber(position). (If position is undefined, this step produces the value NaN).
  types::Number num_pos = JSValue::ToNumber(vm, position);
  
  // 5. If numPos is NaN, let pos be +∞; otherwise, let pos be ToInteger(numPos).
  types::Number pos = num_pos.IsNaN() ? types::Number::Inf() : JSValue::ToInteger(vm, JSHandle<JSValue>{vm, num_pos}); 
  
  // 6. Let len be the number of characters in S.
  std::size_t len = S->GetLength();
  
  // 7. Let start min(max(pos, 0), len).
  std::size_t start = static_cast<std::size_t>(std::min(std::max(pos.GetNumber(), 0.0), 1.0 * len));
  
  // 8. Let searchLen be the number of characters in searchStr.
  std::size_t search_len = search_str->GetLength();
  
  // 9. Return the largest possible nonnegative integer k not larger than start such that
  //    k + searchLen is not greater than len,
  //    and for all nonnegative integers j less than searchLen,
  ///   the character at position k + j of S is the same as the character at position j of searchStr;
  //    but if there is no such integer k, then return the value -1.
  if (auto pos = S->GetString().rfind(search_str->GetString(), start); pos != std::u16string_view::npos) {
    return JSValue{static_cast<int>(pos)};
  } else {
    return JSValue{-1};
  }
}

// String.prototype.slice(start, end)
// Defined in ECMAScript 5.1 Chapter 15.5.4.13
JSValue JSString::Slice(RuntimeCallInfo* argv) {
  VM* vm = argv->GetVM();
  JSHandleScope handle_scope{vm};
  JSHandle<JSValue> this_value = argv->GetThis();
  JSHandle<JSValue> start = argv->GetArg(0);
  JSHandle<JSValue> end = argv->GetArg(1);
  ObjectFactory* factory = vm->GetObjectFactory();
  
  // 1. Call CheckObjectCoercible passing the this value as its argument.
  JSValue::CheckObjectCoercible(vm, this_value);
  RETURN_VALUE_IF_HAS_EXCEPTION(vm, JSValue{});
  
  // 2. Let S be the result of calling ToString, giving it the this value as its argument.
  JSHandle<types::String> S = JSValue::ToString(vm, this_value);
  
  // 3. Let len be the number of characters in S.
  std::size_t len = S->GetLength();
  
  // 4. Let intStart be ToInteger(start).
  types::Number int_start = JSValue::ToInteger(vm, start);
  
  // 5. If end is undefined, let intEnd be len; else let intEnd be ToInteger(end).
  types::Number int_end = end->IsUndefined() ? types::Number{static_cast<int>(len)} : JSValue::ToInteger(vm, end);
  
  // 6. If intStart is negative, let from be max(len + intStart,0); else let from be min(intStart,len).
  std::size_t from = int_start.GetNumber() < 0 ? std::max(len + int_start.GetNumber(), 0.0) : std::min(int_start.GetNumber(), 1.0 * len);
  
  // 7. If intEnd is negative, let to be max(len +intEnd,0); else let to be min(intEnd, len).
  std::size_t to = int_end.GetNumber() < 0 ? std::max(len + int_end.GetNumber(), 0.0) : std::min(int_end.GetNumber(), 1.0 * len);
  
  // 8. Let span be max(to – from,0).
  std::size_t span = std::max(to - from, static_cast<size_t>(0));
  
  // 9. Return a String containing span consecutive characters from S beginning with the character at position from.
  return factory->NewString(S->GetString().substr(from, span)).GetJSValue();
}

// String.prototype.substring(start, end)
// Defined in ECMAScript 5.1 Chapter 15.5.4.15
JSValue JSString::Substring(RuntimeCallInfo* argv) {
  VM* vm = argv->GetVM();
  JSHandleScope handle_scope{vm};
  JSHandle<JSValue> this_value = argv->GetThis();
  JSHandle<JSValue> start = argv->GetArg(0);
  JSHandle<JSValue> end = argv->GetArg(1);
  ObjectFactory* factory = vm->GetObjectFactory();

  // 1. Call CheckObjectCoercible passing the this value as its argument.
  JSValue::CheckObjectCoercible(vm, this_value);
  RETURN_VALUE_IF_HAS_EXCEPTION(vm, JSValue{});
  
  // 2. Let S be the result of calling ToString, giving it the this value as its argument.
  JSHandle<types::String> S = JSValue::ToString(vm, this_value);
  
  // 3. Let len be the number of characters in S.
  std::size_t len = S->GetLength();
  
  // 4. Let intStart be ToInteger(start).
  types::Number int_start = JSValue::ToInteger(vm, start);
  
  // 5. If end is undefined, let intEnd be len; else let intEnd be ToInteger(end).
  types::Number int_end = end->IsUndefined() ? types::Number{static_cast<int>(len)} : JSValue::ToInteger(vm, end);
  
  // 6. Let finalStart be min(max(intStart, 0), len).
  std::size_t final_start = static_cast<std::size_t>(std::min(std::max(int_start.GetNumber(), 0.0), 1.0 * len));
  
  // 7. Let finalEnd be min(max(intEnd, 0), len).
  std::size_t final_end = static_cast<std::size_t>(std::min(std::max(int_end.GetNumber(), 0.0), 1.0 * len));
  
  // 8. Let from be min(finalStart, finalEnd).
  std::size_t from = std::min(final_start, final_end);
  
  // 9. Let to be max(finalStart, finalEnd).
  std::size_t to = std::max(final_start, final_end);
  
  // 10. Return a String whose length is to - from, containing characters from S,
  //     namely the characters with indices from through to − 1, in ascending order.
  return types::String::Substring(vm, S, from, to - from).GetJSValue();
}

// String.prototype.toLowerCase()
// Defined in ECMAScript 5.1 Chapter 15.5.4.16
JSValue JSString::ToLowerCase(RuntimeCallInfo* argv) {
  VM* vm = argv->GetVM();
  JSHandleScope handle_scope{vm};
  JSHandle<JSValue> this_value = argv->GetThis();
  ObjectFactory* factory = vm->GetObjectFactory();

  // 1. Call CheckObjectCoercible passing the this value as its argument.
  JSValue::CheckObjectCoercible(vm, this_value);
  RETURN_VALUE_IF_HAS_EXCEPTION(vm, JSValue{});
  
  // 2. Let S be the result of calling ToString, giving it the this value as its argument.
  JSHandle<types::String> S = JSValue::ToString(vm, this_value);
  
  // 3. Let L be a String where each character of L is either the Unicode lowercase equivalent of
  //    the corresponding character of S or the actual corresponding character of S if no Unicode lowercase equivalent exists.
  std::u16string L {S->GetString()};
  std::transform(L.begin(), L.end(), L.begin(), character::ToLowerCase);
  
  // 4. Return L.
  return factory->NewString(L).GetJSValue();
}

// String.prototype.toUpperCase()
// Defined in ECMAScript 5.1 Chapter 15.5.4.18
JSValue JSString::ToUpperCase(RuntimeCallInfo* argv) {
  VM* vm = argv->GetVM();
  JSHandleScope handle_scope{vm};
  JSHandle<JSValue> this_value = argv->GetThis();
  ObjectFactory* factory = vm->GetObjectFactory();

  // 1. Call CheckObjectCoercible passing the this value as its argument.
  JSValue::CheckObjectCoercible(vm, this_value);
  RETURN_VALUE_IF_HAS_EXCEPTION(vm, JSValue{});
  
  // 2. Let S be the result of calling ToString, giving it the this value as its argument.
  JSHandle<types::String> S = JSValue::ToString(vm, this_value);
  
  // 3. Let L be a String where each character of L is either the Unicode lowercase equivalent of
  //    the corresponding character of S or the actual corresponding character of S if no Unicode lowercase equivalent exists.
  std::u16string L {S->GetString()};
  std::transform(L.begin(), L.end(), L.begin(), character::ToUpperCase);
  
  // 4. Return L.
  return factory->NewString(L).GetJSValue();
}

// String.prototype.trim()
// Defined in ECMAScript 5.1 Chapter 15.5.4.20
JSValue JSString::Trim(RuntimeCallInfo* argv) {
  VM* vm = argv->GetVM();
  JSHandleScope handle_scope{vm};
  JSHandle<JSValue> this_value = argv->GetThis();
  ObjectFactory* factory = vm->GetObjectFactory();

  // 1. Call CheckObjectCoercible passing the this value as its argument.
  JSValue::CheckObjectCoercible(vm, this_value);
  RETURN_VALUE_IF_HAS_EXCEPTION(vm, JSValue{});
  
  // 2. Let S be the result of calling ToString, giving it the this value as its argument.
  JSHandle<types::String> S = JSValue::ToString(vm, this_value);
  
  // 3. Let T be a String value that is a copy of S with both leading and trailing white space removed.
  //    The definition of white space is the union of WhiteSpace and LineTerminator.
  std::u16string T {S->GetString()};
  T.erase(0, T.find_first_not_of(u" "));
  T.erase(T.find_last_not_of(u" ") + 1);
  
  // 4. Return T.
  return factory->NewString(T).GetJSValue();
}

}  // namespace voidjs
}  // namespace builtins
