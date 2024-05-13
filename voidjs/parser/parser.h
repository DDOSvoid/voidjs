#ifndef VOIDJS_PARSER_PARSER_H
#define VOIDJS_PARSER_PARSER_H

#include "voidjs/lexer/lexer.h"
#include "voidjs/ir/expression.h"
#include "voidjs/ir/statement.h"
#include "voidjs/ir/ast.h"
#include "voidjs/utils/error.h"

namespace voidjs {

class Parser {
 public:
  Parser(std::u16string const& src)
    : lexer_(src) {
    lexer_.NextToken();
  }
  
  ~Parser() = default;

  // Non-Copyable
  Parser(const Parser&) = delete;
  Parser& operator=(const Parser&) = delete;


  ast::Program* ParseProgram();

  ast::Statement* ParseStatement();
  ast::Statement* ParseBlockStatement();
  ast::Statement* ParseVariableStatement(); 
  ast::Statement* ParseEmptyStatement();
  ast::Statement* ParseExpressionStatement();
  ast::Statement* ParseIfStatement();
  ast::Statement* ParseDoWhileStatement();
  ast::Statement* ParseWhileStatement();
  ast::Statement* ParseForStatement();
  ast::Statement* ParseContinueStatement();
  ast::Statement* ParseBreakStatement();
  ast::Statement* ParseReturnStatement();
  ast::Statement* ParseWithStatement();
  ast::Statement* ParseSwitchStatement();
  ast::Statement* ParseLabelledStatement();
  ast::Statement* ParseThrowStatement();
  ast::Statement* ParseTryStatement();
  ast::Statement* ParseDebuggerStatement();

  ast::Expression* ParseExpression(bool allow_in = true);
  ast::Expression* ParsePrimaryExpression();
  ast::Expression* ParseLeftHandSideExpression();
  ast::Expression* ParseMemberExpression(bool has_new = false);
  ast::Expression* ParsePostfixExpression();
  ast::Expression* ParseUnaryExpression();
  ast::Expression* ParseBinaryExpression(bool allow_in = true, std::int32_t precedence = 0);
  ast::Expression* ParseConditionalExpression(bool allow_in = true);
  ast::Expression* ParseAssignmentExpression(bool allow_in = true);
  ast::Expression* ParseFunctionExpression();

  ast::Expression* ParseIdentifier();
  ast::VariableDeclarations ParseVariableDeclarationList(bool allow_in = true);
  ast::VariableDeclaration* ParseVariableDeclaration(bool aloow_in = true);
  ast::Expression* ParseArrayLiteral();
  ast::Expressions ParseArguments();
  ast::Expressions ParseArgumentList(TokenType end_token_type);
  ast::Expression* ParseObjectLiteral();
  ast::CaseClauses ParseCaseBlock();
  ast::CaseClauses ParseCaseClauses();
  ast::CaseClause* ParseCaseClause();
  ast::Statement* ParsePotentialLabelledStatement();
  ast::Statement* ParseFunctionDeclaration();
  ast::Expressions ParseFormalParameterList();
  ast::Properties ParsePropertyNameAndValueList();
  ast::Property* ParsePropertyAssignment();
  ast::Expression* ParsePropertyName();
  
 private:
  struct FunctionScopeInfo {
    ast::VariableDeclarations variable_declarations;
    ast::FunctionDeclarations function_declarations;
  };

 private:
  void ThrowSyntaxError(std::string msg);
  bool TryAutomaticInsertSemicolon();
  void EnterFunctionScope();
  void AddVariableDeclaration(ast::VariableDeclaration* var_decl);
  void AddFunctionDeclaration(ast::FunctionDeclaration* func_decl);
  FunctionScopeInfo ExitFunctionScope();

 private:
  Lexer lexer_;

  std::vector<FunctionScopeInfo> function_scode_infos_;

  utils::Error error_;
};

}  // namespace voidjs

#endif  // VOIDJS_PARSER_PARSER_H
