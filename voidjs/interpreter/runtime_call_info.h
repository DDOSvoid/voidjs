#ifndef VOIDJS_INTERPRETER_RUNTIME_CALL_INFO
#define VOIDJS_INTERPRETER_RUNTIME_CALL_INFO

#include "voidjs/types/js_value.h"
#include "voidjs/utils/helper.h"

namespace voidjs {

class RuntimeCallInfo {
 public:
  static constexpr std::size_t VM_OFFSET = 0;
  VM* GetVM() const { return utils::BitGet<VM*>(this, VM_OFFSET); }
  void SetVM(VM* vm) { *utils::BitGet<VM**>(this, VM_OFFSET) = vm; }
  
  static constexpr std::size_t ARGS_NUM_OFFSET = 0;
  std::uint64_t GetArgsNum() const { return *utils::BitGet<std::uint64_t*>(this, ARGS_NUM_OFFSET); }
  void SetArgsNum(std::uint64_t num) { *utils::BitGet<std::uint64_t*>(this, ARGS_NUM_OFFSET) = num; }

  static constexpr std::size_t ARGS_OFFSET = ARGS_NUM_OFFSET + sizeof(std::uint64_t);
  JSValue* GetArgs() const { return utils::BitGet<JSValue*>(this, ARGS_OFFSET); }


  static constexpr std::size_t THIS_INDEX = 0;
  static constexpr std::size_t ARGS_START_INDEX = 1;

  JSValue GetThis() const { return Get(THIS_INDEX); }
  JSValue GetArg(std::size_t idx) const {
    return idx + ARGS_START_INDEX >= GetArgsNum() ?
      JSValue::Undefined() : Get(idx + ARGS_START_INDEX); 
  }
  void SetArg(std::size_t idx, JSValue val) { Set(idx + ARGS_START_INDEX, val); }

 private:
  JSValue Get(std::size_t idx) const { return *(GetArgs() + idx); }
  void Set(std::size_t idx, JSValue val) { *(GetArgs() + idx) = val; }
};

using InternalFunctionType = JSValue (*) (RuntimeCallInfo*);

}  // namespace voidjs

#endif  // VOIDJS_INTERPRETER_RUNTIME_CALL_INFO
