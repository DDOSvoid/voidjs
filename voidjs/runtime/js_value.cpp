#include "voidjs/runtime/js_value.h"

namespace voidjs {

bool JsValue::IsFalse() const {
  return value_ == jsvalue::VALUE_FALSE;
}

bool JsValue::IsTrue() const {
  return value_ == jsvalue::VALUE_TRUE;
}

bool JsValue::IsUndefined() const {
  return value_ == jsvalue::VALUE_UNDEFINED;
}

bool JsValue::IsNull() const {
  return value_ == jsvalue::VALUE_NULL;
}

bool JsValue::IsHole() const {
  return value_ == jsvalue::VALUE_HOLE;
}

bool JsValue::IsObject() const {
  return (value_ & jsvalue::TAG_OBJECT_MASK) == jsvalue::TAG_OBJECT;
}

bool JsValue::IsInt() const {
  return (value_ & jsvalue::TAG_INT_MASK) == jsvalue::TAG_INT; 
}

bool JsValue::IsDouble() const {
  return !IsObject() && !IsInt();
}

}  // namespace voidjs
