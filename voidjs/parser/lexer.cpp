#include "voidjs/parser/lexer.h"
#include "voidjs/parser/character.h"
#include "voidjs/parser/token.h"

#include <iostream>
#include <optional>

namespace voidjs {

// Token ::
//   IdentifierName
//   Punctuator
//   NumericLiteral
//   StringLiteral
// Defined in ECMAScript 5.1 Chapter 7.5
Token Lexer::NextToken() {
  Token token;
  
  // go back to start when encounter Line Terminator
 start:
  SkipWhitespace();
  
  switch (ch_) {
    // Punctuator
    case u'{': {
      token.type = TokenType::LEFT_BRACE;
      NextChar();
      break;
    }
    case u'}': {
      token.type = TokenType::RIGHT_BRACE;
      NextChar();
      break;
    }
    case u'(': {
      token.type = TokenType::LEFT_PAREN;
      NextChar();
      break;
    }
    case u')': {
      token.type = TokenType::RIGHT_PAREN;
      NextChar();
      break;
    }
    case u'[': {
      token.type = TokenType::LEFT_BRACKET;
      NextChar();
      break;
    }
    case u']': {
      token.type = TokenType::RIGHT_BRACKET;
      NextChar();
      break;
    }
    case u'.': {
      // . NumericLiteral
      if (character::IsDecimalDigit(PeekChar())) {
        ScanNumericLiteral();
      } else {
        token.type = TokenType::DOT;
        NextChar();
      }
      break;
    }
    case u';': {
      token.type = TokenType::SEMICOLON;
      NextChar();
      break;
    }
    case u',': {
      token.type = TokenType::COMMA;
      NextChar();
      break;
    }
    case u'<': {
      // < << <= <<=
      NextChar();
      if (ch_ == u'<') {
        NextChar();
        if (ch_ == u'=') {
          token.type = TokenType::LEFT_SHIFT_ASSIGN;
          NextChar();
        } else {
          token.type = TokenType::LEFT_SHIFT;
        }
      } else if (ch_ == u'=') {
        token.type = TokenType::LESS_EQUAL;
        NextChar();
      } else {
        token.type = TokenType::LESS_THAN;
      }
      break;
    }
    case u'>': {
      // > >> >>> >= >>= >>>=
      NextChar();
      if (ch_ == u'>') {
        NextChar();
        if (ch_ == u'>') {
          NextChar();
          if (ch_ == u'=') {
            token.type = TokenType::U_RIGHT_SHIFT_ASSIGN;
            NextChar();
          } else {
            token.type = TokenType::U_RIGHT_SHIFT;
          }
        } else if (ch_ == u'=') {
          token.type = TokenType::RIGHT_SHIFT_ASSIGN;
          NextChar();
        } else {
          token.type = TokenType::RIGHT_SHIFT;
        }
      } else if (ch_ == u'=') {
        token.type = TokenType::GREATER_EQUAL;
        NextChar();
      } else {
        token.type = TokenType::GREATER_THAN;
      }
      break;
    }
    case u'=': {
      // = == ===
      NextChar();
      if (ch_ == u'=') {
        NextChar();
        if (ch_ == u'=') {
          token.type = TokenType::STRICT_EQUAL;
          NextChar();
        } else {
          token.type = TokenType::EQUAL;
        }
      } else {
        token.type = TokenType::ASSIGN;
      }
      break;
    }
    case u'!': {
      // ! != !==
      NextChar();
      if (ch_ == u'=') {
        NextChar();
        if (ch_ == u'=') {
          token.type = TokenType::NOT_STRICT_EQUAL;
          NextChar();
        } else {
          token.type = TokenType::NOT_EQUAL;
        }
      } else {
        token.type = TokenType::LOGICAL_NOT;
      }
      break;
    }
    case u'+': {
      // + ++ +=
      NextChar();
      if (ch_ == u'+') {
        token.type = TokenType::INC;
        NextChar();
      } else if (ch_ == u'=') {
        token.type = TokenType::ADD_ASSIGN;
        NextChar();
      } else {
        token.type = TokenType::ADD;
      }
      break;
    }
    case u'-': {
      // - -- -=
      NextChar();
      if (ch_ == u'-') {
        token.type = TokenType::DEC;
        NextChar();
      } else if (ch_ == u'=') {
        token.type = TokenType::SUB_ASSIGN;
        NextChar();
      } else {
        token.type = TokenType::SUB;
      }
      break;
    }
    case u'*': {
      // * *=
      NextChar();
      if (ch_ == u'=') {
        token.type = TokenType::MUL_ASSIGN;
        NextChar();
      } else {
        token.type = TokenType::MUL;
      }
      break;
    }
    case u'%': {
      // % %=
      NextChar();
      if (ch_ == u'=') {
        token.type = TokenType::MOD_ASSIGN;
        NextChar();
      } else {
        token.type = TokenType::MOD;
      }
      break;
    }
    case u'&': {
      // & && &=
      NextChar();
      if (ch_ == u'&') {
        token.type = TokenType::LOGICAL_AND;
        NextChar();
      } else if (ch_ == u'=') {
        token.type = TokenType::BIT_AND_ASSIGN;
        NextChar();
      } else {
        token.type = TokenType::BIT_AND;
      }
      break;
    }
    case u'|': {
      // | || |=
      NextChar();
      if (ch_ == u'|') {
        token.type = TokenType::LOGICAL_OR;
        NextChar();
      } else if (ch_ == u'=') {
        token.type = TokenType::BIT_OR_ASSIGN;
        NextChar();
      } else {
        token.type = TokenType::BIT_OR;
      }
      break;
    }
    case u'^': {
      // ^ ^=
      NextChar();
      if (ch_ == u'=') {
        token.type = TokenType::BIT_XOR_ASSIGN;
        NextChar();
      } else {
        token.type = TokenType::BIT_XOR;
      }
      break;
    }
    case u'~': {
      token.type = TokenType::BIT_NOT;
      NextChar();
      break;
    }
    case u'?': {
      token.type = TokenType::QUESTION;
      NextChar();
      break;
    }
    case u':': {
      token.type = TokenType::COLON;
      NextChar();
      break;
    }
    case u'/': {
      // / // /* /=
      if (PeekChar() == u'/') {
        SkipSingleLineComment();
        goto start;
      } else if (PeekChar() == u'*') {
        SkipMultiLineComment();
        goto start;
      } else if (PeekChar() == u'=') {
        NextChar();
        token.type = TokenType::DIV_ASSIGN;
        NextChar();
      } else {
        token.type = TokenType::DIV;
        NextChar();
      }
      break;
    }
    default: {
      if (character::IsIdentifierStart(ch_)) {
        token = ScanIdentifier();
      } else if (character::IsDecimalDigit(ch_)) {
        token = ScanNumericLiteral();
      } else if (ch_ == '\'' || ch_ == '"') {
        token = ScanStringLiteral();
      } else if (character::IsLineTerminator(ch_)) {
        SkipLineTerminator();
        goto start;
      } else if (ch_ == character::EOS) {
        token.type = TokenType::EOS;
      } else {
        token.type = TokenType::ILLEGAL;
        NextChar();
      }
      break;
    }
  }
  return token;
}

char16_t Lexer::NextChar() {
  cur_ = nxt_++;
  if (cur_ < src_.size()) {
    ch_ = src_.at(cur_);
  } else {
    ch_= character::EOS;
  }
  return ch_;
}

char16_t Lexer::PeekChar() {
  if (nxt_ < src_.size()) {
    return src_.at(nxt_);
  } else {
    return character::EOS;
  }
}

void Lexer::SkipWhitespace() {
  while (character::IsWhitespace(ch_)) {
    NextChar();
  }
}

// Skip line terminator
// Defined in ECMAScript 5.1 Chapter 7.3
void Lexer::SkipLineTerminator() {
  // The character sequence <CR><LF> is commonly used as a line terminator
  if (ch_ == character::CR && PeekChar() == character::LF) {
    NextChar();
  }
  NextChar();
}

// Skip until LineTerminator
// Defined in ECMAScript 5.1 Chapter 7.4
// Because a single-line comment can contain any character except a LineTerminator character,
// and because of the general rule that a token is always as long as possible,
// a single-line comment always consists of all characters from the // marker to the end of the line.
// However, the LineTerminator at the end of the line is not considered to be part of the single-line comment;
// it is recognised separately by the lexical grammar and becomes part of the stream of input elements for the syntactic grammar.
TokenType Lexer::SkipSingleLineComment() {
  NextChar();
  NextChar();
  while (ch_ != character::EOS && !character::IsLineTerminator(ch_)) {
    NextChar();
  }
  return TokenType::SINGLE_LINE_COMMENT;
}

// Skip multi-line comment
// Defined in ECMAScript 5.1 Chapter 7.4
// Multi-line comments cannot nest.
TokenType Lexer::SkipMultiLineComment() {
  NextChar();
  NextChar();
  while (ch_ != character::EOS) {
    if (ch_ == u'*' && PeekChar() == u'/') {
      NextChar();
      NextChar();
      return TokenType::MULTI_LINE_COMMENT;
    } else {
      NextChar();
    }
  }
  return TokenType::ILLEGAL;
}


// Skip Unicode escape sequence
// Modified from https://github.com/zhuzilin/es/blob/main/es/parser/lexer.h
// IdentifierStart ::
//   UnicodeLetter
//   $
//   _
//   \ UnicodeEscapeSequence
// UnicodeEscapeSequence ::
//   u HexDigit HexDigit HexDigit HexDigit
// Defined in ECMAScript 5.1 7.8.4
std::optional<char16_t> Lexer::SkipUnicodeEscapeSequence() {
  if (PeekChar() != u'u') {
    return std::nullopt;
  }
  NextChar();
  NextChar();
  char16_t ch = 0x0000;
  auto hexdigit_to_decimaldigit = [](char16_t ch) -> char16_t {
    if (u'0' <= u'9') {
      return ch - u'0';
    }
    if (u'a' <= u'f') {
      return ch - u'a' + 10;
    }
    if (u'A' <= u'F') {
      return ch - u'A' + 10;
    }
  };
  for (std::size_t i = 0; i < 4; ++i) {
    if (character::IsHexDigit(ch_)) {
      return std::nullopt;
    }
    ch = ch << 4 | hexdigit_to_decimaldigit(ch);
    NextChar();
  }
  return {ch};
}

// Scan identifier
// Defined in ECMAScript 5.1 Chapter 7.6
// IdentifierName ::
//   IdentifierStart
//   IdentifierName IdentifierPart
// Unicode escape sequences are also permitted in an IdentifierName,
// where they contribute a single character to the IdentifierName,
// as computed by the CV of the UnicodeEscapeSequence (see 7.8.4).
Token Lexer::ScanIdentifier() {
  std::u16string ident_name;
  std::size_t start = cur_;

  // IdentifierStart
  if (ch_ == '\\') {
    if (auto ret = SkipUnicodeEscapeSequence(); ret.has_value()) {
      ident_name.push_back(ret.value());
    } else {
      return {TokenType::ILLEGAL};
    }
  } else {
    ident_name.push_back(ch_);
    NextChar();
  }
  
  while (character::IsIdentifierPart(ch_)) {
    if (ch_ == '\\') {
      if (auto ret = SkipUnicodeEscapeSequence(); ret.has_value()) {
        ident_name.push_back(ret.value());
      } else {
        return {TokenType::ILLEGAL};
      }
    } else {
      ident_name.push_back(ch_);
      NextChar();
    }
  }

  // ReservedWord
  if (ident_name == u"null") {
    return {TokenType::NULL_LITERAL, ident_name, start, cur_};
  } else if (ident_name == u"true" || ident_name == u"false") {
    return {TokenType::BOOLEAN_LITERAL, ident_name, start, cur_};
  } else if (std::find(kKeywords.begin(), kKeywords.end(), ident_name) != kKeywords.end()) {
    return {TokenType::KEYWORD, ident_name, start, cur_};
  } else if (std::find(kFutureReservedWords.begin(),
                       kFutureReservedWords.end(), ident_name) != kFutureReservedWords.end()) {
    return {TokenType::FUTURE_RESERVED_WORD, ident_name, start, cur_};
  } else if (std::find(kStrictModeFutureReservedWords.begin(),
                       kStrictModeFutureReservedWords.end(), ident_name) != kStrictModeFutureReservedWords.end()) {
    return {TokenType::STRICT_MODE_FUTURE_RESERVED_WORD, ident_name, start, cur_};
  } else {
    return {TokenType::IDENTIFIER, ident_name, start, cur_};
  }
}

Token Lexer::ScanNumericLiteral() {
  Token token;
  return token;
}

Token Lexer::ScanStringLiteral() {
  Token token;
  return token;
}

}  // namespace voidjs
