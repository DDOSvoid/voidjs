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

    auto num = interpreter.EvalNumericLiteral(ast_node);
    EXPECT_EQ(0.07, num->GetValue<double>());
  }

  {
    Parser parser(u"42");
  
    Interpreter interpreter;

    auto ast_node = parser.ParsePrimaryExpression();
    ASSERT_TRUE(ast_node->IsNumericLiteral());

    auto num = interpreter.EvalNumericLiteral(ast_node);
    EXPECT_EQ(42, num->GetValue<std::int32_t>());
  }
}

TEST(Intepreter, EvalStringLiteral) {

  Parser parser(uR"("Hello, World!")");
  
  Interpreter interpreter;

  auto ast_node = parser.ParsePrimaryExpression();
  ASSERT_TRUE(ast_node->IsStringLiteral());

  auto str = interpreter.EvalStringLiteral(ast_node);

  EXPECT_EQ(u"Hello, World!", str->GetValue());
}
