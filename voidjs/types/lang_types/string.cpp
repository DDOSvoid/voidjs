#include "voidjs/types/lang_types/string.h"

#include "voidjs/types/object_factory.h"

namespace voidjs {
namespace types {

JSHandle<String> String::Concat(VM* vm, JSHandle<String> str1, JSHandle<String> str2) {
  std::u16string str;
  str += str1->GetString();
  str += str2->GetString();
  return vm->GetObjectFactory()->NewString(str);
}

JSHandle<String> String::Concat(VM* vm, JSHandle<String> str1, JSHandle<String> str2, JSHandle<String> str3) {
  std::u16string str;
  str += str1->GetString();
  str += str2->GetString();
  str += str3->GetString();
  return vm->GetObjectFactory()->NewString(str);
}

JSHandle<String> String::Substring(VM* vm, JSHandle<String> string, std::size_t start, std::size_t length) {
  return vm->GetObjectFactory()->NewString(string->GetString().substr(start, length));
}

JSHandle<String> String::CharAt(VM* vm, JSHandle<String> string, std::size_t pos) {
  return vm->GetObjectFactory()->NewString(string->GetString().substr(pos, 1));
}


}  // namespace types
}  // namespace voidjs
