#ifndef VOIDJS_BUILTINS_GLOBAL_OBJECT_H
#define VOIDJS_BUILTINS_GLOBAL_OBJECT_H

#include "voidjs/types/lang_types/object.h"

namespace voidjs {
namespace builtins {

class GlobalObject : public types::Object {
 public:
  static constexpr std::size_t SIZE = 0;
  static constexpr std::size_t END_OFFSET = types::Object::END_OFFSET + SIZE;
  static_assert(SIZE == 0);
};

}  // namespace builtins
}  // namespace voidjs

#endif  // VOIDJS_BUILTINS_GLOBAL_OBJECT_H
