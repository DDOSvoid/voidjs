#include "voidjs/types/js_value.h"
#include "voidjs/types/lang_types/object.h"

namespace voidjs {

bool JSValue::IsString() const {
  return IsObject() && GetObject()->IsString();
}


}  // namespace voidjs
