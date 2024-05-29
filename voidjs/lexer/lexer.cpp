#include "voidjs/lexer/lexer.h"
#include "voidjs/lexer/character.h"
#include "voidjs/lexer/token.h"
#include "voidjs/lexer/token_type.h"

#include <iostream>
#include <optional>
#include <string>
#include <cmath>

namespace voidjs {

// Token ::
//   IdentifierName
//   Punctuator
//   NumericLiteral
//   StringLiteral
// Defined in ECMAScript 5.1 Chapter 7.5
void Lexer::NextToken() {
  token_.SetType(TokenType::EOS);
  has_line_terminator_ = false;
  
  // go back to start when encounter Line Terminator
 start:
  SkipWhitespace();
  
  switch (ch_) {
    // Punctuator
    case u'{': {
      token_.SetType(TokenType::LEFT_BRACE);
      NextChar();
      break;
    }
    case u'}': {
      token_.SetType(TokenType::RIGHT_BRACE);
      NextChar();
      break;
    }
    case u'(': {
      token_.SetType(TokenType::LEFT_PAREN);
      NextChar();
      break;
    }
    case u')': {
      token_.SetType(TokenType::RIGHT_PAREN);
      NextChar();
      break;
    }
    case u'[': {
      token_.SetType(TokenType::LEFT_BRACKET);
      NextChar();
      break;
    }
    case u']': {
      token_.SetType(TokenType::RIGHT_BRACKET);
      NextChar();
      break;
    }
    case u'.': {
      // . NumericLiteral
      if (character::IsDecimalDigit(PeekChar())) {
        ScanNumericLiteral();
      } else {
        token_.SetType(TokenType::DOT);
        NextChar();
      }
      break;
    }
    case u';': {
      token_.SetType(TokenType::SEMICOLON);
      NextChar();
      break;
    }
    case u',': {
      token_.SetType(TokenType::COMMA);
      NextChar();
      break;
    }
    case u'<': {
      // < << <= <<=
      NextChar();
      if (ch_ == u'<') {
        NextChar();
        if (ch_ == u'=') {
          token_.SetType(TokenType::LEFT_SHIFT_ASSIGN);
          NextChar();
        } else {
          token_.SetType(TokenType::LEFT_SHIFT);
        }
      } else if (ch_ == u'=') {
        token_.SetType(TokenType::LESS_EQUAL);
        NextChar();
      } else {
        token_.SetType(TokenType::LESS_THAN);
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
            token_.SetType(TokenType::U_RIGHT_SHIFT_ASSIGN);
            NextChar();
          } else {
            token_.SetType(TokenType::U_RIGHT_SHIFT);
          }
        } else if (ch_ == u'=') {
          token_.SetType(TokenType::RIGHT_SHIFT_ASSIGN);
          NextChar();
        } else {
          token_.SetType(TokenType::RIGHT_SHIFT);
        }
      } else if (ch_ == u'=') {
        token_.SetType(TokenType::GREATER_EQUAL);
        NextChar();
      } else {
        token_.SetType(TokenType::GREATER_THAN);
      }
      break;
    }
    case u'=': {
      // = == ===
      NextChar();
      if (ch_ == u'=') {
        NextChar();
        if (ch_ == u'=') {
          token_.SetType(TokenType::STRICT_EQUAL);
          NextChar();
        } else {
          token_.SetType(TokenType::EQUAL);
        }
      } else {
        token_.SetType(TokenType::ASSIGN);
      }
      break;
    }
    case u'!': {
      // ! != !==
      NextChar();
      if (ch_ == u'=') {
        NextChar();
        if (ch_ == u'=') {
          token_.SetType(TokenType::NOT_STRICT_EQUAL);
          NextChar();
        } else {
          token_.SetType(TokenType::NOT_EQUAL);
        }
      } else {
        token_.SetType(TokenType::LOGICAL_NOT);
      }
      break;
    }
    case u'+': {
      // + ++ +=
      NextChar();
      if (ch_ == u'+') {
        token_.SetType(TokenType::INC);
        NextChar();
      } else if (ch_ == u'=') {
        token_.SetType(TokenType::ADD_ASSIGN);
        NextChar();
      } else {
        token_.SetType(TokenType::ADD);
      }
      break;
    }
    case u'-': {
      // - -- -=
      NextChar();
      if (ch_ == u'-') {
        token_.SetType(TokenType::DEC);
        NextChar();
      } else if (ch_ == u'=') {
        token_.SetType(TokenType::SUB_ASSIGN);
        NextChar();
      } else {
        token_.SetType(TokenType::SUB);
      }
      break;
    }
    case u'*': {
      // * *=
      NextChar();
      if (ch_ == u'=') {
        token_.SetType(TokenType::MUL_ASSIGN);
        NextChar();
      } else {
        token_.SetType(TokenType::MUL);
      }
      break;
    }
    case u'%': {
      // % %=
      NextChar();
      if (ch_ == u'=') {
        token_.SetType(TokenType::MOD_ASSIGN);
        NextChar();
      } else {
        token_.SetType(TokenType::MOD);
      }
      break;
    }
    case u'&': {
      // & && &=
      NextChar();
      if (ch_ == u'&') {
        token_.SetType(TokenType::LOGICAL_AND);
        NextChar();
      } else if (ch_ == u'=') {
        token_.SetType(TokenType::BIT_AND_ASSIGN);
        NextChar();
      } else {
        token_.SetType(TokenType::BIT_AND);
      }
      break;
    }
    case u'|': {
      // | || |=
      NextChar();
      if (ch_ == u'|') {
        token_.SetType(TokenType::LOGICAL_OR);
        NextChar();
      } else if (ch_ == u'=') {
        token_.SetType(TokenType::BIT_OR_ASSIGN);
        NextChar();
      } else {
        token_.SetType(TokenType::BIT_OR);
      }
      break;
    }
    case u'^': {
      // ^ ^=
      NextChar();
      if (ch_ == u'=') {
        token_.SetType(TokenType::BIT_XOR_ASSIGN);
        NextChar();
      } else {
        token_.SetType(TokenType::BIT_XOR);
      }
      break;
    }
    case u'~': {
      token_.SetType(TokenType::BIT_NOT);
      NextChar();
      break;
    }
    case u'?': {
      token_.SetType(TokenType::QUESTION);
      NextChar();
      break;
    }
    case u':': {
      token_.SetType(TokenType::COLON);
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
        token_.SetType(TokenType::DIV_ASSIGN);
        NextChar();
      } else {
        token_.SetType(TokenType::DIV);
        NextChar();
      }
      break;
    }
    default: {
      if (character::IsIdentifierStart(ch_)) {
        ScanIdentifier();
      } else if (character::IsDecimalDigit(ch_)) {
        ScanNumericLiteral();
      } else if (ch_ == '\'' || ch_ == '"') {
        ScanStringLiteral();
      } else if (character::IsLineTerminator(ch_)) {
        SkipLineTerminatorSequence();
        has_line_terminator_ = true;
        goto start;
      } else if (ch_ == character::EOS) {
        token_.SetType(TokenType::EOS);
      } else {
        token_.SetType(TokenType::ILLEGAL);
        NextChar();
      }
      break;
    }
  }
}

