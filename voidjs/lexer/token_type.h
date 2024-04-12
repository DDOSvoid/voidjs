#ifndef VOIDJS_LEXER_TOKEN_TYPE_H
#define VOIDJS_LEXER_TOKEN_TYPE_H

#include <array>
#include <string>
#include <unordered_map>

namespace voidjs {

// Defined in ECMAScript 5.1 Chapter 7
enum class TokenType {
  // Identifier
  IDENTIFIER,
    
  // ReservedWord
  KEYWORD,                            // not used
  FUTURE_RESERVED_WORD,
  STRICT_MODE_FUTURE_RESERVED_WORD,   // not used
  NULL_LITERAL,
  BOOLEAN_LITERAL,                    // not used
  
  TRUE,
  FALSE,

  KEYWORD_BREAK,
  KEYWORD_DO,
  KEYWORD_INSTANCEOF,
  KEYWORD_TYPEOF,
  KEYWORD_CASE,
  KEYWORD_ELSE,
  KEYWORD_NEW,
  KEYWORD_VAR,
  KEYWORD_CATCH,
  KEYWORD_FINALLY,
  KEYWORD_RETURN,
  KEYWORD_VOID,
  KEYWORD_CONTINUE,
  KEYWORD_FOR,
  KEYWORD_SWITCH,
  KEYWORD_WHILE,
  KEYWORD_DEBUGGER,
  KEYWORD_FUNCTION,
  KEYWORD_THIS,
  KEYWORD_WITH,
  KEYWORD_DEFAULT,
  KEYWORD_IF,
  KEYWORD_THROW,
  KEYWORD_DELETE,
  KEYWORD_IN,
  KEYWORD_TRY,

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

const std::unordered_map<std::u16string, TokenType> kStringToKeywords = {
  {u"break", TokenType::KEYWORD_BREAK},
  {u"do", TokenType::KEYWORD_DO},
  {u"instanceof", TokenType::KEYWORD_INSTANCEOF},
  {u"typeof", TokenType::KEYWORD_TYPEOF},
  {u"case", TokenType::KEYWORD_CASE},
  {u"else", TokenType::KEYWORD_ELSE},
  {u"new", TokenType::KEYWORD_NEW},
  {u"var", TokenType::KEYWORD_VAR},
  {u"catch", TokenType::KEYWORD_CATCH},
  {u"finally", TokenType::KEYWORD_FINALLY},
  {u"return", TokenType::KEYWORD_RETURN},
  {u"void", TokenType::KEYWORD_VOID},
  {u"continue", TokenType::KEYWORD_CONTINUE},
  {u"for", TokenType::KEYWORD_FOR},
  {u"switch", TokenType::KEYWORD_SWITCH},
  {u"while", TokenType::KEYWORD_WHILE},
  {u"debugger", TokenType::KEYWORD_DEBUGGER},
  {u"function", TokenType::KEYWORD_FUNCTION},
  {u"this", TokenType::KEYWORD_THIS},
  {u"with", TokenType::KEYWORD_WITH},
  {u"default", TokenType::KEYWORD_DEFAULT},
  {u"if", TokenType::KEYWORD_IF},
  {u"throw", TokenType::KEYWORD_THROW},
  {u"delete", TokenType::KEYWORD_DELETE},
  {u"in", TokenType::KEYWORD_IN},
  {u"try", TokenType::KEYWORD_TRY},
};

const std::array<std::u16string, 26> kKeywords = {
  u"break",       u"do",         u"instanceof",  u"typeof",
  u"case",        u"else",       u"new",         u"var",
  u"catch",       u"finally",    u"return",      u"void",
  u"continue",    u"for",        u"switch",      u"while",
  u"debugger",    u"function",   u"this",        u"with",
  u"default",     u"if",         u"throw",       u"delete",
  u"in",          u"try",
};

const std::array<std::u16string, 7> kFutureReservedWords = {
  u"class",       u"enum",       u"extends",     u"super",
  u"const",       u"export",     u"import",
};

const std::array<std::u16string, 9> kStrictModeFutureReservedWords = {
  u"implements",  u"let",        u"private",     u"public",
  u"yield",       u"interface",  u"package",     u"protected",
  u"static"
};

}  // namespace voidjs

#endif  // VOIDJS_LEXER_TOKEN_TYPE_H
