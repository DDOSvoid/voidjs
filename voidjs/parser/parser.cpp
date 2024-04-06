#include "voidjs/parser/parser.h"
#include "voidjs/lexer/character.h"
#include "voidjs/lexer/token.h"
#include "voidjs/ir/ast.h"
#include "voidjs/ir/program.h"
#include "voidjs/ir/statement.h"
#include "voidjs/ir/expression.h"
#include "voidjs/ir/literal.h"
#include "voidjs/utils/error.h"
#include "voidjs/utils/helper.h"

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
  
  Statements stmts = ParseStatementList(TokenType::RIGHT_BRACE);
  
  if (token_.type != TokenType::RIGHT_BRACE) {
    ThrowSyntaxError("expects a '}'");
  }
  NextToken();
  
  return new BlockStatement(std::move(stmts));
}

// Parse VariableStatment
// Defined in ECMAScript 5.1 chapter 12.2
// VariableStatement :
//   var VariableDeclarationList ;
Statement* Parser::ParseVariableStatement() {
  // begin with var
  NextToken();
  
  return new VariableStatement(ParseVariableDeclarationList());
}

// Parse EmptyStatement
// Defined in ECMAScript 5.1 Chapter 12.3
// EmptyStatement :
//   ;
Statement* Parser::ParseEmptyStatement() {
  return new EmptyStatement();
}

// Parse ExpressionStatement
// Defined in ECMAScript 5.1 Chapter 12.4
// ExpressionStatement :
//   [lookahead ∉ {{, function}] Expression ;
Statement* Parser::ParseExpressionStatement() {
  if (nxt_token_.type == TokenType::COMMA ||
      nxt_token_.type == TokenType::KEYWORD_FUNCTION) {
    
  }
  Expression* expr = ParseExpression();
  return new ExpressionStatement(expr);
}

// Parse IfStatement
// Defined in ECMAScript 5.1 chapter 12.5
// IfStatement :
//   if ( Expression ) Statement else Statement
//   if ( Expression ) Statement
Statement* Parser::ParseIfStatement() {
  // begin with if
  NextToken();
  
  if (token_.type != TokenType::LEFT_PAREN) {
    ThrowSyntaxError("expects a '('");
  }
  NextToken();
  
  Expression* cond = ParseExpression();
  
  if (token_.type != TokenType::RIGHT_PAREN) {
    ThrowSyntaxError("expects a ')'");
  }
  NextToken();

  Statement* cons = ParseStatement();

  Statement* alt = nullptr;
  if (token_.type == TokenType::KEYWORD_ELSE) {
    NextToken();
    alt = ParseStatement();
  }

  return new IfStatement(cond, cons, alt); 
}

// Parse Expression
// Defined in ECMAScript 5.1 chapter 11.14
// Expression :
//   AssignmentExpression
//   Expression , AssignmentExpression
Expression* Parser::ParseExpression() {
  return nullptr;
}

// Parse PrimaryExpression
// Defined in ECMAScript 5.1 chapter 11.1
//  PrimaryExpression :
//    this
//    Identifier
//    Literal
//    ArrayLiteral
//    ObjectLiteral
//    ( Expression )
Expression* Parser::ParsePrimaryExpression() {
  switch (token_.type) {
    case TokenType::KEYWORD_THIS: {
      auto expr = new ThisExpression();
      NextToken();
      return expr;
    }
    case TokenType::IDENTIFIER: {
      auto ident = new Identifier(token_.value);
      NextToken();
      return ident;
    }
    case TokenType::NULL_LITERAL: {
      auto null = new NullLiteral();
      NextToken();
      return null;
    }
    case TokenType::BOOLEAN_LITERAL: {
      auto boolean = new BooleanLiteral(token_.value == u"true");
      NextToken();
      return boolean;
    }
    case TokenType::NUMBER: {
      auto num = new NumericLiteral(utils::ConvertToNumber(token_.value));
      NextToken();
      return num;
    }
    case TokenType::STRING: {
      auto str = new StringLiteral(utils::ConvertToString(token_.value));
      NextToken();
      return str;
    }
    case TokenType::LEFT_BRACKET: {
      auto arr = ParseArrayLiteral();
      return arr; 
    }
    case TokenType::LEFT_PAREN: {
      NextToken();
      auto expr = ParseExpression();
      if (token_.type != TokenType::RIGHT_PAREN) {
        ThrowSyntaxError("expects a ')'");
      }
      NextToken();
      return expr;
    }
    default: {
      ThrowSyntaxError("parse error");
      return nullptr;
    }
  }
}

// Parse LeftHandSideExpression
// Defined in ECMAScript 5.1 chapter 11.2
//  LeftHandSideExpression :
//    NewExpression
//    MemberExpression
Expression* Parser::ParseLeftHandSideExpression() {
  return ParseMemberExpression(false);
}

