#ifndef VOIDJS_PARSER_PARSER_H
#define VOIDJS_PARSER_PARSER_H

#include "voidjs/ir/statement.h"
#include "voidjs/lexer/lexer.h"
#include "voidjs/ir/ast.h"
#include "voidjs/utils/error.h"

namespace voidjs {

class Parser {
 public:
  Parser(std::u16string const& src)
    : lexer_(src) {
    token_ = lexer_.NextToken();
    nxt_token_ = lexer_.NextToken();
  }
  ~Parser() {}

  // Non-Copyable
  Parser(const Parser&) = delete;
  Parser& operator=(const Parser&) = delete;

 private:
  Token NextToken();
  Token PeekToken();

  void ThrowSyntaxError(std::string msg); 

  std::vector<ast::Statement*> ParseStatementList(TokenType end_token_type);
  
  ast::Program* ParseProgram();

  ast::Statement* ParseStatement();
  ast::Statement* ParseBlockStatement();
  ast::Statement* ParseEmptyStatement();

 private:
  Lexer lexer_;

  Token token_;
  Token nxt_token_;

  utils::Error error_;
};

}  // namespace voidjs

#endif  // VOIDJS_PARSER_PARSER_H
