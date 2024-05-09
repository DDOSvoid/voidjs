#include "gtest/gtest.h"

#include <cstdint>

#include "voidjs/parser/parser.h"
#include "voidjs/types/heap_object.h"
#include "voidjs/types/js_value.h"
#include "voidjs/types/object_factory.h"
#include "voidjs/types/spec_types/completion.h"
#include "voidjs/types/lang_types/string.h"
#include "voidjs/types/lang_types/object.h"
#include "voidjs/types/internal_types/property_map.h"
#include "voidjs/builtins/js_object.h"
#include "voidjs/builtins/js_array.h"
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
    ASSERT_TRUE(comp.GetValue()->IsInt());
    EXPECT_EQ(44, comp.GetValue()->GetInt());
  }

  {
    Parser parser(u"var i = 42, j = i - i; j;");

    Interpreter interpreter;

    auto prog = parser.ParseProgram();
    ASSERT_TRUE(prog->IsProgram());

    auto comp = interpreter.Execute(prog);
    EXPECT_EQ(types::CompletionType::NORMAL, comp.GetType());
    ASSERT_TRUE(comp.GetValue()->IsInt());
    EXPECT_EQ(0, comp.GetValue()->GetInt());
  }

  {
    Parser parser(u"var l = 1, r = 100, mid = l + r >> 1; mid;");

    Interpreter interpreter;

    auto prog = parser.ParseProgram();
    ASSERT_TRUE(prog->IsProgram());

    auto comp = interpreter.Execute(prog);
    EXPECT_EQ(types::CompletionType::NORMAL, comp.GetType());
    ASSERT_TRUE(comp.GetValue()->IsInt());
    EXPECT_EQ(50, comp.GetValue()->GetInt());
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
    ASSERT_TRUE(comp.GetValue()->IsInt());
    EXPECT_EQ(2, comp.GetValue()->GetInt());
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
    ASSERT_TRUE(comp.GetValue()->IsInt());
    EXPECT_EQ(55, comp.GetValue()->GetInt());
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
    ASSERT_TRUE(comp.GetValue()->IsInt());
    EXPECT_EQ(0, comp.GetValue()->GetInt());
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
    ASSERT_TRUE(comp.GetValue()->IsInt());
    EXPECT_EQ(43, comp.GetValue()->GetInt());
  }
}

TEST(Interpreter, EvalContinueStatement) {
  Parser parser(uR"(
var i = 0;
var j = 0;
var n = 10;
var m = 5;
while (i < n) {
    ++i;
    if (i < m) {
        continue;
    }
    ++j;
}
j;
)");

  Interpreter interpreter;

  auto prog = parser.ParseProgram();
  ASSERT_TRUE(prog->IsProgram());

  auto comp = interpreter.Execute(prog);
  EXPECT_EQ(types::CompletionType::NORMAL, comp.GetType());
  ASSERT_TRUE(comp.GetValue()->IsInt());
  EXPECT_EQ(6, comp.GetValue()->GetInt());
}

TEST(Interpreter, EvalBreakStatement) {
  {
    Parser parser(uR"(
var num = 42;
var n = 100, l = 1, r = n, count = 0;
while (l <= r) {
    var mid = l + r >> 1;
    ++count;
    if (mid == num) {
        ans = mid;
        break;
    }
    if (mid < num) {
        r = mid - 1;
    } else {
        l = mid + 1;
    }
}
count;
)");

    Interpreter interpreter;

    auto prog = parser.ParseProgram();
    ASSERT_TRUE(prog->IsProgram());

    auto comp = interpreter.Execute(prog);
    EXPECT_EQ(types::CompletionType::NORMAL, comp.GetType());
    ASSERT_TRUE(comp.GetValue()->IsInt());
    EXPECT_EQ(7, comp.GetValue()->GetInt());
  }
}

TEST(Interpreter, EvalWithStatement) {
  Parser parser(uR"(
var value = 0;
var O = {
    value : 42,
};
with (O) {
    value;
}
)");

  Interpreter interpreter;

  auto prog = parser.ParseProgram();
  ASSERT_TRUE(prog->IsProgram());

  auto comp = interpreter.Execute(prog);
  EXPECT_EQ(types::CompletionType::NORMAL, comp.GetType());
  ASSERT_TRUE(comp.GetValue()->IsInt());
  EXPECT_EQ(42, comp.GetValue()->GetInt());
}


