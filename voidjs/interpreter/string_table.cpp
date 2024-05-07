#include "voidjs/interpreter/string_table.h"

#include "voidjs/types/lang_types/string.h"
#include "voidjs/gc/js_handle.h"
#include "voidjs/interpreter/vm.h"

namespace voidjs {

JSHandle<types::String> StringTable::GetOrInsert(std::u16string_view str_view) {
  std::u16string source{str_view};
  if (map_.find(source) == map_.end()) {
    auto str = vm_->GetObjectFactory()->NewString(source);
    map_.emplace(source, str.GetObject());
    return str;
  } else {
    return JSHandle<types::String>{vm_, map_[source]};
  }
}

}  // namespace voidjs
