#include "voidjs/interpreter/interpreter.h"
#include "voidjs/types/js_value.h"

namespace voidjs {

using namespace ast;
using namespace types;

Null* Interpreter::EvalNullLiteral(AstNode* ast_node) {
  return new Null();
}

Boolean* Interpreter::EvalBooleanLiteral(AstNode* ast_node) {
  return new Boolean(ast_node->AsBooleanLiteral()->GetBoolean());
}

Number* Interpreter::EvalNumericLiteral(AstNode* ast_node) {
  auto literal = ast_node->AsNumericLiteral();
  if (literal->IsInteger()) {
    return new Number(literal->GetNumber<std::int32_t>());
  } else {
    return new Number(literal->GetNumber<double>());
  }
}

String* Interpreter::EvalStringLiteral(AstNode* ast_node) {
  return new String(ast_node->AsStringLiteral()->GetString());
}

}  // namespace voidjs
