#ifndef VOIDJS_PARSER_PARSER_H
#define VOIDJS_PARSER_PARSER_H

#include "voidjs/ir/expression.h"
#include "voidjs/ir/statement.h"
#include "voidjs/lexer/lexer.h"
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

  ast::Expression* ParseExpression(bool allow_in = true);
  ast::Expression* ParsePrimaryExpression();
  ast::Expression* ParseLeftHandSideExpression();
  ast::Expression* ParseMemberExpression(bool has_new = false);
  ast::Expression* ParsePostfixExpression();
  ast::Expression* ParseUnaryExpression();
  ast::Expression* ParseBinaryExpression(bool allow_in = true, std::int32_t precedence = 0);
  ast::Expression* ParseConditionalExpression(bool allow_in = true);
  ast::Expression* ParseAssignmentExpression(bool allow_in = true);

  ast::Expression* ParseIdentifier();
  ast::Statements ParseStatementList(TokenType end_token_type);
  ast::VariableDeclarations ParseVariableDeclarationList(bool allow_in = true);
  ast::VariableDeclaration* ParseVariableDeclaration(bool aloow_in = true);
  ast::Expression* ParseArrayLiteral();
  ast::Expressions ParseArguments();
  ast::Expressions ParseArgumentList(TokenType end_token_type);
  ast::Expression* ParseObjectLiteral(); 

 private:
  void ThrowSyntaxError(std::string msg); 

 private:
  Lexer lexer_;

  utils::Error error_;
};

}  // namespace voidjs

#endif  // VOIDJS_PARSER_PARSER_H
