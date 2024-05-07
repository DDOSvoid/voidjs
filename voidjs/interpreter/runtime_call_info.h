#ifndef VOIDJS_INTERPRETER_RUNTIME_CALL_INFO
#define VOIDJS_INTERPRETER_RUNTIME_CALL_INFO

#include "voidjs/types/js_value.h"
#include "voidjs/gc/js_handle.h"
#include "voidjs/utils/helper.h"

namespace voidjs {

class RuntimeCallInfo {
 public:
  static constexpr std::size_t VM_OFFSET = 0;
  VM* GetVM() const { return *utils::BitGet<VM**>(this, VM_OFFSET); }
  void SetVM(VM* vm) { *utils::BitGet<VM**>(this, VM_OFFSET) = vm; }

  static constexpr std::size_t THIS_OFFSET = VM_OFFSET + sizeof(std::uintptr_t);
  JSHandle<JSValue> GetThis() const { return JSHandle<JSValue>{utils::BitGet<uintptr_t>(this, THIS_OFFSET)}; }
  void SetThis(JSValue value) { *utils::BitGet<JSValue*>(this, THIS_OFFSET) = value; }
  void SetThis(JSHandle<JSValue> handle) { *utils::BitGet<JSValue*>(this, THIS_OFFSET) = handle.GetJSValue(); }
  
  static constexpr std::size_t ARGS_NUM_OFFSET = THIS_OFFSET + sizeof(JSValue);
  std::uint64_t GetArgsNum() const { return *utils::BitGet<std::uint64_t*>(this, ARGS_NUM_OFFSET); }
  void SetArgsNum(std::uint64_t num) { *utils::BitGet<std::uint64_t*>(this, ARGS_NUM_OFFSET) = num; }

  static constexpr std::size_t ARGS_OFFSET = ARGS_NUM_OFFSET + sizeof(std::uint64_t);
  JSValue* GetArgs() const { return utils::BitGet<JSValue*>(this, ARGS_OFFSET); }

  static constexpr std::size_t SIZE = sizeof(std::uintptr_t) + sizeof(JSValue) + sizeof(std::uint64_t);

  JSHandle<JSValue> GetArg(std::size_t idx) const {
    if (idx >= GetArgsNum()) {
      return JSHandle<JSValue>{GetVM(), JSValue::Undefined()};
    } else {
      return JSHandle<JSValue>{reinterpret_cast<std::uintptr_t>(GetArgs() + idx)};
    }
  }
  void SetArg(std::size_t idx, JSValue value) { Set(idx, value); }
  void SetArg(std::size_t idx, JSHandle<JSValue> handle) { Set(idx, handle.GetJSValue()); }

  static RuntimeCallInfo* New(VM* vm, JSHandle<JSValue> this_arg, const std::vector<JSHandle<JSValue>>& args) {
    std::size_t len = args.size();
    RuntimeCallInfo* info =
      reinterpret_cast<RuntimeCallInfo*>(new std::byte[RuntimeCallInfo::SIZE + len * sizeof(JSValue)]);
    info->SetVM(vm);
    info->SetThis(this_arg);
    info->SetArgsNum(len);
    for (std::size_t idx = 0; idx < len; ++idx) {
      info->SetArg(idx, args[idx]);
    }
    return info;
  }

  static void Delete(RuntimeCallInfo* info) {
    delete[] reinterpret_cast<std::byte*>(info);
  }

 private:
  void Set(std::size_t idx, JSValue val) { *(GetArgs() + idx) = val; }
};

using InternalFunctionType = JSValue (*) (RuntimeCallInfo*);

}  // namespace voidjs

#endif  // VOIDJS_INTERPRETER_RUNTIME_CALL_INFO
