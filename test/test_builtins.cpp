#include "gtest/gtest.h"

#include "voidjs/builtins/js_function.h"
#include "voidjs/interpreter/interpreter.h"

using namespace voidjs;

TEST(JSObject, Construct) {
  Parser parser(uR"(
var o = new Object();
o.foo = 42;
o.foo;
)");

  Interpreter interpreter;

  auto prog = parser.ParseProgram();
  ASSERT_TRUE(prog->IsProgram());

  auto comp = interpreter.Execute(prog);
  EXPECT_EQ(types::CompletionType::NORMAL, comp.GetType());
  ASSERT_TRUE(comp.GetValue().IsInt());
  EXPECT_EQ(42, comp.GetValue().GetInt());
}

TEST(JSObject, GetOwnPropertyDescriptor) {
  Parser parser(uR"(
var object = {
  value: 42,
};

var desc = Object.getOwnPropertyDescriptor(object, 'value');

desc.value;
)");

  Interpreter interpreter;

  auto prog = parser.ParseProgram();
  ASSERT_TRUE(prog->IsProgram());

  auto comp = interpreter.Execute(prog);
  EXPECT_EQ(types::CompletionType::NORMAL, comp.GetType());
  ASSERT_TRUE(comp.GetValue().IsInt());
  EXPECT_EQ(42, comp.GetValue().GetInt());
}

TEST(JSObject, DefineProperty) {
  {
    Parser parser(uR"(
var object = {};

Object.defineProperty(object, 'value', {
  value: 42,
  writable: false,
});

object.value;
)");

    Interpreter interpreter;

    auto prog = parser.ParseProgram();
    ASSERT_TRUE(prog->IsProgram());

    auto comp = interpreter.Execute(prog);
    EXPECT_EQ(types::CompletionType::NORMAL, comp.GetType());
    ASSERT_TRUE(comp.GetValue().IsInt());
    EXPECT_EQ(42, comp.GetValue().GetInt());
  }
  
  {
    Parser parser(uR"(
var object = {};

Object.defineProperty(object, 'value', {
  value: 42,
  writable: false,
});

object.value = 43;

object.value;

)");

    Interpreter interpreter;

    auto prog = parser.ParseProgram();
    ASSERT_TRUE(prog->IsProgram());

    auto comp = interpreter.Execute(prog);
    EXPECT_EQ(types::CompletionType::NORMAL, comp.GetType());
    ASSERT_TRUE(comp.GetValue().IsInt());
    EXPECT_EQ(42, comp.GetValue().GetInt()); 
  }
}

TEST(JSObject, PreventExtensions) {
  Parser parser(uR"(
var object = {};

Object.preventExtensions(object);

try {
  Object.defineProperty(object, 'property', {
    value: 42,
  });
} catch (e) {
  "Successfully catch.";
}
)");

  Interpreter interpreter;

  auto prog = parser.ParseProgram();
  ASSERT_TRUE(prog->IsProgram());

  auto comp = interpreter.Execute(prog);
  EXPECT_EQ(types::CompletionType::NORMAL, comp.GetType());
  ASSERT_TRUE(comp.GetValue().IsString());
  EXPECT_EQ(u"Successfully catch.", comp.GetValue().GetHeapObject()->AsString()->GetString()); 
}

TEST(JSObject, IsExtensible) {
  Parser parser(uR"(
var object = {};
var mask = 0;

if (Object.isExtensible(object)) {
  mask |= 1;
}

Object.preventExtensions(object);

if (!Object.isExtensible(object)) {
  mask |= 2; 
}

mask;

)");

  Interpreter interpreter;

  auto prog = parser.ParseProgram();
  ASSERT_TRUE(prog->IsProgram());

  auto comp = interpreter.Execute(prog);
  EXPECT_EQ(types::CompletionType::NORMAL, comp.GetType());
  ASSERT_TRUE(comp.GetValue().IsInt());
  EXPECT_EQ(3, comp.GetValue().GetInt()); 
}

TEST(JSFunction, Construct) {
  Parser parser(uR"(
var add = new Function('a', 'b', 'return a + b;');
add(1, 2);
)");

  Interpreter interpreter;

  auto prog = parser.ParseProgram();
  ASSERT_TRUE(prog->IsProgram());

  auto comp = interpreter.Execute(prog);
  EXPECT_EQ(types::CompletionType::NORMAL, comp.GetType());
  ASSERT_TRUE(comp.GetValue().IsInt());
  EXPECT_EQ(3, comp.GetValue().GetInt());
}
