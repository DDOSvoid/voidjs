#include "voidjs/interpreter/global_constants.h"

#include "voidjs/gc/js_handle_scope.h"
#include "voidjs/types/js_value.h"
#include "voidjs/types/object_factory.h"
#include "voidjs/types/lang_types/string.h"

namespace voidjs {

#define DEFINE_GET_METHOD_FOR_JSVALUE(name, index)              \
  JSValue GlobalConstants::name() const  {                      \
    return constants_[index];                                   \
  }                                                             \
  JSHandle<JSValue> GlobalConstants::Handled##name() const  {   \
    return JSHandle<JSValue>{                                   \
      reinterpret_cast<uintptr_t>(&constants_[index])};         \
  }                                                             \

#define DEFINE_GET_METHOD_FOR_HEAP_OBJECT(name, castname, type, index)  \
  type* GlobalConstants::name() const  {                                \
    return constants_[index].GetHeapObject()->As##castname();           \
  }                                                                     \
  JSHandle<type> GlobalConstants::Handled##name() const  {              \
    return JSHandle<type>{                                              \
      reinterpret_cast<uintptr_t>(&constants_[index])};                 \
  }                                                                     \

DEFINE_GET_METHOD_FOR_JSVALUE(Undefined, 0)
DEFINE_GET_METHOD_FOR_JSVALUE(Null, 1)
DEFINE_GET_METHOD_FOR_JSVALUE(False, 2)
DEFINE_GET_METHOD_FOR_JSVALUE(True, 3)

DEFINE_GET_METHOD_FOR_HEAP_OBJECT(EmptyString, String, types::String, 4)

DEFINE_GET_METHOD_FOR_HEAP_OBJECT(LengthString, String, types::String, 5)

DEFINE_GET_METHOD_FOR_HEAP_OBJECT(UndefinedString, String, types::String, 6)
DEFINE_GET_METHOD_FOR_HEAP_OBJECT(NullString, String, types::String, 7)
DEFINE_GET_METHOD_FOR_HEAP_OBJECT(FalseString, String, types::String, 8)
DEFINE_GET_METHOD_FOR_HEAP_OBJECT(TrueString, String, types::String, 9)

DEFINE_GET_METHOD_FOR_HEAP_OBJECT(ZeroString, String, types::String, 10)
DEFINE_GET_METHOD_FOR_HEAP_OBJECT(NANString, String, types::String, 11)
DEFINE_GET_METHOD_FOR_HEAP_OBJECT(PositiveInfinityString, String, types::String, 12)
DEFINE_GET_METHOD_FOR_HEAP_OBJECT(NegativeInfinityString, String, types::String, 13)

DEFINE_GET_METHOD_FOR_HEAP_OBJECT(ToStringString, String, types::String, 14)
DEFINE_GET_METHOD_FOR_HEAP_OBJECT(ValueOfString, String, types::String, 15)

DEFINE_GET_METHOD_FOR_HEAP_OBJECT(ValueString, String, types::String, 16)
DEFINE_GET_METHOD_FOR_HEAP_OBJECT(WritableString, String, types::String, 17)
DEFINE_GET_METHOD_FOR_HEAP_OBJECT(GetString, String, types::String, 18)
DEFINE_GET_METHOD_FOR_HEAP_OBJECT(SetString, String, types::String, 19)
DEFINE_GET_METHOD_FOR_HEAP_OBJECT(EnumerableString, String, types::String, 20)
DEFINE_GET_METHOD_FOR_HEAP_OBJECT(ConfigurableString, String, types::String, 21)

DEFINE_GET_METHOD_FOR_HEAP_OBJECT(ConstructorString, String, types::String, 22)
DEFINE_GET_METHOD_FOR_HEAP_OBJECT(PrototypeString, String, types::String, 23)

DEFINE_GET_METHOD_FOR_HEAP_OBJECT(ObjectString, String, types::String, 24)
DEFINE_GET_METHOD_FOR_HEAP_OBJECT(FunctionString, String, types::String, 25)
DEFINE_GET_METHOD_FOR_HEAP_OBJECT(ArrayString, String, types::String, 26)
DEFINE_GET_METHOD_FOR_HEAP_OBJECT(StringString, String, types::String, 27)
DEFINE_GET_METHOD_FOR_HEAP_OBJECT(BooleanString, String, types::String, 28)
DEFINE_GET_METHOD_FOR_HEAP_OBJECT(NumberString, String, types::String, 29)
DEFINE_GET_METHOD_FOR_HEAP_OBJECT(DateString, String, types::String, 30)
DEFINE_GET_METHOD_FOR_HEAP_OBJECT(RegExpString, String, types::String, 31)
DEFINE_GET_METHOD_FOR_HEAP_OBJECT(ErrorString, String, types::String, 32)
DEFINE_GET_METHOD_FOR_HEAP_OBJECT(EvalErrorString, String, types::String, 33)
DEFINE_GET_METHOD_FOR_HEAP_OBJECT(RangeErrorString, String, types::String, 34)
DEFINE_GET_METHOD_FOR_HEAP_OBJECT(ReferenceErrorString, String, types::String, 35)
DEFINE_GET_METHOD_FOR_HEAP_OBJECT(SyntaxErrorString, String, types::String, 36)
DEFINE_GET_METHOD_FOR_HEAP_OBJECT(TypeErrorString, String, types::String, 37)
DEFINE_GET_METHOD_FOR_HEAP_OBJECT(URIErrorString, String, types::String, 38)

