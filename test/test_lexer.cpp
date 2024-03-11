#include <vector>
#include <any>

#include "gtest/gtest.h"
#include "voidjs/parser/lexer.h"
#include "voidjs/parser/token.h"

using namespace voidjs;

TEST(Lexer, PunctuatorTest) {
  Lexer lexer(u"{}()[].;,< << <= <<= > >> >>> >= >>= >>>= ! != !== + ++ += - -- -= * *= % %= & && &= | || |= ^ ^= ~ ? : / /=");

  std::vector<std::any> expects = {
    {TokenType::LEFT_BRACE},            // {
    {TokenType::RIGHT_BRACE},           // }
    {TokenType::LEFT_PAREN},            // (
    {TokenType::RIGHT_PAREN},           // )
    {TokenType::LEFT_BRACKET},          // [
    {TokenType::RIGHT_BRACKET},         // ]
    {TokenType::DOT},                   // .
    {TokenType::SEMICOLON},             // ;
    {TokenType::COMMA},                 // ,
    {TokenType::LESS_THAN},             // <
    {TokenType::LEFT_SHIFT},            // <<
    {TokenType::LESS_EQUAL},            // <=
    {TokenType::LEFT_SHIFT_ASSIGN},     // <<=
    {TokenType::GREATER_THAN},          // >
    {TokenType::RIGHT_SHIFT},           // >>
    {TokenType::U_RIGHT_SHIFT},         // >>>
    {TokenType::GREATER_EQUAL},         // >=
    {TokenType::RIGHT_SHIFT_ASSIGN},    // >>=
    {TokenType::U_RIGHT_SHIFT_ASSIGN},  // >>>=
    {TokenType::LOGICAL_NOT},           // !
    {TokenType::NOT_EQUAL},             // !=
    {TokenType::NOT_STRICT_EQUAL},      // !==
    {TokenType::ADD},                   // +
    {TokenType::INC},                   // ++
    {TokenType::ADD_ASSIGN},            // +=
    {TokenType::SUB},                   // -
    {TokenType::DEC},                   // --
    {TokenType::SUB_ASSIGN},            // -=
    {TokenType::MUL},                   // *
    {TokenType::MUL_ASSIGN},            // *= 
    {TokenType::MOD},                   // %
    {TokenType::MOD_ASSIGN},            // %=
    {TokenType::BIT_AND},               // &
    {TokenType::LOGICAL_AND},           // &&
    {TokenType::BIT_AND_ASSIGN},        // &=
    {TokenType::BIT_OR},                // |
    {TokenType::LOGICAL_OR},            // ||
    {TokenType::BIT_OR_ASSIGN},         // |=
    {TokenType::BIT_XOR},               // ^
    {TokenType::BIT_XOR_ASSIGN},        // ^=
    {TokenType::BIT_NOT},               // ~
    {TokenType::QUESTION},              // ?
    {TokenType::COLON},                 // :
    {TokenType::DIV},                   // /
    {TokenType::DIV_ASSIGN},            // /=
    {TokenType::EOS},                   // EOS
  };

  for (auto& expect : expects) {
    EXPECT_EQ(std::any_cast<TokenType>(expect), lexer.NextToken().type);
  }
}
