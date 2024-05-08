#ifndef VOIDJS_INTERPRETER_INTERPRETER_H
#define VOIDJS_INTERPRETER_INTERPRETER_H

#include <variant>
#include <memory>

#include "voidjs/builtins/js_function.h"
#include "voidjs/interpreter/runtime_call_info.h"
#include "voidjs/lexer/token.h"
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
#include "voidjs/interpreter/vm.h"

namespace voidjs {

class Interpreter {
 public:
  Interpreter() : vm_(new VM{this}) {
    Initialize();
  }

  ~Interpreter() {
    delete vm_;
  }
  
  void Initialize(); 
  
  types::Completion Execute(ast::AstNode* ast_node);

  types::Completion EvalProgram(ast::AstNode* ast_node);

  types::Completion EvalStatement(ast::Statement* stmt);
  types::Completion EvalBlockStatement(ast::BlockStatement* block_stmt);
  types::Completion EvalVariableStatement(ast::VariableStatement* var_stmt);
  types::Completion EvalEmptyStatement(ast::EmptyStatement* empty_stmt); 
  types::Completion EvalExpressionStatement(ast::ExpressionStatement* expr_stmt);
  types::Completion EvalIfStatement(ast::IfStatement* if_stmt);
  types::Completion EvalDoWhileStatement(ast::DoWhileStatement* do_while_stmt);
  types::Completion EvalWhileStatement(ast::WhileStatement* while_stmt);
  types::Completion EvalForStatement(ast::ForStatement* for_stmt);
  types::Completion EvalForInStatement(ast::ForInStatement* for_in_stmt);
  types::Completion EvalContinueStatement(ast::ContinueStatement* cont_stmt);
  types::Completion EvalBreakStatement(ast::BreakStatement* break_stmt);
  types::Completion EvalRetrunStatement(ast::ReturnStatement* return_stmt);
  types::Completion EvalWithStatement(ast::WithStatement* with_stmt);
  types::Completion EvalSwitchStatement(ast::SwitchStatement* switch_stmt);
  types::Completion EvalLabelledStatement(ast::LabelledStatement* label_stmt);
  types::Completion EvalThrowStatement(ast::ThrowStatement* throw_stmt);
  types::Completion EvalTryStatement(ast::TryStatement* try_stmt);
  types::Completion EvalDebuggerStatement(ast::DebuggerStatement* debug_stmt);
  
  void EvalFunctionDeclaration(ast::AstNode* ast_node);

  std::variant<JSHandle<JSValue>, types::Reference> EvalExpression(ast::Expression* expr);
  std::variant<JSHandle<JSValue>, types::Reference> EvalSequenceExpression(ast::SequenceExpression* seq_aexpr);
  std::variant<JSHandle<JSValue>, types::Reference> EvalAssignmentExpression(ast::AssignmentExpression* assign_expr);
  std::variant<JSHandle<JSValue>, types::Reference> EvalConditionalExpression(ast::ConditionalExpression* cond_expre);
  std::variant<JSHandle<JSValue>, types::Reference> EvalBinaryExpression(ast::BinaryExpression* binary_expr);
  std::variant<JSHandle<JSValue>, types::Reference> EvalUnaryExpression(ast::UnaryExpression* unary_expr);
  std::variant<JSHandle<JSValue>, types::Reference> EvalPostfixExpression(ast::PostfixExpression* post_expr);
  std::variant<JSHandle<JSValue>, types::Reference> EvalMemberExpression(ast::MemberExpression* mem_expr);
  std::variant<JSHandle<JSValue>, types::Reference> EvalNewExpression(ast::NewExpression* new_expr);
  std::variant<JSHandle<JSValue>, types::Reference> EvalCallExpression(ast::CallExpression* call_expr);
  JSHandle<JSValue> EvalFunctionExpression(ast::FunctionExpression* func_expr);
  JSHandle<JSValue> EvalObjectLiteral(ast::ObjectLiteral* object);
  JSHandle<JSValue> EvalArrayLiteral(ast::ArrayLiteral* array);
  JSHandle<JSValue> EvalNullLiteral(ast::NullLiteral* nul);
  JSHandle<JSValue> EvalBooleanLiteral(ast::BooleanLiteral* boolean);
  JSHandle<JSValue> EvalNumericLiteral(ast::NumericLiteral* num);
  JSHandle<JSValue> EvalStringLiteral(ast::StringLiteral* str);
  types::Reference EvalIdentifier(ast::Identifier* ident);

