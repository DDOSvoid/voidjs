#ifndef VOIDJS_TYPES_SPEC_TYPES_LEXICAL_ENVIRONMENT_H
#define VOIDJS_TYPES_SPEC_TYPES_LEXICAL_ENVIRONMENT_H

#include "voidjs/types/js_value.h"
#include "voidjs/types/heap_object.h"
#include "voidjs/types/spec_types/environment_record.h"
#include "voidjs/types/spec_types/reference.h"

namespace voidjs {
namespace types {

class LexicalEnvironment : public HeapObject {
 public:
  // LexicalEnvironment*
  static constexpr std::size_t OUTER_OFFSET = HeapObject::END_OFFSET;
  JSValue GetOuter() const { return *utils::BitGet<JSValue*>(this, OUTER_OFFSET); }
  void SetOuter(JSValue value) { *utils::BitGet<JSValue*>(this, OUTER_OFFSET) = value; }
  void SetOuter(JSHandle<JSValue> handle) { *utils::BitGet<JSValue*>(this, OUTER_OFFSET) = handle.GetJSValue(); }

  // EnvironmentRecord*
  static constexpr std::size_t ENV_REC_OFFSET = OUTER_OFFSET + sizeof(std::uintptr_t);
  JSValue GetEnvRec() const { return *utils::BitGet<JSValue*>(this, ENV_REC_OFFSET); }
  void SetEnvRec(JSValue value) { *utils::BitGet<JSValue*>(this, ENV_REC_OFFSET) = value; }
  void SetEnvRec(JSHandle<JSValue> handle) { *utils::BitGet<JSValue*>(this, ENV_REC_OFFSET) = handle.GetJSValue(); }
  
  static Reference GetIdentifierReference(VM* vm, JSHandle<LexicalEnvironment> lex, JSHandle<String> name, bool strict);
  static JSHandle<LexicalEnvironment> NewDeclarativeEnvironmentRecord(VM* vm, JSHandle<LexicalEnvironment> E);
  static JSHandle<LexicalEnvironment> NewObjectEnvironmentRecord(VM* vm, JSHandle<JSValue> O, JSHandle<LexicalEnvironment> E);

  static constexpr std::size_t SIZE = sizeof(std::uintptr_t) + sizeof(std::uintptr_t);
  static constexpr std::size_t END_OFFSET = HeapObject::END_OFFSET + SIZE;
};

}  // namespace types
}  // namespace voidjs

#endif  // VOIDJS_TYPES_SPEC_TYPES_LEXICAL_ENVIRONMENT_H
