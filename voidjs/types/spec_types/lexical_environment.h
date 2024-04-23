#ifndef VOIDJS_TYPES_SPEC_TYPES_LEXICAL_ENVIRONMENT_H
#define VOIDJS_TYPES_SPEC_TYPES_LEXICAL_ENVIRONMENT_H

#include "voidjs/types/js_value.h"
#include "voidjs/types/spec_types/environment_record.h"
#include "voidjs/types/spec_types/reference.h"

namespace voidjs {
namespace types {

class LexicalEnvironment {
 public:
  LexicalEnvironment(LexicalEnvironment* outer, EnvironmentRecord* env_rec)
    : outer_(outer), env_rec_(env_rec)
  {}
  
  static Reference GetIdentifierReference(LexicalEnvironment* lex, String* name, bool strict);
  static LexicalEnvironment* NewDeclarativeEnvironmentRecord(LexicalEnvironment* E);
  static LexicalEnvironment* NewObjectEnvironmentRecord(JSValue O, LexicalEnvironment* E);

  EnvironmentRecord* GetEnvRec() const { return env_rec_; }
  LexicalEnvironment* GetOuter() const { return outer_; }

 private:
  LexicalEnvironment* outer_;
  EnvironmentRecord* env_rec_;
};

}  // namespace types
}  // namespace voidjs

#endif  // VOIDJS_TYPES_SPEC_TYPES_LEXICAL_ENVIRONMENT_H