TEST(Interpreter, EvalSwitchStatement) {
  {
    Parser parser(uR"(
var price = 0.0;
var expr = "橙子";
switch (expr) {
  case "橙子":
    price = 0.59;
    break;
  case "苹果":
    price = 0.32;
    break;
  case "芒果":
  case "木瓜":
    price = 2.79;
    break;
  default:
    price = -1.0;
}
price;
)");

    Interpreter interpreter;

    auto prog = parser.ParseProgram();
    ASSERT_TRUE(prog->IsProgram());

    auto comp = interpreter.Execute(prog);
    EXPECT_EQ(types::CompletionType::NORMAL, comp.GetType());
    ASSERT_TRUE(comp.GetValue()->IsNumber());
    EXPECT_DOUBLE_EQ(0.59, comp.GetValue()->GetNumber());
  }


  {
    Parser parser(uR"(
var foo = 5;
switch (foo) {
  case 2:
    foo = 2;
    break;
  default:
    foo = 0;
  case 1:
    foo = 1;
}
)");

    Interpreter interpreter;

    auto prog = parser.ParseProgram();
    ASSERT_TRUE(prog->IsProgram());

    auto comp = interpreter.Execute(prog);
    EXPECT_EQ(types::CompletionType::NORMAL, comp.GetType());
    ASSERT_TRUE(comp.GetValue()->IsInt());
    EXPECT_EQ(0, comp.GetValue()->GetInt());
  }

  {
    Parser parser(uR"(
var Animal = "长颈鹿";
var ans;
switch (Animal) {
  case "奶牛":
  case "长颈鹿":
  case "狗":
  case "猪":
    ans = "这类动物没有灭绝。";
    break;
  case "恐龙":
  default:
    ans = "这类动物已经灭绝。";
}
)");

    Interpreter interpreter;

    auto prog = parser.ParseProgram();
    ASSERT_TRUE(prog->IsProgram());

    auto comp = interpreter.Execute(prog);
    EXPECT_EQ(types::CompletionType::NORMAL, comp.GetType());
    ASSERT_TRUE(comp.GetValue()->IsString());
    EXPECT_EQ(u"这类动物没有灭绝。", comp.GetValue()->GetString());
  }

  {
    Parser parser(uR"(
var foo = 1;
var output = "输出：";
switch (foo) {
  case 0:
    output += "所以";
  case 1:
    output += "你的";
    output += "名字";
  case 2:
    output += "叫";
  case 3:
    output += "什么";
  case 4:
    output += "？";
    break;
  case 5:
    output += "！";
    break;
  default:
    break;
}
output;
)");

    Interpreter interpreter;

    auto prog = parser.ParseProgram();
    ASSERT_TRUE(prog->IsProgram());

    auto comp = interpreter.Execute(prog);
    EXPECT_EQ(types::CompletionType::NORMAL, comp.GetType());
    ASSERT_TRUE(comp.GetValue()->IsString());
    EXPECT_EQ(u"输出：你的名字叫什么？", comp.GetValue()->GetString());
  }
}

TEST(Interpreter, EvalLabelledStatement) {
  {
    Parser parser(uR"(
var val = 0;
outerBlock: {
  innerBlock: {
    val |= 1;
    break outerBlock; // 同时跳出 innerBlock 和 outerBlock
    val |= 2;         // 跳过这一行
  }
  val |= 4;           // 跳过这一行
}
val;
)");

    Interpreter interpreter;

    auto prog = parser.ParseProgram();
    ASSERT_TRUE(prog->IsProgram());

    auto comp = interpreter.Execute(prog);
    EXPECT_EQ(types::CompletionType::NORMAL, comp.GetType());
    ASSERT_TRUE(comp.GetValue()->IsInt());
    EXPECT_EQ(1, comp.GetValue()->GetInt());
  }

  {
    Parser parser(uR"(
var n = 10;
var cnt = 0;
for (var i = 0; i < n; ++i) {
  test: {
    break test;
  }
  ++cnt;
}
cnt;
)");

    Interpreter interpreter;

    auto prog = parser.ParseProgram();
    ASSERT_TRUE(prog->IsProgram());

    auto comp = interpreter.Execute(prog);
    EXPECT_EQ(types::CompletionType::NORMAL, comp.GetType());
    ASSERT_TRUE(comp.GetValue()->IsInt());
    EXPECT_EQ(10, comp.GetValue()->GetInt());
  }
}

