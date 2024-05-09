#include "voidjs/types/heap_object.h"

#include "voidjs/types/internal_types/binding.h"
#include "voidjs/types/internal_types/hash_map.h"
#include "voidjs/types/internal_types/internal_function.h"
#include "voidjs/types/js_type.h"
#include "voidjs/types/js_value.h"
#include "voidjs/types/lang_types/object.h"
#include "voidjs/types/lang_types/string.h"
#include "voidjs/types/internal_types/array.h"
#include "voidjs/types/internal_types/property_map.h"
#include "voidjs/builtins/js_object.h"
#include "voidjs/builtins/js_function.h"
#include "voidjs/builtins/js_array.h"
#include "voidjs/builtins/global_object.h"
#include "voidjs/builtins/js_boolean.h"
#include "voidjs/builtins/js_error.h"
#include "voidjs/builtins/js_number.h"
#include "voidjs/builtins/js_string.h"
#include "voidjs/gc/js_handle.h"
#include "voidjs/types/spec_types/environment_record.h"
#include "voidjs/types/spec_types/lexical_environment.h"
#include "voidjs/types/spec_types/property_descriptor.h"

namespace voidjs {

std::size_t HeapObject::GetSize(JSValue value) {
  if (!value.IsHeapObject()) {
    return sizeof(JSValue);
  }

  switch (value.GetHeapObject()->GetType()) {
    case JSType::STRING: {
      types::String* string = value.GetHeapObject()->AsString();
      return string->GetLength() * sizeof(char16_t) + types::String::SIZE + HeapObject::SIZE;
    }
    case JSType::OBJECT: {
      return types::Object::SIZE + HeapObject::SIZE;
    }
    case JSType::ARRAY: {
      types::Array* array = value.GetHeapObject()->AsArray();
      return array->GetLength() * sizeof(JSValue) + types::Array::SIZE + HeapObject::SIZE;
    }
    case JSType::DATA_PROPERTY_DESCRIPTOR: {
      return types::DataPropertyDescriptor::SIZE + HeapObject::SIZE;
    }
    case JSType::ACCESSOR_PROPERTY_DESCRIPTOR: {
      return types::AccessorPropertyDescriptor::SIZE + HeapObject::SIZE;
    }
    case JSType::GENERIC_PROPERTY_DESCRIPTOR: {
      return types::GenericPropertyDescriptor::SIZE + HeapObject::SIZE;
    }
    case JSType::PROPERTY_MAP: {
      types::PropertyMap* prop_map = value.GetHeapObject()->AsPropertyMap();
      return prop_map->GetLength() * sizeof(JSValue) + types::PropertyMap::SIZE + types::HashMap::SIZE + types::Array::SIZE + HeapObject::SIZE;
    }
    case JSType::BINDING: {
      return types::Binding::SIZE + HeapObject::SIZE;
    }
    case JSType::INTERNAL_FUNCTION: {
      return types::InternalFunction::SIZE + HeapObject::SIZE;
    }
    case JSType::HASH_MAP: {
      types::HashMap* hashmap = value.GetHeapObject()->AsHashMap();
      return hashmap->GetLength() * sizeof(JSValue) + types::HashMap::SIZE + types::Array::SIZE + HeapObject::SIZE;
    }
    case JSType::ENVIRONMENT_RECORD: {
      return types::EnvironmentRecord::SIZE + HeapObject::SIZE;
    }
    case JSType::DECLARATIVE_ENVIRONMENT_RECORD: {
      return types::DeclarativeEnvironmentRecord::SIZE + types::EnvironmentRecord::SIZE + HeapObject::SIZE;
    }
    case JSType::OBJECT_ENVIRONMENT_RECORD: {
      return types::ObjectEnvironmentRecord::SIZE + types::EnvironmentRecord::SIZE + HeapObject::SIZE;
    }
    case JSType::LEXICAL_ENVIRONMENT: {
      return types::LexicalEnvironment::SIZE + HeapObject::SIZE;
    }
    case JSType::GLOBAL_OBJECT: {
      return builtins::GlobalObject::SIZE + types::Object::SIZE + HeapObject::SIZE;
    }
    case JSType::JS_OBJECT: {
      return builtins::JSObject::SIZE + types::Object::SIZE + HeapObject::SIZE;
    }
    case JSType::JS_FUNCTION: {
      return builtins::JSFunction::SIZE + types::Object::SIZE + HeapObject::SIZE;
    }
    case JSType::JS_ARRAY: {
      return builtins::JSArray::SIZE + types::Object::SIZE + HeapObject::SIZE;
    }
    case JSType::JS_STRING: {
      return builtins::JSString::SIZE + types::Object::SIZE + HeapObject::SIZE;
    }
    case JSType::JS_BOOLEAN: {
      return builtins::JSBoolean::SIZE + types::Object::SIZE + HeapObject::SIZE;
    }
    case JSType::JS_NUMBER: {
      return builtins::JSNumber::SIZE + types::Object::SIZE + HeapObject::SIZE;
    }
    case JSType::JS_ERROR: {
      return builtins::JSError::SIZE + types::Object::SIZE + HeapObject::SIZE;
    }
  }
}
  
std::size_t HeapObject::GetSize(JSHandle<JSValue> handle) {
  return GetSize(handle.GetJSValue());
}

std::vector<JSHandle<JSValue>> HeapObject::GetValues(JSValue value) {
  if (!value.IsHeapObject()) {
    return {};
  }
  switch (value.GetHeapObject()->GetType()) {
    case JSType::STRING: {
      return {};
    }
    case JSType::OBJECT: {
      return {
        JSHandle<JSValue>{value.GetRawData() + types::Object::PROPERTIES_OFFSET},
        JSHandle<JSValue>{value.GetRawData() + types::Object::PROTOTYPE_OFFSET},
      };
    }
    case JSType::ARRAY: {
      types::Array* array = value.GetHeapObject()->AsArray();
      std::size_t length = array->GetLength();
      std::vector<JSHandle<JSValue>> handles;
      for (std::size_t idx = 0; idx < length; ++idx) {
        handles.emplace_back(value.GetRawData() + types::Array::DATA_OFFSET + idx);
      }
      return handles;
    }
    case JSType::DATA_PROPERTY_DESCRIPTOR: {
      return {JSHandle<JSValue>{value.GetRawData() + types::DataPropertyDescriptor::VALUE_OFFSET}};
    }
    case JSType::ACCESSOR_PROPERTY_DESCRIPTOR: {
      return {
        JSHandle<JSValue>{value.GetRawData() + types::AccessorPropertyDescriptor::GETTER_OFFSET},
        JSHandle<JSValue>{value.GetRawData() + types::AccessorPropertyDescriptor::SETTER_OFFSET},
      };
    }
    case JSType::GENERIC_PROPERTY_DESCRIPTOR: {
      return {};
    }
    case JSType::PROPERTY_MAP: {
      types::Array* array = value.GetHeapObject()->AsArray();
      std::size_t length = array->GetLength();
      std::vector<JSHandle<JSValue>> handles;
      for (std::size_t idx = 0; idx < length; ++idx) {
        handles.emplace_back(value.GetRawData() + types::Array::DATA_OFFSET + idx);
      }
      return handles;
    }
    case JSType::BINDING: {
      return {JSHandle<JSValue>{value.GetRawData() + types::Binding::VALUE_OFFSET}};
    }
    case JSType::INTERNAL_FUNCTION: {
      return {};
    }
    case JSType::HASH_MAP: {
      types::Array* array = value.GetHeapObject()->AsArray();
      std::size_t length = array->GetLength();
      std::vector<JSHandle<JSValue>> handles;
      for (std::size_t idx = 0; idx < length; ++idx) {
        handles.emplace_back(value.GetRawData() + types::Array::DATA_OFFSET + idx);
      }
      return handles;
    }
    case JSType::ENVIRONMENT_RECORD: {
      return {};
    }
    case JSType::DECLARATIVE_ENVIRONMENT_RECORD: {
      return {JSHandle<JSValue>{value.GetRawData() + types::DeclarativeEnvironmentRecord::BINDING_MAP_OFFSET}};
    }
    case JSType::OBJECT_ENVIRONMENT_RECORD: {
      return {JSHandle<JSValue>{value.GetRawData() + types::ObjectEnvironmentRecord::OBJECT_OFFSET}};
    }
    case JSType::LEXICAL_ENVIRONMENT: {
      return {
        JSHandle<JSValue>{value.GetRawData() + types::LexicalEnvironment::ENV_REC_OFFSET},
        JSHandle<JSValue>{value.GetRawData() + types::LexicalEnvironment::OUTER_OFFSET}
      };
    }
    case JSType::GLOBAL_OBJECT: {
      return {
        JSHandle<JSValue>{value.GetRawData() + types::Object::PROPERTIES_OFFSET},
        JSHandle<JSValue>{value.GetRawData() + types::Object::PROTOTYPE_OFFSET}
      };
    }
    case JSType::JS_OBJECT: {
      return {
        JSHandle<JSValue>{value.GetRawData() + types::Object::PROPERTIES_OFFSET},
        JSHandle<JSValue>{value.GetRawData() + types::Object::PROTOTYPE_OFFSET}
      };
    }
    case JSType::JS_FUNCTION: {
      return {
        JSHandle<JSValue>{value.GetRawData() + types::Object::PROPERTIES_OFFSET},
        JSHandle<JSValue>{value.GetRawData() + types::Object::PROTOTYPE_OFFSET},
        JSHandle<JSValue>{value.GetRawData() + builtins::JSFunction::CODE_OFFSET}
      };
    }
    case JSType::JS_ARRAY: {
      return {
        JSHandle<JSValue>{value.GetRawData() + types::Object::PROPERTIES_OFFSET},
        JSHandle<JSValue>{value.GetRawData() + types::Object::PROTOTYPE_OFFSET}
      };
    }
    case JSType::JS_STRING: {
      return {
        JSHandle<JSValue>{value.GetRawData() + types::Object::PROPERTIES_OFFSET},
        JSHandle<JSValue>{value.GetRawData() + types::Object::PROTOTYPE_OFFSET},
        JSHandle<JSValue>{value.GetRawData() + builtins::JSString::PRIMITIVE_VALUE_OFFSET}
      };
    }
    case JSType::JS_BOOLEAN: {
      return {
        JSHandle<JSValue>{value.GetRawData() + types::Object::PROPERTIES_OFFSET},
        JSHandle<JSValue>{value.GetRawData() + types::Object::PROTOTYPE_OFFSET},
        JSHandle<JSValue>{value.GetRawData() + builtins::JSBoolean::PRIMITIVE_VALUE_OFFSET}
      };
    }
    case JSType::JS_NUMBER: {
      return {
        JSHandle<JSValue>{value.GetRawData() + types::Object::PROPERTIES_OFFSET},
        JSHandle<JSValue>{value.GetRawData() + types::Object::PROTOTYPE_OFFSET},
        JSHandle<JSValue>{value.GetRawData() + builtins::JSNumber::PRIMITIVE_VALUE_OFFSET}
      };
    }
    case JSType::JS_ERROR: {
      return {
        JSHandle<JSValue>{value.GetRawData() + types::Object::PROPERTIES_OFFSET},
        JSHandle<JSValue>{value.GetRawData() + types::Object::PROTOTYPE_OFFSET}
      };
    }
  }
}
  
}  // namespace voidjs