  types::Completion EvalSourceElements(const ast::Statements& stmts);
  types::Completion EvalStatementList(const ast::Statements& stmts);
  void EvalVariableDeclarationList(const ast::VariableDeclarations& decls);
  JSHandle<JSValue> EvalVariableDeclaration(ast::VariableDeclaration* decl);
  JSHandle<JSValue> EvalPropertyNameAndValueList(const ast::Properties& props);
  std::pair<JSHandle<types::String>, types::PropertyDescriptor> EvalPropertyAssignment(ast::Property* prop);
  std::vector<JSHandle<JSValue>> EvalArgumentList(const ast::Expressions& exprs);
  types::Completion EvalCaseBlock(const ast::CaseClauses& cases, JSHandle<JSValue> input);
  types::Completion EvalCatch(ast::Expression* catch_name, ast::Statement* catch_block, JSHandle<JSValue> C);
  JSHandle<JSValue> EvalElementList(const ast::Expressions& exprs);
  
  JSHandle<JSValue> ApplyCompoundAssignment(TokenType op, JSHandle<JSValue> lval, JSHandle<JSValue> rval);
  JSHandle<JSValue> ApplyLogicalOperator(TokenType op, ast::Expression* left, ast::Expression* right);
  JSHandle<JSValue> ApplyBitwiseOperator(TokenType op, ast::Expression* left, ast::Expression* right);
  JSHandle<JSValue> ApplyEqualityOperator(TokenType op, ast::Expression* left, ast::Expression* right);
  JSHandle<JSValue> ApplyRelationalOperator(TokenType op, ast::Expression* left, ast::Expression* right);
  JSHandle<JSValue> ApplyShiftOperator(TokenType op, ast::Expression* left, ast::Expression* right);
  JSHandle<JSValue> ApplyAdditiveOperator(TokenType op, ast::Expression* left, ast::Expression* right);
  JSHandle<JSValue> ApplyMultiplicativeOperator(TokenType op, ast::Expression* left, ast::Expression* right);
  JSHandle<JSValue> ApplyUnaryOperator(TokenType op, ast::Expression* expr);
  JSHandle<JSValue> ApplyPostfixOperator(TokenType op, ast::Expression* expr);

  types::Reference IdentifierResolution(JSHandle<types::String> ident);
  bool AbstractEqualityComparison(JSHandle<JSValue> x, JSHandle<JSValue> y);
  bool StrictEqualityComparison(JSHandle<JSValue> x, JSHandle<JSValue> y);
  JSHandle<JSValue> AbstractRelationalComparison(JSHandle<JSValue> x, JSHandle<JSValue> y, bool left_first);
  JSHandle<JSValue> CreateFunctionObjects();
  
  JSHandle<JSValue> GetValue(const std::variant<JSHandle<JSValue>, types::Reference>& V);
  JSHandle<JSValue> GetUsedByGetValue(JSHandle<JSValue> base, JSHandle<types::String> P);
  void PutValue(const std::variant<JSHandle<JSValue>, types::Reference>& V, JSHandle<JSValue> W);
  void PutUsedByPutValue(JSHandle<JSValue> base, JSHandle<types::String> P, JSHandle<JSValue> W, bool Throw);

  VM* GetVM() const { return vm_; }

 private:
  VM* vm_;
};

}  // namespace voidjs

#endif  // VOIDJS_INTERPRETER_INTERPRETER_H
