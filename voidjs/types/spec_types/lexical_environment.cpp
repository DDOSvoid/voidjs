#include "voidjs/types/spec_types/lexical_environment.h"

#include "voidjs/types/spec_types/reference.h"
#include "voidjs/types/spec_types/environment_record.h"

namespace voidjs {
namespace types {

Reference LexicalEnvironment::GetIdentifierReference(LexicalEnvironment* lex, String* name, bool strict) {
  // 1. If lex is the value null, then
  if (!lex) {
    // a. Return a value of type Reference whose base value is undefined,
    //  whose referenced name is name, and whose strict mode flag is strict.
    return Reference(JSValue::Undefined(), name, strict);
  }

  // 2. Let envRec be lex’s environment record.
  auto env_rec = lex->GetEnvRec();

  // 3. Let exists be the result of calling the HasBinding(N) concrete method of envRec passing name as the argument N.
  auto exists = env_rec->HasBinding(name);

  // 4. If exists is true, then
  if (exists) {
    // a. Return a value of type Reference whose base value is envRec,
    //    whose referenced name is name, and whose strict mode flag is strict.
    return Reference(env_rec, name, strict);
  }
  // 5. Else
  else {
    // a. Let outer be the value of lex’s outer environment reference.
    auto outer = lex->GetOuter();
    
    // b. Return the result of calling GetIdentifierReference passing outer, name, and strict as arguments.
    return GetIdentifierReference(outer, name, strict);
  }
}

LexicalEnvironment* LexicalEnvironment::NewDeclarativeEnvironmentRecord(LexicalEnvironment* E) {
  // 1. Let env be a new Lexical Environment.
  // 2. Let envRec be a new declarative environment record containing no bindings.
  // 3. Set env’s environment record to be envRec.
  // 4. Set the outer lexical environment reference of env to E.
  // 5. Return env.
  auto env_rec = new DeclarativeEnvironmentRecord();
  return new LexicalEnvironment(E, env_rec);
}

LexicalEnvironment* LexicalEnvironment::NewObjectEnvironmentRecord(JSValue O, LexicalEnvironment* E) {
  // 1. Let env be a new Lexical Environment.
  // 2. Let envRec be a new object environment record containing O as the binding object.
  // 3. Set env’s environment record to be envRec.
  // 4. Set the outer lexical environment reference of env to E.
  // 5. Return env.
  auto env_rec = new ObjectEnvironmentRecord(O.GetHeapObject()->AsObject());
  return new LexicalEnvironment(E, env_rec);
}

}  // namespace types
}  // namespace voidjs
