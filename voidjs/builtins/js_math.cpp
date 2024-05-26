#include "voidjs/builtins/js_math.h"

#include <cmath>
#include <limits>
#include <random>

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
      return types::Number{-num.GetDouble()};
    } else {
      return types::Number{-num.GetInt()};
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

  if (!num.IsNaN() && -1 <= num.GetNumber() && num.GetNumber() <= 1) {
    return types::Number{std::acos(num.GetNumber())};
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

  if (!num.IsNaN() && -1 <= num.GetNumber() && num.GetNumber() <= 1) {
    return types::Number{std::asin(num.GetNumber())};
  } else {
    return types::Number::NaN();
  }
}

// Math.atan(x)
// Defined in ECMAScript 5.1 Chapter 15.8.2.4
JSValue JSMath::Atan(RuntimeCallInfo* argv) {
  VM* vm = argv->GetVM();
  JSHandleScope handle_scope{vm};
  JSHandle<JSValue> x = argv->GetArg(0);

  // 1. If x is NaN, the result is NaN.
  // 2. If x is +0, the result is +0.
  // 3. If x is −0, the result is −0.
  // 4. If x is +∞, the result is an implementation-dependent approximation to +π/2.
  // 5. If x is −∞, the result is an implementation-dependent approximation to −π/2.
  types::Number num = JSValue::ToNumber(vm, x);

  if (num.IsNaN()) {
    return types::Number::NaN();
  } else {
    return types::Number{std::atan(num.GetNumber())};
  }
}

// Math.atan2(y, x)
// Defined in ECMAScript 5.1 Chapter 15.8.2.5
JSValue JSMath::Atan2(RuntimeCallInfo* argv) {
  VM* vm = argv->GetVM();
  JSHandleScope handle_scope{vm};
  JSHandle<JSValue> y = argv->GetArg(0);
  JSHandle<JSValue> x = argv->GetArg(1);

  // 1. If either x or y is NaN, the result is NaN.
  // 2. If y>0 and x is +0, the result is an implementation-dependent approximation to +π/2.
  // 3. If y>0 and x is −0, the result is an implementation-dependent approximation to +π/2.
  // 4. If y is +0 and x>0, the result is +0.
  // 5. If y is +0 and x is +0, the result is +0.
  // 6. If y is +0 and x is −0, the result is an implementation-dependent approximation to +π.
  // 7. If y is +0 and x<0, the result is an implementation-dependent approximation to +π.
  // 8. If y is −0 and x>0, the result is −0.
  // 9. If y is −0 and x is +0, the result is −0.
  // 10. If y is −0 and x is −0, the result is an implementation-dependent approximation to −π.
  // 11. If y is −0 and x<0, the result is an implementation-dependent approximation to −π.
  // 12. If y<0 and x is +0, the result is an implementation-dependent approximation to −π/2.
  // 13. If y<0 and x is −0, the result is an implementation-dependent approximation to −π/2.
  // 14. If y>0 and y is finite and x is +∞, the result is +0.
  // 15. If y>0 and y is finite and x is −∞, the result if an implementation-dependent approximation to +π.
  // 16. If y<0 and y is finite and x is +∞, the result is −0.
  // 17. If y<0 and y is finite and x is −∞, the result is an implementation-dependent approximation to −π.
  // 18. If y is +∞ and x is finite, the result is an implementation-dependent approximation to +π/2.
  // 19. If y is −∞ and x is finite, the result is an implementation-dependent approximation to −π/2.
  // 20. If y is +∞ and x is +∞, the result is an implementation-dependent approximation to +π/4.
  // 21. If y is +∞ and x is −∞, the result is an implementation-dependent approximation to +3π/4.
  // 22. If y is −∞ and x is +∞, the result is an implementation-dependent approximation to −π/4.
  // 23. If y is −∞ and x is −∞, the result is an implementation-dependent approximation to −3π/4.
  
  types::Number num_y = JSValue::ToNumber(vm, y);
  types::Number num_x = JSValue::ToNumber(vm, x);

  if (num_y.GetNumber() == 0 && num_x.GetNumber() > 0) {
    return num_y;
  } else if (num_y.IsFinite() && num_x == types::Number::Inf()) {
    return num_y.GetNumber() > 0 ? types::Number{0.0} : types::Number{-0.0};
  } else if (!num_y.IsNaN() && !num_x.IsNaN()) {
    return types::Number{std::atan2(num_y.GetNumber(), num_x.GetNumber())};
  } else {
    return types::Number::NaN();
  }
}

// Math.ceil(x)
// Defined in ECMAScript 5.1 Chapter 15.8.2.6
JSValue JSMath::Ceil(RuntimeCallInfo* argv) {
  VM* vm = argv->GetVM();
  JSHandleScope handle_scope{vm};
  JSHandle<JSValue> x = argv->GetArg(0);

  // 1. If x is NaN, the result is NaN.
  // 2. If x is +0, the result is +0.
  // 3. If x is −0, the result is −0.
  // 4. If x is +∞, the result is +∞.
  // 5. If x is −∞, the result is −∞.
  // 6. If x is less than 0 but greater than -1, the result is −0.
  types::Number num = JSValue::ToNumber(vm, x);

  if (num.IsNaN() || num.IsInf()) {
    return num;
  } else {
    return types::Number{std::ceil(num.GetNumber())};
  }
}

// Math.cos(x)
// Defined in ECMAScript 5.1 Chapter 15.8.2.7
JSValue JSMath::Cos(RuntimeCallInfo* argv) {
  VM* vm = argv->GetVM();
  JSHandleScope handle_scope{vm};
  JSHandle<JSValue> x = argv->GetArg(0);
  
  // 1. If x is NaN, the result is NaN.
  // 2. If x is +0, the result is 1.
  // 3. If x is −0, the result is 1.
  // 4. If x is +∞, the result is NaN.
  // 5. If x is −∞, the result is NaN.
  types::Number num = JSValue::ToNumber(vm, x);

  if (num.IsNaN() || num.IsInf()) {
    return types::Number::NaN();
  } else {
    return types::Number{std::cos(num.GetNumber())};
  }
}

// Math.exp(x)
// Defined in ECMAScript 5.1 Chapter 15.8.2.8
JSValue JSMath::Exp(RuntimeCallInfo* argv) {
  VM* vm = argv->GetVM();
  JSHandleScope handle_scope{vm};
  JSHandle<JSValue> x = argv->GetArg(0);

  // 1. If x is NaN, the result is NaN.
  // 2. If x is +0, the result is 1.
  // 3. If x is −0, the result is 1.
  // 4. If x is +∞, the result is +∞.
  // 5. If x is −∞, the result is +0.
  types::Number num = JSValue::ToNumber(vm, x);

  if (num.IsNaN()) {
    return num;
  } else {
    return types::Number{std::exp(num.GetNumber())};
  }
}

// Math.floor(x)
// Defined in ECMAScript 5.1 Chapter 15.8.2.9
JSValue JSMath::Floor(RuntimeCallInfo* argv) {
  VM* vm = argv->GetVM();
  JSHandleScope handle_scope{vm};
  JSHandle<JSValue> x = argv->GetArg(0);

  // 1. If x is NaN, the result is NaN.
  // 2. If x is +0, the result is +0.
  // 3. If x is −0, the result is −0.
  // 4. If x is +∞, the result is +∞.
  // 5. If x is −∞, the result is −∞.
  // 6. If x is greater than 0 but less than 1, the result is +0.
  types::Number num = JSValue::ToNumber(vm, x);

  if (num.IsNaN() || num.IsInf()) {
    return num;
  } else {
    return types::Number{std::floor(num.GetNumber())};
  }
}

// Math.log(x)
// Defined in ECMAScript 5.1 Chapter 15.8.2.10
JSValue JSMath::Log(RuntimeCallInfo* argv) {
  VM* vm = argv->GetVM();
  JSHandleScope handle_scope{vm};
  JSHandle<JSValue> x = argv->GetArg(0);

  // 1. Returns an implementation-dependent approximation to the natural logarithm of x.
  // 2. If x is NaN, the result is NaN.
  // 3. If x is less than 0, the result is NaN.
  // 4. If x is +0 or −0, the result is −∞.
  // 5. If x is 1, the result is +0.
  // 6. If x is +∞, the result is +∞.
  types::Number num = JSValue::ToNumber(vm, x);

  if (num.IsNaN() || num.GetNumber() < 0) {
    return types::Number::NaN();
  } else {
    return types::Number{std::log(num.GetNumber())};
  }
}

// Math.max([value1 [, value2 [, ...]]])
// Defined in ECMAScript 5.1 Chapter 15.8.2.11
JSValue JSMath::Max(RuntimeCallInfo* argv) {
  VM* vm = argv->GetVM();
  JSHandleScope handle_scope{vm};
  JSHandle<JSValue> x = argv->GetArg(0);
  std::size_t args_num = argv->GetArgsNum();
  
  // 1. If no arguments are given, the result is −∞.
  // 2. If any value is NaN, the result is NaN.
  // 3. The comparison of values to determine the largest value is done as in 11.8.5 except that
  //    +0 is considered to be larger than −0.
  types::Number result = types::Number::NegativeInf();
  for (std::size_t idx = 0; idx < args_num; ++idx) {
    types::Number num = JSValue::ToNumber(vm, argv->GetArg(idx));
    if (num.IsNaN()) {
      return num;
    }
    if (num.GetNumber() > result.GetNumber()) {
      result = num;
    } else if (num.GetNumber() == 0 && result.GetNumber() == 0) {
      result = types::Number{0};
    }
  }

  return result;
}

// Math.max([value1 [, value2 [, ...]]])
// Defined in ECMAScript 5.1 Chapter 15.8.2.12
JSValue JSMath::Min(RuntimeCallInfo* argv) {
  VM* vm = argv->GetVM();
  JSHandleScope handle_scope{vm};
  JSHandle<JSValue> x = argv->GetArg(0);
  std::size_t args_num = argv->GetArgsNum();

  // 1. If no arguments are given, the result is +∞.
  // 2. If any value is NaN, the result is NaN.
  // 3. The comparison of values to determine the smallest value is done as in 11.8.5 except that
  //    +0 is considered to be larger than −0.
  types::Number result = types::Number::Inf();
  for (std::size_t idx = 0; idx < args_num; ++idx) {
    types::Number num = JSValue::ToNumber(vm, argv->GetArg(idx));
    if (num.IsNaN()) {
      return num;
    }
    if (num.GetNumber() < result.GetNumber()) {
      result = num;
    } else if (num.IsNegativeZero() && result.GetNumber() == 0) {
      result = num;
    }
  }

  return result;
}

// Math.pow(x, y)
// Defined in ECMAScript 5.1 Chapter 15.8.2.13
JSValue JSMath::Pow(RuntimeCallInfo* argv) {
  VM* vm = argv->GetVM();
  JSHandleScope handle_scope{vm};
  JSHandle<JSValue> x = argv->GetArg(0);
  JSHandle<JSValue> y = argv->GetArg(1);
  
  // 1. If y is NaN, the result is NaN.
  // 2. If y is +0, the result is 1, even if x is NaN.
  // 3. If y is −0, the result is 1, even if x is NaN.
  // 4. If x is NaN and y is nonzero, the result is NaN.
  // 5. If abs(x)>1 and y is +∞, the result is +∞.
  // 6. If abs(x)>1 and y is −∞, the result is +0.
  // 7. If abs(x)==1 and y is +∞, the result is NaN.
  // 8. If abs(x)==1 and y is −∞, the result is NaN.
  // 9. If abs(x)<1 and y is +∞, the result is +0.
  // 10. If abs(x)<1 and y is −∞, the result is +∞.
  // 11. If x is +∞ and y>0, the result is +∞.
  // 12. If x is +∞ and y<0, the result is +0.
  // 13. If x is −∞ and y>0 and y is an odd integer, the result is −∞.
  // 14. If x is −∞ and y>0 and y is not an odd integer, the result is +∞.
  // 15. If x is −∞ and y<0 and y is an odd integer, the result is −0.
  // 16. If x is −∞ and y<0 and y is not an odd integer, the result is +0.
  // 17. If x is +0 and y>0, the result is +0.
  // 18. If x is +0 and y<0, the result is +∞.
  // 19. If x is −0 and y>0 and y is an odd integer, the result is −0.
  // 20. If x is −0 and y>0 and y is not an odd integer, the result is +0.
  // 21. If x is −0 and y<0 and y is an odd integer, the result is −∞.
  // 22. If x is −0 and y<0 and y is not an odd integer, the result is +∞.
  // 23. If x<0 and x is finite and y is finite and y is not an integer, the result is NaN.
  types::Number num_x = JSValue::ToNumber(vm, x);
  types::Number num_y = JSValue::ToNumber(vm, y);

  if (std::abs(num_x.GetNumber()) == 1 && num_y.IsInf()) {
    return types::Number::NaN();
  }

  types::Number result = types::Number{std::pow(num_x.GetNumber(), num_y.GetNumber())};
  if (result.IsNaN()) {
    return types::Number::NaN();
  }
  return result;
}

// Math.random()
// Defined in ECMAScript 5.1 Chapter 15.8.2.14
JSValue JSMath::Random(RuntimeCallInfo* argv) {
  VM* vm = argv->GetVM();
  JSHandleScope handle_scope{vm};

  std::ranlux48 engine{std::random_device{}()};
  std::uniform_real_distribution<double> distrib{0, 1.0};
  return types::Number{distrib(engine)};
}

// Math.round(x)
// Defined in ECMAScript 5.1 Chapter 15.8.2.15
JSValue JSMath::Round(RuntimeCallInfo* argv) {
  VM* vm = argv->GetVM();
  JSHandleScope handle_scope{vm};
  JSHandle<JSValue> x = argv->GetArg(0);

  // 1. If x is NaN, the result is NaN.
  // 2. If x is +0, the result is +0.
  // 3. If x is −0, the result is −0.
  // 4. If x is +∞, the result is +∞.
  // 5. If x is −∞, the result is −∞.
  // 6. If x is greater than 0 but less than 0.5, the result is +0.
  // 7. If x is less than 0 but greater than or equal to -0.5, the result is −0.
  types::Number num = JSValue::ToNumber(vm, x);
  
  if (num.IsNaN() || num.IsInf() || num.GetNumber() == 0) {
    return num;
  }

  if (num.GetNumber() < 0 && num.GetNumber() >= -0.5) {
    return types::Number{-0.0};
  }

  if (num.GetNumber() > 0 && num.GetNumber() < 0.5) {
    return types::Number{0.0};
  }

  double result = std::ceil(num.GetNumber());
  if (result - num.GetNumber() > 0.5) {
    result -= 1; 
  }
  return types::Number{result};
}

// Math.sin(x)
// Defined in ECMAScript 5.1 Chapter 15.8.2.16
JSValue JSMath::Sin(RuntimeCallInfo* argv) {
  VM* vm = argv->GetVM();
  JSHandleScope handle_scope{vm};
  JSHandle<JSValue> x = argv->GetArg(0);

  // 1. If x is NaN, the result is NaN.
  // 2. If x is +0, the result is +0.
  // 3. If x is −0, the result is −0.
  // 4. If x is +∞ or −∞, the result is NaN.
  types::Number num = JSValue::ToNumber(vm, x);

  if (num.IsNaN() || num.IsInf()) {
    return types::Number::NaN();
  }

  return types::Number{std::sin(num.GetNumber())};
}

// Math.sqrt(x)
// Defined in ECMAScript 5.1 Chapter
JSValue JSMath::Sqrt(RuntimeCallInfo *argv) {
  VM* vm = argv->GetVM();
  JSHandleScope handle_scope{vm};
  JSHandle<JSValue> x = argv->GetArg(0);

  // 1. If x is NaN, the result is NaN.
  // 2. If x is less than 0, the result is NaN.
  // 3. If x is +0, the result is +0.
  // 4. If x is −0, the result is −0.
  // 5. If x is +∞, the result is +∞.
  types::Number num = JSValue::ToNumber(vm, x);

  if (num.IsNaN() || num.GetNumber() < 0) {
    return types::Number::NaN();
  }

  return types::Number{std::sqrt(num.GetNumber())};
}

// Math.tan(x)
// Defined in ECMAScript 5.1 Chapter
JSValue JSMath::Tan(RuntimeCallInfo *argv) {
  VM* vm = argv->GetVM();
  JSHandleScope handle_scope{vm};
  JSHandle<JSValue> x = argv->GetArg(0);

  // 1. If x is NaN, the result is NaN.
  // 2. If x is +0, the result is +0.
  // 3. If x is −0, the result is −0.
  // 4. If x is +∞ or −∞, the result is NaN.
  types::Number num = JSValue::ToNumber(vm, x);

  if (num.IsNaN() || num.IsInf()) {
    return types::Number::NaN();
  }

  return types::Number{std::tan(num.GetNumber())};
}

}  // namespace builtins
}  // namespace voidjs
