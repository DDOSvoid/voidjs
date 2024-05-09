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

}
}
