#include "voidjs/lexer/lexer.h"
#include "voidjs/lexer/character.h"
#include "voidjs/lexer/token.h"

#include <iostream>
#include <optional>
#include <string>

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
        token = ScanNumericLiteral();
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
        SkipLineTerminatorSequence();
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
bool Lexer::SkipLineTerminatorSequence() {
  if (!character::IsLineTerminator(ch_)) {
    NextChar(); // skip the illegal char
    return false;
  }
  // The character sequence <CR><LF> is commonly used as a line terminator
  if (ch_ == character::CR && PeekChar() == character::LF) {
    NextChar();
  }
  NextChar();
  return true;
}

// Skip until LineTerminator
// Defined in ECMAScript 5.1 Chapter 7.4
// Because a single-line comment can contain any character except a LineTerminator character,
// and because of the general rule that a token is always as long as possible,
// a single-line comment always consists of all characters from the // marker to the end of the line.
// However, the LineTerminator at the end of the line is not considered to be part of the single-line comment;
// it is recognised separately by the lexical grammar and becomes part of the stream of input elements for the syntactic grammar.
TokenType Lexer::SkipSingleLineComment() {
  if (ch_ != u'/' && PeekChar() != u'/') {
    NextChar();  // skip the illegal char
    NextChar();  // skip the illegal char
    return TokenType::ILLEGAL;
  }
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
  if (ch_ != u'/' && PeekChar() != u'*') {
    NextChar();  // skip the illegal char
    NextChar();  // skip the illegal char
    return TokenType::ILLEGAL;
  }
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
  NextChar();  // skip the illegal char
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
  if (ch_ != u'u') {
    NextChar();  // skip the illegal char
    return std::nullopt;
  }
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
    if (!character::IsHexDigit(ch_)) {
      NextChar();  // skip the illegal char
      return std::nullopt;
    }
    ch = ch << 4 | hexdigit_to_decimaldigit(ch);
    NextChar();
  }
  return {ch};
}

// Skip DecimalDigits
// Defined in ECMAScript 5.1 Chapter 7.8.3
bool Lexer::SkipDecimalDigits() {
  if (!character::IsDecimalDigit(ch_)) {
    NextChar();  // skip the illegal char
    return false;
  } 
  while (character::IsDecimalDigit(ch_)) {
    NextChar();
  }
  return true;
}

// Skip SignedInteger
// Defined in ECMAScript 5.1 Chapter 7.8.3
bool Lexer::SkipSignedInteger() {
  if (ch_ != u'+' && ch_ != u'-' && !character::IsDecimalDigit(ch_)) {
    NextChar();  // skip the illegal char
    return false;
  }
  if (ch_ == u'+' || ch_ == u'-') {
    NextChar();
  }
  return SkipDecimalDigits();
}

// Skip ExponentPart
// Defined in ECMAScript 5.1 Chapter 7.8.3
bool Lexer::SkipExponentPart() {
  if (ch_ != u'e' && ch_ != u'E') {
    NextChar(); // skip the illegal char
    return false; 
  }
  NextChar();
  return SkipSignedInteger();
}

// Skip DecimalIntegerLiteral
// Defined in ECMAScript 5.1 Chapter 7.8.3
bool Lexer::SkipDecimalIntegerLiteral() {
  if (!character::IsDecimalDigit(ch_)) {
    NextChar();  // skip the illegal char
    return false;
  }
  if (ch_ == u'0') {
    NextChar();
  } else {
    NextChar();
    while (character::IsDecimalDigit(ch_)) {
      NextChar();
    }
  }
  return true;
}

bool Lexer::SkipHexDigits() {
  if (!character::IsHexDigit(ch_)) {
    NextChar();  // skip the illegal char
    return false;
  }
  while (character::IsHexDigit(ch_)) {
    NextChar();
  }
  return true;
}

// Skip CharacterEscapeSequence
// Defined in ECMAScript 5.1 Chapter 7.8.4
bool Lexer::SkipCharacterEscapeSequence() {
  if (!character::IsCharacterEscapeSequence(ch_)) {
    NextChar();  // skip the illegal char
    return false;
  }
  NextChar();
  return true;
}

// Skip HexEscapeSequence
// Defined in ECMAScript 5.1 Chapter 7.8.4
// HexEscapeSequence ::
//   x HexDigit HexDigit
bool Lexer::SkipHexEscapeSequence() {
  if (ch_ != u'x') {
    NextChar();  // skip the illegal char
    return false;
  }
  NextChar();
  if (!character::IsHexDigit(ch_)) {
    NextChar();  // skip the illegal char
    return false;
  }
  NextChar();
  if (!character::IsHexDigit(ch_)) {
    NextChar();  // skip the illegal char
    return false;
  }
  NextChar();
  return true;
}

