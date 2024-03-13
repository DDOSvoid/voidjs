#include "voidjs/parser/parser.h"
#include "voidjs/lexer/character.h"
#include "voidjs/lexer/token.h"

namespace voidjs {

using namespace ast;

// Parse Program
// Defined in ECMAScript 5.1 Chapter 14
// Program :
//   SourceElementsopt
// SourceElements :
//   SourceElement
//   SourceElements SourceElement
// SourceElement :
//   Statement
//   FunctionDeclaration
Program* Parser::ParseProgram() {
  auto ret = new Program;
  if (token_.type == TokenType::STRING &&
      (token_.value == uR"('use strict')" || token_.value == uR"("use strict")")) {
    // use strict
    }
  while (token_.type != TokenType::EOS) {
    auto stmt = ParseStatement();
    ret->Statements().push_back(stmt);
  }
  return ret;
}

Statement* Parser::ParseStatement() {
  auto ret = new Statement;
  return ret;
}

// Parse BlockStatement
// Defined in ECMAScript 5.1 Chapter 12.1
// Block :
//   { StatementList_opt }
Statement* Parser::ParseBlockStatement() {
  auto ret = new BlockStatement;
  if (token_.type != TokenType::LEFT_BRACE) {
    // error handling
  }
  NextToken();
  if (nxt_token_.type != TokenType::RIGHT_BRACE) {
    auto stmts = ParseStatementList();
    std::swap(ret->Statements(), stmts);
  }
  if (token_.type != TokenType::RIGHT_BRACE) {
    // error handling
  }
  NextToken();
  return ret;
}

// Parse StatementList
// Defined in ECMAScript 5.1 Chapter 12.1
// StatementList :
//   Statement
//   StatementList Statement
std::vector<Statement*> Parser::ParseStatementList() {
  std::vector<Statement*> stmts;
  if (token_.type == TokenType::EOS) {
    // error handling
  }
  while (token_.type != TokenType::EOS) {
    stmts.push_back(ParseStatement());
  }
  return stmts;
}

Token Parser::NextToken() {
  std::swap(token_, nxt_token_);
  nxt_token_ = lexer_.NextToken();
  return token_;
}

}  // namespace voidjs
