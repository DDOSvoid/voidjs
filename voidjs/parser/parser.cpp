#include "voidjs/parser/parser.h"
#include "voidjs/lexer/character.h"
#include "voidjs/lexer/token.h"
#include "voidjs/ir/ast.h"
#include "voidjs/ir/program.h"
#include "voidjs/ir/statement.h"
#include "voidjs/ir/expression.h"
#include "voidjs/utils/error.h"

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
  if (token_.type == TokenType::STRING &&
      (token_.value == uR"('use strict')" || token_.value == uR"("use strict")")) {
    // use strict
  }
  Statements stmts;
  while (token_.type != TokenType::EOS) {
    try {
      auto stmt = ParseStatement();
      stmts.push_back(stmt);
    } catch(const utils::Error& e) {
      error_ = e;
      return nullptr;
    }
  }
  return new Program(std::move(stmts)); 
}

Statement* Parser::ParseStatement() {
  return nullptr;
}

// Parse BlockStatement
// Defined in ECMAScript 5.1 Chapter 12.1
// Block :
//   { StatementList_opt }
Statement* Parser::ParseBlockStatement() {
  // begin with {
  NextToken();
  Statements stmts;
  stmts = ParseStatementList(TokenType::RIGHT_BRACE);
  if (token_.type != TokenType::RIGHT_BRACE) {
    ThrowSyntaxError("expects a '{'");
  }
  NextToken();
  return new BlockStatement(std::move(stmts));
}

// Parse EmptyStatement
// Defined in ECMAScript 5.1 Chapter 12.3
// EmptyStatement :
//   ;
Statement* Parser::ParseEmptyStatement() {
  return new EmptyStatement();
}

// Parse StatementList
// Defined in ECMAScript 5.1 Chapter 12.1
// StatementList :
//   Statement
//   StatementList Statement
Statements Parser::ParseStatementList(TokenType end_type) {
  // Empty StatementList is permitted
  std::vector<Statement*> stmts;
  while (token_.type != end_type) {
    stmts.push_back(ParseStatement());
  }
  return stmts;
}

Token Parser::NextToken() {
  std::swap(token_, nxt_token_);
  nxt_token_ = lexer_.NextToken();
  return token_;
}

void Parser::ThrowSyntaxError(std::string msg) {
  throw utils::Error{utils::ErrorType::SYNTAX_ERROR, std::move(msg)};
}

}  // namespace voidjs