TEST(Interpreter, EvalTryStatement) {
  {
    Parser parser(uR"(
try {
var i = 2;
i *= i;
} catch (error) {
42;
}
)");

    Interpreter interpreter;

    auto prog = parser.ParseProgram();
    ASSERT_TRUE(prog->IsProgram());

    auto comp = interpreter.Execute(prog);
    EXPECT_EQ(types::CompletionType::NORMAL, comp.GetType());
    ASSERT_TRUE(comp.GetValue()->IsInt());
    EXPECT_EQ(4, comp.GetValue()->GetInt());
  }

  {
    Parser parser(uR"(
try {
throw 'Error!';
} catch (error) {
error;
}
)");

    Interpreter interpreter;

    auto prog = parser.ParseProgram();
    ASSERT_TRUE(prog->IsProgram());

    auto comp = interpreter.Execute(prog);
    EXPECT_EQ(types::CompletionType::NORMAL, comp.GetType());
    ASSERT_TRUE(comp.GetValue()->IsString());
    EXPECT_EQ(u"Error!", comp.GetValue()->GetString());
  }
}

TEST(Interpreter, EvalDebuggerStatement) {
   Parser parser(uR"(
debugger ;
)");

    Interpreter interpreter;

    auto prog = parser.ParseProgram();
    ASSERT_TRUE(prog->IsProgram());

    auto comp = interpreter.Execute(prog);
    EXPECT_EQ(types::CompletionType::NORMAL, comp.GetType());
}

TEST(Interpreter, EvalFunctionDeclaration) {
  Parser parser(uR"(
function foo(a, b) {
  return a + b;
}
)");

    Interpreter interpreter;

    auto prog = parser.ParseProgram();
    ASSERT_TRUE(prog->IsProgram());

    auto comp = interpreter.Execute(prog);
    EXPECT_EQ(types::CompletionType::NORMAL, comp.GetType());
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
  ASSERT_TRUE(comp.GetValue()->IsInt());
  EXPECT_EQ(83, comp.GetValue()->GetInt());
}

TEST(Interpreter, EvalNewExpression) {
  {
    Parser parser(uR"(
var obj = new Object();
obj;
)");

    Interpreter interpreter;

    auto prog = parser.ParseProgram();
    ASSERT_TRUE(prog->IsProgram());

    auto comp = interpreter.Execute(prog);
    EXPECT_EQ(types::CompletionType::NORMAL, comp.GetType());
    ASSERT_TRUE(comp.GetValue()->IsObject());
  }

  {
    Parser parser(uR"(
var obj1 = {
    value : 42,
};
var obj2 = new Object(obj1);
obj2["value"];
)");

    Interpreter interpreter;

    auto prog = parser.ParseProgram();
    ASSERT_TRUE(prog->IsProgram());

    auto comp = interpreter.Execute(prog);
    EXPECT_EQ(types::CompletionType::NORMAL, comp.GetType());
    ASSERT_TRUE(comp.GetValue()->IsInt());
    EXPECT_EQ(42, comp.GetValue()->GetInt());
  }
}


TEST(Interpreter, EvalCallExpression) {
  {
    Parser parser(uR"(
function foo(a, b) {
  return a + b; 
}

foo(1, 2); 
)");

    Interpreter interpreter;

    auto prog = parser.ParseProgram();
    ASSERT_TRUE(prog->IsProgram());

    auto comp = interpreter.Execute(prog);
    EXPECT_EQ(types::CompletionType::NORMAL, comp.GetType());
    ASSERT_TRUE(comp.GetValue()->IsInt());
    EXPECT_EQ(3, comp.GetValue()->GetInt());
  }

  {
    Parser parser(uR"(
function foo(i, n) {
  return i < n ? 1 + foo(i + 1, n) : 0; 
}

foo(0, 10);
)");

    Interpreter interpreter;

    auto prog = parser.ParseProgram();
    ASSERT_TRUE(prog->IsProgram());

    auto comp = interpreter.Execute(prog);
    EXPECT_EQ(types::CompletionType::NORMAL, comp.GetType());
    ASSERT_TRUE(comp.GetValue()->IsInt());
    EXPECT_EQ(10, comp.GetValue()->GetInt());
  }

  {
    Parser parser(uR"(
function fib(n) {
  if (n < 0) return 0;
  if (n < 2) return n;
  return fib(n - 1) + fib(n - 2);
}

fib(20);
)");

    Interpreter interpreter;

    auto prog = parser.ParseProgram();
    ASSERT_TRUE(prog->IsProgram());

    auto comp = interpreter.Execute(prog);
    EXPECT_EQ(types::CompletionType::NORMAL, comp.GetType());
    ASSERT_TRUE(comp.GetValue()->IsInt());
    EXPECT_EQ(6765, comp.GetValue()->GetInt());
  }
}

