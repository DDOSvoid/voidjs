#ifndef VOIDJS_INTERPRETER_INTERPRETER_H
#define VOIDJS_INTERPRETER_INTERPRETER_H

#include <variant>

#include "voidjs/lexer/token_type.h"
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

  types::Completion EvalStatement(ast::Statement* stmt);
  types::Completion EvalExpressionStatement(ast::ExpressionStatement* expr_stmt); 

  std::variant<JSValue, types::Reference> EvalExpression(ast::Expression* expr);
  std::variant<JSValue, types::Reference> EvalSequenceExpression(ast::SequenceExpression* seq_aexpr);
  std::variant<JSValue, types::Reference> EvalAssignmentExpression(ast::AssignmentExpression* assign_expr);
  std::variant<JSValue, types::Reference> EvalConditionalExpression(ast::ConditionalExpression* cond_expre);
  std::variant<JSValue, types::Reference> EvalBinaryExpression(ast::BinaryExpression* binary_expr);
  std::variant<JSValue, types::Reference> EvalUnaryExpression(ast::UnaryExpression* unary_expr);
  std::variant<JSValue, types::Reference> EvalPostfixExpression(ast::PostfixExpression* post_expr);
  std::variant<JSValue, types::Reference> EvalMemberExpression(ast::MemberExpression* ast_node);
  JSValue EvalNullLiteral(ast::NullLiteral* nul);
  JSValue EvalBooleanLiteral(ast::BooleanLiteral* boolean);
  JSValue EvalNumericLiteral(ast::NumericLiteral* num);
  JSValue EvalStringLiteral(ast::StringLiteral* str);

  types::Completion EvalFunctionDeclaration(ast::AstNode* ast_node);
  
  JSValue GetValue(const std::variant<JSValue, types::Reference>& V);
  void PutValue(const std::variant<JSValue, types::Reference>& V, JSValue W);

 private:
  JSValue ApplyCompoundAssignment(TokenType op, JSValue lval, JSValue rval);
  JSValue ApplyBinaryOperator(TokenType op, JSValue lval, JSValue rval);
  JSValue ApplyUnaryOperator(TokenType op, JSValue val);
};

}  // namespace voidjs

#endif  // VOIDJS_INTERPRETER_INTERPRETER_H
