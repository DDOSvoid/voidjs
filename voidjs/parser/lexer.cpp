#include "voidjs/parser/lexer.h"
#include "voidjs/parser/character.h"
#include "voidjs/parser/token.h"

namespace voidjs {

// Token ::
//          IdentifierName
//          Punctuator
//          NumericLiteral
//          StringLiteral
// Defined in ECMAScript 5.1 Chapter 7.5
Token Lexer::NextToken() {
  Token token;
  
  // go back to start when encounter Line Terminator
 start:
  SkipWhitespace();
  
  switch (ch_) {
    // Punctuator
    case u'{':
      token.type = TokenType::LEFT_BRACE;
      NextChar();
      break;
    case u'}':
      token.type = TokenType::RIGHT_BRACE;
      NextChar();
      break;
    case u'(':
      token.type = TokenType::LEFT_PAREN;
      NextChar();
      break;
    case u')':
      token.type = TokenType::RIGHT_PAREN;
      NextChar();
      break;
    case u'[':
      token.type = TokenType::LEFT_BRACKET;
      NextChar();
      break;
    case u']':
      token.type = TokenType::RIGHT_BRACKET;
      NextChar();
      break;
    case u'.':
      token.type = TokenType::DOT;
      NextChar();
      break;
    case u';':
      token.type = TokenType::SEMICOLON;
      NextChar();
      break;
    case u',':
      token.type = TokenType::COMMA;
      NextChar();
      break;
    case u'<':
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
    case u'>':
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
    case u'=':
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
    case u'!':
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
    case u'+':
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
    case u'-':
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
    case u'*':
      // * *=
      NextChar();
      if (ch_ == u'=') {
        token.type = TokenType::MUL_ASSIGN;
        NextChar();
      } else {
        token.type = TokenType::MUL;
      }
      break;
    case u'%':
      // % %=
      NextChar();
      if (ch_ == u'=') {
        token.type = TokenType::MOD_ASSIGN;
        NextChar();
      } else {
        token.type = TokenType::MOD;
      }
      break;
    case u'&':
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
    case u'|':
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
    case u'^':
      // ^ ^=
      NextChar();
      if (ch_ == u'=') {
        token.type = TokenType::BIT_XOR_ASSIGN;
        NextChar();
      } else {
        token.type = TokenType::BIT_XOR;
      }
      break;
    case u'~':
      token.type = TokenType::BIT_NOT;
      NextChar();
      break;
    case u'?':
      token.type = TokenType::QUESTION;
      NextChar();
      break;
    case u':':
      token.type = TokenType::COLON;
      NextChar();
      break;
    case u'/':
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
    default:
      if (character::IsLineTerminator(ch_)) {
        SkipLineTerminator();
        goto start;
      } else {
        
      }
      break;
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

}  // namespace voidjs
