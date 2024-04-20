#include "voidjs/types/internal_types/array.h"
#include "voidjs/types/heap_object.h"
#include "voidjs/types/js_value.h"


namespace voidjs {
namespace types {

JSValue Array::Append(JSValue first, JSValue second) {
  // assert(val.IsHeapObject() && val.GetHeapObject()->IsArray();
  auto first_arr = first.GetHeapObject()->AsArray();
  auto second_arr = second.GetHeapObject()->AsArray();
  auto first_len = first_arr->GetLength();
  auto second_len = second_arr->GetLength();
  auto new_len = first_len + second_len;
  auto new_arr = New(new_len);
  new_arr->SetLength(new_len);
  std::copy_n(first_arr->GetData(), first_len, new_arr->GetData());
  std::copy_n(second_arr->GetData(), second_len, new_arr->GetData() + first_len);
  return JSValue(new_arr);
}

}  // namespace types
}  // namespace voidjs
