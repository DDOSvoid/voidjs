#ifndef VOIDJS_BUILTINS_ARGUMENTS
#define VOIDJS_BUILTINS_ARGUMENTS

#include "voidjs/types/lang_types/object.h"

namespace voidjs {
namespace builtins {

class Arguments : public types::Object {
 public:
  // Object*
  static constexpr std::size_t PARAMETER_MAP_OFFSET = types::Object::END_OFFSET;
  JSValue GetParameterMap() const { return *utils::BitGet<JSValue*>(this, PARAMETER_MAP_OFFSET); }
  void SetParameterMap(JSValue value) { *utils::BitGet<JSValue*>(this, PARAMETER_MAP_OFFSET) = value; }
  void SetParameterMap(JSHandle<JSValue> handle) { *utils::BitGet<JSValue*>(this, PARAMETER_MAP_OFFSET) = handle.GetJSValue(); }
  
  static constexpr std::size_t SIZE = sizeof(JSValue);
  static constexpr std::size_t END_OFFSET = types::Object::END_OFFSET + SIZE;
};

}  // namespace builtins
}  // namespace voidjs

#endif  // VOIDJS_BUILTINS_ARGUMENTS