TEST(Interpreter, EvalFunctionExpression) {
  {
    Parser parser(uR"(
var add = function (a, b) {
  return a + b; 
};

add(2 * 2, 1);
)");

    Interpreter interpreter;

    auto prog = parser.ParseProgram();
    ASSERT_TRUE(prog->IsProgram());

    auto comp = interpreter.Execute(prog);
    EXPECT_EQ(types::CompletionType::NORMAL, comp.GetType());
    ASSERT_TRUE(comp.GetValue()->IsInt());
    EXPECT_EQ(5, comp.GetValue()->GetInt());
  }
  
  {
    Parser parser(uR"(
function add(k) {
  return function (n) {
    return n + k;
  };
}

var foo = add(42);
foo(2);
)");

    Interpreter interpreter;

    auto prog = parser.ParseProgram();
    ASSERT_TRUE(prog->IsProgram());

    auto comp = interpreter.Execute(prog);
    EXPECT_EQ(types::CompletionType::NORMAL, comp.GetType());
    ASSERT_TRUE(comp.GetValue()->IsInt());
    EXPECT_EQ(44, comp.GetValue()->GetInt());
  }
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
  ASSERT_TRUE(comp.GetValue()->IsDouble());
  EXPECT_DOUBLE_EQ(14.2857, comp.GetValue()->GetDouble());
}

TEST(Interpreter, EvalUnaryExpression) {
  {
    Parser parser(u"1 + -2");

    Interpreter interpreter;

    auto ast_node = parser.ParseBinaryExpression();
    ASSERT_TRUE(ast_node->IsBinaryExpression());

    auto val = interpreter.GetValue(interpreter.EvalExpression(ast_node->AsBinaryExpression()));
    ASSERT_TRUE(val->IsInt());
    EXPECT_EQ(-1, val->GetInt());
  }

  {
    Parser parser(uR"(
var obj = {
  value1 : 42,
  value2 : 43
};
delete obj.value1;
obj.value1;
)");

  Interpreter interpreter;

  auto prog = parser.ParseProgram();
  ASSERT_TRUE(prog->IsProgram());
  
  auto comp = interpreter.Execute(prog);
  EXPECT_EQ(types::CompletionType::NORMAL, comp.GetType());
  EXPECT_TRUE(comp.GetValue()->IsUndefined());
  }

  {
    Parser parser(uR"(
var obj = {
  value1 : 42,
  value2 : 43
};
typeof(obj);
)");

    Interpreter interpreter;

    auto prog = parser.ParseProgram();
    ASSERT_TRUE(prog->IsProgram());
  
    auto comp = interpreter.Execute(prog);
    EXPECT_EQ(types::CompletionType::NORMAL, comp.GetType());
    ASSERT_TRUE(comp.GetValue()->IsString());
    EXPECT_EQ(u"object", comp.GetValue()->GetString());
  }
}

