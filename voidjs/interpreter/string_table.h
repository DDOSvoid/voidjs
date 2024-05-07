#ifndef VOIDJS_INTERPRETER_STRING_TABLE_H
#define VOIDJS_INTERPRETER_STRING_TABLE_H

#include <map>

#include "voidjs/types/object_factory.h"

namespace voidjs {

namespace types {

class String;

}  // namespace types

class VM;
template <typename T>
class JSHandle;

class StringTable {
 public:
  StringTable(VM* vm)
    : vm_(vm) {}
    
  JSHandle<types::String> GetOrInsert(std::u16string_view str_view);
  
 private:
  VM* vm_;
  std::map<std::u16string, types::String*> map_;
};

}  // namespace voidjs

#endif  // VOIDJS_INTERPRETER_STRING_TABLE_H
