#ifndef VOIDJS_PARSER_H
#define VOIDJS_PARSER_H

#include "voidjs/parser/lexer.h"
#include "voidjs/parser/ast.h"

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

  std::vector<ast::Statement*> ParseStatementList();
  
  ast::Program* ParseProgram();

  ast::Statement* ParseStatement();
  ast::Statement* ParseBlockStatement();

 private:
  Lexer lexer_;

  Token token_;
  Token nxt_token_;
};

}  // namespace voidjs

#endif  // VOIDJS_PARSER_H
