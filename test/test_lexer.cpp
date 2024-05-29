#include <vector>
#include <any>

#include "gtest/gtest.h"
#include "voidjs/lexer/lexer.h"
#include "voidjs/lexer/token.h"

using namespace voidjs;

TEST(Lexer, Punctuator) {
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
  };

  for (auto& expect : expects) {
    lexer.NextToken();
    EXPECT_EQ(std::any_cast<TokenType>(expect), lexer.GetToken().GetType());
  }
}

TEST(Lexer, Comment) {
  std::u16string source = uR"(
+
// -- ++
/* << >>
&&& */
-
|||
)";

  Lexer lexer(source); 

  std::vector<std::any> expects = {
    {TokenType::ADD},
    {TokenType::SUB},
    {TokenType::LOGICAL_OR},
    {TokenType::BIT_OR},
  };

  for (auto& expect : expects) {
    lexer.NextToken();
    EXPECT_EQ(std::any_cast<TokenType>(expect), lexer.GetToken().GetType());
  }
}

TEST(Lexer, Identifier) {
  std::u16string source = uR"(
value x
var = // dsdsfdf
enum private
/* public */ test
A.B.C
)";

  Lexer lexer(source);

  std::vector<Token> expects = {
    {TokenType::IDENTIFIER, u"value"},
    {TokenType::IDENTIFIER, u"x"},
    {TokenType::KEYWORD_VAR},
    {TokenType::ASSIGN},
    {TokenType::FUTURE_RESERVED_WORD, u"enum"},
    {TokenType::IDENTIFIER, u"private"},
    {TokenType::IDENTIFIER, u"test"},
    {TokenType::IDENTIFIER, u"A"},
    {TokenType::DOT},
    {TokenType::IDENTIFIER, u"B"},
    {TokenType::DOT},
    {TokenType::IDENTIFIER, u"C"},
  };
  
  for (auto& expect_token : expects) {
    lexer.NextToken();
    auto& token = lexer.GetToken();
    EXPECT_EQ(expect_token.GetType(), token.GetType());
    if (token.HasString()) {
      EXPECT_EQ(expect_token.GetString(), token.GetString());
    }
  }
}

TEST(Lexer, NumericLiteral) {
  // This tests is copied from
  // https://github.com/zhuzilin/es/blob/main/test/test_lexer.cc#L77
  std::u16string source = uR"(
0 101 0.01 12.05 .8 0xAbC09
1e10 101E02 0.01E5 .8E5 12.05e05 123e-1 0x7ffffffe
)";

  Lexer lexer(source);

  std::vector<Token> expects = {
    {TokenType::NUMBER, u"", 0},
    {TokenType::NUMBER, u"", 101},
    {TokenType::NUMBER, u"", 0.01},
    {TokenType::NUMBER, u"", 12.05},
    {TokenType::NUMBER, u"", 0.8},
    {TokenType::NUMBER, u"", 703497},
    {TokenType::NUMBER, u"", 1e10},
    {TokenType::NUMBER, u"", 10100},
    {TokenType::NUMBER, u"", 1000},
    {TokenType::NUMBER, u"", 80000},
    {TokenType::NUMBER, u"", 1205000},
    {TokenType::NUMBER, u"", 12.3},
    {TokenType::NUMBER, u"", 2147483646},
  };
  
  for (auto& expect_token : expects) {
    lexer.NextToken();
    auto& token = lexer.GetToken();
    EXPECT_EQ(expect_token.GetType(), token.GetType());
    EXPECT_DOUBLE_EQ(expect_token.GetNumber(), token.GetNumber());
  }
}

TEST(Lexer, StringLiteral) {
  std::u16string source = uR"(
'asd' '' ""
"
"
"😊"
'\u1234'
"输出：你的名字叫什么？"
)";

  Lexer lexer(source);

  std::vector<Token> expects = {
    {TokenType::STRING, uR"(asd)"},
    {TokenType::STRING, uR"()"},
    {TokenType::STRING, uR"()"},
    {TokenType::STRING, u"\n"},
    {TokenType::STRING, uR"(😊)"},
    {TokenType::STRING, std::u16string(1, 4660)},
    {TokenType::STRING, u"输出：你的名字叫什么？"},
  };
  
  for (auto& expect_token : expects) {
    lexer.NextToken();
    auto& token = lexer.GetToken();
    EXPECT_EQ(expect_token.GetType(), token.GetType());
    if (token.HasString()) {
      EXPECT_EQ(expect_token.GetString(), token.GetString());
    }
  }
}

TEST(Lexer, Lexer) {
  std::u16string source = uR"(
function fnSupportsStrict() {
  "use strict";
  try {
      eval('with ({}) {}');
      return false;
  } catch (e) {
      return true;
  }
}
)";

  Lexer lexer(source);

  std::vector<Token> expects = {
    {TokenType::KEYWORD_FUNCTION},
    {TokenType::IDENTIFIER, u"fnSupportsStrict"},
    {TokenType::LEFT_PAREN},
    {TokenType::RIGHT_PAREN},
    {TokenType::LEFT_BRACE},
    {TokenType::STRING, uR"(use strict)"},
    {TokenType::SEMICOLON},
    {TokenType::KEYWORD_TRY},
    {TokenType::LEFT_BRACE},
    {TokenType::IDENTIFIER, u"eval"},
    {TokenType::LEFT_PAREN},
    {TokenType::STRING, uR"(with ({}) {})"},
    {TokenType::RIGHT_PAREN},
    {TokenType::SEMICOLON},
    {TokenType::KEYWORD_RETURN},
    {TokenType::FALSE, },
    {TokenType::SEMICOLON},
    {TokenType::RIGHT_BRACE},
    {TokenType::KEYWORD_CATCH},
    {TokenType::LEFT_PAREN},
    {TokenType::IDENTIFIER, u"e"},
    {TokenType::RIGHT_PAREN},
    {TokenType::LEFT_BRACE},
    {TokenType::KEYWORD_RETURN},
    {TokenType::TRUE},
    {TokenType::SEMICOLON},
    {TokenType::RIGHT_BRACE},
    {TokenType::RIGHT_BRACE},
    {TokenType::EOS},
  };
  
  for (auto& expect_token : expects) {
    lexer.NextToken();
    auto& token = lexer.GetToken();
    EXPECT_EQ(expect_token.GetType(), token.GetType());
    if (token.HasString()) {
      EXPECT_EQ(expect_token.GetString(), token.GetString());
    }
  }
}
