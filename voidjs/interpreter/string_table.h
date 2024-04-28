#ifndef VOIDJS_INTERPRETER_STRING_TABLE_H
#define VOIDJS_INTERPRETER_STRING_TABLE_H

#include <map>

#include "voidjs/types/object_factory.h"

namespace voidjs {

namespace types {

class String;

}  // namespace types

class StringTable {
 public:
  types::String* GetOrInsert(std::u16string_view source) {
    if (map_.find(source) == map_.end()) {
      auto str = factory_->NewString(source);
      map_.emplace(source, str);
      return str;
    } else {
      return map_[source];
    }
  }
  
 private:
  std::map<std::u16string_view, types::String*> map_;
  ObjectFactory* factory_;
};

}  // namespace voidjs

#endif  // VOIDJS_INTERPRETER_STRING_TABLE_H
