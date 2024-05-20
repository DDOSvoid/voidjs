#include "voidjs/builtins/global_object.h"

#include <iostream>

#include "voidjs/types/heap_object.h"
#include "voidjs/types/js_value.h"
#include "voidjs/types/lang_types/string.h"
#include "voidjs/interpreter/runtime_call_info.h"
#include "voidjs/utils/helper.h"

namespace voidjs {
namespace builtins {

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