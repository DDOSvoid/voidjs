#include "voidjs/builtins/global_object.h"

#include <iostream>

#include "voidjs/types/heap_object.h"
#include "voidjs/types/js_value.h"
#include "voidjs/types/lang_types/number.h"
#include "voidjs/types/lang_types/string.h"
#include "voidjs/interpreter/runtime_call_info.h"
#include "voidjs/utils/helper.h"

namespace voidjs {
namespace builtins {

// isNaN(number)
// Defined in ECMAScript 5.1 Chapter 15.1.2.4
JSValue GlobalObject::IsNaN(RuntimeCallInfo* argv) {
  VM* vm = argv->GetVM();
  JSHandleScope handle_scope{vm};
  JSHandle<JSValue> number = argv->GetArg(0);

  // 1. If ToNumber(number) is NaN, return true.
  if (JSValue::ToNumber(vm, number).IsNaN()) {
    return JSValue::True();
  } 
  // 2. Otherwise, return false.
  else {
    return JSValue::False();
  }
}

// isFinite(number)
// Defined in ECMAScript 5.1 Chapter 15.1.2.5
JSValue GlobalObject::IsFinite(RuntimeCallInfo* argv) {
  VM* vm = argv->GetVM();
  JSHandleScope handle_scope{vm};
  JSHandle<JSValue> number = argv->GetArg(0);

  // 1. If ToNumber(number) is NaN, +∞, or −∞, return false.
  if (JSValue::ToNumber(vm, number).IsFinite()) {
    return JSValue::True();
  }
  // 2. Otherwise, return true.
  else {
    return JSValue::False();
  }
}

JSValue GlobalObject::Print(RuntimeCallInfo* argv) {
  VM* vm = argv->GetVM();
  JSHandleScope handle_scope{vm};
  std::size_t args_num = argv->GetArgsNum();

  std::string output_string;
  for (std::size_t idx = 0; idx < args_num; ++idx) {
    JSHandle<JSValue> value = argv->GetArg(idx);
    JSHandle<types::String> string = JSValue::ToString(vm, value);
    output_string += utils::U16StrToU8Str(std::u16string{string->GetString()});
    if (idx != 0) {
      output_string.push_back(' ');
    }
  }

  std::cout << output_string << std::endl;

  return JSValue::Undefined();
}

}  // namespace voidjs
}  // namespace builtins
