#include "voidjs/parser/lexer.h"

namespace voidjs {

Token Lexer::NextToken() {
  Token token;
  
  while (character::IsWhitespace(ch_)) NextChar();

  // Token ::
  //          IdentifierName
  //          Punctuator
  //          NumericLiteral
  //          StringLiteral
  // Defined in ECMAScript 5.1 Chapter 7.5
  switch (ch_) {
    
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

}  // namespace voidjs