Token Lexer::NextRewindToken() {
  std::size_t start = cur_;
  auto token = token_;
  
  NextToken();
  auto ret = token_;
  
  cur_ = start;
  nxt_ = cur_ + 1;
  if (cur_ < src_.size()) {
    ch_ = src_.at(cur_);
  } else {
    ch_ = character::EOS;
  }
  has_line_terminator_ = false;
  token_ = token;
  
  return ret;
}

bool Lexer::HasLineTerminator() {
  return has_line_terminator_;
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

// Assume input string is always vaild
double Lexer::ConvertToNumber(std::u16string source) {
  double ret = 0;
  double val = 0;
  bool dot = false;
  bool sign = false;
  bool exp = false;
  bool hex = false;
  double base = 10;
  auto ToDigit = [](char16_t ch) -> std::int32_t {
    if (u'0' <= ch && ch <= u'9') {
      return ch - u'0';
    }
    if (u'a' <= ch && ch <= u'f') {
      return ch - u'a' + 10;
    }
    if (u'A' <= ch && ch <= u'F') {
      return ch - u'A' + 10;
    }
    return 0; 
  };
  for (std::size_t i = 0; i < source.size(); ++i) {
    char16_t ch = source[i];
    if (ch == u'.') {
      dot = true;
      ret += val;
      val = 0;
      base = 0.1;
    } else if (!hex && (ch == u'e' || ch == u'E')) {
      exp = true;
      dot = false;
      ret += val;
      val = 0;
      base = 10;
    } else if (ch == u'-' || ch == u'+') {
      if (ch == u'-') {
        sign = true;
      }
    } else if (ch == u'x' || ch == u'X') {
      hex = true;
      base = 16;
    } else {
      if (dot) {
        val += ToDigit(ch) * base;
        base *= 0.1;
      } else {
        val = val * base + ToDigit(ch);
      }
    }
  }
  if (exp) {
    ret *= std::pow(10, sign ? -val : val);
  } else {
    ret += val;
  }
  return ret;
}

// Assume input string is always valid
// Copy from https://github.com/zhuzilin/es/blob/67fb4d579bb142669acd8384ea34c62cd052945c/es/impl/base-impl.h#L121
std::u16string Lexer::ConvertToString(std::u16string source) {
  size_t pos = 1;
  std::u16string vals;
  auto ToDigit = [](char16_t ch) -> char16_t {
    if (u'0' <= ch && ch <= u'9') {
      return ch - u'0';
    }
    if (u'a' <= ch && ch <= u'f') {
      return ch - u'a' + 10;
    }
    if (u'A' <= ch && ch <= u'F') {
      return ch - u'A' + 10;
    }
    return 0; 
  };
  while (pos < source.size() - 1) {
    char16_t c = source[pos];
    switch (c) {
      case u'\\': {
        pos++;
        c = source[pos];
        switch (c) {
          case u'b':
            pos++;
            vals.push_back(u'\b');
            break;
          case u't':
            pos++;
            vals.push_back(u'\t');
            break;
          case u'n':
            pos++;
            vals.push_back(u'\n');
            break;
          case u'v':
            pos++;
            vals.push_back(u'\v');
            break;
          case u'f':
            pos++;
            vals.push_back(u'\f');
            break;
          case u'r':
            pos++;
            vals.push_back(u'\r');
            break;
          case u'0': {
            pos++;
            vals.append(std::u16string(1, 0));
            break;
          }
          case u'x': {
            pos++;  // skip 'x'
            char16_t hex = 0;
            for (size_t i = 0; i < 2; i++) {
              hex *= 16;
              hex += ToDigit(source[pos]);
              pos++;
            }
            vals.append(std::u16string(1, hex));
            break;
          }
          case u'u': {
            pos++;  // skip 'u'
            char16_t hex = 0;
            for (size_t i = 0; i < 4; i++) {
              hex *= 16;
              hex += ToDigit(source[pos]);
              pos++;
            }
            vals.append(std::u16string(1, hex));
            break;
          }
          default:
            c = source[pos];
            if (character::IsLineTerminator(c)) {
              pos++;
              continue;
            }
            pos++;
            vals.append(std::u16string(1, c));
        }
        break;
      }
      default: {
        size_t start = pos;
        while (true) {
          if (pos == source.size() - 1 || source[pos] == u'\\')
            break;
          pos++;
        }
        size_t end = pos;
        if (end == source.size() - 1 && vals.size() == 0)
          return source.substr(start, end - start);
        vals.append(source.substr(start, end - start));
      }
    }
  }
  if (vals.size() == 0) {
    return {};
  } else if (vals.size() == 1) {
    return std::u16string(1, vals[0]);
  }
  return vals;
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
void Lexer::ScanIdentifier() {
  std::u16string ident_name;
  std::size_t start = cur_;

  // IdentifierStart
  if (ch_ == u'\\') {
    if (auto ret = SkipUnicodeEscapeSequence(); ret.has_value()) {
      ident_name.push_back(ret.value());
    } else {
      token_.SetType(TokenType::ILLEGAL);
      return ;
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
        token_.SetType(TokenType::ILLEGAL);
        return ;
      }
    } else {
      ident_name.push_back(ch_);
      NextChar();
    }
  }

  // ReservedWord
  if (ident_name == u"null") {
    token_.SetType(TokenType::NULL_LITERAL);
  } else if (ident_name == u"true" || ident_name == u"false") {
    token_.SetType(ident_name == u"true" ? TokenType::TRUE: TokenType::FALSE);
  } else if (std::find(kKeywords.begin(), kKeywords.end(), ident_name) != kKeywords.end()) {
    token_.SetType(kStringToKeywords.at(ident_name));
    token_.SetString(std::move(ident_name));
  } else if (std::find(kFutureReservedWords.begin(),
                       kFutureReservedWords.end(), ident_name) != kFutureReservedWords.end()) {
    token_.SetType(TokenType::FUTURE_RESERVED_WORD);
    token_.SetString(std::move(ident_name));
  }
  // else if (std::find(kStrictModeFutureReservedWords.begin(),
  //                    kStrictModeFutureReservedWords.end(), ident_name) != kStrictModeFutureReservedWords.end()) {
  //   token_.SetType(TokenType::STRICT_MODE_FUTURE_RESERVED_WORD);
  //   token_.SetString(std::move(ident_name));
  // }
  else {
    token_.SetType(TokenType::IDENTIFIER);
    token_.SetString(std::move(ident_name));
  }
}

// Scan NumericLiteral
// Defined in ECMAScript 5.1 Chapter 7.8.3
void Lexer::ScanNumericLiteral() {
  std::size_t start = cur_;
  if (ch_ == u'.') {
    // . DecimalDigits ExponentPart_opt
    NextChar();
    if (!SkipDecimalDigits()) {
      token_.SetType(TokenType::ILLEGAL);
      return ;
    }
    if ((ch_ == u'e' || ch_ == u'E') && !SkipExponentPart()) {
      token_.SetType(TokenType::ILLEGAL);
      return ;
    }
  } else if ((ch_ == u'0' && PeekChar() != u'x' && PeekChar() != u'X') ||
             (ch_ != u'0' && character::IsDecimalDigit(ch_))) {
    // DecimalIntegerLiteral . DecimalDigits_opt ExponentPart_opt
    // DecimalIntegerLiteral ExponentPart_opt
    if (!SkipDecimalIntegerLiteral()) {
      token_.SetType(TokenType::ILLEGAL);
      return ;
    }
    if (ch_ == u'.') {
      NextChar();
      if (character::IsDecimalDigit(ch_) && !SkipDecimalDigits()) {
        token_.SetType(TokenType::ILLEGAL);
        return; 
      }
      if ((ch_ == u'e' || ch_ == u'E') && !SkipExponentPart()) {
        token_.SetType(TokenType::ILLEGAL);
        return ;
      }
    } else {
      if ((ch_ == u'e' || ch_ == u'E') && !SkipExponentPart()) {
        token_.SetType(TokenType::ILLEGAL);
        return ;
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
      token_.SetType(TokenType::ILLEGAL);
      return ;
    }
  } else {
    NextChar();  // skip the illegal char
    token_.SetType(TokenType::ILLEGAL);
    return ;
  }
  token_.SetType(TokenType::NUMBER);
  token_.SetNumber(ConvertToNumber(src_.substr(start, cur_ - start)));
}

// Scan StringLiteral
// Defined in ECMAScript 5.1 Chapter 7.8.4
void Lexer::ScanStringLiteral() {
  std::size_t start = cur_;
  if (ch_ == u'\'') {
    NextChar();
    while (ch_ != u'\'' && ch_ != character::EOS) {
      if (ch_ == u'\\') {
        if (!SkipSingleStringCharacter()) {
          token_.SetType(TokenType::ILLEGAL);
      return ;
        }
      } else {
        NextChar();
      }
    }
    // ' not found
    if (ch_ == character::EOS) {
      token_.SetType(TokenType::ILLEGAL);
      return ;
    }
    NextChar();
  } else if (ch_ == u'"') {
    NextChar();
    while (ch_ != u'"' && ch_ != character::EOS) {
      if (ch_ == u'\\') {
        if (!SkipSingleStringCharacter()) {
          token_.SetType(TokenType::ILLEGAL);
      return ;
        }
      } else {
        NextChar();
      }
    }
    // " not found
    if (ch_ == character::EOS) {
      token_.SetType(TokenType::ILLEGAL);
      return ;
    }
    NextChar();
  } else {
    token_.SetType(TokenType::ILLEGAL);
      return ;
  }
  token_.SetType(TokenType::STRING);
  token_.SetString(ConvertToString(src_.substr(start, cur_ - start)));
}

}  // namespace voidjs
