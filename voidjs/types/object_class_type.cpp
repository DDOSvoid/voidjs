#include "voidjs/types/object_class_type.h"

#include "voidjs/types/lang_types/string.h"

namespace voidjs {

inline types::String* ObjectClassTypeToString(ObjectClassType type) {
  switch (type) {
    case ObjectClassType::GLOBAL_OBJECT: {
      return ObjectFactory::NewString(u"GlobalObject");
    }
    case ObjectClassType::ARGUMENTS: {
      return ObjectFactory::NewString(u"Arguments");
    }
    case ObjectClassType::ARRAY: {
      return ObjectFactory::NewString(u"Array");
    }
    case ObjectClassType::BOOLEAN: {
      return ObjectFactory::NewString(u"Boolean");
    }
    case ObjectClassType::DATE: {
      return ObjectFactory::NewString(u"Date");
    }
    case ObjectClassType::ERROR: {
      return ObjectFactory::NewString(u"Error");
    }
    case ObjectClassType::FUNCTION: {
      return ObjectFactory::NewString(u"Function");
    }
    case ObjectClassType::JSON: {
      return ObjectFactory::NewString(u"JSON");
    }
    case ObjectClassType::MATH: {
      return ObjectFactory::NewString(u"Math");
    }
    case ObjectClassType::NUMBER: {
      return ObjectFactory::NewString(u"Number");
    }
    case ObjectClassType::OBJECT: {
      return ObjectFactory::NewString(u"Object");
    }
    case ObjectClassType::REG_EXP: {
      return ObjectFactory::NewString(u"RegExp");
    }
    case ObjectClassType::STRING: {
      return ObjectFactory::NewString(u"String");
    }
  }
}

}  // namespace voidjs
