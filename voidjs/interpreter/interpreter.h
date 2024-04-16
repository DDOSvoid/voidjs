#ifndef VOIDJS_INTERPRETER_INTERPRETER_H
#define VOIDJS_INTERPRETER_INTERPRETER_H

#include "voidjs/ir/ast.h"
#include "voidjs/ir/program.h"
#include "voidjs/ir/expression.h"
#include "voidjs/ir/statement.h"
#include "voidjs/ir/literal.h"
#include "voidjs/parser/parser.h"
#include "voidjs/types/js_value.h"
#include "voidjs/types/lang_types/null.h"
#include "voidjs/types/lang_types/boolean.h"
#include "voidjs/types/lang_types/number.h"
#include "voidjs/types/lang_types/string.h"
#include "voidjs/types/spec_types/reference.h"

namespace voidjs {

class Interpreter {
 public:
  void Execute();

  JSValue EvalExpression(ast::AstNode* ast_node);
  JSValue EvalPrimaryExpression(ast::AstNode* ast_node);
  JSValue EvalLeftHandSideExpression(ast::AstNode* ast_node);

  JSValue EvalNullLiteral(ast::AstNode* ast_node);
  JSValue EvalBooleanLiteral(ast::AstNode* ast_node);
  JSValue EvalNumericLiteral(ast::AstNode* ast_node);
  JSValue EvalStringLiteral(ast::AstNode* ast_node);

 private:
  template <typename T>
  JSValue GetValue(T V);
};

}  // namespace voidjs

#endif  // VOIDJS_INTERPRETER_INTERPRETER_H
