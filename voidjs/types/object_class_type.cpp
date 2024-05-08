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
      return factory->NewString(u"GlobalObject");
    }
    case ObjectClassType::ARGUMENTS: {
      return factory->NewString(u"Arguments");
    }
    case ObjectClassType::ARRAY: {
      return factory->NewString(u"Array");
    }
    case ObjectClassType::BOOLEAN: {
      return factory->NewString(u"Boolean");
    }
    case ObjectClassType::DATE: {
      return factory->NewString(u"Date");
    }
    case ObjectClassType::ERROR: {
      return factory->NewString(u"Error");
    }
    case ObjectClassType::FUNCTION: {
      return factory->NewString(u"Function");
    }
    case ObjectClassType::JSON: {
      return factory->NewString(u"JSON");
    }
    case ObjectClassType::MATH: {
      return factory->NewString(u"Math");
    }
    case ObjectClassType::NUMBER: {
      return factory->NewString(u"Number");
    }
    case ObjectClassType::OBJECT: {
      return factory->NewString(u"Object");
    }
    case ObjectClassType::REG_EXP: {
      return factory->NewString(u"RegExp");
    }
    case ObjectClassType::STRING: {
      return factory->NewString(u"String");
    }
  }
}

}  // namespace voidjs
