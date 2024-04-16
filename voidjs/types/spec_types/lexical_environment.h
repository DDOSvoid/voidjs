#ifndef VOIDJS_TYPES_SPEC_TYPES_LEXICAL_ENVIRONMENT_H
#define VOIDJS_TYPES_SPEC_TYPES_LEXICAL_ENVIRONMENT_H

#include "voidjs/types/js_value.h"
#include "voidjs/types/spec_types/environment_record.h"
#include "voidjs/types/spec_types/reference.h"

namespace voidjs {
namespace types {

class LexicalEnvironment {
 public:

  // Defined in ECMAScript 5.1 Chapter 10.2.2.1
  Reference GetIdentifierReference(std::u16string_view name, bool strict);

  // Defined in ECMAScript 5.1 Chapter 10.2.2.2
  EnvironmentRecord* NewDeclarativeEnvironmentRecord(LexicalEnvironment* E);

  // Defined in ECMAScript 5.1 Chapter 10.2.2.3
  EnvironmentRecord* NewObjectEnvironmentRecord(JSValue O, LexicalEnvironment* E);

 private:
  LexicalEnvironment* outer_;
  EnvironmentRecord* env_rec_;
};

}  // namespace types
}  // namespace voidjs

#endif  // VOIDJS_TYPES_SPEC_TYPES_LEXICAL_ENVIRONMENT_H
