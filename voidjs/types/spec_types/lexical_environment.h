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
  static constexpr std::size_t OUTER_OFFSET = HeapObject::END_OFFSET;
  LexicalEnvironment* GetOuter() const { return *utils::BitGet<LexicalEnvironment**>(this, OUTER_OFFSET); }
  void SetOuter(LexicalEnvironment* outer) { *utils::BitGet<LexicalEnvironment**>(this, OUTER_OFFSET) = outer; }
  
  static constexpr std::size_t ENV_REC_OFFSET = OUTER_OFFSET + sizeof(std::uintptr_t);
  EnvironmentRecord* GetEnvRec() const { return *utils::BitGet<EnvironmentRecord**>(this, ENV_REC_OFFSET); }
  void SetEnvRec(EnvironmentRecord* env_rec) { *utils::BitGet<EnvironmentRecord**>(this, ENV_REC_OFFSET) = env_rec; }
  
  static Reference GetIdentifierReference(VM* vm, LexicalEnvironment* lex, String* name, bool strict);
  static LexicalEnvironment* NewDeclarativeEnvironmentRecord(VM* vm, LexicalEnvironment* E);
  static LexicalEnvironment* NewObjectEnvironmentRecord(VM* vm, JSValue O, LexicalEnvironment* E);

  static constexpr std::size_t SIZE = sizeof(std::uintptr_t) + sizeof(std::uintptr_t);
  static constexpr std::size_t END_OFFSET = HeapObject::END_OFFSET + SIZE;
};

}  // namespace types
}  // namespace voidjs

#endif  // VOIDJS_TYPES_SPEC_TYPES_LEXICAL_ENVIRONMENT_H
