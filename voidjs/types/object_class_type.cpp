#include "voidjs/types/object_class_type.h"

#include "voidjs/types/lang_types/string.h"
#include "voidjs/interpreter/vm.h"
#include "voidjs/interpreter/string_table.h"

namespace voidjs {

inline types::String* ObjectClassTypeToString(VM* vm, ObjectClassType type) {
  auto factory = vm->GetObjectFactory();
  
  switch (type) {
    case ObjectClassType::GLOBAL_OBJECT: {
      return factory->NewStringFromTable(u"GlobalObject");
    }
    case ObjectClassType::ARGUMENTS: {
      return factory->NewStringFromTable(u"Arguments");
    }
    case ObjectClassType::ARRAY: {
      return factory->NewStringFromTable(u"Array");
    }
    case ObjectClassType::BOOLEAN: {
      return factory->NewStringFromTable(u"Boolean");
    }
    case ObjectClassType::DATE: {
      return factory->NewStringFromTable(u"Date");
    }
    case ObjectClassType::ERROR: {
      return factory->NewStringFromTable(u"Error");
    }
    case ObjectClassType::FUNCTION: {
      return factory->NewStringFromTable(u"Function");
    }
    case ObjectClassType::JSON: {
      return factory->NewStringFromTable(u"JSON");
    }
    case ObjectClassType::MATH: {
      return factory->NewStringFromTable(u"Math");
    }
    case ObjectClassType::NUMBER: {
      return factory->NewStringFromTable(u"Number");
    }
    case ObjectClassType::OBJECT: {
      return factory->NewStringFromTable(u"Object");
    }
    case ObjectClassType::REG_EXP: {
      return factory->NewStringFromTable(u"RegExp");
    }
    case ObjectClassType::STRING: {
      return factory->NewStringFromTable(u"String");
    }
  }
}

}  // namespace voidjs
