#include "voidjs/parser/parser.h"

#include <iostream>

#include "voidjs/lexer/character.h"
#include "voidjs/lexer/token.h"
#include "voidjs/ir/ast.h"
#include "voidjs/ir/program.h"
#include "voidjs/ir/statement.h"
#include "voidjs/ir/expression.h"
#include "voidjs/ir/literal.h"
#include "voidjs/lexer/token_type.h"
#include "voidjs/utils/error.h"
#include "voidjs/utils/helper.h"

namespace voidjs {

using namespace ast;

// Parse Program
// Defined in ECMAScript 5.1 Chapter 14
// Program :
//   SourceElements_opt
// SourceElements :
//   SourceElement
//   SourceElements SourceElement
// SourceElement :
//   Statement
//   FunctionDeclaration
Program* Parser::ParseProgram() {
  bool is_strict = false;
  if (lexer_.GetToken().GetType() == TokenType::STRING &&
      lexer_.GetToken().GetString() == u"use strict") {
    is_strict = true;
    lexer_.NextToken();

    if (!TryAutomaticInsertSemicolon()) {
      ThrowSyntaxError("expects a ';'");
    }
  }
  
  EnterFunctionScope();
  Statements stmts;
  while (lexer_.GetToken().GetType() != TokenType::EOS) {
    try {
      if (lexer_.GetToken().GetType() == TokenType::KEYWORD_FUNCTION) {
        stmts.push_back(ParseFunctionDeclaration());
      } else {
        stmts.push_back(ParseStatement());
      }
    } catch(const utils::Error& e) {
      error_ = e;
      std::cout << e.GetMessage() << std::endl;
      return nullptr;
    }
  }
  auto [var_decls, func_decls] = ExitFunctionScope();
  return new Program(std::move(stmts), is_strict, std::move(var_decls), std::move(func_decls)); 
}

Statement* Parser::ParseStatement() {
  switch (lexer_.GetToken().GetType()) {
    case TokenType::LEFT_BRACE: {
      return ParseBlockStatement();
    }
    case TokenType::KEYWORD_VAR: {
      return ParseVariableStatement();
    }
    case TokenType::SEMICOLON: {
      return ParseEmptyStatement();
    }
    case TokenType::KEYWORD_IF: {
      return ParseIfStatement();
    }
    case TokenType::KEYWORD_DO: {
      return ParseDoWhileStatement();
    }
    case TokenType::KEYWORD_WHILE: {
      return ParseWhileStatement();
    }
    case TokenType::KEYWORD_FOR: {
      return ParseForStatement();
    }
    case TokenType::KEYWORD_CONTINUE: {
      return ParseContinueStatement();
    }
    case TokenType::KEYWORD_BREAK: {
      return ParseBreakStatement();
    }
    case TokenType::KEYWORD_RETURN: {
      return ParseReturnStatement();
    }
    case TokenType::KEYWORD_WITH: {
      return ParseWithStatement();
    }
    case TokenType::KEYWORD_SWITCH: {
      return ParseSwitchStatement();
    }
    case TokenType::KEYWORD_THROW: {
      return ParseThrowStatement();
    }
    case TokenType::KEYWORD_TRY: {
      return ParseTryStatement();
    }
    case TokenType::KEYWORD_DEBUGGER: {
      return ParseDebuggerStatement();
    }
    case TokenType::IDENTIFIER: {
      return ParsePotentialLabelledStatement();
    }
    default: {
      return ParseExpressionStatement();
    }
  }
}

// Parse BlockStatement
// Defined in ECMAScript 5.1 Chapter 12.1
// Block :
//   { StatementList_opt }
Statement* Parser::ParseBlockStatement() {
  // begin with {
  lexer_.NextToken();

  Statements stmts;

  while (lexer_.GetToken().GetType() != TokenType::RIGHT_BRACE) {
    stmts.push_back(ParseStatement());
  }
  
  if (lexer_.GetToken().GetType() != TokenType::RIGHT_BRACE) {
    ThrowSyntaxError("expects a '}'");
  }
  lexer_.NextToken();
  
  return new BlockStatement(std::move(stmts));
}

// Parse VariableStatment
// Defined in ECMAScript 5.1 chapter 12.2
// VariableStatement :
//   var VariableDeclarationList ;
Statement* Parser::ParseVariableStatement() {
  // begin with var
  lexer_.NextToken();

  auto var_stmt = new VariableStatement(ParseVariableDeclarationList());

  if (!TryAutomaticInsertSemicolon()) {
    ThrowSyntaxError("expects a ';'");
  }

  return var_stmt;
}

// Parse EmptyStatement
// Defined in ECMAScript 5.1 Chapter 12.3
// EmptyStatement :
//   ;
Statement* Parser::ParseEmptyStatement() {
  auto empty_stmt = new EmptyStatement();

  if (lexer_.GetToken().GetType() != TokenType::SEMICOLON) {
    ThrowSyntaxError("expects a ';'");
  }
  lexer_.NextToken();

  return empty_stmt;
}

// Parse ExpressionStatement
// Defined in ECMAScript 5.1 Chapter 12.4
// ExpressionStatement :
//   [lookahead ∉ {{, function}] Expression ;
Statement* Parser::ParseExpressionStatement() {
  if (auto token = lexer_.NextRewindToken();
      token.GetType() == TokenType::COMMA ||
      token.GetType() == TokenType::KEYWORD_FUNCTION) {
    ThrowSyntaxError("current token should not be ',' or function");
  }
  
  Expression* expr = ParseExpression();

  if (!TryAutomaticInsertSemicolon()) {
    ThrowSyntaxError("expects a ';'");
  }
  
  return new ExpressionStatement(expr);
}

// Parse IfStatement
// Defined in ECMAScript 5.1 chapter 12.5
// IfStatement :
//   if ( Expression ) Statement else Statement
//   if ( Expression ) Statement
Statement* Parser::ParseIfStatement() {
  // begin with if
  lexer_.NextToken();
  
  if (lexer_.GetToken().GetType() != TokenType::LEFT_PAREN) {
    ThrowSyntaxError("expects a '('");
  }
  lexer_.NextToken();
  
  auto cond = ParseExpression();
  
  if (lexer_.GetToken().GetType() != TokenType::RIGHT_PAREN) {
    ThrowSyntaxError("expects a ')'");
  }
  lexer_.NextToken();

  auto cons = ParseStatement();

  Statement* alt = nullptr;
  if (lexer_.GetToken().GetType() == TokenType::KEYWORD_ELSE) {
    lexer_.NextToken();
    alt = ParseStatement();
  }

  return new IfStatement(cond, cons, alt); 
}

// ParseDoWhileStatement
// Defined in ECMAScript 5.1 Chapter 12.6
//  IterationStatement :
//    do Statement while ( Expression );
Statement* Parser::ParseDoWhileStatement() {
  // begin with do
  lexer_.NextToken();

  auto body = ParseStatement();

  if (lexer_.GetToken().GetType() != TokenType::KEYWORD_WHILE) {
    ThrowSyntaxError("expects 'while' here");
  }
  lexer_.NextToken();

  if (lexer_.GetToken().GetType() != TokenType::LEFT_PAREN) {
    ThrowSyntaxError("expects a '('");
  }
  lexer_.NextToken();

  auto cond = ParseExpression();

  if (lexer_.GetToken().GetType() != TokenType::RIGHT_PAREN) {
    ThrowSyntaxError("expects a ')'");
  }
  lexer_.NextToken();

  if (!TryAutomaticInsertSemicolon()) {
    ThrowSyntaxError("expects a '('");
  }

  return new DoWhileStatement(cond, body);
}

// ParseWhileStatement
// Defined in ECMAScript 5.1 Chapter 12.6
//  IterationStatement :
//    while ( Expression ) Statement
Statement* Parser::ParseWhileStatement() {
  // begin with while
  lexer_.NextToken();

  if (lexer_.GetToken().GetType() != TokenType::LEFT_PAREN) {
    ThrowSyntaxError("expects a '('");
  }
  lexer_.NextToken();

  auto cond = ParseExpression();

  if (lexer_.GetToken().GetType() != TokenType::RIGHT_PAREN) {
    ThrowSyntaxError("expects a ')'");
  }
  lexer_.NextToken();

  auto body = ParseStatement();

  return new WhileStatement(cond, body);
}

// ParseForStatement
// Defined in ECMAScript 5.1 Chapter 12.6
//  IterationStatement :
//    for ( ExpressionNoIn_opt; Expression_opt ; Expression_opt ) Statement
//    for ( var VariableDeclarationListNoIn; Expression_opt ; Expression_opt ) Statement
//    for ( LeftHandSideExpression in Expression ) Statement
//    for ( var VariableDeclarationNoIn in Expression ) Statement
Statement* Parser::ParseForStatement() {
  // begin with for
  lexer_.NextToken();

  if (lexer_.GetToken().GetType() != TokenType::LEFT_PAREN) {
    ThrowSyntaxError("expects a '('");
  }
  lexer_.NextToken();

  if (lexer_.GetToken().GetType() == TokenType::KEYWORD_VAR) {
    lexer_.NextToken();
    
    auto decls = ParseVariableDeclarationList(false);  // allow_in = false

    if (lexer_.GetToken().GetType() == TokenType::KEYWORD_IN) {
      lexer_.NextToken();
      
      if (decls.size() != 1) {
        ThrowSyntaxError("invalid declartion in for-in statement");
      }
      
      auto left = decls[0];

      auto right = ParseExpression();

      if (lexer_.GetToken().GetType() != TokenType::RIGHT_PAREN) {
        ThrowSyntaxError("expects a ')'");
      }
      lexer_.NextToken();

      auto body = ParseStatement();

      return new ForInStatement(left, right, body);
    } else {
      auto init = new VariableStatement(decls);

      if (lexer_.GetToken().GetType() != TokenType::SEMICOLON) {
        ThrowSyntaxError("expects a ';'");
      }
      lexer_.NextToken();

      Expression* cond = nullptr;
      if (lexer_.GetToken().GetType() != TokenType::SEMICOLON) {
        cond = ParseExpression();
      }

      if (lexer_.GetToken().GetType() != TokenType::SEMICOLON) {
        ThrowSyntaxError("expects a ';'");
      }
      lexer_.NextToken();

      Expression* update = nullptr;
      if (lexer_.GetToken().GetType() != TokenType::RIGHT_PAREN) {
        update = ParseExpression();
      }

      if (lexer_.GetToken().GetType() != TokenType::RIGHT_PAREN) {
        ThrowSyntaxError("expects a ')'");
      }
      lexer_.NextToken();

      auto body = ParseStatement();

      return new ForStatement(init, cond, update, body);
    }
  } else {
    AstNode* init = nullptr;
    
    if (lexer_.GetToken().GetType() != TokenType::SEMICOLON) {
      auto expr = ParseExpression(false);  // allow_in = false
      
      if (lexer_.GetToken().GetType() == TokenType::KEYWORD_IN) {
        lexer_.NextToken();
      
        if (expr->IsSequenceExpression()) {
          ThrowSyntaxError("invalid expression in for-in statement");
        }
        
        auto left = expr;
        if (!left->IsLeftHandSideExpression()) {
          ThrowSyntaxError("need to be LeftHandSideExpression");
        }

        auto right = ParseExpression();

        if (lexer_.GetToken().GetType() != TokenType::RIGHT_PAREN) {
          ThrowSyntaxError("expects a ')'");
        }
        lexer_.NextToken();

        auto body = ParseStatement();

        return new ForInStatement(left, right, body);
      } else {
        init = expr;
      }
    }
    
    if (lexer_.GetToken().GetType() != TokenType::SEMICOLON) {
      ThrowSyntaxError("expects a ';'");
    }
    lexer_.NextToken();

    Expression* cond = nullptr;
    if (lexer_.GetToken().GetType() != TokenType::SEMICOLON) {
      cond = ParseExpression();
    }

    if (lexer_.GetToken().GetType() != TokenType::SEMICOLON) {
      ThrowSyntaxError("expects a ';'");
    }
    lexer_.NextToken();

    Expression* update = nullptr;
    if (lexer_.GetToken().GetType() != TokenType::RIGHT_PAREN) {
      update = ParseExpression();
    }

    if (lexer_.GetToken().GetType() != TokenType::RIGHT_PAREN) {
      ThrowSyntaxError("expects a ')'");
    }
    lexer_.NextToken();

    auto body = ParseStatement();

    return new ForStatement(init, cond, update, body);
  }
}

// Parse ContinueStatement
// Defined in ECMAScript 5.1 Chapter 12.7
//  ContinueStatement :
//    continue ;
//    continue [no LineTerminator here] Identifier;
Statement* Parser::ParseContinueStatement() {
  // begin with continue
  lexer_.NextToken();

  Expression* ident = nullptr;

  if (lexer_.GetToken().GetType() == TokenType::IDENTIFIER &&
      !lexer_.HasLineTerminator()) {
    ident = ParseIdentifier();
  }
  
  if (!TryAutomaticInsertSemicolon()) {
    ThrowSyntaxError("expects a ';'");
  }

  return new ContinueStatement(ident);
}

// Parse BreakStatement
// Defined in ECMAScript 5.1 Chapter 12.8
//  BreakStatement :
//    break ;
//    break [no LineTerminator here] Identifier ;
Statement* Parser::ParseBreakStatement() {
  // begin with break
  lexer_.NextToken();

  Expression* ident = nullptr;

  if (lexer_.GetToken().GetType() == TokenType::IDENTIFIER &&
      !lexer_.HasLineTerminator()) {
    ident = ParseIdentifier();
  }
  
  if (!TryAutomaticInsertSemicolon()) {
    ThrowSyntaxError("expects a ';'");
  }
      
  return new BreakStatement(ident);
}

// Parse ReturnStatement
// Defined in ECMAScript 5.1 Chapter 12.9
//  ReturnStatement :
//    return ;
//    return [no LineTerminator here] Expression ;
Statement* Parser::ParseReturnStatement() {
  // begin with return
  lexer_.NextToken();

  Expression* expr = nullptr;

  if (lexer_.GetToken().GetType() != TokenType::SEMICOLON &&
      !lexer_.HasLineTerminator()) {
    expr = ParseExpression();
  }
  
  if (!TryAutomaticInsertSemicolon()) {
    ThrowSyntaxError("expects a ';'");
  }

  return new ReturnStatement(expr);
}

// Parse WithStatemnet
// Defined in ECMAScript 5.1 Chapter 12.10
//  WithStatement :
//    with ( Expression ) Statement
Statement* Parser::ParseWithStatement() {
  // begin with with
  lexer_.NextToken();

  if (lexer_.GetToken().GetType() != TokenType::LEFT_PAREN) {
    ThrowSyntaxError("expects a '('");
  }
  lexer_.NextToken();

  auto ctx = ParseExpression();

  if (lexer_.GetToken().GetType() != TokenType::RIGHT_PAREN) {
    ThrowSyntaxError("expects a ')'");
  }
  lexer_.NextToken();

  auto body = ParseStatement();

  return new WithStatement(ctx, body);
}

// Parse SwitchStatement
// Defined in ECMAScript 5.1 Chapter 12.11
//  SwitchStatement :
//    switch ( Expression ) CaseBlock
Statement* Parser::ParseSwitchStatement() {
  // begin with switch
  lexer_.NextToken();

  if (lexer_.GetToken().GetType() != TokenType::LEFT_PAREN) {
    ThrowSyntaxError("expects a '('");
  }
  lexer_.NextToken();

  auto expr = ParseExpression();

  if (lexer_.GetToken().GetType() != TokenType::RIGHT_PAREN) {
    ThrowSyntaxError("expects a ')'");
  }
  lexer_.NextToken();

  auto cases = ParseCaseBlock();

  return new SwitchStatement(expr, std::move(cases));
}

// Parse LabelledStatement
// Defined in ECMAScript 5.1 Chapter 12.12
//  LabelledStatement :
//    Identifier : Statement
Statement* Parser::ParseLabelledStatement() {
  // begin with Identifier
  auto label = ParseIdentifier();

  if (lexer_.GetToken().GetType() != TokenType::COLON) {
    ThrowSyntaxError("expects a ':'"); 
  }
  lexer_.NextToken();

  auto body = ParseStatement();

  return new LabelledStatement(label, body);
}

// Parse ThrowStatement
// Defined in ECMAScript 5.1 Chapter 12.13
//  ThrowStatement :
//    throw [no LineTerminator here] Expression ;
Statement* Parser::ParseThrowStatement() {
  // begin with throw
  lexer_.NextToken();

  if (lexer_.HasLineTerminator()) {
    ThrowSyntaxError("expects no line terminator here");
  }
  auto expr = ParseExpression();

  if (!TryAutomaticInsertSemicolon()) {
    ThrowSyntaxError("expects a ';'");
  }

  return new ThrowStatement(expr);
}

// Parse TryStatement
// Defined in ECMAScript 5.1 Chapter 12.14
//  TryStatement :
//    try Block Catch
//    try Block Finally
//    try Block Catch Finally
// 
//  Catch :
//    catch ( Identifier ) Block

//  Finally :
//    finally Block
Statement* Parser::ParseTryStatement() {
  // begin with try
  lexer_.NextToken();

  if (lexer_.GetToken().GetType() != TokenType::LEFT_BRACE) {
    ThrowSyntaxError("expects a '{'");
  }
  auto body = ParseBlockStatement();

  if (lexer_.GetToken().GetType() == TokenType::KEYWORD_CATCH) {
    lexer_.NextToken();

    if (lexer_.GetToken().GetType() != TokenType::LEFT_PAREN) {
      ThrowSyntaxError("expects a '('");
    }
    lexer_.NextToken();

    if (lexer_.GetToken().GetType() != TokenType::IDENTIFIER) {
      ThrowSyntaxError("expects an identifier");
    }
    auto catch_name = ParseIdentifier();

    if (lexer_.GetToken().GetType() != TokenType::RIGHT_PAREN) {
      ThrowSyntaxError("expects a ')'");
    }
    lexer_.NextToken();
    
    if (lexer_.GetToken().GetType() != TokenType::LEFT_BRACE) {
      ThrowSyntaxError("expects a '{'");
    }
    auto catch_block = ParseBlockStatement();

    if (lexer_.GetToken().GetType() == TokenType::KEYWORD_FINALLY) {
      lexer_.NextToken();

      if (lexer_.GetToken().GetType() != TokenType::LEFT_BRACE) {
        ThrowSyntaxError("expects a '{'");
      }
      auto finally_block = ParseBlockStatement();

      return new TryStatement(body, catch_name, catch_block, finally_block);
    } else {
      return new TryStatement(body, catch_name, catch_block, nullptr);
    }
  } else if (lexer_.GetToken().GetType() == TokenType::KEYWORD_FINALLY) {
    lexer_.NextToken();
    
    if (lexer_.GetToken().GetType() != TokenType::LEFT_BRACE) {
      ThrowSyntaxError("expects a '{'");
    }
    auto finally_block = ParseBlockStatement();

    return new TryStatement(body, nullptr, nullptr, finally_block);
  } else {
    return new TryStatement(body, nullptr, nullptr, nullptr);
  }
}

// Parse DebuggerStatement
// Defined in ECMAScript 5.1 Chapter 12.15
//  DebuggerStatement :
//    debugger ;
Statement* Parser::ParseDebuggerStatement() {
  // begin with debugger
  lexer_.NextToken();

  if (!TryAutomaticInsertSemicolon()) {
    ThrowSyntaxError("expects a ';'");
  }

  return new DebuggerStatement();
}

// Parse Expression
// Defined in ECMAScript 5.1 chapter 11.14
// Expression :
//   AssignmentExpression
//   Expression , AssignmentExpression
Expression* Parser::ParseExpression(bool allow_in) {
  auto expr = ParseAssignmentExpression(allow_in);
  if (lexer_.GetToken().GetType() == TokenType::COMMA) {
    Expressions exprs;
    exprs.push_back(expr);

    while (lexer_.GetToken().GetType() == TokenType::COMMA) {
      lexer_.NextToken();

      exprs.push_back(ParseAssignmentExpression(allow_in));
    }

    return new SequenceExpression(std::move(exprs));
  } else {
    return expr;
  }
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
  switch (lexer_.GetToken().GetType()) {
    case TokenType::KEYWORD_THIS: {
      auto expr = new This();
      lexer_.NextToken();
      return expr;
    }
    case TokenType::IDENTIFIER: {
      auto ident = ParseIdentifier();
      return ident;
    }
    case TokenType::NULL_LITERAL: {
      auto null = new NullLiteral();
      lexer_.NextToken();
      return null;
    }
    case TokenType::TRUE:
    case TokenType::FALSE: {
      auto boolean = new BooleanLiteral(lexer_.GetToken().GetType() == TokenType::TRUE);
      lexer_.NextToken();
      return boolean;
    }
    case TokenType::NUMBER: {
      auto num = new NumericLiteral(lexer_.GetToken().GetNumber());
      lexer_.NextToken();
      return num;
    }
    case TokenType::STRING: {
      auto str = new StringLiteral(lexer_.GetToken().GetString());
      lexer_.NextToken();
      return str;
    }
    case TokenType::LEFT_BRACKET: {
      auto arr = ParseArrayLiteral();
      return arr; 
    }
    case TokenType::LEFT_BRACE: {
      auto obj = ParseObjectLiteral();
      return obj;
    }
    case TokenType::LEFT_PAREN: {
      lexer_.NextToken();
      auto expr = ParseExpression();
      if (lexer_.GetToken().GetType() != TokenType::RIGHT_PAREN) {
        ThrowSyntaxError("expects a ')'");
      }
      lexer_.NextToken();
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
  return ParseMemberExpression();
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
  if (lexer_.GetToken().GetType() == TokenType::KEYWORD_NEW) {
    lexer_.NextToken();
    callee = ParseMemberExpression(true);
    if (lexer_.GetToken().GetType() == TokenType::LEFT_PAREN) {
      auto args = ParseArguments();
      callee = new NewExpression(callee, std::move(args));
    } else {
      callee = new NewExpression(callee, {});
    }
  } else {
    if (lexer_.GetToken().GetType() == TokenType::KEYWORD_FUNCTION) {
      callee = ParseFunctionExpression();
    } else {
      callee = ParsePrimaryExpression();
    }
  }

  while (true) {
    switch (lexer_.GetToken().GetType()) {
      case TokenType::LEFT_BRACKET: {
        lexer_.NextToken();
        auto expr = ParseExpression();
        if (lexer_.GetToken().GetType() != TokenType::RIGHT_BRACKET) {
          ThrowSyntaxError("expects a ']'");
        }
        lexer_.NextToken();
        callee = new MemberExpression(callee, expr, false);
        break;
      }
      case TokenType::DOT: {
        lexer_.NextToken();
        if (lexer_.GetToken().IsIdentifierName()) {
          auto ident = ParseIdentifier();
          callee = new MemberExpression(callee, ident, true);
        } else {
          ThrowSyntaxError("expects identifier_name");
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

// Parse PostfixExpression
// Defined in ECMASCript 5.1 chapter 11.3
//  PostfixExpression :
//    LeftHandSideExpression
//    LeftHandSideExpression [no LineTerminator here] ++
//    LeftHandSideExpression [no LineTerminator here] --
Expression* Parser::ParsePostfixExpression() {
  auto lhs = ParseLeftHandSideExpression();
  if (!lexer_.HasLineTerminator() &&
      (lexer_.GetToken().GetType() == TokenType::INC || lexer_.GetToken().GetType() == TokenType::DEC)) {
    auto type = lexer_.GetToken().GetType();
    lexer_.NextToken();
    return new PostfixExpression(type, lhs);
  } else {
    return lhs;
  }
}

// Parse UnaryExpression
// Defined in ECMASCript 5.1 chapter 11.4
//  UnaryExpression :
//    PostfixExpression
//    delete UnaryExpression
//    void UnaryExpression
//    typeof UnaryExpression
//    ++ UnaryExpression
//    -- UnaryExpression
//    + UnaryExpression
//    - UnaryExpression
//    ~ UnaryExpression
//    ! UnaryExpression
Expression* Parser::ParseUnaryExpression() {
  if (lexer_.GetToken().GetType() == TokenType::KEYWORD_DELETE ||
      lexer_.GetToken().GetType() == TokenType::KEYWORD_VOID   ||
      lexer_.GetToken().GetType() == TokenType::KEYWORD_TYPEOF ||
      lexer_.GetToken().GetType() == TokenType::INC            ||
      lexer_.GetToken().GetType() == TokenType::DEC            ||
      lexer_.GetToken().GetType() == TokenType::ADD            ||
      lexer_.GetToken().GetType() == TokenType::SUB            ||
      lexer_.GetToken().GetType() == TokenType::BIT_NOT        ||
      lexer_.GetToken().GetType() == TokenType::LOGICAL_NOT) {
    auto type = lexer_.GetToken().GetType();
    lexer_.NextToken();
    return new UnaryExpression(type, ParseUnaryExpression());
  } else {
    return ParsePostfixExpression();
  }
}

// Parse BinaryExpression
// Here we don't use the traditional recursive descent method;
// instead, we use Pratt Parsing to parse BinaryExpression,
// which is a collection of the following kinds of Expression.
//  MultiplicativeExpression :
//    UnaryExpression
//    MultiplicativeExpression * UnaryExpression
//    MultiplicativeExpression / UnaryExpression
//    MultiplicativeExpression % UnaryExpression
// 
//  AdditiveExpression :
//    MultiplicativeExpression
//    AdditiveExpression + MultiplicativeExpression
//    AdditiveExpression - MultiplicativeExpression
// 
//  ShiftExpression :
//    AdditiveExpression
//    ShiftExpression << AdditiveExpression
//    ShiftExpression >> AdditiveExpression
//    ShiftExpression >>> AdditiveExpression
// 
//  RelationalExpression :
//    ShiftExpression
//    RelationalExpression < ShiftExpression
//    RelationalExpression > ShiftExpression
//    RelationalExpression <= ShiftExpression
//    RelationalExpression >= ShiftExpression
//    RelationalExpression instanceof ShiftExpression
//    RelationalExpression in ShiftExpression
// 
//  EqualityExpression :
//    RelationalExpression
//    EqualityExpression == RelationalExpression
//    EqualityExpression != RelationalExpression
//    EqualityExpression === RelationalExpression
//    EqualityExpression !== RelationalExpression
// 
//  BitwiseANDExpression :
//    EqualityExpression
//    BitwiseANDExpression & EqualityExpression
// 
//  BitwiseXORExpression :
//    BitwiseANDExpression
//    BitwiseXORExpression ^ BitwiseANDExpression
// 
//  BitwiseORExpression :
//    BitwiseXORExpression
//    BitwiseORExpression | BitwiseXORExpression

//  LogicalANDExpression :
//    BitwiseORExpression
//    LogicalANDExpression && BitwiseORExpression

//  LogicalORExpression :
//    LogicalANDExpression
//    LogicalORExpression || LogicalANDExpression
Expression* Parser::ParseBinaryExpression(bool allow_in, std::int32_t precedence) {
  auto left = ParseUnaryExpression();

  while (true) {
    auto token = lexer_.GetToken();
    if (!token.IsBinaryOperator(allow_in) ||
        token.GetPrecedence() <= precedence) {
      break;
    }
    lexer_.NextToken();
    auto right = ParseBinaryExpression(allow_in, token.GetPrecedence());
    left = new BinaryExpression(token.GetType(), left, right);
  }
  return left;
}

// Parse ConditionalExpression
// Defined in ECMAScript 5.1 Chapter 11.12
//  ConditionalExpression :
//    LogicalORExpression
//    LogicalORExpression ? AssignmentExpression : AssignmentExpression
Expression* Parser::ParseConditionalExpression(bool allow_in) {
  auto cond = ParseBinaryExpression(allow_in);
  if (lexer_.GetToken().GetType() == TokenType::QUESTION) {
    lexer_.NextToken();
    
    auto cons = ParseAssignmentExpression(allow_in);
    
    if (lexer_.GetToken().GetType() != TokenType::COLON) {
      ThrowSyntaxError("expects a ':'");
    }
    lexer_.NextToken();
    
    auto alt = ParseAssignmentExpression(allow_in);
    
    return new ConditionalExpression(cond, cons, alt);
  } else {
    return cond;
  }
}

// Parse AssignmentExpression
// Defined in ECMAScript 5.1 Chapter 11.13
//  AssignmentExpression :
//    ConditionalExpression
//    LeftHandSideExpression AssignmentOperator AssignmentExpression
Expression* Parser::ParseAssignmentExpression(bool allow_in) {
  auto left = ParseConditionalExpression(allow_in);

  if (!lexer_.GetToken().IsAssignmentOperator()) {
    return left;
  } else {
    if (!left->IsLeftHandSideExpression()) {
      ThrowSyntaxError("invalid assignment left-hand side");
    } else {
      auto type = lexer_.GetToken().GetType();
    
      if (!lexer_.GetToken().IsAssignmentOperator()) {
        ThrowSyntaxError("expects an assignment operator");
      }
      lexer_.NextToken();
    
      auto right = ParseAssignmentExpression(allow_in);

      return new AssignmentExpression(type, left, right);
    }
  }
  return nullptr;
}

// Parse FunctionExpression
// Defined in ECMAScript 5.1 Chapter 13
//  FunctionExpression :
//    function Identifieropt ( FormalParameterList_opt ) { FunctionBody }
Expression* Parser::ParseFunctionExpression() {
  // begin with function
  lexer_.NextToken();

  Expression* ident = nullptr;
  if (lexer_.GetToken().GetType() == TokenType::IDENTIFIER) {
    ident = ParseIdentifier();
  }

  if (lexer_.GetToken().GetType() != TokenType::LEFT_PAREN) {
    ThrowSyntaxError("expects a '('");
  }
  lexer_.NextToken();

  Expressions params;
  if (lexer_.GetToken().GetType() != TokenType::RIGHT_PAREN) {
    params = ParseFormalParameterList();
  }
  
  if (lexer_.GetToken().GetType() != TokenType::RIGHT_PAREN) {
    ThrowSyntaxError("expects a ')'");
  }
  lexer_.NextToken();

  if (lexer_.GetToken().GetType() != TokenType::LEFT_BRACE) {
    ThrowSyntaxError("expects a '{'");
  }
  lexer_.NextToken();

  bool is_strict = false;
  if (lexer_.GetToken().GetType() == TokenType::STRING &&
      lexer_.GetToken().GetString() == u"use strict") {
    is_strict = true;
    lexer_.NextToken();
    
    if (!TryAutomaticInsertSemicolon()) {
      ThrowSyntaxError("expects a ';'");
    }
  }

  EnterFunctionScope();
  Statements stmts;
  while (lexer_.GetToken().GetType() != TokenType::RIGHT_BRACE) {
    if (lexer_.GetToken().GetType() == TokenType::KEYWORD_FUNCTION) {
      stmts.push_back(ParseFunctionDeclaration());
    } else {
      stmts.push_back(ParseStatement());
    }
  }

  if (lexer_.GetToken().GetType() != TokenType::RIGHT_BRACE) {
    ThrowSyntaxError("expects a '}'");
  }
  lexer_.NextToken();

  auto [var_decls, func_decls] = ExitFunctionScope();
  return new FunctionExpression(ident, std::move(params), std::move(stmts), is_strict,
                                std::move(var_decls), std::move(func_decls));
}

// Parse Identifier
Expression* Parser::ParseIdentifier() {
  auto ident = new Identifier(lexer_.GetToken().GetString());
  lexer_.NextToken();
  return ident;
}

// Parse VariableDeclarationList
// Defined in ECMAScript 5.1 chapter 12.2
// VariableDeclarationList :
//   VariableDeclaration
//   VariableDeclarationList , VariableDeclaration
VariableDeclarations Parser::ParseVariableDeclarationList(bool allow_in) {
  VariableDeclarations var_decls;
  var_decls.push_back(ParseVariableDeclaration(allow_in));
  while (lexer_.GetToken().GetType() == TokenType::COMMA) {
    lexer_.NextToken();
    var_decls.push_back(ParseVariableDeclaration());
  }
  return var_decls;
}

// Parse VariableDeclaration
// Defined in ECMAScript 5.1 chapter 12.2
// 
//  VariableDeclaration :
//    Identifier Initialiser_opt
//
//  Initialiser :
//    = AssignmentExpression
VariableDeclaration* Parser::ParseVariableDeclaration(bool allow_in) {
  if (lexer_.GetToken().GetType() != TokenType::IDENTIFIER) {
    ThrowSyntaxError("expects an identifier");
  }  
  auto ident = ParseIdentifier();

  if (lexer_.GetToken().GetType() != TokenType::ASSIGN) {
    auto var_decl = new VariableDeclaration(ident, nullptr);
    AddVariableDeclaration(var_decl);
    return var_decl;
  }
  lexer_.NextToken();

  auto init = ParseAssignmentExpression(allow_in);

  auto var_decl = new VariableDeclaration(ident, init);
  AddVariableDeclaration(var_decl);
  return var_decl;
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
  lexer_.NextToken();

  Expressions exprs;
  
  while (lexer_.GetToken().GetType() != TokenType::RIGHT_BRACKET) {
    if (lexer_.GetToken().GetType() == TokenType::COMMA) {
      lexer_.NextToken();
      
      if (lexer_.GetToken().GetType() == TokenType::COMMA) {
        exprs.push_back(nullptr);
      }
    } else {
      exprs.push_back(ParseAssignmentExpression());
    }
  }

  if (!exprs.empty() && exprs.back() == nullptr) {
    exprs.pop_back();
  }

  if (lexer_.GetToken().GetType() != TokenType::RIGHT_BRACKET) {
    ThrowSyntaxError("expects a ']'");
  }
  lexer_.NextToken();

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
  lexer_.NextToken();

  // () 
  if (lexer_.GetToken().GetType() == TokenType::RIGHT_PAREN) {
    lexer_.NextToken();
    return {}; 
  }

  auto args = ParseArgumentList(TokenType::RIGHT_PAREN);

  if (lexer_.GetToken().GetType() != TokenType::RIGHT_PAREN) {
    ThrowSyntaxError("expects a ')'");
  }
  lexer_.NextToken();

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
  while (lexer_.GetToken().GetType() != end_token_type) {
    if (lexer_.GetToken().GetType() != TokenType::COMMA) {
      ThrowSyntaxError("expects a ','");
    }
    lexer_.NextToken();
    args.push_back(ParseAssignmentExpression());
  }
  return args;
}

// Parse ObjectLiteral
// Defined in ECMAScript 5.1 Chapter 11.1.5
//  ObjectLiteral :
//    { }
//    { PropertyNameAndValueList }
//    { PropertyNameAndValueList , }
Expression* Parser::ParseObjectLiteral() {
  // begin with {
  lexer_.NextToken();

  Properties props;
  if (lexer_.GetToken().GetType() != TokenType::RIGHT_BRACE) {
    props = ParsePropertyNameAndValueList();
  }
  
  // Extra comma is consumed by ParsePropertyNameAndValueList()
  // if (lexer_.GetToken().GetType() == TokenType::COMMA) {
  //   lexer_.NextToken();
  // }

  if (lexer_.GetToken().GetType() != TokenType::RIGHT_BRACE) {
    ThrowSyntaxError("expects a '}'");
  }
  lexer_.NextToken();

  return new ObjectLiteral(std::move(props));
}

// Parse CaseBlock
// Defined in ECMAScript 5.1 Chapter 12.11
//  CaseBlock :
//    { CaseClauses_opt }
//    { CaseClauses_opt DefaultClause CaseClauses_opt }
CaseClauses Parser::ParseCaseBlock() {
  if (lexer_.GetToken().GetType() != TokenType::LEFT_BRACE) {
    ThrowSyntaxError("expects a '{'");
  }
  lexer_.NextToken();

  CaseClauses cases;

  if (lexer_.GetToken().GetType() == TokenType::KEYWORD_CASE ||
      lexer_.GetToken().GetType() == TokenType::KEYWORD_DEFAULT) {
    cases = ParseCaseClauses();
  }

  if (lexer_.GetToken().GetType() != TokenType::RIGHT_BRACE) {
    ThrowSyntaxError("expects a '}'");
  }
  lexer_.NextToken();

  return cases;
}

// Parse CaseClauses
// Defined in ECMAScript 5.1 Chapter 12.11
//  CaseClauses :
//    CaseClause
//    CaseClauses CaseClause
CaseClauses Parser::ParseCaseClauses() {
  CaseClauses cases;
  while (lexer_.GetToken().GetType() == TokenType::KEYWORD_CASE ||
         lexer_.GetToken().GetType() == TokenType::KEYWORD_DEFAULT) {
    cases.push_back(ParseCaseClause());
  }
  return cases;
}

// ParseCaseClause
// Defined in ECMAScript 5.1 Chapter 12.11
//  CaseClause :
//    case Expression : StatementListopt
//  DefaultClause :
//    default : StatementListopt
CaseClause* Parser::ParseCaseClause() {
  if (lexer_.GetToken().GetType() == TokenType::KEYWORD_CASE) {
    lexer_.NextToken();
    
    auto cond = ParseExpression();

    if (lexer_.GetToken().GetType() != TokenType::COLON) {
      ThrowSyntaxError("expects a ':'");
    }
    lexer_.NextToken();

    Statements stmts;

    while (lexer_.GetToken().GetType() != TokenType::KEYWORD_CASE &&
           lexer_.GetToken().GetType() != TokenType::KEYWORD_DEFAULT &&
           lexer_.GetToken().GetType() != TokenType::RIGHT_BRACE) {
      stmts.push_back(ParseStatement());
    }

    return new CaseClause(cond, std::move(stmts));
  } else if (lexer_.GetToken().GetType() == TokenType::KEYWORD_DEFAULT) {
    lexer_.NextToken();

    if (lexer_.GetToken().GetType() != TokenType::COLON) {
      ThrowSyntaxError("expects a ':'");
    }
    lexer_.NextToken();

    Statements stmts;

    while (lexer_.GetToken().GetType() != TokenType::KEYWORD_CASE &&
           lexer_.GetToken().GetType() != TokenType::KEYWORD_DEFAULT &&
           lexer_.GetToken().GetType() != TokenType::RIGHT_BRACE) {
      stmts.push_back(ParseStatement());
    }

    return new CaseClause(nullptr, std::move(stmts));
  }
  return nullptr;
}

// Parse Potential LabelledStatement
Statement* Parser::ParsePotentialLabelledStatement() {
  if (lexer_.NextRewindToken().GetType() == TokenType::COLON) {
    return ParseLabelledStatement();
  } else {
    return ParseExpressionStatement();
  }
}

// Parse FunctionDeclaration
// Parse FunctionExpression
// Defined in ECMAScript 5.1 Chapter 13
//  FunctionExpression :
//    function Identifieropt ( FormalParameterList_opt ) { FunctionBody }
Statement* Parser::ParseFunctionDeclaration() {
  // begin with function
  lexer_.NextToken();

  if (lexer_.GetToken().GetType() != TokenType::IDENTIFIER) {
    ThrowSyntaxError("expects an identifier");
  }
  auto ident = ParseIdentifier();

  if (lexer_.GetToken().GetType() != TokenType::LEFT_PAREN) {
    ThrowSyntaxError("expects a '('");
  }
  lexer_.NextToken();

  Expressions params;
  if (lexer_.GetToken().GetType() != TokenType::RIGHT_PAREN) {
    params = ParseFormalParameterList();
  }
  
  if (lexer_.GetToken().GetType() != TokenType::RIGHT_PAREN) {
    ThrowSyntaxError("expects a ')'");
  }
  lexer_.NextToken();

  if (lexer_.GetToken().GetType() != TokenType::LEFT_BRACE) {
    ThrowSyntaxError("expects a '{'");
  }
  lexer_.NextToken();

  bool is_strict = false;
  if (lexer_.GetToken().GetType() == TokenType::STRING &&
      lexer_.GetToken().GetString() == u"use strict") {
    is_strict = true;
    lexer_.NextToken();
    
    if (!TryAutomaticInsertSemicolon()) {
      ThrowSyntaxError("expects a ';'");
    }
  }

  EnterFunctionScope();
  Statements stmts;
  while (lexer_.GetToken().GetType() != TokenType::RIGHT_BRACE) {
    if (lexer_.GetToken().GetType() == TokenType::KEYWORD_FUNCTION) {
      stmts.push_back(ParseFunctionDeclaration());
    } else {
      stmts.push_back(ParseStatement());
    }
  }

  if (lexer_.GetToken().GetType() != TokenType::RIGHT_BRACE) {
    ThrowSyntaxError("expects a '}'");
  }
  lexer_.NextToken();

  auto [var_decls, func_decls] = ExitFunctionScope();
  auto func_decl = new FunctionDeclaration(ident, std::move(params), std::move(stmts), is_strict,
                                           std::move(var_decls), std::move(func_decls));
  AddFunctionDeclaration(func_decl);
  return func_decl;
}

// Parse FormalParameterList
// Defined in ECMAScript 5.1 Chapter 13
//  FormalParameterList :
//    Identifier
//    FormalParameterList , Identifier
Expressions Parser::ParseFormalParameterList() {
  Expressions idents;
  
  idents.push_back(ParseIdentifier());

  while (lexer_.GetToken().GetType() == TokenType::COMMA) {
    lexer_.NextToken();

    idents.push_back(ParseIdentifier());
  }

  return idents;
}

// Parse PropertyNameAndValueList
// Defined in ECMAScript 5.1 Chapter 11.1.5
//  PropertyNameAndValueList :
//    PropertyAssignment
//    PropertyNameAndValueList , PropertyAssignment
Properties Parser::ParsePropertyNameAndValueList() {
  Properties props;

  props.push_back(ParsePropertyAssignment());

  while (lexer_.GetToken().GetType() == TokenType::COMMA) {
    lexer_.NextToken();

    if (lexer_.GetToken().GetType() == TokenType::RIGHT_BRACE) {
      break;
    }
    
    props.push_back(ParsePropertyAssignment());
  }

  return props;
}

// Parse PropertyAssignment
//  PropertyAssignment :
//    PropertyName : AssignmentExpression
//    get PropertyName ( ) { FunctionBody }
//    set PropertyName ( PropertySetParameterList ) { FunctionBody }
Property* Parser::ParsePropertyAssignment() {
  if (lexer_.GetToken().GetType() == TokenType::IDENTIFIER &&
      lexer_.GetToken().GetString() == u"get") {
    auto type = PropertyType::GET;
    lexer_.NextToken();

    if (const auto& token = lexer_.GetToken();
        !token.IsIdentifierName()            &&
        token.GetType() != TokenType::NUMBER &&
        token.GetType() != TokenType::STRING) {
      ThrowSyntaxError("invalid token");
    }
    auto key = ParsePropertyName();

    if (lexer_.GetToken().GetType() != TokenType::LEFT_PAREN) {
      ThrowSyntaxError("expects a '('");
    }
    lexer_.NextToken();

    if (lexer_.GetToken().GetType() != TokenType::RIGHT_PAREN) {
      ThrowSyntaxError("expects a ')'");
    }
    lexer_.NextToken();

    if (lexer_.GetToken().GetType() != TokenType::LEFT_BRACE) {
      ThrowSyntaxError("expects a '{'");
    }
    lexer_.NextToken();

    bool is_strict = false;
    if (lexer_.GetToken().GetType() == TokenType::STRING &&
        lexer_.GetToken().GetString() == u"use strict") {
      is_strict = true;
      lexer_.NextToken();
    
      if (!TryAutomaticInsertSemicolon()) {
        ThrowSyntaxError("expects a ';'");
      }
    }

    EnterFunctionScope();
    Statements stmts;
    while (lexer_.GetToken().GetType() != TokenType::RIGHT_BRACE) {
      if (lexer_.GetToken().GetType() == TokenType::KEYWORD_FUNCTION) {
        stmts.push_back(ParseFunctionDeclaration());
      } else {
        stmts.push_back(ParseStatement());
      }
    }

    if (lexer_.GetToken().GetType() != TokenType::RIGHT_BRACE) {
      ThrowSyntaxError("expects a '}'");
    }
    lexer_.NextToken();
    
    auto [var_decls, func_decls] = ExitFunctionScope();

    auto value = new FunctionExpression(nullptr, Expressions{}, std::move(stmts), is_strict,
                                        std::move(var_decls), std::move(func_decls));

    return new Property(type, key, value);
  } else if (lexer_.GetToken().GetType() == TokenType::IDENTIFIER &&
             lexer_.GetToken().GetString() == u"set") {
    auto type = PropertyType::SET;
    lexer_.NextToken();

    if (const auto& token = lexer_.GetToken();
        !token.IsIdentifierName()            &&
        token.GetType() != TokenType::NUMBER &&
        token.GetType() != TokenType::STRING) {
      ThrowSyntaxError("invalid token");
    }
    auto key = ParsePropertyName();

    if (lexer_.GetToken().GetType() != TokenType::LEFT_PAREN) {
      ThrowSyntaxError("expects a '('");
    }
    lexer_.NextToken();

    if (lexer_.GetToken().GetType() != TokenType::IDENTIFIER) {
      ThrowSyntaxError("expects an identifier");
    }
    Expressions params;
    params.push_back(ParseIdentifier());

    if (lexer_.GetToken().GetType() != TokenType::RIGHT_PAREN) {
      ThrowSyntaxError("expects a ')'");
    }
    lexer_.NextToken();
    
    if (lexer_.GetToken().GetType() != TokenType::LEFT_BRACE) {
      ThrowSyntaxError("expects a '{'");
    }
    lexer_.NextToken();

    bool is_strict = false;
    if (lexer_.GetToken().GetType() == TokenType::STRING &&
        lexer_.GetToken().GetString() == u"use strict") {
      is_strict = true;
      lexer_.NextToken();
    
      if (lexer_.GetToken().GetType() != TokenType::SEMICOLON) {
        ThrowSyntaxError("expects a ';'");
      }
      lexer_.NextToken();
    }

    EnterFunctionScope();
    Statements stmts;
    while (lexer_.GetToken().GetType() != TokenType::RIGHT_BRACE) {
      if (lexer_.GetToken().GetType() == TokenType::KEYWORD_FUNCTION) {
        stmts.push_back(ParseFunctionDeclaration());
      } else {
        stmts.push_back(ParseStatement());
      }
    }

    if (lexer_.GetToken().GetType() != TokenType::RIGHT_BRACE) {
      ThrowSyntaxError("expects a '}'");
    }
    lexer_.NextToken();

    auto [var_decls, func_decls] = ExitFunctionScope();
      
    auto value = new FunctionExpression(nullptr, std::move(params), std::move(stmts), is_strict,
                                        std::move(var_decls), std::move(func_decls));

    return new Property(type, key, value);
  } else {
    auto type = PropertyType::INIT;

    if (const auto& token = lexer_.GetToken();
        !token.IsIdentifierName()            &&
        token.GetType() != TokenType::NUMBER &&
        token.GetType() != TokenType::STRING) {
      ThrowSyntaxError("invalid token");
    }
    auto key = ParsePropertyName();

    if (lexer_.GetToken().GetType() != TokenType::COLON) {
      ThrowSyntaxError("expects a ':'");
    }
    lexer_.NextToken();

    auto value = ParseAssignmentExpression();

    return new Property(type, key, value);
  }
}
// Parse PropertyName
//  PropertyName :
//    IdentifierName
//    StringLiteral
//    NumericLiteral
Expression* Parser::ParsePropertyName() {
  Expression* key = nullptr;
  if (lexer_.GetToken().IsIdentifierName()) {
    key = new Identifier(lexer_.GetToken().GetString());
    lexer_.NextToken();
  } else if (lexer_.GetToken().GetType() == TokenType::NUMBER) {
    key = new NumericLiteral(lexer_.GetToken().GetNumber());
    lexer_.NextToken();
  } else if (lexer_.GetToken().GetType() == TokenType::STRING) {
    key = new StringLiteral(lexer_.GetToken().GetString());
    lexer_.NextToken();
  }
  return key;
}

void Parser::ThrowSyntaxError(std::string msg) {
  throw utils::Error{utils::ErrorType::SYNTAX_ERROR, std::move(msg)};
}

bool Parser::TryAutomaticInsertSemicolon() {
  if (lexer_.GetToken().GetType() == TokenType::SEMICOLON) {
    lexer_.NextToken();
    return true;
  }
  
  if (lexer_.HasLineTerminator()                            ||
      lexer_.GetToken().GetType() == TokenType::RIGHT_BRACE ||
      lexer_.GetToken().GetType() == TokenType::EOS) {
    return true;
  } else {
    return false;
  }
}

void Parser::EnterFunctionScope() {
  function_scode_infos_.emplace_back();
}

void Parser::AddVariableDeclaration(VariableDeclaration* var_decl) {
  auto& func = function_scode_infos_.back();
  func.variable_declarations.push_back(var_decl);
}

void Parser::AddFunctionDeclaration(FunctionDeclaration* func_decl) {
  auto& func = function_scode_infos_.back();
  func.function_declarations.push_back(func_decl);
}

Parser::FunctionScopeInfo Parser::ExitFunctionScope() {
  auto info = function_scode_infos_.back();
  function_scode_infos_.pop_back();
  return info;
}

}  // namespace voidjs
