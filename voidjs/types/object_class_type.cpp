#include "voidjs/types/object_class_type.h"

#include "voidjs/types/lang_types/string.h"
#include "voidjs/gc/js_handle.h"
#include "voidjs/interpreter/vm.h"
#include "voidjs/interpreter/string_table.h"

namespace voidjs {

inline JSHandle<types::String> ObjectClassTypeToString(VM* vm, ObjectClassType type) {
  ObjectFactory* factory = vm->GetObjectFactory();
  
  switch (type) {
    case ObjectClassType::GLOBAL_OBJECT: {
      return factory->GetStringFromTable(u"GlobalObject");
    }
    case ObjectClassType::ARGUMENTS: {
      return factory->GetStringFromTable(u"Arguments");
    }
    case ObjectClassType::ARRAY: {
      return factory->GetStringFromTable(u"Array");
    }
    case ObjectClassType::BOOLEAN: {
      return factory->GetStringFromTable(u"Boolean");
    }
    case ObjectClassType::DATE: {
      return factory->GetStringFromTable(u"Date");
    }
    case ObjectClassType::ERROR: {
      return factory->GetStringFromTable(u"Error");
    }
    case ObjectClassType::FUNCTION: {
      return factory->GetStringFromTable(u"Function");
    }
    case ObjectClassType::JSON: {
      return factory->GetStringFromTable(u"JSON");
    }
    case ObjectClassType::MATH: {
      return factory->GetStringFromTable(u"Math");
    }
    case ObjectClassType::NUMBER: {
      return factory->GetStringFromTable(u"Number");
    }
    case ObjectClassType::OBJECT: {
      return factory->GetStringFromTable(u"Object");
    }
    case ObjectClassType::REG_EXP: {
      return factory->GetStringFromTable(u"RegExp");
    }
    case ObjectClassType::STRING: {
      return factory->GetStringFromTable(u"String");
    }
  }
}

}  // namespace voidjs