// Skip EscapeSequence
// Defined in ECMAScript 5.1 Chapter 7.8.4
// EscapeSequence ::
//   CharacterEscapeSequence
//   0 [lookahead ∉ DecimalDigit]
//   HexEscapeSequence
//   UnicodeEscapeSequence
bool Lexer::SkipEscapeSequence() {
  if (!character::IsCharacterEscapeSequence(ch_) && SkipCharacterEscapeSequence()) {
    return true;
  } else if (ch_ == u'0' && !character::IsDecimalDigit(PeekChar())) {
    NextChar();
    return true;
  } else if (ch_ == u'x' && SkipHexEscapeSequence()) {
    return true;
  } else if (ch_ == u'u' && SkipUnicodeEscapeSequence().has_value()) {
    // UnicodeEscapeSequence is not escaped for now.
    return true;
  } else {
    return false;
  }
}

// Skip SingleStringCharacter
// Defined in ECMAScript 5.1 Chapter 7.8.4
// SingleStringCharacter ::
//   SourceCharacter but not single-quote ' or backslash \ or LineTerminator
//   \ EscapeSequence
//   \ LineTerminatorSequence
bool Lexer::SkipSingleStringCharacter() {
  if (ch_ == u'\\') {
    NextChar();
    return
      character::IsLineTerminator(ch_) && SkipLineTerminatorSequence() ||
      SkipEscapeSequence();
  } else {
    if (ch_ != u'\'' && ch_ != u'\\') {
      NextChar();
      return true;
    } else {
      return false;
    }
  }
}

// Skip DoubleStringCharacter
// Defined in ECMAScript 5.1 Chapter 7.8.4
// SingleStringCharacter ::
//   SourceCharacter but not double-quote " or backslash \ or LineTerminator
//   \ EscapeSequence
//   \ LineTerminatorSequence
bool Lexer::SkipDoubleStringCharacter() {
  if (ch_ == u'\\') {
    NextChar();
    return
      character::IsLineTerminator(ch_) && SkipLineTerminatorSequence() ||
      SkipEscapeSequence();
  } else {
    if (ch_ != u'"' && ch_ != u'\\') {
      NextChar();
      return true;
    } else {
      return false;
    }
  }
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
  if (ch_ == u'\\') {
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
    if (ch_ == u'\\') {
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
    return {kStringToKeywords.at(ident_name), ident_name, start, cur_};
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

// Scan NumericLiteral
// Defined in ECMAScript 5.1 Chapter 7.8.3
Token Lexer::ScanNumericLiteral() {
  std::size_t start = cur_;
  if (ch_ == u'.') {
    // . DecimalDigits ExponentPart_opt
    NextChar();
    if (!SkipDecimalDigits()) {
      return {TokenType::ILLEGAL};
    }
    if ((ch_ == u'e' || ch_ == u'E') && !SkipExponentPart()) {
      return {TokenType::ILLEGAL};
    }
  } else if ((ch_ == u'0' && PeekChar() != u'x' && PeekChar() != u'X') ||
             (ch_ != u'0' && character::IsDecimalDigit(ch_))) {
    // DecimalIntegerLiteral . DecimalDigits_opt ExponentPart_opt
    // DecimalIntegerLiteral ExponentPart_opt
    if (!SkipDecimalIntegerLiteral()) {
      return {TokenType::ILLEGAL};
    }
    if (ch_ == u'.') {
      NextChar();
      if (character::IsDecimalDigit(ch_) && !SkipDecimalDigits()) {
        return {TokenType::ILLEGAL};
      }
      if ((ch_ == u'e' || ch_ == u'E') && !SkipExponentPart()) {
        return {TokenType::ILLEGAL};
      }
    } else {
      if ((ch_ == u'e' || ch_ == u'E') && !SkipExponentPart()) {
        return {TokenType::ILLEGAL};
      }
    }
  } else if (ch_ == u'0' && (PeekChar() == u'x' || PeekChar() == u'X')) {
    // HexIntegerLiteral :: 
    //   0x HexDigit
    //   0X HexDigit
    //   HexIntegerLiteral HexDigit
    NextChar();
    NextChar();
    if (!SkipHexDigits()) {
      return {TokenType::ILLEGAL};
    }
  } else {
    NextChar();  // skip the illegal char
    return {TokenType::ILLEGAL};
  }
  return {TokenType::NUMBER, src_.substr(start, cur_ - start), start, cur_};
}

// Scan StringLiteral
// Defined in ECMAScript 5.1 Chapter 7.8.4
Token Lexer::ScanStringLiteral() {
  std::size_t start = cur_;
  if (ch_ == u'\'') {
    NextChar();
    while (ch_ != u'\'' && ch_ != character::EOS) {
      if (ch_ == u'\\') {
        if (!SkipSingleStringCharacter()) {
          return {TokenType::ILLEGAL};
        }
      } else {
        NextChar();
      }
    }
    // ' not found
    if (ch_ == character::EOS) {
      return {TokenType::ILLEGAL};
    }
    NextChar();
  } else if (ch_ == u'"') {
    NextChar();
    while (ch_ != u'"' && ch_ != character::EOS) {
      if (ch_ == u'\\') {
        if (!SkipSingleStringCharacter()) {
          return {TokenType::ILLEGAL};
        }
      } else {
        NextChar();
      }
    }
    // " not found
    if (ch_ == character::EOS) {
      return {TokenType::ILLEGAL};
    }
    NextChar();
  } else {
    return {TokenType::ILLEGAL};
  }
  return {TokenType::STRING, src_.substr(start, cur_ - start), start, cur_};
}

}  // namespace voidjs
