#include "voidjs/builtins/js_array.h"

#include "voidjs/builtins/js_object.h"
#include "voidjs/types/js_value.h"
#include "voidjs/types/heap_object.h"
#include "voidjs/types/lang_types/number.h"
#include "voidjs/types/lang_types/string.h"
#include "voidjs/builtins/builtin.h"
#include "voidjs/types/object_class_type.h"
#include "voidjs/types/spec_types/property_descriptor.h"
#include "voidjs/utils/macros.h"

namespace voidjs {
namespace builtins {

bool JSArray::DefineOwnProperty(VM* vm, types::Object* O, types::String* P, const types::PropertyDescriptor& Desc, bool Throw) {
  auto factory = vm->GetObjectFactory();
  auto length_str = factory->NewStringFromTable(u"length");
  
  // 1. Let oldLenDesc be the result of calling the [[GetOwnProperty]] internal method of
  //    A passing "length" as the argument. The result will never be undefined or
  ///   an accessor descriptor because Array objects are created with a length data property that
  //    cannot be deleted or reconfigured.
  auto old_len_desc = Object::GetOwnProperty(vm, O, length_str);

  // 2. Let oldLen be oldLenDesc.[[Value]].
  auto old_len = old_len_desc.GetValue().GetInt();
  
  // 3. If P is "length", then
  if (P->GetString() == u"length") {
    // a. If the [[Value]] field of Desc is absent, then
    if (!Desc.HasValue()) {
      // i. Return the result of calling the default [[DefineOwnProperty]] internal method (8.12.9)
      //    on A passing "length", Desc, and Throw as arguments.
      return Object::DefineOwnPropertyDefault(vm, O, length_str, Desc, Throw);
    }
    
    // b. Let newLenDesc be a copy of Desc.
    auto new_len_desc = Desc;
    
    // c. Let newLen be ToUint32(Desc.[[Value]]).
    auto new_len = JSValue::ToUint32(vm, Desc.GetValue());
    
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
      return Object::DefineOwnPropertyDefault(vm, O, length_str, new_len_desc, Throw);
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
    auto new_writable {false};
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
    auto succeeded = Object::DefineOwnPropertyDefault(vm, O, length_str, new_len_desc, Throw);
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
      auto delete_succeeded = Object::Delete(vm, O, JSValue::ToString(vm, JSValue{old_len}), false);
      
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
        Object::DefineOwnPropertyDefault(vm, O, length_str, new_len_desc, false);
        
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
      types::PropertyDescriptor prop;
      prop.SetWritable(false);
      Object::DefineOwnPropertyDefault(vm, O, length_str, prop, false);
    }

    // n. Return true.
    return true;
  }
  // 4.  Else if P is an array index (15.4), then
  else if (auto P_num = JSValue::ToUint32(vm, JSValue{P});
           JSValue::ToString(vm, JSValue{P_num})->Equal(P) &&
           P_num != (1ull << 32) - 1) {
    // a. Let index be ToUint32(P).
    auto index = P_num;
    
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
    auto succeeded = Object::DefineOwnPropertyDefault(vm, O, P, Desc, false);
    
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
      Object::DefineOwnPropertyDefault(vm, O, length_str, old_len_desc, false);
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
  auto vm = argv->GetVM();
  auto args_num = argv->GetArgsNum();
  auto factory = vm->GetObjectFactory();
  auto length_str = factory->NewStringFromTable(u"length");
  
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
    auto arr = factory->NewObject(JSArray::SIZE, JSType::JS_ARRAY, ObjectClassType::ARRAY,
                                  JSValue{vm->GetArrayPrototype()}, true, false, false)->AsJSArray();
    
    auto len = argv->GetArg(0);
    if (len.IsNumber()) {
      if (JSValue::ToUint32(vm, len) != len.GetNumber()) {
        THROW_RANGE_ERROR_AND_RETURN_VALUE(
          vm, u"new Array(len) fails, becacuse len is not a uint32.", JSValue{});
      } else {
        Builtin::SetDataProperty(vm, arr, length_str, JSValue{JSValue::ToUint32(vm, len)}, true, false, false);
      }
    } else {
      Builtin::SetDataProperty(vm, arr, length_str, JSValue{1}, true, false, false);
      Builtin::SetDataProperty(vm, arr, factory->NewStringFromTable(u"0"), len, true, true, true);
    }

    return JSValue{arr};
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
    auto arr = factory->NewObject(JSArray::SIZE, JSType::JS_ARRAY, ObjectClassType::ARRAY,
                                  JSValue{vm->GetArrayPrototype()}, true, false, false)->AsJSArray();
 
    Builtin::SetDataProperty(vm, arr, length_str, JSValue{static_cast<int>(args_num)}, true, false, false);

    for (std::size_t idx = 0; idx < args_num; ++idx) {
      Builtin::SetDataProperty(vm, arr, JSValue::NumberToString(vm, idx), argv->GetArg(idx), true, true, true);
    }

    return JSValue{arr};
  }
}

// Array.IsArray(arg)
// Defined in ECMAScript 5.1 Chapter 15.4.3.2
JSValue JSArray::IsArray(RuntimeCallInfo* argv) {
  auto arg = argv->GetArg(0);
  
  // 1. If Type(arg) is not Object, return false.
  if (!arg.IsObject()) {
    return JSValue::False();
  }
  
  // 2. If the value of the [[Class]] internal property of arg is "Array", then return true.
  if (arg.GetHeapObject()->GetClassType() == ObjectClassType::ARRAY) {
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
  auto vm = argv->GetVM();
  auto this_value = argv->GetThis();
  auto arg_num = argv->GetArgsNum();
  auto factory = vm->GetObjectFactory();
  auto length_str = factory->NewStringFromTable(u"length");
  
  // 1. Let O be the result of calling ToObject passing the this value as the argument.
  auto O = JSValue::ToObject(vm, this_value);
  RETURN_VALUE_IF_HAS_EXCEPTION(vm, JSValue{});
  
  // 2. Let A be a new array created as if by the expression new Array() where
  //    Array is the standard built-in constructor with that name.
  auto A = Construct(factory->NewRuntimeCallInfo(JSValue::Undefined(), std::vector<JSValue>{}));
  
  // 3. Let n be 0.
  auto n = 0;
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
  auto concat = [vm, factory, A, &n](JSValue elem) {
    if (elem.IsObject() && elem.GetHeapObject()->GetClassType() == ObjectClassType::ARRAY) {
      auto arr = elem.GetHeapObject()->AsJSArray();
      auto k = 0;
      auto len = types::Object::Get(vm, arr, factory->NewStringFromTable(u"length")).GetInt();
      
      while (k < len) {
        auto P = JSValue::ToString(vm, JSValue{k});
        
        auto exists = types::Object::HasProperty(vm, arr, P);
        
        if (exists) {
          auto sub_element = types::Object::Get(vm, arr, P);
          DefineOwnProperty(vm, A.GetHeapObject()->AsJSArray(), JSValue::NumberToString(vm, n),
                            types::PropertyDescriptor{sub_element, true, true, true}, false);
        }
        
        ++n;
        ++k;
      }
    } else {
      DefineOwnProperty(vm, A.GetHeapObject()->AsJSArray(), JSValue::NumberToString(vm, n),
                        types::PropertyDescriptor{elem, true, true, true}, false);
      ++n;
    }
  };

  concat(this_value);

  for (std::size_t idx = 0; idx < arg_num; ++idx) {
    concat(argv->GetArg(idx));
  }

  // 6. Return A.
  return A;
}

}  // namespace builtins
}  // namespace voidjs
