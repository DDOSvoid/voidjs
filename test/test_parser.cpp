#include <vector>
#include <any>
#include <string>

#include "gtest/gtest.h"
#include "voidjs/lexer/token.h"
#include "voidjs/ir/ast.h"
#include "voidjs/ir/expression.h"
#include "voidjs/ir/statement.h"
#include "voidjs/ir/literal.h"
#include "voidjs/lexer/token_type.h"
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

TEST(parser, ParseLeftHandSideExpression) {
  // MemberExpression . IdentifierName
  {
    Parser parser(u"Array.length");

    auto expr = parser.ParseLeftHandSideExpression();
    ASSERT_TRUE(expr->IsMemberExpression());
    
    auto mem_expr = expr->AsMemberExpression();
    ASSERT_TRUE(mem_expr->GetObject()->IsIdentifier());
    ASSERT_TRUE(mem_expr->GetProperty()->IsIdentifier());
    EXPECT_EQ(u"Array", mem_expr->GetObject()->AsIdentifier()->GetName());
    EXPECT_EQ(u"length", mem_expr->GetProperty()->AsIdentifier()->GetName());
  }

  // MemberExpression . IdentifierName . IdentifierName
  {
    Parser parser(u"DDOSvoid.ZigZagZing.vid");

    auto expr = parser.ParseLeftHandSideExpression();
    ASSERT_TRUE(expr->IsMemberExpression());
    
    auto mem_expr1 = expr->AsMemberExpression();
    ASSERT_TRUE(mem_expr1->GetObject()->IsMemberExpression());
    ASSERT_TRUE(mem_expr1->GetProperty()->IsIdentifier());
    EXPECT_EQ(u"vid", mem_expr1->GetProperty()->AsIdentifier()->GetName());
    
    auto mem_expr2 = mem_expr1->GetObject()->AsMemberExpression();
    ASSERT_TRUE(mem_expr2->GetObject()->IsIdentifier());
    ASSERT_TRUE(mem_expr2->GetProperty()->IsIdentifier());
    EXPECT_EQ(u"DDOSvoid", mem_expr2->GetObject()->AsIdentifier()->GetName());
    EXPECT_EQ(u"ZigZagZing", mem_expr2->GetProperty()->AsIdentifier()->GetName());
  }

  // New MemberExpression
  {
    Parser parser(u"new A");

    auto expr = parser.ParseLeftHandSideExpression();
    ASSERT_TRUE(expr->IsNewExpression());

    auto new_expr = expr->AsNewExpression();
    ASSERT_TRUE(new_expr->GetConstructor()->IsIdentifier());
    EXPECT_EQ(u"A", new_expr->GetConstructor()->AsIdentifier()->GetName());
  }

  // New MemberExpression Arguments
  {
    Parser parser(u"new A()");

    auto expr = parser.ParseLeftHandSideExpression();
    ASSERT_TRUE(expr->IsNewExpression());

    auto new_expr = expr->AsNewExpression();
    ASSERT_TRUE(new_expr->GetConstructor()->IsIdentifier());
    EXPECT_EQ(u"A", new_expr->GetConstructor()->AsIdentifier()->GetName());
    EXPECT_EQ(0, new_expr->GetArguments().size());
  }

  // New New MemberExpression . IdentifierName Arguments Arguments
  {
    Parser parser(u"new new DDOSvoid.ddos()()");

    auto expr = parser.ParseLeftHandSideExpression();
    ASSERT_TRUE(expr->IsNewExpression());

    auto new_expr1 = expr->AsNewExpression();
    ASSERT_TRUE(new_expr1->GetConstructor()->IsNewExpression());
    EXPECT_EQ(0, new_expr1->GetArguments().size());

    auto new_expr2 = new_expr1->GetConstructor()->AsNewExpression();
    ASSERT_TRUE(new_expr2->GetConstructor()->IsMemberExpression());
    EXPECT_EQ(0, new_expr1->GetArguments().size());

    auto mem_expr = new_expr2->GetConstructor()->AsMemberExpression();
    ASSERT_TRUE(mem_expr->GetObject()->IsIdentifier());
    ASSERT_TRUE(mem_expr->GetProperty()->IsIdentifier());
    EXPECT_EQ(u"DDOSvoid", mem_expr->GetObject()->AsIdentifier()->GetName());
    EXPECT_EQ(u"ddos", mem_expr->GetProperty()->AsIdentifier()->GetName());
  }
}

