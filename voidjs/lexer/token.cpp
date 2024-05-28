#include <cmath>

#include "voidjs/lexer/token.h"
#include "voidjs/lexer/character.h"
#include "voidjs/lexer/token_type.h"

namespace voidjs {

std::int32_t Token::GetPrecedence() const {
  switch (type_) {
    case TokenType::LOGICAL_OR: {
      return 1;
    }
    case TokenType::LOGICAL_AND: {  
      return 2; 
    }
    case TokenType::BIT_OR: {
      return 3;
    }
    case TokenType::BIT_XOR: {
      return 4; 
    }
    case TokenType::BIT_AND: {
      return 5;
    }
    case TokenType::EQUAL:
    case TokenType::NOT_EQUAL:
    case TokenType::STRICT_EQUAL:
    case TokenType::NOT_STRICT_EQUAL: {
      return 6;
    }
    case TokenType::LESS_THAN:
    case TokenType::LESS_EQUAL:
    case TokenType::GREATER_THAN:
    case TokenType::GREATER_EQUAL:
    case TokenType::KEYWORD_INSTANCEOF:
    case TokenType::KEYWORD_IN: {
      return 7;
    }
    case TokenType::LEFT_SHIFT:
    case TokenType::RIGHT_SHIFT:
    case TokenType::U_RIGHT_SHIFT: {
      return 8;
    }
    case TokenType::ADD:
    case TokenType::SUB: {
      return 9;
    }
    case TokenType::MUL:
    case TokenType::DIV:
    case TokenType::MOD: {
      return 10;
    }
    default: {
      return 0;
    }
  }
}

bool Token::IsKeyword() const {
  switch (type_) {
    case TokenType::KEYWORD_BREAK:
    case TokenType::KEYWORD_DO:
    case TokenType::KEYWORD_INSTANCEOF:
    case TokenType::KEYWORD_TYPEOF:
    case TokenType::KEYWORD_CASE:
    case TokenType::KEYWORD_ELSE:
    case TokenType::KEYWORD_NEW:
    case TokenType::KEYWORD_VAR:
    case TokenType::KEYWORD_CATCH:
    case TokenType::KEYWORD_FINALLY:
    case TokenType::KEYWORD_RETURN:
    case TokenType::KEYWORD_VOID:
    case TokenType::KEYWORD_CONTINUE:
    case TokenType::KEYWORD_FOR:
    case TokenType::KEYWORD_SWITCH:
    case TokenType::KEYWORD_WHILE:
    case TokenType::KEYWORD_DEBUGGER:
    case TokenType::KEYWORD_FUNCTION:
    case TokenType::KEYWORD_THIS:
    case TokenType::KEYWORD_WITH:
    case TokenType::KEYWORD_DEFAULT:
    case TokenType::KEYWORD_IF:
    case TokenType::KEYWORD_THROW:
    case TokenType::KEYWORD_DELETE:
    case TokenType::KEYWORD_IN:
    case TokenType::KEYWORD_TRY: {
      return true;
    }
    default: {
      return false;
    }
  }
}

bool Token::IsIdentifierName() const {
  return
    type_ == TokenType::IDENTIFIER ||
    type_ == TokenType::FUTURE_RESERVED_WORD ||
    type_ == TokenType::STRICT_MODE_FUTURE_RESERVED_WORD ||
    IsKeyword();
}

bool Token::IsBinaryOperator(bool allow_in) const {
  switch (type_) {
    case TokenType::LOGICAL_OR:
    case TokenType::LOGICAL_AND:
    case TokenType::BIT_OR:
    case TokenType::BIT_XOR:
    case TokenType::BIT_AND:
    case TokenType::EQUAL:
    case TokenType::NOT_EQUAL:
    case TokenType::STRICT_EQUAL:
    case TokenType::NOT_STRICT_EQUAL:
    case TokenType::LESS_THAN:
    case TokenType::LESS_EQUAL:
    case TokenType::GREATER_THAN:
    case TokenType::GREATER_EQUAL:
    case TokenType::KEYWORD_INSTANCEOF:
    case TokenType::LEFT_SHIFT:
    case TokenType::RIGHT_SHIFT:
    case TokenType::U_RIGHT_SHIFT:
    case TokenType::ADD:
    case TokenType::SUB:
    case TokenType::MUL:
    case TokenType::DIV:
    case TokenType::MOD: {
      return true;
    }
    case TokenType::KEYWORD_IN: {
      return allow_in;
    }
    default: {
      return false;
    }
  }
}

bool Token::IsAssignmentOperator() const {
  switch (type_) {
    case TokenType::ASSIGN:
    case TokenType::MUL_ASSIGN:
    case TokenType::DIV_ASSIGN:
    case TokenType::MOD_ASSIGN:
    case TokenType::ADD_ASSIGN:
    case TokenType::SUB_ASSIGN:
    case TokenType::LEFT_SHIFT_ASSIGN:
    case TokenType::RIGHT_SHIFT_ASSIGN:
    case TokenType::U_RIGHT_SHIFT_ASSIGN:
    case TokenType::BIT_AND_ASSIGN:
    case TokenType::BIT_XOR_ASSIGN:
    case TokenType::BIT_OR_ASSIGN: {
      return true;
    }
    default: {
      return false;
    }
  }
}

bool Token::HasString() const {
  return
    type_ == TokenType::IDENTIFIER ||
    type_ == TokenType::FUTURE_RESERVED_WORD ||
    type_ == TokenType::STRICT_MODE_FUTURE_RESERVED_WORD ||
    type_ == TokenType::STRING;
}

std::string_view Token::TokenTypeToString(TokenType type) {
  switch (type) {
    case TokenType::KEYWORD_BREAK: {
      return "break";
    }
    case TokenType::KEYWORD_DO: {
      return "do";
    }
    case TokenType::KEYWORD_INSTANCEOF: {
      return "instanceof";
    }
    case TokenType::KEYWORD_TYPEOF: {
      return "typeof";
    }
    case TokenType::KEYWORD_CASE: {
      return "case";
    }
    case TokenType::KEYWORD_ELSE: {
      return "else";
    }
    case TokenType::KEYWORD_NEW: {
      return "new";
    }
    case TokenType::KEYWORD_VAR: {
      return "var";
    }
    case TokenType::KEYWORD_CATCH: {
      return "catch";
    }
    case TokenType::KEYWORD_FINALLY: {
      return "finally";
    }
    case TokenType::KEYWORD_RETURN: {
      return "return";
    }
    case TokenType::KEYWORD_VOID: {
      return "void";
    }
    case TokenType::KEYWORD_CONTINUE: {
      return "continue";
    }
    case TokenType::KEYWORD_FOR: {
      return "for";
    }
    case TokenType::KEYWORD_SWITCH: {
      return "switch";
    }
    case TokenType::KEYWORD_WHILE: {
      return "while";
    }
    case TokenType::KEYWORD_DEBUGGER: {
      return "debugger";
    }
    case TokenType::KEYWORD_FUNCTION: {
      return "function";
    }
    case TokenType::KEYWORD_THIS: {
      return "this";
    }
    case TokenType::KEYWORD_WITH: {
      return "with";
    }
    case TokenType::KEYWORD_DEFAULT: {
      return "default";
    }
    case TokenType::KEYWORD_IF: {
      return "if";
    }
    case TokenType::KEYWORD_THROW: {
      return "throw";
    }
    case TokenType::KEYWORD_DELETE: {
      return "delete";
    }
    case TokenType::KEYWORD_IN: {
      return "in";
    }
    case TokenType::KEYWORD_TRY: {
      return "try";
    }
    case TokenType::LEFT_BRACE: {
      return "{";
    }
    case TokenType::RIGHT_BRACE: {
      return "}";
    }
    case TokenType::LEFT_PAREN: {
      return "(";
    }
    case TokenType::RIGHT_PAREN: {
      return ")";
    }
    case TokenType::LEFT_BRACKET: {
      return "[";
    }
    case TokenType::RIGHT_BRACKET: {
      return "]";
    }
    case TokenType::DOT: {
      return ".";
    }
    case TokenType::SEMICOLON: {
      return ";";
    }
    case TokenType::COMMA: {
      return ",";
    }
    case TokenType::LESS_THAN: {
      return "<";
    }
    case TokenType::GREATER_THAN: {
      return ">";
    }
    case TokenType::LESS_EQUAL: {
      return "<=";
    }
    case TokenType::GREATER_EQUAL: {
      return ">=";
    }
    case TokenType::EQUAL: {
      return "==";
    }
    case TokenType::NOT_EQUAL: {
      return "!=";
    }
    case TokenType::STRICT_EQUAL: {
      return "===";
    }
    case TokenType::NOT_STRICT_EQUAL: {
      return "!==";
    }
    case TokenType::ADD: {
      return "+";
    }
    case TokenType::SUB: {
      return "-";
    }
    case TokenType::MUL: {
      return "*";
    }
    case TokenType::MOD: {
      return "%";
    }
    case TokenType::INC: {
      return "++";
    }
    case TokenType::DEC: {
      return "--";
    }
    case TokenType::LEFT_SHIFT: {
      return "<<";
    }
    case TokenType::RIGHT_SHIFT: {
      return ">>";
    }
    case TokenType::U_RIGHT_SHIFT: {
      return ">>>";
    }
    case TokenType::BIT_AND: {
      return "&";
    }
    case TokenType::BIT_OR: {
      return "|";
    }
    case TokenType::BIT_XOR: {
      return "^";
    }
    case TokenType::LOGICAL_NOT: {
      return "!";
    }
    case TokenType::BIT_NOT: {
      return "~";
    }
    case TokenType::LOGICAL_AND: {
      return "&&";
    }
    case TokenType::LOGICAL_OR: {
      return "||";
    }
    case TokenType::QUESTION: {
      return "?";
    }
    case TokenType::COLON: {
      return ":";
    }
    case TokenType::ASSIGN: {
      return "=";
    }
    case TokenType::ADD_ASSIGN: {
      return "+=";
    }
    case TokenType::SUB_ASSIGN: {
      return "-=";
    }
    case TokenType::MUL_ASSIGN: {
      return "*=";
    }
    case TokenType::MOD_ASSIGN: {
      return "%=";
    }
    case TokenType::LEFT_SHIFT_ASSIGN: {
      return "<<=";
    }
    case TokenType::RIGHT_SHIFT_ASSIGN: {
      return ">>=";
    }
    case TokenType::U_RIGHT_SHIFT_ASSIGN: {
      return ">>>=";
    }
    case TokenType::BIT_AND_ASSIGN: {
      return "&=";
    }
    case TokenType::BIT_OR_ASSIGN: {
      return "|=";
    }
    case TokenType::BIT_XOR_ASSIGN: {
      return "^=";
    }
    case TokenType::DIV: {
      return "/";
    }
    case TokenType::DIV_ASSIGN: {
      return "/=";
    }
    default: {
      return "EOS";
    }
  }
}
  
}  // namespace voidjs

