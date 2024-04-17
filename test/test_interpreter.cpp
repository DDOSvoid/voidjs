#include <cstdint>

#include "gtest/gtest.h"
#include "voidjs/parser/parser.h"
#include "voidjs/interpreter/interpreter.h"
#include "voidjs/utils/helper.h"

using namespace voidjs;

TEST(Interpreter, EvalNullLiteral) {
}

TEST(Intepreter, EvalBooleanLiteral) {
}

TEST(Intepreter, EvalNumericLiteral) {
  {
    Parser parser(u"0.07");
  
    Interpreter interpreter;

    auto ast_node = parser.ParsePrimaryExpression();
    ASSERT_TRUE(ast_node->IsNumericLiteral());

    auto num = interpreter.EvalNumericLiteral(ast_node->AsNumericLiteral());
    EXPECT_EQ(0.07, num.GetDouble());
  }

  {
    Parser parser(u"42");
  
    Interpreter interpreter;

    auto ast_node = parser.ParsePrimaryExpression();
    ASSERT_TRUE(ast_node->IsNumericLiteral());

    auto num = interpreter.EvalNumericLiteral(ast_node->AsNumericLiteral());
    EXPECT_EQ(42, num.GetInt());
  }
}

TEST(Intepreter, EvalStringLiteral) {
  Parser parser(uR"("Hello, World!")");
  
  Interpreter interpreter;

  auto ast_node = parser.ParsePrimaryExpression();
  ASSERT_TRUE(ast_node->IsStringLiteral());

  auto str = interpreter.EvalStringLiteral(ast_node->AsStringLiteral());
  ASSERT_TRUE(str.IsString());

  EXPECT_EQ(u"Hello, World!", str.GetObject()->AsString()->GetString());
}

TEST(Intepreter, EvalBinaryExpression) {
  {
    Parser parser(u"1 + (2 + 3)");
  
    Interpreter interpreter;

    auto ast_node = parser.ParseBinaryExpression();
    ASSERT_TRUE(ast_node->IsBinaryExpression());

    auto val = interpreter.GetValue(interpreter.EvalExpression(ast_node->AsBinaryExpression()));
    ASSERT_TRUE(val.IsInt());
    EXPECT_EQ(6, val.GetInt());
  }
}
