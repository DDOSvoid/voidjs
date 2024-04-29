#include "gtest/gtest.h"

#include <cstdint>

#include "voidjs/parser/parser.h"
#include "voidjs/types/js_value.h"
#include "voidjs/types/object_factory.h"
#include "voidjs/types/spec_types/completion.h"
#include "voidjs/types/lang_types/string.h"
#include "voidjs/types/lang_types/object.h"
#include "voidjs/types/internal_types/property_map.h"
#include "voidjs/builtins/js_object.h"
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

TEST(Interpreter, EvalIfStatement) {
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

TEST(Interpreter, EvalMemberExpression) {
  Parser parser(uR"(
var obj = {
    1      : 42,
    'name' : -2,
    value  : 43,
};
obj[1] + obj['value'] + obj.name;
)");

  Interpreter interpreter;

  auto prog = parser.ParseProgram();
  ASSERT_TRUE(prog->IsProgram());

  auto comp = interpreter.Execute(prog);
  EXPECT_EQ(types::CompletionType::NORMAL, comp.GetType());
  ASSERT_TRUE(comp.GetValue().IsInt());
  EXPECT_EQ(83, comp.GetValue().GetInt());
}

TEST(Interpreter, EvalPostfixExpression) {
  Parser parser(uR"(
var i = 14.2857;
--i;
++i;
i;
)");

  Interpreter interpreter;

  auto prog = parser.ParseProgram();
  ASSERT_TRUE(prog->IsProgram());
  
  auto comp = interpreter.Execute(prog);
  EXPECT_TRUE(comp.GetType() == types::CompletionType::NORMAL);
  ASSERT_TRUE(comp.GetValue().IsDouble());
  EXPECT_DOUBLE_EQ(14.2857, comp.GetValue().GetDouble());
}

TEST(Interpreter, EvalUnaryExpresion) {
  Parser parser(u"1 + -2");

  Interpreter interpreter;

  auto ast_node = parser.ParseBinaryExpression();
  ASSERT_TRUE(ast_node->IsBinaryExpression());

  auto val = interpreter.GetValue(interpreter.EvalExpression(ast_node->AsBinaryExpression()));
  ASSERT_TRUE(val.IsInt());
  EXPECT_EQ(-1, val.GetInt());
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

  {
    Parser parser(u"true || false");
  
    Interpreter interpreter;

    auto ast_node = parser.ParseBinaryExpression();
    ASSERT_TRUE(ast_node->IsBinaryExpression());

    auto val = interpreter.GetValue(interpreter.EvalExpression(ast_node->AsBinaryExpression()));
    ASSERT_TRUE(val.IsBoolean());
    EXPECT_EQ(true, val.GetBoolean());
  }

  {
    Parser parser(u"0x7 | 8");
  
    Interpreter interpreter;

    auto ast_node = parser.ParseBinaryExpression();
    ASSERT_TRUE(ast_node->IsBinaryExpression());

    auto val = interpreter.GetValue(interpreter.EvalExpression(ast_node->AsBinaryExpression()));
    ASSERT_TRUE(val.IsInt());
    EXPECT_EQ(15, val.GetInt());
  }

  {
    Parser parser(u"0.00000001 == 0.0");
  
    Interpreter interpreter;

    auto ast_node = parser.ParseBinaryExpression();
    ASSERT_TRUE(ast_node->IsBinaryExpression());

    auto val = interpreter.GetValue(interpreter.EvalExpression(ast_node->AsBinaryExpression()));
    ASSERT_TRUE(val.IsBoolean());
    EXPECT_EQ(false, val.GetBoolean());
  }

  {
    Parser parser(uR"(
"12.3e3" == 12300
)");
  
    Interpreter interpreter;

    auto ast_node = parser.ParseBinaryExpression();
    ASSERT_TRUE(ast_node->IsBinaryExpression());

    auto val = interpreter.GetValue(interpreter.EvalExpression(ast_node->AsBinaryExpression()));
    ASSERT_TRUE(val.IsBoolean());
    EXPECT_EQ(true, val.GetBoolean());
  }

  {
    Parser parser(u"3 * 4 < 12");
  
    Interpreter interpreter;

    auto ast_node = parser.ParseBinaryExpression();
    ASSERT_TRUE(ast_node->IsBinaryExpression());

    auto val = interpreter.GetValue(interpreter.EvalExpression(ast_node->AsBinaryExpression()));
    ASSERT_TRUE(val.IsBoolean());
    EXPECT_EQ(false, val.GetBoolean());
  }

  {
    Parser parser(u"0x7ff << 3");
  
    Interpreter interpreter;

    auto ast_node = parser.ParseBinaryExpression();
    ASSERT_TRUE(ast_node->IsBinaryExpression());

    auto val = interpreter.GetValue(interpreter.EvalExpression(ast_node->AsBinaryExpression()));
    ASSERT_TRUE(val.IsInt());
    EXPECT_EQ(16376, val.GetInt());
  }

  {
    Parser parser(u"(1 + 2) * 3 / 5");
  
    Interpreter interpreter;

    auto ast_node = parser.ParseBinaryExpression();
    ASSERT_TRUE(ast_node->IsBinaryExpression());

    auto val = interpreter.GetValue(interpreter.EvalExpression(ast_node->AsBinaryExpression()));
    ASSERT_TRUE(val.IsDouble());
    EXPECT_DOUBLE_EQ(1.8, val.GetDouble());
  }

  {
    Parser parser(uR"(
("1" + ".5") * 3
)");
  
    Interpreter interpreter;

    auto ast_node = parser.ParseBinaryExpression();
    ASSERT_TRUE(ast_node->IsBinaryExpression());

    auto val = interpreter.GetValue(interpreter.EvalExpression(ast_node->AsBinaryExpression()));
    ASSERT_TRUE(val.IsDouble());
    EXPECT_DOUBLE_EQ(4.5, val.GetDouble());
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

TEST(Interpreter, EvalObjectLiteral) {
  {
    Parser parser(u"{}");

    Interpreter interpreter;
    auto vm = interpreter.GetVM();

    auto ast_node = parser.ParsePrimaryExpression();
    ASSERT_TRUE(ast_node->IsObjectLiteral());

    auto obj = interpreter.EvalObjectLiteral(ast_node->AsObjectLiteral());
    ASSERT_TRUE(obj.IsObject());
    EXPECT_TRUE(JSValue(vm->GetObjectPrototype()) == obj.GetHeapObject()->AsJSObject()->GetPrototype());
  }

  {
    Parser parser(uR"(
{
    1      : 42,
    name   : 'Test',
    value  : 43,
}
)");

    Interpreter interpreter;
    auto vm = interpreter.GetVM();
    auto factory = vm->GetObjectFactory();

    auto ast_node = parser.ParsePrimaryExpression();
    ASSERT_TRUE(ast_node->IsObjectLiteral());

    auto obj_val = interpreter.EvalObjectLiteral(ast_node->AsObjectLiteral());
    ASSERT_TRUE(obj_val.IsObject());
    ASSERT_TRUE(obj_val.GetHeapObject()->IsJSObject());

    auto obj = obj_val.GetHeapObject()->AsJSObject();

    {
      auto key = factory->NewStringFromTable(u"1");
      auto prop = types::Object::GetProperty(vm, obj, key);
      ASSERT_TRUE(prop.GetValue().IsInt());
      EXPECT_EQ(42, prop.GetValue().GetInt());
    }
  }
}
