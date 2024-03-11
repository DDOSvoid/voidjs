#ifndef VOIDJS_TOKEN_H
#define VOIDJS_TOKEN_H

#include <array>
#include <string>

namespace voidjs {

// Defined in ECMAScript 5.1 Chapter 7
enum class TokenType {
  // Identifier
  IDENTIFIER,
    
  // ReservedWord
  KEYWORD,
  FUTURE_RESERVED_WORD,
  NULL_LITERAL,
  BOOLEAN_LITERAL,

  // Punctuator
  LEFT_BRACE,            // {
  RIGHT_BRACE,           // }
  LEFT_PAREN,            // (
  RIGHT_PAREN,           // )
  LEFT_BRACKET,          // [
  RIGHT_BRACKET,         // ]
  DOT,                   // .
  SEMICOLON,             // ;
  COMMA,                 // ,
  LESS_THAN,             // <
  GREATER_THAN,          // >
  LESS_EQUAL,            // <= 
  GREATER_EQUAL,         // >=
  EQUAL,                 // ==
  NOT_EQUAL,             // !=
  STRICT_EQUAL,          // === 
  NOT_STRICT_EQUAL,      // !==
  ADD,                   // +
  SUB,                   // -
  MUL,                   // *
  MOD,                   // %
  INC,                   // ++
  DEC,                   // --
  LEFT_SHIFT,            // <<
  RIGHT_SHIFT,           // >>
  U_RIGHT_SHIFT,         // >>> 
  BIT_AND,               // &
  BIT_OR,                // |
  BIT_XOR,               // ^
  LOGICAL_NOT,           // !
  BIT_NOT,               // ~
  LOGICAL_AND,           // && 
  LOGICAL_OR,            // ||
  QUESTION,              // ?
  COLON,                 // :
  ASSIGN,                // =
  ADD_ASSIGN,            // +=
  SUB_ASSIGN,            // -=
  MUL_ASSIGN,            // *=
  MOD_ASSIGN,            // %=
  LEFT_SHIFT_ASSIGN,     // <<=
  RIGHT_SHIFT_ASSIGN,    // >>=
  U_RIGHT_SHIFT_ASSIGN,  // >>>=
  BIT_AND_ASSIGN,        // &=
  BIT_OR_ASSIGN,         // |= 
  BIT_XOR_ASSIGN,        // ^=

  // DivPunctuator
  DIV,                   // /
  DIV_ASSIGN,            // /=

  // NumericLiteral
  NUMBER,

  // StringLiteral
  STRING,

  // Types defined below are not part of the standard
  
  // Comment
  SINGLE_LINE_COMMENT,
  MULTI_LINE_COMMENT,
  
  // Illegal
  ILLEGAL,

  // End of Source
  EOS, 
};

const std::array<std::u16string, 26> kKeywords = {
  u"break",      u"do",        u"instanceof",  u"typeof",
  u"case",       u"else",      u"new",         u"var",
  u"catch",      u"finally",   u"return",      u"void",
  u"continue",   u"for",       u"switch",      u"while",
  u"debugger",   u"function",  u"this",        u"with",
  u"default",    u"if",        u"throw",
  u"delete",     u"in",        u"try",
};

const std::array<std::u16string, 7> kFutureReservedWords = {
  u"class",      u"enum",      u"extends",     u"super",
  u"const",      u"export",    u"import",
};

const std::array<std::u16string, 9> kStrictModeFutureReservedWords = {
  u"implements", u"let",       u"private",     u"public", u"yield",
  u"interface",  u"package",   u"protected",   u"static"
};

struct Token {
  TokenType type {TokenType::EOS};
  std::u16string str;
  std::size_t start {};
  std::size_t end {};
};
 
}  // namespace voidjs

#endif  // VOIDJS_TOKEN_H
