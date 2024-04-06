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

bool Token::IsBinaryOperator() const {
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
    case TokenType::KEYWORD_IN:
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

}  // namespace voidjs

