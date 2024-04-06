#include <cmath>

#include "voidjs/lexer/token.h"
#include "voidjs/lexer/character.h"

namespace voidjs {

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

bool Token::HasString() const {
  return
    type_ == TokenType::IDENTIFIER ||
    type_ == TokenType::FUTURE_RESERVED_WORD ||
    type_ == TokenType::STRICT_MODE_FUTURE_RESERVED_WORD ||
    type_ == TokenType::STRING;
}

}  // namespace voidjs
