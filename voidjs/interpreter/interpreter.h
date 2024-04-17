#ifndef VOIDJS_INTERPRETER_INTERPRETER_H
#define VOIDJS_INTERPRETER_INTERPRETER_H

#include <variant>

#include "voidjs/ir/ast.h"
#include "voidjs/ir/program.h"
#include "voidjs/ir/expression.h"
#include "voidjs/ir/statement.h"
#include "voidjs/ir/literal.h"
#include "voidjs/parser/parser.h"
#include "voidjs/types/js_value.h"
#include "voidjs/types/lang_types/string.h"
#include "voidjs/types/spec_types/reference.h"
#include "voidjs/types/spec_types/completion.h"

namespace voidjs {

class Interpreter {
 public:
  void Execute();

  types::Completion EvalProgram(ast::AstNode* ast_node);

  types::Completion EvalStatement(ast::AstNode* ast_node);
  types::Completion EvalExpressionStatement(ast::AstNode* ast_node); 

  std::variant<JSValue, types::Reference> EvalExpression(ast::AstNode* ast_node);
  std::variant<JSValue, types::Reference> EvalAssignmentExpression(ast::AstNode* ast_node);
  std::variant<JSValue, types::Reference> EvalConditionalExpression(ast::AstNode* ast_node);
  std::variant<JSValue, types::Reference> EvalBinaryExpression(ast::AstNode* ast_node); 
  std::variant<JSValue, types::Reference> EvalLeftHandSideExpression(ast::AstNode* ast_node);
  std::variant<JSValue, types::Reference> EvalPrimaryExpression(ast::AstNode* ast_node);

  JSValue EvalNullLiteral(ast::AstNode* ast_node);
  JSValue EvalBooleanLiteral(ast::AstNode* ast_node);
  JSValue EvalNumericLiteral(ast::AstNode* ast_node);
  JSValue EvalStringLiteral(ast::AstNode* ast_node);

  types::Completion EvalFunctionDeclaration(ast::AstNode* ast_node); 

 private:
  JSValue ApplyCompoundAssignment(TokenType op, JSValue lval, JSValue rval);
  
  JSValue GetValue(const std::variant<JSValue, types::Reference>& V);
  void PutValue(const std::variant<JSValue, types::Reference>& V, JSValue W);
};

}  // namespace voidjs

#endif  // VOIDJS_INTERPRETER_INTERPRETER_H
