#include "voidjs/builtins/js_array.h"

#include "voidjs/types/js_value.h"
#include "voidjs/types/heap_object.h"
#include "voidjs/types/object_factory.h"
#include "voidjs/types/object_class_type.h"
#include "voidjs/types/lang_types/number.h"
#include "voidjs/types/lang_types/object.h"
#include "voidjs/types/lang_types/string.h"
#include "voidjs/types/spec_types/property_descriptor.h"
#include "voidjs/builtins/builtin.h"
#include "voidjs/builtins/js_function.h"
#include "voidjs/builtins/js_object.h"
#include "voidjs/gc/js_handle_scope.h"
#include "voidjs/interpreter/global_constants.h"
#include "voidjs/interpreter/runtime_call_info.h"
#include "voidjs/utils/macros.h"

namespace voidjs {
namespace builtins {

bool JSArray::DefineOwnProperty(VM* vm, JSHandle<types::Object> O, JSHandle<types::String> P, const types::PropertyDescriptor& Desc, bool Throw) {
  ObjectFactory* factory = vm->GetObjectFactory();
  
  // 1. Let oldLenDesc be the result of calling the [[GetOwnProperty]] internal method of
  //    A passing "length" as the argument. The result will never be undefined or
  ///   an accessor descriptor because Array objects are created with a length data property that
  //    cannot be deleted or reconfigured.
  types::PropertyDescriptor old_len_desc = Object::GetOwnProperty(vm, O, vm->GetGlobalConstants()->HandledLengthString());

  // 2. Let oldLen be oldLenDesc.[[Value]].
  std::int32_t old_len = old_len_desc.GetValue()->GetInt();
  
  // 3. If P is "length", then
  if (P->GetString() == u"length") {
    // a. If the [[Value]] field of Desc is absent, then
    if (!Desc.HasValue()) {
      // i. Return the result of calling the default [[DefineOwnProperty]] internal method (8.12.9)
      //    on A passing "length", Desc, and Throw as arguments.
      return Object::DefineOwnPropertyDefault(vm, O, vm->GetGlobalConstants()->HandledLengthString(), Desc, Throw);
    }
    
    // b. Let newLenDesc be a copy of Desc.
    types::PropertyDescriptor new_len_desc = Desc;
    
    // c. Let newLen be ToUint32(Desc.[[Value]]).
    std::int32_t new_len = JSValue::ToUint32(vm, Desc.GetValue());
    
    // d. If newLen is not equal to ToNumber( Desc.[[Value]]), throw a RangeError exception.
    if (new_len != JSValue::ToNumber(vm, Desc.GetValue()).GetNumber()) {
      THROW_RANGE_ERROR_AND_RETURN_VALUE(vm, u"Array.length is not in uint32.", false);
    }
    
    // e. Set newLenDesc.[[Value] to newLen.
    new_len_desc.SetValue(JSValue{new_len});

    // f. If newLen ≥oldLen, then
    if (new_len >= old_len) {
      // a. Return the result of calling the default [[DefineOwnProperty]] internal method (8.12.9)
      ///   on A passing "length", newLenDesc, and Throw as arguments.
      return Object::DefineOwnPropertyDefault(vm, O, vm->GetGlobalConstants()->HandledLengthString(), new_len_desc, Throw);
    }

    // g. Reject if oldLenDesc.[[Writable]] is false.
    if (!old_len_desc.GetWritable()) {
      if (Throw) {
        THROW_TYPE_ERROR_AND_RETURN_VALUE(
          vm, u"Array.DefineOwnProprety fails when it's attribute [[Writable]] is false", false);
      } else {
        return false;
      }
    }

    // h. If newLenDesc.[[Writable]] is absent or has the value true, let newWritable be true.
    bool new_writable = false;
    if (!new_len_desc.HasWritable() || new_len_desc.GetWritable()) {
      new_writable = true;
    }
    // i. Else,
    else {
      // i. Need to defer setting the [[Writable]] attribute to false in case any elements cannot be deleted.
      
      // ii. Let newWritable be false.
      new_writable = false;
      
      // iii. Set newLenDesc.[[Writable] to true.
      new_len_desc.SetWritable(true);
    }

    // j. Let succeeded be the result of calling the default [[DefineOwnProperty]] internal method (8.12.9)
    //    on A passing "length", newLenDesc, and Throw as arguments.
    bool succeeded = Object::DefineOwnPropertyDefault(vm, O, vm->GetGlobalConstants()->HandledLengthString(), new_len_desc, Throw);
    RETURN_VALUE_IF_HAS_EXCEPTION(vm, false);

    // k. If succeeded is false, return false..
    if (!succeeded) {
      return false;
    }
    
    // l. While newLen < oldLen repeat,
    while (new_len < old_len) {
      // i. Set oldLen to oldLen – 1.
      --old_len;
      
      // ii. Let deleteSucceeded be the result of calling the [[Delete]] internal method of
      //     A passing ToString(oldLen) and false as arguments.
      bool delete_succeeded = Object::Delete(vm, O, JSValue::ToString(vm, JSHandle<JSValue>{vm, JSValue{old_len}}), false);
      
      // iii. If deleteSucceeded is false, then
      if (!delete_succeeded) {
        // 1. Set newLenDesc.[[Value] to oldLen+1.
        new_len_desc.SetValue(JSValue{old_len + 1});
        
        // 2. If newWritable is false, set newLenDesc.[[Writable] to false.
        if (!new_writable) {
          new_len_desc.SetWritable(false);
        }
        
        // 3. Call the default [[DefineOwnProperty]] internal method (8.12.9) on
        //    A passing "length", newLenDesc, and false as arguments.
        Object::DefineOwnPropertyDefault(vm, O, vm->GetGlobalConstants()->HandledLengthString(), new_len_desc, false);
        
        // 4. Reject.
        if (Throw) {
          THROW_TYPE_ERROR_AND_RETURN_VALUE(
            vm, u"Array.DefineOwnProprety fails.", false);
        } else {
          return false;
        }
      }
    }

    // m. If newWritable is false, then
    if (!new_writable) {
      // i. Call the default [[DefineOwnProperty]] internal method (8.12.9) on A passing "length",
      //    Property Descriptor{[[Writable]]: false}, and false as arguments. This call will always return true.
      types::PropertyDescriptor prop{vm};
      prop.SetWritable(false);
      Object::DefineOwnPropertyDefault(vm, O, vm->GetGlobalConstants()->HandledLengthString(), prop, false);
    }

    // n. Return true.
    return true;
  }
  // 4.  Else if P is an array index (15.4), then
  else if (auto P_num = JSValue::ToUint32(vm, P.As<JSValue>());
           JSValue::ToString(vm, JSHandle<JSValue>{vm, JSValue{P_num}})->Equal(P) &&
           P_num != (1ull << 32) - 1) {
    // a. Let index be ToUint32(P).
    std::int32_t index = P_num;
    
    // b. Reject if index ≥ oldLen and oldLenDesc.[[Writable]] is false.
    if (index >= old_len && !old_len_desc.GetWritable()) {
      if (Throw) {
        THROW_TYPE_ERROR_AND_RETURN_VALUE(
          vm, u"Array.DefineOwnProprety fails.", false);
      } else {
        return false;
      }
    }
    
    // c. Let succeeded be the result of calling the default [[DefineOwnProperty]] internal method (8.12.9) on
    //    A passing P, Desc, and false as arguments.
    bool succeeded = Object::DefineOwnPropertyDefault(vm, O, P, Desc, false);
    
    // d. Reject if succeeded is false.
    if (!succeeded) {
      if (Throw) {
        THROW_TYPE_ERROR_AND_RETURN_VALUE(
          vm, u"Array.DefineOwnProprety fails.", false);
      } else {
        return false;
      }
    }
    
    // e. If index ≥ oldLen
    if (index >= old_len) {
      // i. Set oldLenDesc.[[Value]] to index + 1.
      old_len_desc.SetValue(JSValue{index + 1});
      
      // ii .Call the default [[DefineOwnProperty]] internal method (8.12.9) on
      //     A passing "length", oldLenDesc, and false as arguments. This call will always return true.
      Object::DefineOwnPropertyDefault(vm, O, vm->GetGlobalConstants()->HandledLengthString(), old_len_desc, false);
    }

    // f. Retrun true.
    return true;
  }

  // 5. Return the result of calling the default [[DefineOwnProperty]] internal method (8.12.9) on
  //    A passing P, Desc, and Throw as arguments.
  return Object::DefineOwnPropertyDefault(vm, O, P, Desc, Throw);
}

JSValue JSArray::ArrayConstructorCall(RuntimeCallInfo* argv) {
  return ArrayConstructorConstruct(argv);
}

JSValue JSArray::ArrayConstructorConstruct(RuntimeCallInfo* argv) {
  VM* vm = argv->GetVM();
  JSHandleScope handle_scope{vm};
  std::size_t args_num = argv->GetArgsNum();
  ObjectFactory* factory = vm->GetObjectFactory();
  
  if (args_num == 1) {
    // The [[Prototype]] internal property of the newly constructed object is set to the original Array prototype object,
    // the one that is the initial value of Array.prototype (15.4.3.1).
    // The [[Class]] internal property of the newly constructed object is set to "Array".
    // The [[Extensible]] internal property of the newly constructed object is set to true.
    
    // If the argument len is a Number and ToUint32(len) is equal to len,
    // then the length property of the newly constructed object is set to ToUint32(len).
    // If the argument len is a Number and ToUint32(len) is not equal to len,
    // a RangeError exception is thrown.
    
    // If the argument len is not a Number,
    // then the length property of the newly constructed object is set to 1 and
    // the 0 property of the newly constructed object is set to len
    // with attributes {[[Writable]]: true, [[Enumerable]]: true, [[Configurable]]: true}
    JSHandle<JSArray> arr = factory->NewObject(
      JSArray::SIZE, JSType::JS_ARRAY, ObjectClassType::ARRAY,
      vm->GetArrayPrototype().As<JSValue>(), true, false, false).As<JSArray>();
    
    JSHandle<JSValue> len = argv->GetArg(0);
    if (len->IsNumber()) {
      if (JSValue::ToUint32(vm, len) != len->GetNumber()) {
        THROW_RANGE_ERROR_AND_RETURN_VALUE(
          vm, u"new Array(len) fails, becacuse len is not a uint32.", JSValue{});
      } else {
        Builtin::SetDataProperty(vm, arr, vm->GetGlobalConstants()->HandledLengthString(), JSHandle<JSValue>{vm, JSValue{JSValue::ToUint32(vm, len)}}, true, false, false);
      }
    } else {
      Builtin::SetDataProperty(vm, arr, vm->GetGlobalConstants()->HandledLengthString(), JSHandle<JSValue>{vm, JSValue{1}}, true, false, false);
      Builtin::SetDataProperty(vm, arr, vm->GetGlobalConstants()->HandledZeroString(), len, true, true, true);
    }

    return arr.GetJSValue();
  } else {
    // The [[Prototype]] internal property of the newly constructed object is set to the original Array prototype object,
    // the one that is the initial value of Array.prototype (15.4.3.1).

    // The [[Class]] internal property of the newly constructed object is set to "Array".

    // The [[Extensible]] internal property of the newly constructed object is set to true.

    // The length property of the newly constructed object is set to the number of arguments.

    // The 0 property of the newly constructed object is set to item0 (if supplied);
    // the 1 property of the newly constructed object is set to item1 (if supplied);
    // and, in general, for as many arguments as there are,
    // the k property of the newly constructed object is set to argument k,
    // where the first argument is considered to be argument number 0.
    // These properties all have the attributes {[[Writable]]: true, [[Enumerable]]: true, [[Configurable]]: true}.
    JSHandle<JSArray> arr = factory->NewObject(JSArray::SIZE, JSType::JS_ARRAY, ObjectClassType::ARRAY,
                                  vm->GetArrayPrototype().As<JSValue>(), true, false, false).As<JSArray>();
 
    Builtin::SetDataProperty(vm, arr, vm->GetGlobalConstants()->HandledLengthString(), JSHandle<JSValue>{vm, JSValue{static_cast<int>(args_num)}}, true, false, false);

    for (std::size_t idx = 0; idx < args_num; ++idx) {
      Builtin::SetDataProperty(vm, arr, factory->NewStringFromInt(idx), argv->GetArg(idx), true, true, true);
    }

    return arr.GetJSValue();
  }
}

// Array.IsArray(arg)
// Defined in ECMAScript 5.1 Chapter 15.4.3.2
JSValue JSArray::IsArray(RuntimeCallInfo* argv) {
  JSHandle<JSValue> arg = argv->GetArg(0);
  
  // 1. If Type(arg) is not Object, return false.
  if (!arg->IsObject()) {
    return JSValue::False();
  }
  
  // 2. If the value of the [[Class]] internal property of arg is "Array", then return true.
  if (arg->GetHeapObject()->GetClassType() == ObjectClassType::ARRAY) {
    return JSValue::True();
  }
  
  // 3. Return false.
  return JSValue::False();
}

// Array.prototype.toString()
// Defined in ECMAScript 5.1 Chapter 15.4.4.2
JSValue JSArray::ToString(RuntimeCallInfo* argv) {
  VM* vm = argv->GetVM();
  JSHandleScope handle_scope{vm};
  JSHandle<JSValue> this_value = argv->GetThis();
  ObjectFactory* factory = vm->GetObjectFactory();
  
  // 1. Let array be the result of calling ToObject on the this value.
  JSHandle<JSArray> array = this_value.As<JSArray>();
  
  // 2. Let func be the result of calling the [[Get]] internal method of array with argument "join".
  JSHandle<JSValue> func = types::Object::Get(vm, array, factory->NewString(u"join"));
  
  // 3. If IsCallable(func) is false, then let func be the standard built-in method Object.prototype.toString (15.2.4.2).
  if (!func.As<types::Object>()->GetCallable()) {
    func = types::Object::Get(vm, vm->GetObjectPrototype(), factory->NewString(u"toString"));
  }
  
  // 4. Return the result of calling the [[Call]] internal method of func providing array as the this value and an empty arguments list.
  return types::Object::Call(vm, func.As<types::Object>(), array.As<JSValue>(), {}).GetJSValue();
}

// Array.prototype.toLocaleString()
// Defined in ECMAScript 5.1 Chapter 15.4.4.3
// todo
JSValue JSArray::ToLocaleString(RuntimeCallInfo* argv) {
}

// Array.prototype.concat([item1[,item2[,...]]])
// Defined in ECMAScript 5.1 Chapter 15.4.4.4
JSValue JSArray::Concat(RuntimeCallInfo* argv) {
  VM* vm = argv->GetVM();
  JSHandleScope handle_scope{vm};
  JSHandle<JSValue> this_value = argv->GetThis();
  std::size_t arg_num = argv->GetArgsNum();
  ObjectFactory* factory = vm->GetObjectFactory();
  
  // 1. Let O be the result of calling ToObject passing the this value as the argument.
  JSHandle<types::Object> O = JSValue::ToObject(vm, this_value);
  RETURN_VALUE_IF_HAS_EXCEPTION(vm, JSValue{});
  
  // 2. Let A be a new array created as if by the expression new Array() where
  //    Array is the standard built-in constructor with that name.
  JSHandle<JSArray> A = types::Object::Construct(
    vm, vm->GetArrayConstructor(),
    vm->GetGlobalConstants()->HandledUndefined(), {}).As<JSArray>();
  
  // 3. Let n be 0.
  std::int32_t n = 0;
  // 4. Let items be an internal List whose first element is O and whose subsequent elements are,
  //    in left to right order, the arguments that were passed to this function invocation.
  // 5. Repeat, while items is not empty
  // a. Remove the first element from items and let E be the value of the element.
  // b. If the value of the [[Class]] internal property of E is "Array", then
  // i. Let k be 0.
  // ii. Let len be the result of calling the [[Get]] internal method of E with argument "length".
  // iii. Repeat, while k < len
  // 1. Let P be ToString(k).
  // 2. Let exists be the result of calling the [[HasProperty]] internal method of E with P.
  // 3. If exists is true, then
  // a. Let subElement be the result of calling the [[Get]] internal method of E with argument P.
  // b. Call the [[DefineOwnProperty]] internal method of A with arguments ToString(n),
  //    Property Descriptor {[[Value]]: subElement, [[Writable]]: true, [[Enumerable]]: true, [[Configurable]]: true}, and false.
  // 4. Increase n by 1.
  // 5. Increase k by 1.
  // c. Else, E is not an Array
  // i. Call the [[DefineOwnProperty]] internal method of A with arguments ToString(n),
  //    Property Descriptor {[[Value]]: E, [[Writable]]: true, [[Enumerable]]: true, [[Configurable]]: true}, and false.
  // ii .Increase n by 1.
  auto concat = [vm, factory, A, &n](JSHandle<JSValue> elem) {
    if (elem->IsObject() && elem->GetHeapObject()->GetClassType() == ObjectClassType::ARRAY) {
      JSHandle<JSArray> arr = elem.As<JSArray>();
      std::int32_t k = 0;
      std::int32_t len = types::Object::Get(vm, arr, vm->GetGlobalConstants()->HandledLengthString())->GetInt();
      
      while (k < len) {
        JSHandle<types::String> P = JSValue::ToString(vm, JSHandle<JSValue>{vm, JSValue{k}});
        
        auto exists = types::Object::HasProperty(vm, arr, P);
        
        if (exists) {
          JSHandle<JSValue> sub_element = types::Object::Get(vm, arr, P);
          DefineOwnProperty(vm, A, factory->NewStringFromInt(n),
                            types::PropertyDescriptor{vm, sub_element, true, true, true}, false);
        }
        
        ++n;
        ++k;
      }
    } else {
      DefineOwnProperty(vm, A, factory->NewStringFromInt(n),
                        types::PropertyDescriptor{vm, elem, true, true, true}, false);
      ++n;
    }
  };

  concat(this_value);

  for (std::size_t idx = 0; idx < arg_num; ++idx) {
    concat(argv->GetArg(idx));
  }

  // 6. Return A.
  return A.GetJSValue();
}

// Array.prototype.join(separator)
// Defined in ECMAScript 5.1 Chapter 15.4.4.5
JSValue JSArray::Join(RuntimeCallInfo* argv) {
  VM* vm = argv->GetVM();
  JSHandleScope handle_scope{vm};
  JSHandle<JSValue> this_value = argv->GetThis();
  JSHandle<JSValue> separator = argv->GetArg(0);
  ObjectFactory* factory = vm->GetObjectFactory();

  // 1. Let O be the result of calling ToObject passing the this value as the argument.
  JSHandle<Object> O = JSValue::ToObject(vm, this_value);
  RETURN_VALUE_IF_HAS_EXCEPTION(vm, JSValue{});
  
  // 2. Let lenVal be the result of calling the [[Get]] internal method of O with argument "length".
  JSHandle<JSValue> len_val = types::Object::Get(vm, O, vm->GetGlobalConstants()->HandledLengthString());
  
  // 3. Let len be ToUint32(lenVal).
  std::int32_t len = JSValue::ToUint32(vm, len_val);
  
  // 4. If separator is undefined, let separator be the single-character String ",".
  if (separator->IsUndefined()) {
    separator = factory->NewString(u",").As<JSValue>();
  }
  
  // 5. Let sep be ToString(separator).
  JSHandle<types::String> sep = JSValue::ToString(vm, separator);
  
  // 6. If len is zero, return the empty String.
  if (len == 0) {
    return JSValue{vm->GetGlobalConstants()->EmptyString()};
  }
  
  // 7. Let element0 be the result of calling the [[Get]] internal method of O with argument "0".
  JSHandle<JSValue> element0 = types::Object::Get(vm, O, vm->GetGlobalConstants()->HandledZeroString());
  
  // 8. If element0 is undefined or null, let R be the empty String;
  //    otherwise, Let R be ToString(element0).
  JSHandle<types::String> R = element0->IsUndefined() || element0->IsNull() ?
    vm->GetGlobalConstants()->HandledEmptyString() : JSValue::ToString(vm, element0);
  
  // 9. Let k be 1.
  std::int32_t k = 1;
  
  // 10. Repeat, while k < len
  while (k < len) {
    // a. Let S be the String value produced by concatenating R and sep.
    JSHandle<types::String> S = types::String::Concat(vm, R, sep);
    
    // b. Let element be the result of calling the [[Get]] internal method of O with argument ToString(k).
    JSHandle<JSValue> element = types::Object::Get(vm, O, factory->NewStringFromInt(k));
    
    // c. If element is undefined or null, Let next be the empty String; otherwise, let next be ToString(element).
    JSHandle<types::String> next = element->IsUndefined() || element->IsNull() ?
      vm->GetGlobalConstants()->HandledEmptyString() : JSValue::ToString(vm, element);
    
    // d. Let R be a String value produced by concatenating S and next.
    R = types::String::Concat(vm, S, next);
    
    // e. Increase k by 1.
    ++k;
  }
  
  // 11. Return R.
  return R.GetJSValue();
}

// Array.prototype.pop()
// Defined in ECMASCript 5.1 Chapter
JSValue JSArray::Pop(RuntimeCallInfo* argv) {
  VM* vm = argv->GetVM();
  JSHandleScope handle_scope{vm};
  JSHandle<JSValue> this_value = argv->GetThis();
  ObjectFactory* factory = vm->GetObjectFactory();
  
  // 1. Let O be the result of calling ToObject passing the this value as the argument.
  JSHandle<Object> O = JSValue::ToObject(vm, this_value);
  RETURN_VALUE_IF_HAS_EXCEPTION(vm, JSValue{});
  
  // 2. Let lenVal be the result of calling the [[Get]] internal method of O with argument "length".
  JSHandle<JSValue> len_val = types::Object::Get(vm, O, vm->GetGlobalConstants()->HandledLengthString());
  
  // 3. Let len be ToUint32(lenVal).
  std::int32_t len = JSValue::ToUint32(vm, len_val);
  
  // 4. If len is zero,
  if (len == 0) { 
    // a. Call the [[Put]] internal method of O with arguments "length", 0, and true.
    types::Object::Put(vm, O, vm->GetGlobalConstants()->HandledLengthString(), JSHandle<JSValue>{vm, JSValue{0}}, true);
    RETURN_VALUE_IF_HAS_EXCEPTION(vm, JSValue{});
    
    // b. Return undefined.
    return JSValue::Undefined();
  }
  // 5. Else, len > 0
  else {
    // a. Let indx be ToString(len–1).
    JSHandle<types::String> index = factory->NewStringFromInt(len - 1);
    
    // b. Let element be the result of calling the [[Get]] internal method of O with argument indx.
    JSHandle<JSValue> element = types::Object::Get(vm, O, index);
    
    // c. Call the [[Delete]] internal method of O with arguments indx and true.
    types::Object::Delete(vm, O, index, true);
    RETURN_VALUE_IF_HAS_EXCEPTION(vm, JSValue{});
    
    // d. Call the [[Put]] internal method of O with arguments "length", indx, and true.
    types::Object::Put(vm, O, vm->GetGlobalConstants()->HandledLengthString(), JSHandle<JSValue>{vm, JSValue{len - 1}}, true);
    
    // e. Return element.
    return element.GetJSValue();
  }
}

// Array.prototype.push([item1[,item2[,...]]])
JSValue JSArray::Push(RuntimeCallInfo* argv) {
  VM* vm = argv->GetVM();
  JSHandleScope handle_scope{vm};
  JSHandle<JSValue> this_value = argv->GetThis();
  ObjectFactory* factory = vm->GetObjectFactory();
  
  // 1. Let O be the result of calling ToObject passing the this value as the argument.
  JSHandle<types::Object> O = JSValue::ToObject(vm, this_value);
  RETURN_VALUE_IF_HAS_EXCEPTION(vm, JSValue{});
  
  // 2. Let lenVal be the result of calling the [[Get]] internal method of O with argument "length".
  JSHandle<JSValue> len_val = types::Object::Get(vm, O, vm->GetGlobalConstants()->HandledLengthString());
  
  // 3. Let n be ToUint32(lenVal).
  std::int32_t n = JSValue::ToUint32(vm, len_val);
  
  // 4. Let items be an internal List whose elements are, in left to right order, the arguments that were passed to this function invocation.
  // 5. Repeat, while items is not empty
  std::size_t args_num = argv->GetArgsNum();
  for (std::size_t idx = 0; idx < args_num; ++idx) {
    // a. Remove the first element from items and let E be the value of the element.
    JSHandle<JSValue> E = argv->GetArg(idx);
    
    // b. Call the [[Put]] internal method of O with arguments ToString(n), E, and true.
    types::Object::Put(vm, O, factory->NewStringFromInt(n), E, true);
    RETURN_VALUE_IF_HAS_EXCEPTION(vm, JSValue{});
    
    // c. Increase n by 1.
    ++n;
  }
  
  // 6. Call the [[Put]] internal method of O with arguments "length", n, and true.
  types::Object::Put(vm, O, vm->GetGlobalConstants()->HandledLengthString(), JSHandle<JSValue>{vm, JSValue{n}}, true);
  RETURN_VALUE_IF_HAS_EXCEPTION(vm, JSValue{});
  
  // 7. Return n.
  return JSValue{n};
}

// Array.prototype.reverse()
// Defined in ECMAScript 5.1 Chapter 15.4.4.8
JSValue JSArray::Reverse(RuntimeCallInfo* argv) {
  VM* vm = argv->GetVM();
  JSHandleScope handle_scope{vm};
  JSHandle<JSValue> this_value = argv->GetThis();
  ObjectFactory* factory = vm->GetObjectFactory();
  
  // 1. Let O be the result of calling ToObject passing the this value as the argument.
  JSHandle<types::Object> O = JSValue::ToObject(vm, this_value);
  RETURN_VALUE_IF_HAS_EXCEPTION(vm, JSValue{});
  
  // 2. Let lenVal be the result of calling the [[Get]] internal method of O with argument "length".
  JSHandle<JSValue> len_val = types::Object::Get(vm, O, vm->GetGlobalConstants()->HandledLengthString());
  
  // 3. Let len be ToUint32(lenVal).
  std::uint32_t len = JSValue::ToUint32(vm, len_val);
  
  // 4. Let middle be floor(len/2).
  std::uint32_t middle = len / 2;
  
  // 5. Let lower be 0.
  std::uint32_t lower = 0;
  
  // 6. Repeat, while lower ≠ middle
  while (lower != middle) {
    // a. Let upper be len − lower −1.
    std::uint32_t upper = len - lower - 1;
    
    // b. Let upperP be ToString(upper).
    JSHandle<types::String> upper_p = factory->NewStringFromInt(upper);
    
    // c. Let lowerP be ToString(lower).
    JSHandle<types::String> lower_p = factory->NewStringFromInt(lower);
    
    // d. Let lowerValue be the result of calling the [[Get]] internal method of O with argument lowerP.
    JSHandle<JSValue> lower_value = types::Object::Get(vm, O, lower_p);
    
    // e. Let upperValue be the result of calling the [[Get]] internal method of O with argument upperP .
    JSHandle<JSValue> upper_value = types::Object::Get(vm, O, upper_p);
    
    // f. Let lowerExists be the result of calling the [[HasProperty]] internal method of O with argument lowerP.
    bool lower_exists = types::Object::HasProperty(vm, O, lower_p);
    
    // g. Let upperExists be the result of calling the [[HasProperty]] internal method of O with argument upperP.
    bool upper_exists = types::Object::HasProperty(vm, O, upper_p);
    
    // h. If lowerExists is true and upperExists is true, then
    if (lower_exists && upper_exists) {
      // i. Call the [[Put]] internal method of O with arguments lowerP, upperValue, and true .
      types::Object::Put(vm, O, lower_p, upper_value, true);
      RETURN_VALUE_IF_HAS_EXCEPTION(vm, JSValue{});
      
      // ii. Call the [[Put]] internal method of O with arguments upperP, lowerValue, and true .
      types::Object::Put(vm, O, upper_p, lower_value, true);
      RETURN_VALUE_IF_HAS_EXCEPTION(vm, JSValue{});
    }
    // i. Else if lowerExists is false and upperExists is true, then
    else if (!lower_exists && upper_exists) {
      // i. Call the [[Put]] internal method of O with arguments lowerP, upperValue, and true .
      types::Object::Put(vm, O, lower_p, upper_value, true);
      RETURN_VALUE_IF_HAS_EXCEPTION(vm, JSValue{});
      
      // ii. Call the [[Delete]] internal method of O, with arguments upperP and true.
      types::Object::Delete(vm, O, upper_p, true);
      RETURN_VALUE_IF_HAS_EXCEPTION(vm, JSValue{});
    }
    // j. Else if lowerExists is true and upperExists is false, then
    else if (lower_exists && !upper_exists) {
      // i. Call the [[Delete]] internal method of O, with arguments lowerP and true .
      types::Object::Delete(vm, O, lower_p, true);
      RETURN_VALUE_IF_HAS_EXCEPTION(vm, JSValue{});
      
      // ii. Call the [[Put]] internal method of O with arguments upperP, lowerValue, and true .
      types::Object::Put(vm, O, upper_p, lower_value, true);
      RETURN_VALUE_IF_HAS_EXCEPTION(vm, JSValue{});
    }
    // k. Else, both lowerExists and upperExists are false
    else {
      // i. No action is required.
    }

    // i. Increase lower by 1.
    ++lower;
  }

  // 7. Return O.
  return O.GetJSValue();
}

// Array.prototype.shift()
// Defined in ECMAScript 5.1 Chapter 15.4.4.9
JSValue JSArray::Shift(RuntimeCallInfo* argv) {
  VM* vm = argv->GetVM();
  JSHandleScope handle_scope{vm};
  JSHandle<JSValue> this_value = argv->GetThis();
  ObjectFactory* factory = vm->GetObjectFactory();

  // 1. Let O be the result of calling ToObject passing the this value as the argument.
  JSHandle<types::Object> O = JSValue::ToObject(vm, this_value);
  RETURN_VALUE_IF_HAS_EXCEPTION(vm, JSValue{});
  
  // 2. Let lenVal be the result of calling the [[Get]] internal method of O with argument "length".
  JSHandle<JSValue> len_val = types::Object::Get(vm, O, vm->GetGlobalConstants()->HandledLengthString());
  
  // 3. Let len be ToUint32(lenVal).
  std::uint32_t len = JSValue::ToUint32(vm, len_val);
  
  // 4. If len is zero, then
  if (len == 0) {
    // a. Call the [[Put]] internal method of O with arguments "length", 0, and true.
    types::Object::Put(vm, O, vm->GetGlobalConstants()->HandledLengthString(), JSHandle<JSValue>{vm, JSValue{0}}, true);
    RETURN_VALUE_IF_HAS_EXCEPTION(vm, JSValue{});
    
    // b. Return undefined.
    return JSValue::Undefined();
  }

  // 5. Let first be the result of calling the [[Get]] internal method of O with argument "0".
  JSHandle<JSValue> first = types::Object::Get(vm, O, vm->GetGlobalConstants()->HandledZeroString());
  
  // 6. Let k be 1.
  std::uint32_t k = 1;

  // 7. Repeat, while k < len
  while (k < len) {
    // a. Let from be ToString(k).
    JSHandle<types::String> from = factory->NewStringFromInt(k);
    
    // b. Let to be ToString(k–1).
    JSHandle<types::String> to = factory->NewStringFromInt(k - 1);
    
    // c. Let fromPresent be the result of calling the [[HasProperty]] internal method of O with argument from.
    bool from_present = types::Object::HasProperty(vm, O, from);
    
    // d. If fromPresent is true, then
    if (from_present) {
      // i. Let fromVal be the result of calling the [[Get]] internal method of O with argument from.
      JSHandle<JSValue> from_val = types::Object::Get(vm, O, from);
      
      // ii. Call the [[Put]] internal method of O with arguments to, fromVal, and true.
      types::Object::Put(vm, O, to, from_val, true);
      RETURN_VALUE_IF_HAS_EXCEPTION(vm, JSValue{});
    }
    // e. Else, fromPresent is false
    else {
      // i. Call the [[Delete]] internal method of O with arguments to and true.
      types::Object::Delete(vm, O, to, true);
      RETURN_VALUE_IF_HAS_EXCEPTION(vm, JSValue{});
    }

    // f. Increase k by 1.
    ++k;
  }

  // 8. Call the [[Delete]] internal method of O with arguments ToString(len–1) and true.
  types::Object::Delete(vm, O, factory->NewStringFromInt(len - 1), true);
  RETURN_VALUE_IF_HAS_EXCEPTION(vm, JSValue{});
  
  // 9. Call the [[Put]] internal method of O with arguments "length", (len–1) , and true.
  types::Object::Put(vm, O, vm->GetGlobalConstants()->HandledLengthString(), JSHandle<JSValue>{vm, JSValue{len - 1}}, true);
  RETURN_VALUE_IF_HAS_EXCEPTION(vm, JSValue{});
  
  // 10. Return first.
  return first.GetJSValue();
}

// Array.prototype.slice(start, end)
// Defined in ECMAScript 5.1 Chapter 15.4.4.10
JSValue JSArray::Slice(RuntimeCallInfo* argv) {
  VM* vm = argv->GetVM();
  JSHandleScope handle_scope{vm};
  JSHandle<JSValue> this_value = argv->GetThis();
  JSHandle<JSValue> start = argv->GetArg(0);
  JSHandle<JSValue> end = argv->GetArg(1);
  ObjectFactory* factory = vm->GetObjectFactory();

  // 1. Let O be the result of calling ToObject passing the this value as the argument.
  JSHandle<types::Object> O = JSValue::ToObject(vm, this_value);
  RETURN_VALUE_IF_HAS_EXCEPTION(vm, JSValue{});
  
  // 2. Let A be a new array created as if by the expression new Array() where
  //    Array is the standard built-in constructor with that name.
  JSHandle<JSArray> A = types::Object::Construct(vm, vm->GetArrayConstructor(), {}, {}).As<JSArray>();

  // 3. Let lenVal be the result of calling the [[Get]] internal method of O with argument "length".
  JSHandle<JSValue> len_val = types::Object::Get(vm, O, vm->GetGlobalConstants()->HandledLengthString());
  
  // 4. Let len be ToUint32(lenVal).
  std::uint32_t len = JSValue::ToUint32(vm, len_val);
  
  // 5. Let relativeStart be ToInteger(start).
  types::Number relative_start = JSValue::ToInteger(vm, start);
  
  // 6. If relativeStart is negative, let k be max((len +relativeStart),0); else let k be min(relativeStart,len).
  double tmp_k = relative_start.GetNumber() < 0 ?
    std::max(len + relative_start.GetNumber(), 0.0) : std::min(relative_start.GetNumber(), static_cast<double>(len));
  auto k = static_cast<std::uint32_t>(tmp_k);
  
  // 7. If end is undefined, let relativeEnd be len; else let relativeEnd be ToInteger(end).
  types::Number relative_end = end->IsUndefined() ?
    types::Number{len} : JSValue::ToInteger(vm, end);
  
  // 8. If relativeEnd is negative, let final be max((len + relativeEnd),0); else let final be min(relativeEnd,len).
  double tmp_fin = relative_end.GetNumber() < 0 ?
    std::max(len + relative_end.GetNumber(), 0.0) : std::min(relative_end.GetNumber(), static_cast<double>(len));
  auto fin = static_cast<std::uint32_t>(tmp_fin);
  
  // 9. Let n be 0.
  std::uint32_t n = 0;
  
  // 10. Repeat, while k < final
  while (k < fin) {
    // a. Let Pk be ToString(k).
    JSHandle<types::String> pk = factory->NewStringFromInt(k);
    
    // b. Let kPresent be the result of calling the [[HasProperty]] internal method of O with argument Pk.
    bool k_present = types::Object::HasProperty(vm, O, pk);
    
    // c. If kPresent is true, then
    if (k_present) {
      // i. Let kValue be the result of calling the [[Get]] internal method of O with argument Pk.
      JSHandle<JSValue> k_value = types::Object::Get(vm, O, pk);
      
      // ii. Call the [[DefineOwnProperty]] internal method of A with arguments ToString(n),
      //     Property Descriptor {[[Value]]: kValue, [[Writable]]: true, [[Enumerable]]: true,
      //     [[Configurable]]: true}, and false.
      types::Object::DefineOwnProperty(vm, A, factory->NewStringFromInt(n),
                                       types::PropertyDescriptor{vm, k_value, true, true, true}, false);
    }

    // d. Increase k by 1.
    ++k;

    // e. Increase n by 1.
    ++n;
  }

  // 11. Return A.
  return A.GetJSValue();
}

// Array.prototype.sort(comparefn)
// Defined in ECMAScript 5.1 Chapter 15.4.4.11
JSValue JSArray::Sort(RuntimeCallInfo* argv) {
  VM* vm = argv->GetVM();
  JSHandleScope handle_scope{vm};
  JSHandle<JSValue> this_value = argv->GetThis();
  JSHandle<JSValue> comparefn = argv->GetArg(0);
  ObjectFactory* factory = vm->GetObjectFactory();

  JSHandle<types::Object> obj = JSValue::ToObject(vm, this_value);
  RETURN_VALUE_IF_HAS_EXCEPTION(vm, JSValue{});

  std::uint32_t len =
    JSValue::ToUint32(vm, types::Object::Get(vm, obj, vm->GetGlobalConstants()->HandledLengthString()));

  if (!comparefn->IsUndefined() && !comparefn->IsCallable()) {
    THROW_TYPE_ERROR_AND_RETURN_VALUE(vm, u"comparefn of Array.prototype.sort is not callable", JSValue{});
  }

  std::vector<JSHandle<JSValue>> tmp;
  for (std::uint32_t idx = 0; idx < len; ++idx) {
    JSHandle<types::String> idx_str = factory->NewStringFromInt(idx);
    
    if (HasProperty(vm, obj, idx_str)) {
      tmp.push_back(types::Object::Get(vm, obj, idx_str));
    } else {
      tmp.emplace_back();
    }
  }

  std::sort(tmp.begin(), tmp.end(), [=](JSHandle<JSValue> j, JSHandle<JSValue> k) mutable {
    if (k.IsEmpty()) {
      return true;
    }

    if (j.IsEmpty() && !k.IsEmpty()) {
      return false;
    }
    
    // 1. Let jString be ToString(j).
    // 2. Let kString be ToString(k).
    // 3. Let hasj be the result of calling the [[HasProperty]] internal method of obj with argument jString.
    // 4. Let hask be the result of calling the [[HasProperty]] internal method of obj with argument kString.
    // 5. If hasj and hask are both false, then return +0.
    // 6. If hasj is false, then return 1.
    // 7. If hask is false, then return –1.
    
    // 8. Let x be the result of calling the [[Get]] internal method of obj with argument jString.
    JSHandle<JSValue> x = j;
    
    // 9. Let y be the result of calling the [[Get]] internal method of obj with argument kString.
    JSHandle<JSValue> y = k;
    
    // 10. If x and y are both undefined, return +0.
    // 11. If x is undefined, return 1.
    // 12. If y is undefined, return −1.
    if (y->IsUndefined()) {
      return false;
    }
    
    if (x->IsUndefined() && !y->IsUndefined()) {
      return true;
    }
    
    // 13. If the argument comparefn is not undefined, then
    if (!comparefn->IsUndefined()) {
      // a. If IsCallable(comparefn) is false, throw a TypeError exception.
      // b. Return the result of calling the [[Call]] internal method of comparefn passing
      //    undefined as the this value and with arguments x and y.
      JSHandle<JSValue> ret = types::Object::Call(vm, comparefn.As<types::Object>(),
                                                  vm->GetGlobalConstants()->HandledUndefined(), {x, y});
      RETURN_VALUE_IF_HAS_EXCEPTION(vm, false);
      return JSValue::ToNumber(vm, ret).GetNumber() < 0;
    }

    // 14. Let xString be ToString(x).
    JSHandle<types::String> x_string = JSValue::ToString(vm, x);
    
    // 15. Let yString be ToString(y).
    JSHandle<types::String> y_string = JSValue::ToString(vm, y);
    
    // 16. If xString < yString, return −1.
    // 17. If xString > yString, return 1.
    return x_string->GetString() <  y_string->GetString();
  });
  RETURN_VALUE_IF_HAS_EXCEPTION(vm, JSValue{});

  for (std::uint32_t idx = 0; idx < len; ++idx) {
    JSHandle<types::String> idx_str = factory->NewStringFromInt(idx);
    
    if (!tmp[idx].IsEmpty()) {
      types::Object::Put(vm, obj, idx_str, tmp[idx], true);
      RETURN_VALUE_IF_HAS_EXCEPTION(vm, JSValue{});
    } else {
      types::Object::Delete(vm, obj, idx_str, true);
      RETURN_VALUE_IF_HAS_EXCEPTION(vm, JSValue{});
    }
  }
  
  return obj.GetJSValue();
}

// Array.prototype.forEach(callbackfn, [, thisArg])
// Defined in ECMAScript 5.1 Chapter
JSValue JSArray::ForEach(RuntimeCallInfo* argv) {
  VM* vm = argv->GetVM();
  JSHandleScope handle_scope{vm};
  JSHandle<JSValue> this_value = argv->GetThis();
  JSHandle<JSValue> callbackfn = argv->GetArg(0);
  JSHandle<JSValue> this_arg = argv->GetArg(1);
  ObjectFactory* factory = vm->GetObjectFactory();
  
  // 1. Let O be the result of calling ToObject passing the this value as the argument.
  JSHandle<types::Object> O = JSValue::ToObject(vm, this_value);
  RETURN_VALUE_IF_HAS_EXCEPTION(vm, JSValue{});
  
  // 2. Let lenValue be the result of calling the [[Get]] internal method of O with the argument "length".
  JSHandle<JSValue> len_value = types::Object::Get(vm, O, vm->GetGlobalConstants()->HandledLengthString());
  
  // 3. Let len be ToUint32(lenValue).
  std::uint32_t len = JSValue::ToUint32(vm, len_value);
  
  // 4. If IsCallable(callbackfn) is false, throw a TypeError exception.
  if (!callbackfn->IsObject() || !callbackfn->IsCallable()) {
    THROW_TYPE_ERROR_AND_RETURN_VALUE(vm, u"callbackfn of Array.prototype.sort is not callable.", JSValue{});
  }
  
  // 5. If thisArg was supplied, let T be thisArg; else let T be undefined.
  // 6. Let k be 0.
  std::uint32_t k = 0;
  
  // 7. Repeat, while k < len
  while (k < len) {
    // a. Let Pk be ToString(k).
    JSHandle<types::String> pk = factory->NewStringFromInt(k);
    
    // b. Let kPresent be the result of calling the [[HasProperty]] internal method of O with argument Pk.
    bool k_present = types::Object::HasProperty(vm, O, pk);
    
    // c. If kPresent is true, then
    if (k_present) {
      // i. Let kValue be the result of calling the [[Get]] internal method of O with argument Pk.
      JSHandle<JSValue> k_value = types::Object::Get(vm, O, pk);
      
      // ii. Call the [[Call]] internal method of callbackfn with T as the this value and argument list containing kValue, k, and O.
      types::Object::Call(vm, callbackfn.As<types::Object>(), this_arg, {k_value, JSHandle<JSValue>{vm, JSValue{k}}, O.As<JSValue>()});
    }

    // d. Increase k by 1.
    ++k;
  }

  return JSValue::Undefined();
}

// Array.prototype.map(callbackfn, [, thisArg])
// Defined in ECMAScript 5.1 Chapter 15.4.4.19
JSValue JSArray::Map(RuntimeCallInfo* argv) {
  VM* vm = argv->GetVM();
  JSHandleScope handle_scope{vm};
  JSHandle<JSValue> this_value = argv->GetThis();
  JSHandle<JSValue> callbackfn = argv->GetArg(0);
  JSHandle<JSValue> this_arg = argv->GetArg(1);
  ObjectFactory* factory = vm->GetObjectFactory();
  
  // 1. Let O be the result of calling ToObject passing the this value as the argument.
  JSHandle<types::Object> O = JSValue::ToObject(vm, this_value);
  RETURN_VALUE_IF_HAS_EXCEPTION(vm, JSValue{});
  
  // 2. Let lenValue be the result of calling the [[Get]] internal method of O with the argument "length".
  JSHandle<JSValue> len_value = types::Object::Get(vm, O, vm->GetGlobalConstants()->HandledLengthString());
  
  // 3. Let len be ToUint32(lenValue).
  std::uint32_t len = JSValue::ToUint32(vm, len_value);
  
  // 4. If IsCallable(callbackfn) is false, throw a TypeError exception.
  if (!callbackfn->IsObject() || !callbackfn->GetHeapObject()->GetCallable()) {
    THROW_TYPE_ERROR_AND_RETURN_VALUE(vm, u"callbackfn of Array.prototype.map is not callable.", JSValue{});
  }
  
  // 5. If thisArg was supplied, let T be thisArg; else let T be undefined.
  JSHandle<JSValue> T = this_arg;
  
  // 6. Let A be a new array created as if by the expression new Array( len) where
  //    Array is the standard built-in constructor with that name and len is the value of len.
  JSHandle<JSArray> A = types::Object::Construct(
    vm, vm->GetArrayConstructor(),
    vm->GetGlobalConstants()->HandledUndefined(), {len_value}).As<JSArray>();
  
  // 7. Let k be 0.
  std::uint32_t k = 0;
  
  // 8. Repeat, while k < len
  while (k < len) {
    // a. Let Pk be ToString(k).
    JSHandle<types::String> pk = factory->NewStringFromInt(k);
    
    // b. Let kPresent be the result of calling the [[HasProperty]] internal method of O with argument Pk.
    bool k_present = types::Object::HasProperty(vm, O, pk);
    
    // c. If kPresent is true, then
    if (k_present) {
      // i. Let kValue be the result of calling the [[Get]] internal method of O with argument Pk.
      JSHandle<JSValue> k_value = types::Object::Get(vm, O, pk);
      
      // ii. Let mappedValue be the result of calling the [[Call]] internal method of callbackfn with
      ///    T as the this value and argument list containing kValue, k, and O.
      auto mapped_value = types::Object::Call(
        vm, callbackfn.As<types::Object>(), T, {k_value, JSHandle<JSValue>{vm, JSValue{k}}, O.As<JSValue>()});
      RETURN_VALUE_IF_HAS_EXCEPTION(vm, JSValue{});
      
      // iii. Call the [[DefineOwnProperty]] internal method of A with arguments Pk,
      //      Property Descriptor {[[Value]]: mappedValue, [[Writable]]: true, [[Enumerable]]: true, [[Configurable]]: true}, and false.
      types::Object::DefineOwnProperty(vm, A, pk, types::PropertyDescriptor{vm, mapped_value, true, true, true}, false);
    }

    // d. Increase k by 1.
    ++k;
  }

  // 9. Return A.
  return A.GetJSValue();
}

// Array.prototype.filter(callbackfn, [, thisArg])
// Defined in ECMAScript 5.1 Chapter 15.4.4.20
JSValue JSArray::Filter(RuntimeCallInfo* argv) {
  VM* vm = argv->GetVM();
  JSHandleScope handle_scope{vm};
  JSHandle<JSValue> this_value = argv->GetThis();
  JSHandle<JSValue> callbackfn = argv->GetArg(0);
  JSHandle<JSValue> this_arg = argv->GetArg(1);
  ObjectFactory* factory = vm->GetObjectFactory();

  // 1. Let O be the result of calling ToObject passing the this value as the argument.
  JSHandle<types::Object> O = JSValue::ToObject(vm, this_value);
  RETURN_VALUE_IF_HAS_EXCEPTION(vm, JSValue{});
  
  // 2. Let lenValue be the result of calling the [[Get]] internal method of O with the argument "length".
  JSHandle<JSValue> len_value = types::Object::Get(vm, O, vm->GetGlobalConstants()->HandledLengthString());
  
  // 3. Let len be ToUint32(lenValue).
  std::uint32_t len = JSValue::ToUint32(vm, len_value);
  
  // 4. If IsCallable(callbackfn) is false, throw a TypeError exception.
  if (!callbackfn->IsObject() || !callbackfn->GetHeapObject()->GetCallable()) {
    THROW_TYPE_ERROR_AND_RETURN_VALUE(vm, u"callbackfn of Array.prototype.filter is not callable.", JSValue{});
  }
  
  // 5. If thisArg was supplied, let T be thisArg; else let T be undefined.
  JSHandle<JSValue> T = this_arg;
  
  // 6. Let A be a new array created as if by the expression new Array() where
  //    Array is the standard built-in constructor with that name and len is the value of len.
  JSHandle<JSArray> A = types::Object::Construct(
    vm, vm->GetArrayConstructor(),
    vm->GetGlobalConstants()->HandledUndefined(), {}).As<JSArray>();
  
  // 7. Let k be 0.
  std::uint32_t k = 0;
  
  // 8. Let to be 0.
  std::uint32_t to = 0;
  
  // 9. Repeat, while k < len
  while (k < len) {
    // a. Let Pk be ToString(k).
    JSHandle<types::String> pk = factory->NewStringFromInt(k);
    
    // b. Let kPresent be the result of calling the [[HasProperty]] internal method of O with argument Pk.
    bool k_present = types::Object::HasProperty(vm, O, pk);
    
    // c. If kPresent is true, then
    if (k_present) {
      // i. Let kValue be the result of calling the [[Get]] internal method of O with argument Pk.
      JSHandle<JSValue> k_value = types::Object::Get(vm, O, pk);
      
      // ii. Let selected be the result of calling the [[Call]] internal method of
      //     callbackfn with T as the this value and argument list containing kValue, k, and O.
      JSHandle<JSValue> selected = types::Object::Call(
        vm, callbackfn.As<types::Object>(), T, {k_value, JSHandle<JSValue>{vm, JSValue{k}}, O.As<JSValue>()});
      
      // iii. If ToBoolean(selected) is true, then
      if (JSValue::ToBoolean(vm, selected)) {
        // 1. Call the [[DefineOwnProperty]] internal method of A with arguments ToString(to),
        //    Property Descriptor {[[Value]]: kValue, [[Writable]]: true, [[Enumerable]]: true, [[Configurable]]: true}, and false.
        types::Object::DefineOwnProperty(
          vm, A, factory->NewStringFromInt(to), types::PropertyDescriptor{vm, k_value, true, true, true}, false);
        
          // 2. Increase to by 1.
        ++to;
      }
    }

    // d. Increase k by 1.
    ++k;
  }

  // 10. Return A.
  return A.GetJSValue();
}

}  // namespace builtins
}  // namespace voidjs
