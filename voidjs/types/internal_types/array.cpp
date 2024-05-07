#include "voidjs/types/internal_types/array.h"

#include "voidjs/types/heap_object.h"
#include "voidjs/types/js_value.h"
#include "voidjs/types/object_factory.h"
#include "voidjs/interpreter/vm.h"

namespace voidjs {
namespace types {

JSHandle<Array> Array::Append(VM* vm, JSHandle<Array> first, JSHandle<Array> second) {
  // assert(val.IsHeapObject() && val.GetHeapObject()->IsArray();
  auto first_len = first->GetLength();
  auto second_len = second->GetLength();
  auto new_len = first_len + second_len;
  auto new_arr = vm->GetObjectFactory()->NewArray(new_len);
  new_arr->SetLength(new_len);
  std::copy_n(first->GetData(), first_len, new_arr->GetData());
  std::copy_n(second->GetData(), second_len, new_arr->GetData() + first_len);
  return new_arr;
}

}  // namespace types
}  // namespace voidjs