TEST(Interpreter, EvalBinaryExpression) {
  {
    Parser parser(u"1 + (2 + 3)");
  
    Interpreter interpreter;

    auto ast_node = parser.ParseBinaryExpression();
    ASSERT_TRUE(ast_node->IsBinaryExpression());

    auto val = interpreter.GetValue(interpreter.EvalExpression(ast_node->AsBinaryExpression()));
    ASSERT_TRUE(val->IsInt());
    EXPECT_EQ(6, val->GetInt());
  }

  {
    Parser parser(u"true || false");
  
    Interpreter interpreter;

    auto ast_node = parser.ParseBinaryExpression();
    ASSERT_TRUE(ast_node->IsBinaryExpression());

    auto val = interpreter.GetValue(interpreter.EvalExpression(ast_node->AsBinaryExpression()));
    ASSERT_TRUE(val->IsBoolean());
    EXPECT_EQ(true, val->GetBoolean());
  }

  {
    Parser parser(u"0x7 | 8");
  
    Interpreter interpreter;

    auto ast_node = parser.ParseBinaryExpression();
    ASSERT_TRUE(ast_node->IsBinaryExpression());

    auto val = interpreter.GetValue(interpreter.EvalExpression(ast_node->AsBinaryExpression()));
    ASSERT_TRUE(val->IsInt());
    EXPECT_EQ(15, val->GetInt());
  }

  {
    Parser parser(u"0.00000001 == 0.0");
  
    Interpreter interpreter;

    auto ast_node = parser.ParseBinaryExpression();
    ASSERT_TRUE(ast_node->IsBinaryExpression());

    auto val = interpreter.GetValue(interpreter.EvalExpression(ast_node->AsBinaryExpression()));
    ASSERT_TRUE(val->IsBoolean());
    EXPECT_EQ(false, val->GetBoolean());
  }

  {
    Parser parser(uR"(
"12.3e3" == 12300
)");
  
    Interpreter interpreter;

    auto ast_node = parser.ParseBinaryExpression();
    ASSERT_TRUE(ast_node->IsBinaryExpression());

    auto val = interpreter.GetValue(interpreter.EvalExpression(ast_node->AsBinaryExpression()));
    ASSERT_TRUE(val->IsBoolean());
    EXPECT_EQ(true, val->GetBoolean());
  }

  {
    Parser parser(u"3 * 4 < 12");
  
    Interpreter interpreter;

    auto ast_node = parser.ParseBinaryExpression();
    ASSERT_TRUE(ast_node->IsBinaryExpression());

    auto val = interpreter.GetValue(interpreter.EvalExpression(ast_node->AsBinaryExpression()));
    ASSERT_TRUE(val->IsBoolean());
    EXPECT_EQ(false, val->GetBoolean());
  }

  {
    Parser parser(u"0x7ff << 3");
  
    Interpreter interpreter;

    auto ast_node = parser.ParseBinaryExpression();
    ASSERT_TRUE(ast_node->IsBinaryExpression());

    auto val = interpreter.GetValue(interpreter.EvalExpression(ast_node->AsBinaryExpression()));
    ASSERT_TRUE(val->IsInt());
    EXPECT_EQ(16376, val->GetInt());
  }

  {
    Parser parser(u"(1 + 2) * 3 / 5");
  
    Interpreter interpreter;

    auto ast_node = parser.ParseBinaryExpression();
    ASSERT_TRUE(ast_node->IsBinaryExpression());

    auto val = interpreter.GetValue(interpreter.EvalExpression(ast_node->AsBinaryExpression()));
    ASSERT_TRUE(val->IsDouble());
    EXPECT_DOUBLE_EQ(1.8, val->GetDouble());
  }
  
  {
    Parser parser(u"1 + 100 >> 1");
  
    Interpreter interpreter;

    auto ast_node = parser.ParseBinaryExpression();
    ASSERT_TRUE(ast_node->IsBinaryExpression());

    auto val = interpreter.GetValue(interpreter.EvalExpression(ast_node->AsBinaryExpression()));
    ASSERT_TRUE(val->IsInt());
    EXPECT_DOUBLE_EQ(50, val->GetInt());
  }

  {
    Parser parser(uR"(
("1" + ".5") * 3
)");
  
    Interpreter interpreter;

    auto ast_node = parser.ParseBinaryExpression();
    ASSERT_TRUE(ast_node->IsBinaryExpression());

    auto val = interpreter.GetValue(interpreter.EvalExpression(ast_node->AsBinaryExpression()));
    ASSERT_TRUE(val->IsDouble());
    EXPECT_DOUBLE_EQ(4.5, val->GetDouble());
  }

  {
    Parser parser(uR"(
var a = 'Hello';
var b = ', ';
var c = 'World';
var d = '!';
var e = a + b;
var f = c + d;
e + f;
)");
  
    Interpreter interpreter;

    auto prog = parser.ParseProgram();
    ASSERT_TRUE(prog->IsProgram());

    auto comp = interpreter.Execute(prog);
    EXPECT_EQ(types::CompletionType::NORMAL, comp.GetType());
    ASSERT_TRUE(comp.GetValue()->IsString());
    EXPECT_EQ(u"Hello, World!", comp.GetValue()->GetString());
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
    EXPECT_EQ(0.07, num->GetDouble());
  }

  {
    Parser parser(u"42");
  
    Interpreter interpreter;

    auto ast_node = parser.ParsePrimaryExpression();
    ASSERT_TRUE(ast_node->IsNumericLiteral());

    auto num = interpreter.EvalNumericLiteral(ast_node->AsNumericLiteral());
    EXPECT_EQ(42, num->GetInt());
  }
}

