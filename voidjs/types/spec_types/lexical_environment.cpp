#include "voidjs/types/spec_types/lexical_environment.h"

#include <iostream>

#include "voidjs/types/object_factory.h"
#include "voidjs/types/lang_types/string.h"
#include "voidjs/types/spec_types/reference.h"
#include "voidjs/types/spec_types/environment_record.h"

namespace voidjs {
namespace types {

Reference LexicalEnvironment::GetIdentifierReference(VM* vm, JSHandle<LexicalEnvironment> lex, JSHandle<String> name, bool strict) {
  // 1. If lex is the value null, then
  if (lex.IsEmpty()) {
    // a. Return a value of type Reference whose base value is undefined,
    //  whose referenced name is name, and whose strict mode flag is strict.
    return Reference{JSHandle<JSValue>{vm, JSValue::Undefined()}, name, strict};
  }

  // 2. Let envRec be lex’s environment record.
  auto env_rec = JSHandle<EnvironmentRecord>{vm, lex->GetEnvRec()};

  // 3. Let exists be the result of calling the HasBinding(N) concrete method of envRec passing name as the argument N.
  auto exists = EnvironmentRecord::HasBinding(vm, env_rec, name);

  // 4. If exists is true, then
  if (exists) {
    // a. Return a value of type Reference whose base value is envRec,
    //    whose referenced name is name, and whose strict mode flag is strict.
    return Reference(env_rec.As<JSValue>(), name, strict);
  }
  // 5. Else
  else {
    // a. Let outer be the value of lex’s outer environment reference.
    auto outer = JSHandle<LexicalEnvironment>{vm, lex->GetOuter()};
    
    // b. Return the result of calling GetIdentifierReference passing outer, name, and strict as arguments.
    return GetIdentifierReference(vm, outer, name, strict);
  }
}

JSHandle<LexicalEnvironment> LexicalEnvironment::NewDeclarativeEnvironmentRecord(VM* vm, JSHandle<LexicalEnvironment> E) {
  auto factory = vm->GetObjectFactory();
    
  // 1. Let env be a new Lexical Environment.
  // 2. Let envRec be a new declarative environment record containing no bindings.
  // 3. Set env’s environment record to be envRec.
  // 4. Set the outer lexical environment reference of env to E.
  // 5. Return env.
  auto env_rec = factory->NewDeclarativeEnvironmentRecord();
  return factory->NewLexicalEnvironment(E, env_rec);
}

JSHandle<LexicalEnvironment> LexicalEnvironment::NewObjectEnvironmentRecord(VM* vm, JSHandle<JSValue> O, JSHandle<LexicalEnvironment> E) {
  auto factory = vm->GetObjectFactory();
  
  // 1. Let env be a new Lexical Environment.
  // 2. Let envRec be a new object environment record containing O as the binding object.
  // 3. Set env’s environment record to be envRec.
  // 4. Set the outer lexical environment reference of env to E.
  // 5. Return env.
  auto env_rec = factory->NewObjectEnvironmentRecord(O.As<Object>());
  return factory->NewLexicalEnvironment(E, env_rec);
}

}  // namespace types
}  // namespace voidjs