// Parse MemberExpression
// Defined in ECMAScript 5.1 chapter 11.2
//  MemberExpression :
//    PrimaryExpression
//    FunctionExpression
//    MemberExpression [ Expression ]
//    MemberExpression . IdentifierName
//    new MemberExpression Arguments
// 
//  NewExpression :
//    MemberExpression
//    new NewExpression
// 
//  CallExpression :
//    MemberExpression Arguments
//    CallExpression Arguments
//    CallExpression [ Expression ]
//    CallExpression . IdentifierName
// Note that one of the MemberExpression's Production is clearly wrong,
// i.e., FunctionExpression or PrimaryExpression is followed by
// FunctionExpression or PrimaryExpression, so I chose not to
// Parse this case directly here.
// We use boolean has_new here to deal with expressions
// like 'new MemberExpression Arguments'.
Expression* Parser::ParseMemberExpression(bool has_new) {
  Expression* callee = nullptr;
  if (token_.type == TokenType::KEYWORD_NEW) {
    NextToken();
    callee = ParseMemberExpression(true);
    if (token_.type == TokenType::LEFT_PAREN) {
      auto args = ParseArguments();
      return new NewExpression(callee, std::move(args));
    } else {
      return new NewExpression(callee, {});
    }
  } else {
    if (token_.type == TokenType::KEYWORD_FUNCTION) {
      // callee = ParseFunctionExpression();
    } else {
      callee = ParsePrimaryExpression();
    }
  }

  while (true) {
    switch (token_.type) {
      case TokenType::LEFT_BRACKET: {
        NextToken();
        auto expr = ParseExpression();
        if (token_.type != TokenType::RIGHT_BRACKET) {
          ThrowSyntaxError("expects a ']'");
        }
        NextToken();
        callee = new MemberExpression(callee, expr);
        break;
      }
      case TokenType::DOT: {
        NextToken();
        if (token_.type == TokenType::IDENTIFIER) {
          auto ident = new Identifier(token_.value);
          NextToken();
          callee = new MemberExpression(callee, ident);
        } else {
          ThrowSyntaxError("expects identifier");
        }
        break;
      }
      case TokenType::LEFT_PAREN: {
        if (!has_new) {
          auto args = ParseArguments();
          callee = new CallExpression(callee, args);
        } else {
          return callee;
        }
        break;
      }
      default: {
        return callee;
        break;
      }
    }
  }
}


// Parse AssignmentExpression
Expression* Parser::ParseAssignmentExpression() {
  return nullptr;
}


// Parse StatementList
// Defined in ECMAScript 5.1 Chapter 12.1
// StatementList :
//   Statement
//   StatementList Statement
Statements Parser::ParseStatementList(TokenType end_type) {
  // Empty StatementList is permitted
  Statements stmts;
  while (token_.type != end_type) {
    stmts.push_back(ParseStatement());
  }
  return stmts;
}

// Parse VariableDeclarationList
// Defined in ECMAScript 5.1 chapter 12.2
// VariableDeclarationList :
//   VariableDeclaration
//   VariableDeclarationList , VariableDeclaration
VariableDeclarations Parser::ParseVariableDeclarationList() {
  VariableDeclarations var_decls;
  var_decls.push_back(ParseVariableDeclaration());
  while (token_.type == TokenType::COMMA) {
    NextToken();
    var_decls.push_back(ParseVariableDeclaration());
  }
  return var_decls;
}

// Parse VariableDeclaration
// Defined in ECMAScript 5.1 chapter 12.2
// VariableDeclaration :
//   Identifier Initialiser_opt
VariableDeclaration* Parser::ParseVariableDeclaration() {
  Expression* ident = ParseExpression();
  // need to test if ident is Identifier

  Expression* init = ParseExpression();
  // need to test if init is valid

  return new VariableDeclaration(ident, init); 
}

// Parse ArrayLiteral
// Defined in ECMAScript 5.1 chapter 11.1.4
//  ArrayLiteral :
//    [ Elision_opt ]
//    [ ElementList ]
//    [ ElementList , Elision_opt ]
//  ElementList :
//    Elision_opt AssignmentExpression
//    ElementList , Elision_opt AssignmentExpression
//  Elision :
//    ,
//    Elision ,
Expression* Parser::ParseArrayLiteral() {
  // begin with [
  NextToken();

  Expressions exprs;
  
  while (token_.type != TokenType::RIGHT_BRACKET) {
    if (token_.type == TokenType::COMMA) {
      exprs.push_back(nullptr);
      NextToken();
    } else {
      exprs.push_back(ParseAssignmentExpression());
    }
  }

  if (token_.type != TokenType::RIGHT_BRACKET) {
    ThrowSyntaxError("expects a ']'");
  }
  NextToken();

  return new ArrayLiteral(std::move(exprs));
}

// Parse Arguments
// Defined in ECMASCript 5.1 chapter 11.2
//  Arguments :
//    ()
//    ( ArgumentList )
//
Expressions Parser::ParseArguments() {
  // begin with (
  NextToken();

  // () 
  if (token_.type == TokenType::RIGHT_PAREN) {
    NextToken();
    return {}; 
  }

  auto args = ParseArgumentList(TokenType::RIGHT_PAREN);

  if (token_.type != TokenType::RIGHT_PAREN) {
    ThrowSyntaxError("expects a ')'");
  }
  NextToken();

  return args;
}

// Parse ArgumentList
// Defined in ECMAScript 5.1 chapter 11.2
//  ArgumentList :
//    AssignmentExpression
//    ArgumentList , AssignmentExpression
Expressions Parser::ParseArgumentList(TokenType end_token_type) {
  Expressions args;
  args.push_back(ParseAssignmentExpression());
  while (token_.type != end_token_type) {
    if (token_.type != TokenType::COMMA) {
      ThrowSyntaxError("expects a ','");
    }
    NextToken();
    args.push_back(ParseAssignmentExpression());
  }
  return args;
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