#define SET_CONSTANT(name, value, index)         \
  constants_[index] = value       \


void GlobalConstants::Initialize() {
  JSHandleScope handle_scope{vm_};
  auto factory = vm_->GetObjectFactory();
    
  SET_CONSTANT(Undefined, JSValue::Undefined(), 0);
  SET_CONSTANT(Null, JSValue::Null(), 1);
  SET_CONSTANT(Null, JSValue::False(), 2);
  SET_CONSTANT(Null, JSValue::True(), 3);

  SET_CONSTANT(EmptyString, factory->NewString<GCFlag::CONST>(u"").GetJSValue(), 4);
  
  SET_CONSTANT(LengthString, factory->NewString<GCFlag::CONST>(u"length").GetJSValue(), 5);
  
  SET_CONSTANT(UndefinedString, factory->NewString<GCFlag::CONST>(u"undefined").GetJSValue(), 6);
  SET_CONSTANT(NullString, factory->NewString<GCFlag::CONST>(u"null").GetJSValue(), 7);
  SET_CONSTANT(FalseString, factory->NewString<GCFlag::CONST>(u"false").GetJSValue(), 8);
  SET_CONSTANT(TrueString, factory->NewString<GCFlag::CONST>(u"true").GetJSValue(), 9);
  
  SET_CONSTANT(ZeroString, factory->NewString<GCFlag::CONST>(u"0").GetJSValue(), 10);
  SET_CONSTANT(NANString, factory->NewString<GCFlag::CONST>(u"NAN").GetJSValue(), 11);
  SET_CONSTANT(PositiveInfinityString, factory->NewString<GCFlag::CONST>(u"Infinity").GetJSValue(), 12);
  SET_CONSTANT(NegativeInfinityString, factory->NewString<GCFlag::CONST>(u"-Infinity").GetJSValue(), 13);

  SET_CONSTANT(ToStringString, factory->NewString<GCFlag::CONST>(u"toString").GetJSValue(), 14);
  SET_CONSTANT(ValueOfString, factory->NewString<GCFlag::CONST>(u"valueOf").GetJSValue(), 15);

  SET_CONSTANT(ValueString, factory->NewString<GCFlag::CONST>(u"value").GetJSValue(), 16);
  SET_CONSTANT(WritableString, factory->NewString<GCFlag::CONST>(u"writable").GetJSValue(), 17);
  SET_CONSTANT(GetString, factory->NewString<GCFlag::CONST>(u"get").GetJSValue(), 18);
  SET_CONSTANT(SetString, factory->NewString<GCFlag::CONST>(u"set").GetJSValue(), 19);
  SET_CONSTANT(EnumerableString, factory->NewString<GCFlag::CONST>(u"enumerable").GetJSValue(), 20);
  SET_CONSTANT(ConfigurableString, factory->NewString<GCFlag::CONST>(u"configurable").GetJSValue(), 21);
  
  SET_CONSTANT(ConstructorString, factory->NewString<GCFlag::CONST>(u"constructor").GetJSValue(), 22);
  SET_CONSTANT(PrototypeString, factory->NewString<GCFlag::CONST>(u"prototype").GetJSValue(), 23);

  SET_CONSTANT(ObjectString, factory->NewString<GCFlag::CONST>(u"Object").GetJSValue(), 24);
  SET_CONSTANT(FunctionString, factory->NewString<GCFlag::CONST>(u"Function").GetJSValue(), 25);
  SET_CONSTANT(ArrayString, factory->NewString<GCFlag::CONST>(u"Array").GetJSValue(), 26);
  SET_CONSTANT(StringString, factory->NewString<GCFlag::CONST>(u"String").GetJSValue(), 27);
  SET_CONSTANT(BooleanString, factory->NewString<GCFlag::CONST>(u"Boolean").GetJSValue(), 28);
  SET_CONSTANT(NumberString, factory->NewString<GCFlag::CONST>(u"Number").GetJSValue(), 29);
  SET_CONSTANT(DateString, factory->NewString<GCFlag::CONST>(u"Date").GetJSValue(), 30);
  SET_CONSTANT(RegExpString, factory->NewString<GCFlag::CONST>(u"RegExp").GetJSValue(), 31);
  SET_CONSTANT(ErrorString, factory->NewString<GCFlag::CONST>(u"Error").GetJSValue(), 32);
  SET_CONSTANT(EvalErrorString, factory->NewString<GCFlag::CONST>(u"EvalError").GetJSValue(), 33);
  SET_CONSTANT(RangeErrorString, factory->NewString<GCFlag::CONST>(u"RangeError").GetJSValue(), 34);
  SET_CONSTANT(ReferenceErrorString, factory->NewString<GCFlag::CONST>(u"ReferenceError").GetJSValue(), 35);
  SET_CONSTANT(SynTaxString, factory->NewString<GCFlag::CONST>(u"SyntaxError").GetJSValue(), 36);
  SET_CONSTANT(TypeErrorString, factory->NewString<GCFlag::CONST>(u"TypeError").GetJSValue(), 37);
  SET_CONSTANT(URIErrorString, factory->NewString<GCFlag::CONST>(u"URIError").GetJSValue(), 38);
}

#undef DEFINE_GET_METHOD_FOR_JSVALUE
#undef DEFINE_GET_METHOD_FOR_HEAP_OBJECT

}  // namespace voidjs
