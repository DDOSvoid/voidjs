#include "voidjs/types/js_value.h"
#include "voidjs/parser/parser.h"
#include "voidjs/types/heap_object.h"
#include "voidjs/types/lang_types/string.h"

namespace voidjs {

bool JSValue::IsString() const {
  return IsHeapObject() && GetHeapObject()->IsString();
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
    return val.GetHeapObject()->AsString()->GetLength() == 0 ? True() : False();
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

// Check if object is callable
// Defined in ECMAScript 5.1 Chapter 9.11
bool JSValue::IsCallable() const {
  return IsObject() && GetHeapObject()->GetCallable();
}

// Check if x and y are the same
// Defined in ECMAScript 5.1 Chapter 9.12
bool JSValue::SameValue(JSValue x, JSValue y) {
  // Return true if x and y refer to the same object. 
  if (x.GetRawData() == y.GetRawData()) {
    return true;
  }
  
  // If Type(x) and Type(y) is Undefined, return true.
  if (x.IsUndefined() && y.IsUndefined()) {
    return true;
  }
  
  // If Type(x) and Type(y) is Null, return true.
  if (x.IsNull() && y.IsNull()) {
    return true;
  }

  // todo
  if (x.IsNumber() && y.IsNumber()) {
    if (x.IsInt() && y.IsInt()) {
      return x.GetInt() == y.GetInt();
    }
  }

  // If Type(x) is String,
  // then return true if x and y are exactly the same sequence of characters
  // (same length and same characters in corresponding positions);
  // otherwise, return false.
  if (x.IsString()) {
    return
      x.GetHeapObject()->AsString()->GetString() ==
      y.GetHeapObject()->AsString()->GetString();
  }
  
  return false;
}

}  // namespace voidjs
