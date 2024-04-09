#include "voidjs/parser/parser.h"
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
  if (lexer_.GetToken().GetType() == TokenType::STRING &&
      (lexer_.GetToken().GetString() == uR"('use strict')" ||
       lexer_.GetToken().GetString() == uR"("use strict")")) {
    // use strict
  }
  Statements stmts;
  while (lexer_.GetToken().GetType() != TokenType::EOS) {
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
  
  Statements stmts = ParseStatementList(TokenType::RIGHT_BRACE);
  
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

  if (lexer_.GetToken().GetType() != TokenType::SEMICOLON) {
    ThrowSyntaxError("expects a ';'");
  }
  lexer_.NextToken();

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

  if (lexer_.GetToken().GetType() != TokenType::SEMICOLON) {
    ThrowSyntaxError("expects a ';'");
  }
  lexer_.NextToken();
  
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
  
  Expression* cond = ParseExpression();
  
  if (lexer_.GetToken().GetType() != TokenType::RIGHT_PAREN) {
    ThrowSyntaxError("expects a ')'");
  }
  lexer_.NextToken();

  Statement* cons = ParseStatement();

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

  if (lexer_.GetToken().GetType() != TokenType::SEMICOLON) {
    ThrowSyntaxError("expects a '('");
  }
  lexer_.NextToken();

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

Statement* Parser::ParseContinueStatement() {
  // begin with continue
  lexer_.NextToken();

  Expression* ident = nullptr;

  if (lexer_.GetToken().GetType() == TokenType::IDENTIFIER &&
      !lexer_.HasLineTerminator()) {
    ident = ParseIdentifier();
  }
  
  if (lexer_.GetToken().GetType() != TokenType::SEMICOLON) {
    ThrowSyntaxError("expects a ';'");
  }
  lexer_.NextToken();

  return new ContinueStatement(ident);
}

Statement* Parser::ParseBreakStatement() {
  // begin with break
  lexer_.NextToken();

  Expression* ident = nullptr;

  if (lexer_.GetToken().GetType() == TokenType::IDENTIFIER &&
      !lexer_.HasLineTerminator()) {
    ident = ParseIdentifier();
  }
  
  if (lexer_.GetToken().GetType() != TokenType::SEMICOLON) {
    ThrowSyntaxError("expects a ';'");
  }
  lexer_.NextToken();

  return new BreakStatement(ident);
}

Statement* Parser::ParseReturnStatement() {
  // begin with return
  lexer_.NextToken();

  Expression* expr = nullptr;

  if (lexer_.GetToken().GetType() != TokenType::SEMICOLON &&
      !lexer_.HasLineTerminator()) {
    expr = ParseExpression();
  }
  
  if (lexer_.GetToken().GetType() != TokenType::SEMICOLON) {
    ThrowSyntaxError("expects a ';'");
  }
  lexer_.NextToken();

  return new ReturnStatement(expr);
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
      return new NewExpression(callee, std::move(args));
    } else {
      return new NewExpression(callee, {});
    }
  } else {
    if (lexer_.GetToken().GetType() == TokenType::KEYWORD_FUNCTION) {
      // callee = ParseFunctionExpression();
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
        callee = new MemberExpression(callee, expr);
        break;
      }
      case TokenType::DOT: {
        lexer_.NextToken();
        if (lexer_.GetToken().IsIdentifierName()) {
          auto ident = ParseIdentifier();
          callee = new MemberExpression(callee, ident);
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

// Parse Identifier
Expression* Parser::ParseIdentifier() {
  auto ident = new Identifier(lexer_.GetToken().GetString());
  lexer_.NextToken();
  return ident;
}

// Parse StatementList
// Defined in ECMAScript 5.1 Chapter 12.1
// StatementList :
//   Statement
//   StatementList Statement
Statements Parser::ParseStatementList(TokenType end_type) {
  // Empty StatementList is permitted
  Statements stmts;
  while (lexer_.GetToken().GetType() != end_type) {
    stmts.push_back(ParseStatement());
  }
  return stmts;
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
    ThrowSyntaxError("expects a '='");
  }
  lexer_.NextToken();

  auto init = ParseAssignmentExpression(allow_in);

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

//  PropertyNameAndValueList :
//    PropertyAssignment
//    PropertyNameAndValueList , PropertyAssignment

//  PropertyAssignment :
//    PropertyName : AssignmentExpression
//    get PropertyName ( ) { FunctionBody }
//    set PropertyName ( PropertySetParameterList ) { FunctionBody }

//  PropertyName :
//    IdentifierName
//    StringLiteral
//    NumericLiteral

//  PropertySetParameterList :
//    Identifier
Expression* Parser::ParseObjectLiteral() {
  // todo
  return nullptr;
}

void Parser::ThrowSyntaxError(std::string msg) {
  throw utils::Error{utils::ErrorType::SYNTAX_ERROR, std::move(msg)};
}

}  // namespace voidjs
