#include "voidjs/types/js_value.h"

#include <cmath>
#include <limits>

#include "voidjs/parser/parser.h"
#include "voidjs/types/heap_object.h"
#include "voidjs/types/object_factory.h"
#include "voidjs/types/lang_types/string.h"

namespace voidjs {

bool JSValue::IsString() const {
  return IsHeapObject() && GetHeapObject()->IsString();
}

// To Primitive
// Defined in ECMAScript 5.1 Chapter 9.1
JSValue JSValue::ToPrimitive(JSValue val, PreferredType type) {
  if (val.IsPrimitive()) {
    return val;
  } else if (val.IsObject()) {
    // todo
  }
  
  // this branch is unreachable
  return JSValue{};
}

// To Boolean
// Defined in ECMAScript 5.1 Chapter 9.2
bool JSValue::ToBoolean(JSValue val) {
  if (val.IsUndefined() || val.IsNull()) {
    return false;
  } else if (val.IsBoolean()) {
    return val == True();
  } else if (val.IsNumber()) {
    if (val.IsInt()) {
      return val.GetInt() == 0;
    }
    if (val.IsDouble()) {
      auto d = val.GetDouble();
      return !std::isnan(d) && d != 0;
    }
  } else if (val.IsString()) {
    return val.GetHeapObject()->AsString()->GetLength() == 0;
  } else if (val.IsObject()) {
    return true;
  }
  
  // this branch is unreachable
  return false;
}

// To Number
// Defined in ECMAScript 5.1 Chapter 9.3
JSValue JSValue::ToNumber(JSValue val) {
  if (val.IsUndefined()) {
    return JSValue(std::numeric_limits<double>::quiet_NaN());
  } else if (val.IsNull()) {
    return JSValue(0);
  } else if (val.IsBoolean()) {
    return JSValue(val.GetBoolean() ? 1 : 0);
  } else if (val.IsNumber()) {
    return val;
  } else if (val.IsString()) {
    // todo
  } else if (val.IsObject()) {
    auto prim_val = ToPrimitive(val, PreferredType::NUMBER);
    return ToNumber(prim_val);
  }

  // this branch is unreachable
  return JSValue{};
}

// To String
// Defined in ECMAScript 5.1 Chapter 9.8
JSValue JSValue::ToString(JSValue val) {
  if (val.IsUndefined()) {
    return JSValue(ObjectFactory::NewString(u"undefined"));
  } else if (val.IsNull()) {
    return JSValue(ObjectFactory::NewString(u"null"));
  } else if (val.IsBoolean()) {
    if (val.IsTrue()) {
      return JSValue(ObjectFactory::NewString(u"true"));
    } else {
      return JSValue(ObjectFactory::NewString(u"false"));
    }
  } else if (val.IsNumber()) {
    // todo
  } else if (val.IsString()) {
    return val;
  } else if (val.IsObject()) {
    // todo
  }

  // this branch is unreachable
  return JSValue{};
}

// ToObject
// Defined in ECMAScript 5.1 Chapter 9.9
JSValue JSValue::ToObject(JSValue val) {
  if (val.IsUndefined() || val.IsNull()) {
    // todo
  }
  if (val.IsBoolean()) {
    // todo
  }
  if (val.IsNull()) {
    // todo
  }
  if (val.IsString()) {
    // todo
  }
  if (val.IsObject()) {
    return val;
  }
  
  // this branch is unreachable
  return JSValue{};
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
  
  // If Type(x) is Boolean,
  // return true if x and y are both true or both false;
  // otherwise, return false.
  if (x.IsBoolean() && y.IsBoolean()) {
    return x.GetBoolean() == y.GetBoolean();
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
  if (x.IsString() && y.IsString()) {
    return
      x.GetHeapObject()->AsString()->GetString() ==
      y.GetHeapObject()->AsString()->GetString();
  }
  
  return false;
}

}  // namespace voidjs
