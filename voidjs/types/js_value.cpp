#include "voidjs/types/js_value.h"
#include "voidjs/types/lang_types/object.h"
#include "voidjs/types/lang_types/string.h"

namespace voidjs {

bool JSValue::IsString() const {
  return IsObject() && GetObject()->IsString();
}

// To Primitive
// Defined in ECMAScript 5.1 Chapter 9.1
JSValue JSValue::ToPrimitive(JSValue val) {
  if (val.IsPrimitive()) {
    return val;
  } else {
    // todo
  }
}

// To Boolean
// Defined in ECMAScript 5.1 Chapter 9.2
JSValue JSValue::ToBoolean(JSValue val) {
  if (val.IsUndefined() || val.IsNull()) {
    return False();
  } else if (val.IsBoolean()) {
    return val;
  } else if (val.IsNumber()) {
    // todo
    return val.IsInt() && val.GetInt() == 0 ? True() : False();
  } else if (val.IsString()) {
    return val.GetObject()->AsString()->GetLength() == 0 ? True() : False();
  } else if (val.IsObject()) {
    return True();
  }
}

// To String
// Defined in ECMAScript 5.1 Chapter 9.8
JSValue JSValue::ToString(JSValue val) {
  if (val.IsUndefined()) {
    return JSValue(types::String::New(u"undefined"));
  } else if (val.IsNull()) {
    return JSValue(types::String::New(u"null"));
  } else if (val.IsBoolean()) {
    if (val.IsTrue()) {
      return JSValue(types::String::New(u"true"));
    } else {
      return JSValue(types::String::New(u"false"));
    }
  } else if (val.IsNumber()) {
    // todo
  } else if (val.IsString()) {
    return val;
  } else if (val.IsObject()) {
    // todo
  }
}

// Check Object Coercible
// Defined in ECMAScript 5.1 Chapter 9.10
void JSValue::CheckObjectCoercible() const {
  if (IsUndefined() || IsNull()) {
    // Throw a TypeError exception
  }
}

}  // namespace voidjs
