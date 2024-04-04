#include <vector>
#include <any>

#include "gtest/gtest.h"
#include "voidjs/lexer/token.h"
#include "voidjs/ir/ast.h"
#include "voidjs/ir/expression.h"
#include "voidjs/ir/statement.h"
#include "voidjs/ir/literal.h"
#include "voidjs/parser/parser.h"

using namespace voidjs;

TEST(parser, ParsePrimaryExpression) {

  // Null
  {
    std::u16string source = u"null";

    Parser parser(source);

    auto expr = parser.ParsePrimaryExpression();
    EXPECT_EQ(ast::AstNodeType::NULL_LITERAL, expr->GetType());
  }

  // Boolean
  {
    std::u16string source = u"true false";

    Parser parser(source);

    std::vector<std::int32_t> expects = {
      true,
      false,
    };

    for (auto& expect : expects) {
      auto expr = parser.ParsePrimaryExpression();
      EXPECT_EQ(ast::AstNodeType::BOOLEAN_LITERAL, expr->GetType());
      EXPECT_EQ(expect, expr->AsLiteral()->GetBoolean());
    }
  }

  // Identifier
  {
    std::u16string source = u"True False DDOSvoid Void";

    Parser parser(source);

    std::vector<std::u16string> expects = {
      u"True",
      u"False",
      u"DDOSvoid",
      u"Void",
    };

    for (auto& expect : expects) {
      auto expr = parser.ParsePrimaryExpression();
      EXPECT_EQ(ast::AstNodeType::IDENTIFIER, expr->GetType());
      EXPECT_EQ(expect, expr->AsIdentifier()->GetName());
    }
  }

  // Number
  {
    std::u16string source = uR"(
0 101 0.01 12.05 .8 0xAbC09
)";

    Parser parser(source);

    std::vector<double> expects = {
      0,
      101,
      0.01,
      12.05,
      0.8,
      703497,
    };

    for (auto& expect : expects) {
      auto expr = parser.ParsePrimaryExpression();
      EXPECT_EQ(ast::AstNodeType::NUMERIC_LITERAL, expr->GetType());
      EXPECT_DOUBLE_EQ(expect, expr->AsLiteral()->GetDouble());
      EXPECT_EQ(static_cast<std::int32_t>(expect), expr->AsLiteral()->GetInt());
    }
  }

  // String
  {
     std::u16string source = uR"(
"hello, world"
'😊'
'\u1234'
)";

    Parser parser(source);

    std::vector<std::u16string> expects = {
      uR"(hello, world)",
      uR"(😊)",
      std::u16string(1, 4660),
    };

    for (auto& expect : expects) {
      auto expr = parser.ParsePrimaryExpression();
      EXPECT_EQ(ast::AstNodeType::STRING_LITERAL, expr->GetType());
      EXPECT_EQ(expect, expr->AsLiteral()->GetString());
    }
  }

}