TEST(Intepreter, EvalStringLiteral) {
  {
    Parser parser(uR"("Hello, World!")");
  
    Interpreter interpreter;

    auto ast_node = parser.ParsePrimaryExpression();
    ASSERT_TRUE(ast_node->IsStringLiteral());

    auto str = interpreter.EvalStringLiteral(ast_node->AsStringLiteral());
    ASSERT_TRUE(str->IsString());

    EXPECT_EQ(u"Hello, World!", str->GetString());
  }

  {
    Parser parser(uR"("输出：你的名字叫什么？")");
  
    Interpreter interpreter;

    auto ast_node = parser.ParsePrimaryExpression();
    ASSERT_TRUE(ast_node->IsStringLiteral());

    auto str = interpreter.EvalStringLiteral(ast_node->AsStringLiteral());
    ASSERT_TRUE(str->IsString());

    EXPECT_EQ(u"输出：你的名字叫什么？", str->GetString());
  }
}

TEST(Interpreter, EvalObjectLiteral) {
  {
    Parser parser(u"{}");

    Interpreter interpreter;
    auto vm = interpreter.GetVM();

    auto ast_node = parser.ParsePrimaryExpression();
    ASSERT_TRUE(ast_node->IsObjectLiteral());

    auto obj = interpreter.EvalObjectLiteral(ast_node->AsObjectLiteral());
    ASSERT_TRUE(obj->IsObject());
    EXPECT_TRUE(vm->GetObjectPrototype().GetJSValue() == obj.As<types::Object>()->GetPrototype());
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
    ASSERT_TRUE(obj_val->IsObject());
    ASSERT_TRUE(obj_val->GetHeapObject()->IsJSObject());

    auto obj = obj_val.As<types::Object>();

    {
      auto key = factory->NewString(u"1");
      auto prop = types::Object::GetProperty(vm, obj, key);
      ASSERT_TRUE(prop.GetValue()->IsInt());
      EXPECT_EQ(42, prop.GetValue()->GetInt());
    }
  }

  {
    Parser parser(uR"(
var language = {
  set current(name) {
    this.log.push(name);
  },
  get current() {
    return this.log.join(' ');
  },
  log: [],
};

language.current = "ZH";
language.current = "FA";
language.current = "EN";

language.current;
)");

    Interpreter interpreter;

    auto prog = parser.ParseProgram();
    ASSERT_TRUE(prog->IsProgram());

    auto comp = interpreter.Execute(prog);
    EXPECT_EQ(types::CompletionType::NORMAL, comp.GetType());
    ASSERT_TRUE(comp.GetValue()->IsString());
    EXPECT_EQ(u"ZH FA EN", comp.GetValue()->GetString());
  }
}

TEST(Interpreter, EvalArrayLiteral) {
  {
    Parser parser(u"[]");

    Interpreter interpreter;
    auto vm = interpreter.GetVM();

    auto ast_node = parser.ParsePrimaryExpression();
    ASSERT_TRUE(ast_node->IsArrayLiteral());

    auto array = interpreter.EvalArrayLiteral(ast_node->AsArrayLiteral());
    ASSERT_TRUE(array->IsObject());
    ASSERT_TRUE(array->GetHeapObject()->IsJSArray());
  }

  {
    Parser parser(u"['Hello', 1, 2, , , , 3,]");

    Interpreter interpreter;
    auto vm = interpreter.GetVM();
    auto factory = vm->GetObjectFactory();

    auto ast_node = parser.ParsePrimaryExpression();
    ASSERT_TRUE(ast_node->IsArrayLiteral());

    auto array = interpreter.EvalArrayLiteral(ast_node->AsArrayLiteral());
    ASSERT_TRUE(array->IsObject());
    ASSERT_TRUE(array->GetHeapObject()->IsJSArray());

    {
      auto key = factory->NewString(u"0");
      auto prop = types::Object::GetOwnProperty(vm, array.As<builtins::JSArray>(), key);
      ASSERT_TRUE(prop.GetValue()->IsString());
      EXPECT_EQ(u"Hello", prop.GetValue()->GetString());
    }

    {
      auto key = factory->NewString(u"length");
      auto prop = types::Object::GetOwnProperty(vm, array.As<builtins::JSArray>(), key);
      ASSERT_TRUE(prop.GetValue()->IsInt());
      EXPECT_EQ(7, prop.GetValue()->GetInt());
    }
  }
}
