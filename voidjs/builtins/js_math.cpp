#include "voidjs/builtins/js_math.h"

#include <cmath>
#include <limits>

#include "voidjs/types/heap_object.h"
#include "voidjs/types/js_value.h"
#include "voidjs/types/lang_types/number.h"
#include "voidjs/gc/js_handle_scope.h"
#include "voidjs/interpreter/runtime_call_info.h"

namespace voidjs {
namespace builtins {

// Math.abs(x)
// Defined in ECMAScript 5.1 Chapter 15.8.2.1
JSValue JSMath::Abs(RuntimeCallInfo* argv) {
  VM* vm = argv->GetVM();
  JSHandleScope handle_scope{vm};
  JSHandle<JSValue> x = argv->GetArg(0);
  
  // 1. If x is NaN, the result is NaN.
  // 2. If x is −0, the result is +0.
  // 3. If x is −∞, the result is +∞.
  types::Number num = JSValue::ToNumber(vm, x);

  if (num.IsDouble()) {
    return JSValue{std::fabs(num.GetDouble())};
  } else {
    if (num.GetInt() == std::numeric_limits<int>::min()) {
      return JSValue{-num.GetDouble()};
    } else {
      return JSValue{-num.GetInt()};
    }
  }
}

// Math.acos(x)
// Defined in ECMAScript 5.1 Chapter 15.8.2.2
JSValue JSMath::Acos(RuntimeCallInfo* argv) {
  VM* vm = argv->GetVM();
  JSHandleScope handle_scope{vm};
  JSHandle<JSValue> x = argv->GetArg(0);

  // 1. If x is NaN, the result is NaN.
  // 2. If x is greater than 1, the result is NaN.
  // 3. If x is less than −1, the result is NaN.
  // 4. If x is exactly 1, the result is +0.
  types::Number num = JSValue::ToNumber(vm, x);

  if (!std::isnan(num.GetNumber()) && -1 <= num.GetNumber() && num.GetNumber() <= 1) {
    return JSValue{std::acos(num.GetNumber())};
  } else {
    return types::Number::NaN();
  }
}

// Math.asin(x)
// Defined in ECMAScript 5.1 Chapter 15.8.2.3
JSValue JSMath::Asin(RuntimeCallInfo* argv) {
  VM* vm = argv->GetVM();
  JSHandleScope handle_scope{vm};
  JSHandle<JSValue> x = argv->GetArg(0);

  // 1. If x is NaN, the result is NaN.
  // 2. If x is greater than 1, the result is NaN.
  // 3. If x is less than –1, the result is NaN.
  // 4. If x is +0, the result is +0.
  // 5. If x is −0, the result is −0.
  types::Number num = JSValue::ToNumber(vm, x);

  if (!std::isnan(num.GetNumber()) && -1 <= num.GetNumber() && num.GetNumber() <= 1) {
    return JSValue{std::asin(num.GetNumber())};
  } else {
    return types::Number::NaN();
  }
}

}  // namespace builtins
}  // namespace voidjs
