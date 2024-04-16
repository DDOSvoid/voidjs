#include "voidjs/interpreter/interpreter.h"
#include "voidjs/ir/ast.h"
#include "voidjs/types/js_value.h"

namespace voidjs {

using namespace ast;
using namespace types;

JSValue Interpreter::EvalExpression(ast::AstNode* ast_node) {
  return JSValue();
}

JSValue Interpreter::EvalPrimaryExpression(ast::AstNode* ast_node) {
  switch (ast_node->GetType()) {
    case ast::AstNodeType::THIS: {
      return JSValue();
    }
    case ast::AstNodeType::IDENTIFIER: {
      return JSValue();
    }
    case ast::AstNodeType::NULL_LITERAL: {
      return EvalNullLiteral(ast_node);
    }
    case ast::AstNodeType::BOOLEAN_LITERAL: {
      return EvalBooleanLiteral(ast_node);
    }
    case ast::AstNodeType::NUMERIC_LITERAL: {
      return EvalNumericLiteral(ast_node); 
    }
    case ast::AstNodeType::STRING_LITERAL: {
      return EvalStringLiteral(ast_node);
    }
    case ast::AstNodeType::ARRAY_LITERAL: {
      return JSValue();
    }
    case ast::AstNodeType::OBJECT_LITERAL: {
      return JSValue();
    }
    default: {
      return EvalExpression(ast_node);
    }
  }
}

JSValue Interpreter::EvalNullLiteral(AstNode* ast_node) {
  return JSValue::Null();
}

JSValue Interpreter::EvalBooleanLiteral(AstNode* ast_node) {
  return JSValue(ast_node->AsBooleanLiteral()->GetBoolean());
}

JSValue Interpreter::EvalNumericLiteral(AstNode* ast_node) {
  auto literal = ast_node->AsNumericLiteral();
  if (literal->IsInteger()) {
    return JSValue(literal->GetNumber<std::int32_t>());
  } else {
    return JSValue(literal->GetNumber<double>());
  }
}

JSValue Interpreter::EvalStringLiteral(AstNode* ast_node) {
  auto str = String::New(ast_node->AsStringLiteral()->GetString());
  return JSValue(str);
}

// // GetValue(V)
// // Defined in ECMAScript 5.1 Chapter 8.7.1
// template <typename T>
// JSValue* Interpreter::GetValue(T* V) {
//   static_assert(std::is_same_v<JSValue, T> ||
//                 std::is_same_v<Reference, T>);

//   // 1. If Type(V) is not Reference, return V.
//   if constexpr (std::is_same_v<JSValue, T>) {
//     return V;
//   }

//   // 2. Let base be the result of calling GetBase(V).
//   auto base = V->GetBase();

//   // 3. If IsUnresolvableReference(V), throw a ReferenceError exception.
//   if (V->IsUnresolvableReference()) {
    
//   }

// }



}  // namespace voidjs
