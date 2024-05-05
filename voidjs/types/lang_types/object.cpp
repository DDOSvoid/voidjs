#include "voidjs/types/lang_types/object.h"

#include "voidjs/builtins/js_boolean.h"
#include "voidjs/interpreter/execution_context.h"
#include "voidjs/types/heap_object.h"
#include "voidjs/types/js_value.h"
#include "voidjs/types/object_factory.h"
#include "voidjs/types/lang_types/string.h"
#include "voidjs/types/internal_types/property_map.h"
#include "voidjs/types/internal_types/internal_function.h"
#include "voidjs/builtins/js_object.h"
#include "voidjs/builtins/js_function.h"
#include "voidjs/builtins/js_array.h"
#include "voidjs/builtins/js_boolean.h"
#include "voidjs/builtins/js_number.h"
#include "voidjs/interpreter/runtime_call_info.h"
#include "voidjs/interpreter/interpreter.h"
#include "voidjs/utils/macros.h"


namespace voidjs {
namespace types {

// GetOwnProperty
// Defind in ECMAScript 5.1 Chapter 8.12.2
PropertyDescriptor Object::GetOwnProperty(VM* vm, Object* O, String* P) {
  auto props = O->GetPropertyMap();
  
  // 1. If O doesn’t have an own property with name P, return undefined.
  auto prop = props->GetProperty(vm, P);
  if (prop.IsEmpty()) {
    return PropertyDescriptor{};
  }

  // 2. Let D be a newly created Property Descriptor with no fields.
  PropertyDescriptor D;

  // 3. Let X be O’s own property named P.
  auto X = prop.GetHeapObject();
  
  // 4. If X is a data property, then
  if (X->IsDataPropertyDescriptor()) {
    // 1. Set D.[[Value]] to the value of X’s [[Value]] attribute.
    D.SetValue(X->AsDataPropertyDescriptor()->GetValue());
    
    // 2. Set D.[[Writable]] to the value of X’s [[Writable]] attribute
    D.SetWritable(X->GetWritable());
  }
  // 5. Else X is an accessor property, so
  else if (X->IsAccessorPropertyDescriptor()) {
    // 1. Set D.[[Get]] to the value of X’s [[Get]] attribute.
    D.SetGetter(X->AsAccessorPropertyDescriptor()->GetGetter());

    // 2. Set D.[[Set]] to the value of X’s [[Set]] attribute.
    D.SetSetter(X->AsAccessorPropertyDescriptor()->GetSetter());
  }

  // 6. Set D.[[Enumerable]] to the value of X’s [[Enumerable]] attribute.
  D.SetEnumerable(X->GetEnumerable());

  // 7. Set D.[[Configurable]] to the value of X’s [[Configurable]] attribute.
  D.SetConfigurable(X->GetConfigurable());

  // 8. Return D.
  return D;
}

// GetProperty
// Defined in ECMAScript 5.1 Chapter 8.12.2
PropertyDescriptor Object::GetProperty(VM* vm, Object* O, String* P) {
  // 1. Let prop be the result of calling the [[GetOwnProperty]] internal method of O with property name P.
  auto prop = GetOwnProperty(vm, O, P);

  // 2. If prop is not undefined, return prop.
  if (!prop.IsEmpty()) {
    return prop;
  }

  // 3. Let proto be the value of the [[Prototype]] internal property of O.
  auto proto = O->GetPrototype();

  // 4. If proto is null, return undefined.
  if (proto.IsNull()) {
    return PropertyDescriptor{};
  }

  // 5. Return the result of calling the [[GetProperty]] internal method of proto with argument P.
  return GetProperty(vm, proto.GetHeapObject()->AsObject(), P);
}

// Get
// Defined in ECMASCript 5.1 Chapter 8.12.3
JSValue Object::Get(VM* vm, Object* O, String* P) {
  // 1. Let desc be the result of calling the [[GetProperty]] internal method of O with property name P.
  auto desc = GetProperty(vm, O, P);

  // 2. If desc is undefined, return undefined.
  if (desc.IsEmpty()) {
    return JSValue::Undefined();
  }

  // 3. If IsDataDescriptor(desc) is true, return desc.[[Value]].
  if (desc.IsDataDescriptor()) {
    return desc.GetValue();
  }

  // 4. Otherwise, IsAccessorDescriptor(desc) must be true so, let getter be desc.[[Get]].
  auto getter = desc.GetGetter();

  // 5. If getter is undefined, return undefined.
  if (getter.IsUndefined()) {
    return JSValue::Undefined();
  }

  // 6. Return the result calling the [[Call]] internal method of getter providing O as the this value and providing no arguments.
  // todo
}

// CanPut
// Defined in ECMAScript 5.1 Chapter 8.12.4
bool Object::CanPut(VM* vm, Object* O, String* P) {
  // 1. Let desc be the result of calling the [[GetOwnProperty]] internal method of O with argument P.
  auto desc = GetOwnProperty(vm, O, P);

  // 2. If desc is not undefined, then
  if (!desc.IsEmpty()) {
    // a. If IsAccessorDescriptor(desc) is true, then
    if (desc.IsAccessorDescriptor()) {
      // i. If desc.[[Set]] is undefined, then return false.
      // ii.Else return true.
      return !desc.GetSetter().IsUndefined();
    }
    // b. Else, desc must be a DataDescriptor so return the value of desc.[[Writable]].
    else {
      return desc.GetWritable();
    }
  }

  // 3. Let proto be the [[Prototype]] internal property of O.
  auto proto = O->GetPrototype();

  // 4. If proto is null, then return the value of the [[Extensible]] internal property of O.
  if (proto.IsNull()) {
    return O->GetExtensible();
  }

  // 5. Let inherited be the result of calling the [[GetProperty]] internal method of proto with property name P.
  auto inherited = GetProperty(vm, proto.GetHeapObject()->AsObject(), P);

  // 6. If inherited is undefined, return the value of the [[Extensible]] internal property of O.
  if (inherited.IsEmpty()) {
    return O->GetExtensible();
  }

  // 7. If IsAccessorDescriptor(inherited) is true, then
  if (inherited.IsAccessorDescriptor()) {
    // a. If inherited.[[Set]] is undefined, then return false.
    // b. Return true;
    return !inherited.GetSetter().IsUndefined();
  }
  // 8. Else, inherited must be a DataDescriptor
  else {
    // a. If the [[Extensible]] internal property of O is false, return false.
    if (!O->GetExtensible()) {
      return false;
    }

    // b. Else return the value of inherited.[[Writable]].
    return inherited.GetWritable();
  }
}

// Put
// Defined in ECMAScript 5.1 Chapter 8.12.5
void Object::Put(VM* vm, Object* O, String* P, JSValue V, bool Throw) {
  // 1. If the result of calling the [[CanPut]] internal method of O with argument P is false, then
  if (!CanPut(vm, O, P)) {
    // a. If Throw is true, then throw a TypeError exception.
    // b. Else return.
    if (Throw) {
      THROW_TYPE_ERROR_AND_RETURN_VOID(
        vm, u"Object.Put cannot put property when Object.CanPut return false");
    } else {
      return ;
    }
  }
  
  // 2. Let ownDesc be the result of calling the [[GetOwnProperty]] internal method of O with argument P.
  auto own_desc = GetOwnProperty(vm, O, P);

  // 3. If IsDataDescriptor(ownDesc) is true, then
  if (own_desc.IsDataDescriptor()) {
    // a. Let valueDesc be the Property Descriptor {[[Value]]: V}.
    auto value_desc = PropertyDescriptor(V);

    // b. Call the [[DefineOwnProperty]] internal method of O passing P, valueDesc, and Throw as arguments.
    DefineOwnProperty(vm, O, P, value_desc, Throw);
    RETURN_VOID_IF_HAS_EXCEPTION(vm);

    // c. Return.
    return ;
  }

  // 4. Let desc be the result of calling the [[GetProperty]] internal method of O with argument P.
  //    This may be either an own or inherited accessor property descriptor or an inherited data property descriptor.
  auto desc = GetProperty(vm, O, P);

  // 5. If IsAccessorDescriptor(desc) is true, then
  if (desc.IsAccessorDescriptor()) {
    // a. Let setter be desc.[[Set]] which cannot be undefined.
    auto setter = desc.GetSetter();

    // b. Call the [[Call]] internal method of setter providing O as the this value and providing V as the sole argument.
    // todo
  }
  // 6. Else, create a named data property named P on object O as follows
  else {
    // a. Let newDesc be the Property Descriptor
    //    {[[Value]]: V, [[Writable]]: true, [[Enumerable]]: true, [[Configurable]]: true}.
    auto new_desc = PropertyDescriptor(V, true, true, true);

    // b. Call the [[DefineOwnProperty]] internal method of O passing P, newDesc, and Throw as arguments.
    DefineOwnProperty(vm, O, P, new_desc, Throw);
    RETURN_VOID_IF_HAS_EXCEPTION(vm);
  }

  // 7. Return.
}

// HasProperty
// Defined in ECMAScript 5.1 Chapter 8.12.6
bool Object::HasProperty(VM* vm, Object* O, String* P) {
  // 1. Let desc be the result of calling the [[GetProperty]] internal method of O with property name P.
  auto desc = GetProperty(vm, O, P);

  // 2. If desc is undefined, then return false.
  // 3. Else return true.
  return !desc.IsEmpty();
}

// Delete
// Defined in ECMAScript 5.1 Chapter 8.12.7
bool Object::Delete(VM* vm, Object* O, String* P, bool Throw) {
  // 1. Let desc be the result of calling the [[GetOwnProperty]] internal method of O with property name P.
  auto desc = GetOwnProperty(vm, O, P);

  // 2. If desc is undefined, then return true.
  if (desc.IsEmpty()) {
    return true;
  }

  // 3. If desc.[[Configurable]] is true, then
  if (desc.GetConfigurable()) {
    // a. Remove the own property with name P from O.
    O->GetPropertyMap()->DeleteProperty(vm, P);

    // b. Return true.
    return true;
  }
  // 4. Else if Throw, then throw a TypeError exception.
  else if (Throw) {
    THROW_TYPE_ERROR_AND_RETURN_VALUE(
      vm, u"Object.Delete cannot delete proprety when it's attribute [[Configurable]] is false", false);
  }

  // 5. Return false.
  return false;
}

// DefaultValue
// Defined in ECMAScript 5.1 Chapter 8.12.8
JSValue Object::DefaultValue(VM* vm, Object* O, PreferredType hint) {
  auto factory = vm->GetObjectFactory();
  
  if (hint == PreferredType::STRING) {
    // 1. Let toString be the result of calling the [[Get]] internal method of object O with argument "toString".
    auto to_string = Get(vm, O, vm->GetObjectFactory()->NewStringFromTable(u"toString"));

    // 2. If IsCallable(toString) is true then,
    if (to_string.IsCallable()) {
      // a. Let str be the result of calling the [[Call]] internal method of toString,
      //    with O as the this value and an empty argument list.
      auto str = Call(to_string.GetHeapObject()->AsObject(),
                      factory->NewRuntimeCallInfo(JSValue{O}, std::vector<JSValue>{}));

      // b. If str is a primitive value, return str.
      if (str.IsPrimitive()) {
        return str;
      }
    }

    // 3. Let valueOf be the result of calling the [[Get]] internal method of object O with argument "valueOf".
    auto value_of = Object::Get(vm, O, factory->NewStringFromTable(u"valueOf"));

    // 4. If IsCallable(valueOf) is true then,
    if (value_of.IsCallable()) {
      // a. Let val be the result of calling the [[Call]] internal method of valueOf,
      //    with O as the this value and an empty argument list.
      auto val = Call(value_of.GetHeapObject()->AsObject(),
                      factory->NewRuntimeCallInfo(JSValue{O}, std::vector<JSValue>{}));
      
      // b. If val is a primitive value, return val.
      if (val.IsPrimitive()) {
        return val;
      }
    }

    // 5. Throw a TypeError exception.
    THROW_TYPE_ERROR_AND_RETURN_VALUE(vm, u"Object.DefaultValue fails when toString and valueOf both fail", JSValue{});
  } else {
    // hint must be PreferredType::NUMBER

    // 1. Let valueOf be the result of calling the [[Get]] internal method of object O with argument "valueOf".
    auto value_of = Object::Get(vm, O, factory->NewStringFromTable(u"valueOf"));

    // 2. If IsCallable(valueOf) is true then,
    if (value_of.IsCallable()) {
      // a. Let val be the result of calling the [[Call]] internal method of valueOf,
      //    with O as the this value and an empty argument list.
      auto val = Call(value_of.GetHeapObject()->AsObject(),
                      factory->NewRuntimeCallInfo(JSValue{O}, std::vector<JSValue>{}));
      
      // b. If val is a primitive value, return val.
      if (val.IsPrimitive()) {
        return val;
      }
    }

    // 3. Let toString be the result of calling the [[Get]] internal method of object O with argument "toString".
    auto to_string = Get(vm, O, vm->GetObjectFactory()->NewStringFromTable(u"toString"));

    // 4. If IsCallable(toString) is true then,
    if (to_string.IsCallable()) {
      // a. Let str be the result of calling the [[Call]] internal method of toString,
      //    with O as the this value and an empty argument list.
      auto str = Call(to_string.GetHeapObject()->AsObject(),
                      factory->NewRuntimeCallInfo(JSValue{O}, std::vector<JSValue>{}));

      // b. If str is a primitive value, return str.
      if (str.IsPrimitive()) {
        return str;
      }
    }

    // 5. Throw a TypeError exception.
    THROW_TYPE_ERROR_AND_RETURN_VALUE(vm, u"Object.DefaultValue fails when toString and valueOf both fail", JSValue{});
  }
}

// only used to forward
bool Object::DefineOwnProperty(VM* vm, Object* O, String* P, const PropertyDescriptor& Desc, bool Throw) {
  if (O->IsJSArray()) {
    return builtins::JSArray::DefineOwnProperty(vm, O, P, Desc, Throw);
  } else {
    return DefineOwnPropertyDefault(vm, O, P, Desc, Throw);
  }
}

// DefineOwnProperty
// Defined in ECMAScript 5.1 Chapter 8.12.9
bool Object::DefineOwnPropertyDefault(VM* vm, Object* O, String* P, const PropertyDescriptor& Desc, bool Throw) {
  // 1. Let current be the result of calling the [[GetOwnProperty]] internal method of O with property name P.
  auto current = GetOwnProperty(vm, O, P);

  // 2. Let extensible be the value of the [[Extensible]] internal property of O.
  auto extensible = O->GetExtensible();

  // 3. If current is undefined and extensible is false, then Reject.
  if (current.IsEmpty() && !extensible) {
    if (Throw) {
      THROW_TYPE_ERROR_AND_RETURN_VALUE(
        vm, u"Object.DefineOwnProperty fails when it's attribute [[Extensible]] is false", false);
    } else {
      return false;
    }
  }

  // 4. If current is undefined and extensible is true, then
  if (current.IsEmpty() && extensible) {
    // a. If IsGenericDescriptor(Desc) or IsDataDescriptor(Desc) is true, then
    if (Desc.IsGenericDescriptor() || Desc.IsDataDescriptor()) {
      // i. Create an own data property named P of object O
      //    whose [[Value]], [[Writable]], [[Enumerable]] and [[Configurable]] attribute values are described by Desc.
      //    If the value of an attribute field of Desc is absent,
      //    the attribute of the newly created property is set to its default value.
      auto own_desc = PropertyDescriptor(Desc.GetValue(), Desc.GetWritable(), Desc.GetEnumerable(), Desc.GetConfigurable());
      auto prop_map = O->GetPropertyMap();
      O->SetPropertyMap(PropertyMap::SetProperty(vm, prop_map, P, own_desc));
    }
    // b. Else, Desc must be an accessor Property Descriptor so,
    else {
      // i. Create an own accessor property named P of object O
      //    whose [[Get]], [[Set]], [[Enumerable]] and [[Configurable]] attribute values are described by Desc.
      //    If the value of an attribute field of Desc is absent,
      //    the attribute of the newly created property is set to its default value.
      auto own_desc = PropertyDescriptor(Desc.GetGetter(), Desc.GetSetter(), Desc.GetEnumerable(), Desc.GetConfigurable());
      auto prop_map = O->GetPropertyMap();
      O->SetPropertyMap(PropertyMap::SetProperty(vm, prop_map, P, own_desc));
    }

    // c. Return true
    return true;
  }

  // 5. Return true, if every field in Desc is absent.
  if (Desc.IsEmpty()) {
    return true;
  }

  // 6. Return true,
  //    if every field in Desc also occurs in current and
  //    the value of every field in Desc is the same value as the corresponding field in current
  //    when compared using the SameValue algorithm (9.12).
  {
    bool flag = true;
    if (Desc.HasValue() && !current.HasValue() ||
        Desc.HasValue() && current.HasValue() && !JSValue::SameValue(Desc.GetValue(), current.GetValue())) {
      flag = false;
    }
    if (Desc.HasGetter() && !current.HasGetter() ||
        Desc.HasGetter() && current.HasGetter() && !JSValue::SameValue(Desc.GetGetter(), current.GetGetter())) {
      flag = false;
    }
    if (Desc.HasSetter() && !current.HasSetter() ||
        Desc.HasSetter() && current.HasSetter() && !JSValue::SameValue(Desc.GetSetter(), current.GetSetter())) {
      flag = false;
    }
    if (Desc.HasWritable() && !current.HasWritable() ||
        Desc.HasWritable() && current.HasWritable() && Desc.GetWritable() != current.GetWritable()) {
      flag = false;
    }
    if (Desc.HasEnumerable() && !current.HasEnumerable() ||
        Desc.HasEnumerable() && current.HasEnumerable() && Desc.GetEnumerable() != current.GetEnumerable()) {
      flag = false;
    }
    if (Desc.HasConfigurable() && !current.HasConfigurable() ||
        Desc.HasConfigurable() && current.HasConfigurable() && Desc.GetConfigurable() != current.GetConfigurable()) {
      flag = false;
    }
    if (flag) {
      return true;
    }
  }
  
  // 7. If the [[Configurable]] field of current is false then
  if (!current.GetConfigurable()) {
    // a. Reject, if the [[Configurable]] field of Desc is true.
    // b. Reject, if the [[Enumerable]] field of Desc is present and the
    //    [[Enumerable]] fields of current and Desc are the Boolean negation of each other.
    if (Desc.GetConfigurable() || (Desc.HasEnumerable() && current.GetEnumerable() != Desc.GetEnumerable())) {
      if (Throw) {
        THROW_TYPE_ERROR_AND_RETURN_VALUE(
          vm, u"Object.DefineOwnProprety fails when it's attribute [[Configurable]] is false", false);
      } else {
        return false;
      }
    }
  }

  // 8. If IsGenericDescriptor(Desc) is true, then no further validation is required.
  if (Desc.IsGenericDescriptor()) {
    // todo
  }
  // 9. Else, if IsDataDescriptor(current) and IsDataDescriptor(Desc) have different results, then
  else if (current.IsDataDescriptor() != Desc.IsDataDescriptor()) {
    // a. Reject, if the [[Configurable]] field of current is false.
    if (!current.GetConfigurable()) {
      if (Throw) {
        THROW_TYPE_ERROR_AND_RETURN_VALUE(
          vm, u"Object.DefineOwnProprety fails when it's attribute [[Configurable]] is false", false);
      } else {
        return false;
      }
    }
    
    // b. If IsDataDescriptor(current) is true, then
    if (current.IsDataDescriptor()) {
      // i. Convert the property named P of object O from a data property to an accessor property.
      //    Preserve the existing values of the converted
      //    property’s [[Configurable]] and [[Enumerable]] attributes
      //    and set the rest of the property’s attributes to their default values.
      auto own_desc = PropertyDescriptor(JSValue::Undefined(), JSValue::Undefined(),
                                         current.GetEnumerable(), current.GetConfigurable());
      auto prop_map = O->GetPropertyMap();
      O->SetPropertyMap(PropertyMap::SetProperty(vm, prop_map, P, own_desc));
    }
    // c. Else
    else {
      // i. Convert the property named P of object O from an accessor property to a data property.
      //    Preserve the existing values of the converted
      //    property’s [[Configurable]] and [[Enumerable]] attributes
      //    and set the rest of the property’s attributes to their default values.
      auto own_desc = PropertyDescriptor(JSValue::Undefined(), current.GetWritable(),
                                         current.GetEnumerable(), current.GetConfigurable());
      auto prop_map = O->GetPropertyMap();
      O->SetPropertyMap(PropertyMap::SetProperty(vm, prop_map, P, own_desc));
    }
  }
  // 10. Else, if IsDataDescriptor(current) and IsDataDescriptor(Desc) are both true, then
  else if (current.IsDataDescriptor() && Desc.IsDataDescriptor()) {
    // a. If the [[Configurable]] field of current is false, then
    if (!current.GetConfigurable()) {
      // i. Reject, if the [[Writable]] field of current is false
      //    and the [[Writable]] field of Desc is true.
      if (!current.GetWritable() && Desc.GetWritable()) {
        if (Throw) {
          THROW_TYPE_ERROR_AND_RETURN_VALUE(
            vm, u"Object.DefineOwnProprety fails when it's attribute [[Configurable]] is false", false);
        } else {
          return false;
        }
      }
      
      // ii. If the [[Writable]] field of current is false, then
      if (!current.GetWritable()) {
        // 1. Reject, if the [[Value]] field of Desc is present
        //    and SameValue(Desc.[[Value]], current.[[Value]]) is false.
        if (!Desc.GetValue().IsEmpty() && !JSValue::SameValue(Desc.GetValue(), current.GetValue())) {
          if (Throw) {
            THROW_TYPE_ERROR_AND_RETURN_VALUE(
              vm, u"Object.DefineOwnProprety fails when it's attribute [[Configurable]] is false", false);
          } else {
            return false;
          }
        }
      }
    }
    // b. else, the [[Configurable]] field of current is true, so any change is acceptable.
    else {
      // todo
    }
  }
  // 11. Else, IsAccessorDescriptor(current) and IsAccessorDescriptor(Desc) are both true so,
  else {
    // a. If the [[Configurable]] field of current is false, then
    if (!current.GetConfigurable()) {
      // i. Reject, if the [[Set]] field of Desc is present
      //    and SameValue(Desc.[[Set]], current.[[Set]]) is false.
      if (!Desc.GetSetter().IsEmpty() && !JSValue::SameValue(Desc.GetSetter(), current.GetSetter())) {
        if (Throw) {
          THROW_TYPE_ERROR_AND_RETURN_VALUE(
            vm, u"Object.DefineOwnProprety fails when it's attribute [[Configurable]] is false", false);
        } else {
          return false;
        }
      }

      // ii. Reject, if the [[Get]] field of Desc is present
      //     and SameValue(Desc.[[Get]], current.[[Get]]) is false.
      if (!Desc.GetGetter().IsEmpty() && !JSValue::SameValue(Desc.GetGetter(), current.GetGetter())) {
        if (Throw) {
          THROW_TYPE_ERROR_AND_RETURN_VALUE(
            vm, u"Object.DefineOwnProprety fails when it's attribute [[Configurable]] is false", false);
        } else {
          return false;
        }
      }
    }
  }

  // 12. For each attribute field of Desc that is present,
  //     set the correspondingly named attribute of the property named P of object O to the value of the field.
  {
    if (Desc.HasValue()) {
      current.SetValue(Desc.GetValue());
    }
    if (Desc.HasGetter()) {
      current.SetGetter(Desc.GetGetter());
    }
    if (Desc.HasSetter()) {
      current.SetSetter(Desc.GetSetter());
    }
    if (Desc.HasWritable()) {
      current.SetWritable(Desc.GetWritable());
    }
    if (Desc.HasEnumerable()) {
      current.SetEnumerable(Desc.GetEnumerable());
    }
    if (Desc.HasConfigurable()) {
      current.SetConfigurable(Desc.GetConfigurable());
    }
    auto prop_map = O->GetPropertyMap();
    O->SetPropertyMap(PropertyMap::SetProperty(vm, prop_map, P, current));
  }

  // 13. Return true.
  return true;
}

// Construct
// Only used for forwarding to concrete [[Construct]]
JSValue Object::Construct(Object* O, RuntimeCallInfo* argv) {
  auto vm = argv->GetVM();
  
  if (O == vm->GetObjectConstructor()) {
    return JSValue{builtins::JSObject::Construct(argv)};
  }

  if (O == vm->GetFunctionConstructor()) {
    return JSValue{builtins::JSFunction::Construct(argv)};
  }

  if (O == vm->GetArrayConstructor()) {
    return JSValue{builtins::JSArray::Construct(argv)};
  }

  if (O == vm->GetBooleanConstructor()) {
    return JSValue{builtins::JSBoolean::Construct(argv)};
  }

  if (O == vm->GetNumberConstructor()) {
    return JSValue{builtins::JSNumber::Construct(argv)};
  }
}

// Call
// Only used for forwarding to concrete [[Call]]
JSValue Object::Call(Object* O, RuntimeCallInfo* argv) {
  auto vm = argv->GetVM();
  
  if (O == vm->GetObjectConstructor()) {
    return builtins::JSObject::Call(argv);
  }

  if (O == vm->GetFunctionConstructor()) {
    return builtins::JSFunction::Call(argv);
  }

  if (O == vm->GetArrayConstructor()) {
    return builtins::JSArray::Call(argv);
  }

  if (O->IsJSFunction()) {
    auto this_value {argv->GetThis()};
    std::vector<JSValue> args {argv->GetArgs(), argv->GetArgs() + argv->GetArgsNum()};
  
    auto F = O->AsJSFunction();
    // 1. Let funcCtx be the result of establishing a new execution context for function code
    //    using the value of F's [[FormalParameters]] internal property,
    //    the passed arguments List args, and the this value as described in 10.4.3.
    ExecutionContext::EnterFunctionCode(vm, F->GetCode(), F, this_value, args);
    RETURN_VALUE_IF_HAS_EXCEPTION(vm, JSValue{});
    
    // 2. Let result be the result of evaluating the FunctionBody that is the value of F's [[Code]] internal property.
    //    If F does not have a [[Code]] internal property or if its value is an empty FunctionBody,
    //    then result is (normal, undefined, empty).
    const auto& stmts = std::invoke([=]() {
      auto ast_node = F->GetCode();
      if (ast_node->AsFunctionDeclaration()) {
        return ast_node->AsFunctionDeclaration()->GetStatements();
      } else {
        // ast_node must be FunctionExpression
        return ast_node->AsFunctionExpression()->GetStatements();
      }
    });
    auto result = vm->GetInterpreter()->EvalSourceElements(stmts);
    
    // 3. Exit the execution context funcCtx, restoring the previous execution context.
    vm->PopExecutionContext();
    
    // 4. If result.type is throw then throw result.value.
    RETURN_VALUE_IF_HAS_EXCEPTION(vm, JSValue{});
    
    // 5. If result.type is return then return result.value.
    if (result.GetType() == CompletionType::RETURN) {
      return result.GetValue();
    }
    // 6. Otherwise result.type must be normal. Return undefined.
    else {
      return JSValue::Undefined();
    }
  }

  if (O->IsInternalFunction()) {
    auto func = O->AsInternalFunction()->GetFunction();
    return func(argv);
  }
}

}  // namespace types
}  // namespace voidjs
