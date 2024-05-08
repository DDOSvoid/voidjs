#include "voidjs/builtins/js_array.h"

#include "voidjs/builtins/js_object.h"
#include "voidjs/gc/js_handle_scope.h"
#include "voidjs/interpreter/runtime_call_info.h"
#include "voidjs/types/js_value.h"
#include "voidjs/types/heap_object.h"
#include "voidjs/types/lang_types/number.h"
#include "voidjs/types/lang_types/object.h"
#include "voidjs/types/lang_types/string.h"
#include "voidjs/types/object_class_type.h"
#include "voidjs/types/spec_types/property_descriptor.h"
#include "voidjs/builtins/builtin.h"
#include "voidjs/builtins/js_function.h"
#include "voidjs/interpreter/global_constants.h"
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

JSValue JSArray::Call(RuntimeCallInfo* argv) {
  return Construct(argv);
}

JSValue JSArray::Construct(RuntimeCallInfo* argv) {
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
      Builtin::SetDataProperty(vm, arr, factory->GetStringFromTable(u"0"), len, true, true, true);
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
      Builtin::SetDataProperty(vm, arr, factory->GetIntString(idx), argv->GetArg(idx), true, true, true);
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
// todo
JSValue JSArray::ToString(RuntimeCallInfo* argv) {
  
}

// Array.prototype.toLocaleString()
// Defined in ECMAScript 5.1 Chapter 15.4.4.3
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
      std::int32_t len = types::Object::Get(vm, arr, factory->GetStringFromTable(u"length"))->GetInt();
      
      while (k < len) {
        JSHandle<types::String> P = JSValue::ToString(vm, JSHandle<JSValue>{vm, JSValue{k}});
        
        auto exists = types::Object::HasProperty(vm, arr, P);
        
        if (exists) {
          JSHandle<JSValue> sub_element = types::Object::Get(vm, arr, P);
          DefineOwnProperty(vm, A, factory->GetIntString(n),
                            types::PropertyDescriptor{vm, sub_element, true, true, true}, false);
        }
        
        ++n;
        ++k;
      }
    } else {
      DefineOwnProperty(vm, A, factory->GetIntString(n),
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
    separator = factory->GetStringFromTable(u",").As<JSValue>();
  }
  
  // 5. Let sep be ToString(separator).
  JSHandle<types::String> sep = JSValue::ToString(vm, separator);
  
  // 6. If len is zero, return the empty String.
  if (len == 0) {
    return JSValue{vm->GetGlobalConstants()->EmptyString()};
  }
  
  // 7. Let element0 be the result of calling the [[Get]] internal method of O with argument "0".
  JSHandle<JSValue> element0 = types::Object::Get(vm, O, factory->GetIntString(0));
  
  // 8. If element0 is undefined or null, let R be the empty String;
  //    otherwise, Let R be ToString(element0).
  JSHandle<types::String> R = element0->IsUndefined() || element0->IsNull() ?
    vm->GetGlobalConstants()->HandledLengthString() : JSValue::ToString(vm, element0);
  
  // 9. Let k be 1.
  std::int32_t k = 1;
  
  // 10. Repeat, while k < len
  while (k < len) {
    // a. Let S be the String value produced by concatenating R and sep.
    JSHandle<types::String> S = types::String::Concat(vm, R, sep);
    
    // b. Let element be the result of calling the [[Get]] internal method of O with argument ToString(k).
    JSHandle<JSValue> element = types::Object::Get(vm, O, factory->GetIntString(k));
    
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
    JSHandle<types::String> index = factory->GetIntString(len - 1);
    
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
    types::Object::Put(vm, O, factory->GetIntString(n), E, true);
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

}  // namespace builtins
}  // namespace voidjs