TEST(parser, ParsePostfixExpression) {
  // ++
  {
    Parser parser(u"Array.length++");

    auto expr = parser.ParsePostfixExpression();
    ASSERT_TRUE(expr->IsPostfixExpression());

    auto post_expr = expr->AsPostfixExpression();
    EXPECT_EQ(TokenType::INC, post_expr->GetOperator());
    ASSERT_TRUE(post_expr->GetExpression()->IsMemberExpression());
    
    auto mem_expr = post_expr->GetExpression()->AsMemberExpression();
    ASSERT_TRUE(mem_expr->GetObject()->IsIdentifier());
    ASSERT_TRUE(mem_expr->GetProperty()->IsIdentifier());
    EXPECT_EQ(u"Array", mem_expr->GetObject()->AsIdentifier()->GetName());
    EXPECT_EQ(u"length", mem_expr->GetProperty()->AsIdentifier()->GetName());
  }

  // --
  {
    Parser parser(u"i--");

    auto expr = parser.ParsePostfixExpression();
    ASSERT_TRUE(expr->IsPostfixExpression());

    auto post_expr = expr->AsPostfixExpression();
    ASSERT_TRUE(post_expr->GetExpression()->IsIdentifier());
    EXPECT_EQ(TokenType::DEC, post_expr->GetOperator());
    EXPECT_EQ(u"i", post_expr->GetExpression()->AsIdentifier()->GetName());
  }
}

TEST(parser, ParseUnaryExpression) {
  // delete UnaryExpression
  {
    Parser parser(u"delete Array");

    auto expr = parser.ParseUnaryExpression();
    ASSERT_TRUE(expr->IsUnaryExpression());

    auto unary_expr = expr->AsUnaryExpression();
    ASSERT_TRUE(unary_expr->GetExpression()->IsIdentifier());
    EXPECT_EQ(TokenType::KEYWORD_DELETE, unary_expr->GetOperator());
    EXPECT_EQ(u"Array", unary_expr->GetExpression()->AsIdentifier()->GetName());
  }
}

TEST(parser, ParseBinaryExpression) {
  {
    Parser parser(u"1 + 2 * 3");

    auto expr = parser.ParseBinaryExpression();
    ASSERT_TRUE(expr->IsBinaryExpression());

    auto binary_expr1 = expr->AsBinaryExpression();
    ASSERT_TRUE(binary_expr1->GetLeft()->IsNumericLiteral());
    ASSERT_TRUE(binary_expr1->GetRight()->IsBinaryExpression());
    EXPECT_EQ(TokenType::ADD, binary_expr1->GetOperator());
    EXPECT_EQ(1, binary_expr1->GetLeft()->AsNumericLiteral()->GetNumber<std::int32_t>());

    auto binary_expr2 = binary_expr1->GetRight()->AsBinaryExpression();
    ASSERT_TRUE(binary_expr2->GetLeft()->IsNumericLiteral());
    ASSERT_TRUE(binary_expr2->GetRight()->IsNumericLiteral());
    EXPECT_EQ(TokenType::MUL, binary_expr2->GetOperator());
    EXPECT_EQ(2, binary_expr2->GetLeft()->AsNumericLiteral()->GetNumber<std::int32_t>());
    EXPECT_EQ(3, binary_expr2->GetRight()->AsNumericLiteral()->GetNumber<std::int32_t>());
  }
}

