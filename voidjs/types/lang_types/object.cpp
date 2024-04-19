#include "voidjs/types/lang_types/object.h"
#include "voidjs/types/js_value.h"

namespace voidjs {
namespace types {

std::optional<PropertyDescriptor> Object::GetOwnProperty(JSValue P) const {
  // assert(P.IsPropertyName());
  
  // 1. If O doesn’t have an own property with name P, return undefined.
  // todo
  if (false) {
    return {};
  }

  // 2. Let D be a newly created Property Descriptor with no fields.
  PropertyDescriptor D;

  // 3. Let X be O’s own property named P.
  
  
}

}  // namespace types
}  // namespace voidjs
