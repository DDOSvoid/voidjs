#include <cstdint>

#include "gtest/gtest.h"
#include "voidjs/parser/parser.h"
#include "voidjs/types/js_value.h"
#include "voidjs/types/object_factory.h"
#include "voidjs/types/spec_types/completion.h"
#include "voidjs/types/lang_types/string.h"
#include "voidjs/types/lang_types/object.h"
#include "voidjs/types/internal_types/property_map.h"
#include "voidjs/interpreter/interpreter.h"
#include "voidjs/utils/helper.h"

using namespace voidjs;

TEST(Interpreter, EvalBlockStatement) {
  {
    Parser parser(u"{ }");

    Interpreter interpreter;

    auto ast_node = parser.ParseBlockStatement();
    ASSERT_TRUE(ast_node->IsBlockStatement());

    auto comp = interpreter.EvalBlockStatement(ast_node->AsBlockStatement());
    EXPECT_EQ(types::CompletionType::NORMAL, comp.GetType());
    EXPECT_TRUE(comp.GetValue().IsEmpty());
  }
}

TEST(Interpreter, EvalVariableStatement) {
  {
    Parser parser(u"var i = 42; i + 2;");

    Interpreter interpreter;

    auto prog = parser.ParseProgram();
    ASSERT_TRUE(prog->IsProgram());

    auto comp = interpreter.Execute(prog);
    EXPECT_EQ(types::CompletionType::NORMAL, comp.GetType());
    ASSERT_TRUE(comp.GetValue().IsInt());
    EXPECT_EQ(44, comp.GetValue().GetInt());
  }

  {
    Parser parser(u"var i = 42, j = i - i; j;");

    Interpreter interpreter;

    auto prog = parser.ParseProgram();
    ASSERT_TRUE(prog->IsProgram());

    auto comp = interpreter.Execute(prog);
    EXPECT_EQ(types::CompletionType::NORMAL, comp.GetType());
    ASSERT_TRUE(comp.GetValue().IsInt());
    EXPECT_EQ(0, comp.GetValue().GetInt());
  }
}

TEST(Interpreter, EvalEmptyStatement) {
  Parser parser(u";");
  
  Interpreter interpreter;

  auto ast_node = parser.ParseEmptyStatement();
  ASSERT_TRUE(ast_node->IsEmptyStatement());

  auto comp = interpreter.EvalEmptyStatement(ast_node->AsEmptyStatement());
  EXPECT_EQ(types::CompletionType::NORMAL, comp.GetType());
}

TEST(Interpreter, EvalExpressionStatement) {
}

TEST(Itnerpreter, EvalIfStatement) {
  {
    std::u16string source = uR"(
var i = 42, j = i - 40;
if (i - 42) {
  i;
} else {
  j;
}
)";

    Parser parser(source);

    Interpreter interpreter;

    auto prog = parser.ParseProgram();
    ASSERT_TRUE(prog->IsProgram());

    auto comp = interpreter.Execute(prog);
    EXPECT_EQ(types::CompletionType::NORMAL, comp.GetType());
    ASSERT_TRUE(comp.GetValue().IsInt());
    EXPECT_EQ(2, comp.GetValue().GetInt());
  }
}

TEST(Interpreter, EvalDoWhileStatement) {
  {
    std::u16string source = uR"(
var i = 0;
var n = 10;
var count = 0;
do {
    i += 1;
    count += i;
} while (i < n);
count;
)";

    Parser parser(source);

    Interpreter interpreter;

    auto prog = parser.ParseProgram();
    ASSERT_TRUE(prog->IsProgram());

    auto comp = interpreter.Execute(prog);
    EXPECT_EQ(types::CompletionType::NORMAL, comp.GetType());
    ASSERT_TRUE(comp.GetValue().IsInt());
    EXPECT_EQ(55, comp.GetValue().GetInt());
  }
}

TEST(Interpreter, EvalWhileStatement) {
  {
    std::u16string source = uR"(
var i = 0;
var n = i;
while (i < n) {
    i += 1; 
}
i;
)";

    Parser parser(source);

    Interpreter interpreter;

    auto prog = parser.ParseProgram();
    ASSERT_TRUE(prog->IsProgram());

    auto comp = interpreter.Execute(prog);
    EXPECT_EQ(types::CompletionType::NORMAL, comp.GetType());
    ASSERT_TRUE(comp.GetValue().IsInt());
    EXPECT_EQ(0, comp.GetValue().GetInt());
  }
}

TEST(Interpreter, EvalForStatement) {
  {
    std::u16string source = uR"(
var count = 0;
var n = 1000000000;
var f = 1, g = 1;
for (; g < n; count += 1) {
    f = f + g;
    var tmp = f;
    f = g;
    g = tmp;
}
count;
)";
    
    Parser parser(source);

    Interpreter interpreter;

    auto prog = parser.ParseProgram();
    ASSERT_TRUE(prog->IsProgram());

    auto comp = interpreter.Execute(prog);
    EXPECT_EQ(types::CompletionType::NORMAL, comp.GetType());
    ASSERT_TRUE(comp.GetValue().IsInt());
    EXPECT_EQ(43, comp.GetValue().GetInt());
  }
}

TEST(Interpreter, EvalBinaryExpression) {
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

  EXPECT_EQ(u"Hello, World!", str.GetHeapObject()->AsString()->GetString());
}
