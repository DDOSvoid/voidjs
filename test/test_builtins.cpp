#include "gtest/gtest.h"

#include "voidjs/builtins/js_function.h"
#include "voidjs/builtins/js_array.h"
#include "voidjs/builtins/js_boolean.h"
#include "voidjs/interpreter/interpreter.h"
#include "voidjs/types/js_value.h"

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

TEST(JSArray, prototype) {
  Parser parser(uR"(
Array.prototype.bubbleSort = function () {
    for (var i = 0; i < this.length - 1; i++) {
        for (var j = 0; j < this.length - 1 - i; j++) {
            if (this[j] > this[j + 1]) {
                var tmp = this[j];
                this[j] = this[j + 1];
                this[j + 1] = tmp;
            }
        }
    }
};
var arr = [5, 4, 3, 2, 1];
arr.bubbleSort();
arr.join();
)");

  Interpreter interpreter;

  auto prog = parser.ParseProgram();
  ASSERT_TRUE(prog->IsProgram());

  auto comp = interpreter.Execute(prog);
  EXPECT_EQ(types::CompletionType::NORMAL, comp.GetType());
  ASSERT_TRUE(comp.GetValue().IsString());
  EXPECT_EQ(u"5,4,3,2,1", comp.GetValue().GetString()->GetString());
}

TEST(JSArray, IsArray) {
  Parser parser(uR"(
var count = 0;

count += Array.isArray([1, 3, 5]);

count += !Array.isArray('[]');

count += Array.isArray(new Array(5));

count += Array.isArray(new Array(13, 15));
)");

  Interpreter interpreter;

  auto prog = parser.ParseProgram();
  ASSERT_TRUE(prog->IsProgram());

  auto comp = interpreter.Execute(prog);
  EXPECT_EQ(types::CompletionType::NORMAL, comp.GetType());
  ASSERT_TRUE(comp.GetValue().IsInt());
  EXPECT_EQ(4, comp.GetValue().GetInt());
}

TEST(JSArray, Concat) {
  {
    Parser parser(uR"(
var array1 = ['a', 'b', 'c'];
var array2 = ['d', 'e', 'f'];
var array3 = array1.concat(array2);
array3;
)");

    Interpreter interpreter;
    auto vm = interpreter.GetVM();
    auto factory = vm->GetObjectFactory();

    auto prog = parser.ParseProgram();
    ASSERT_TRUE(prog->IsProgram());

    auto comp = interpreter.Execute(prog);
    EXPECT_EQ(types::CompletionType::NORMAL, comp.GetType());
    ASSERT_TRUE(comp.GetValue().IsObject() && comp.GetValue().GetHeapObject()->IsJSArray());

    auto array = comp.GetValue().GetHeapObject()->AsJSArray();
    auto key0 = factory->NewStringFromTable(u"0");
    auto key1 = factory->NewStringFromTable(u"1");
    auto key3 = factory->NewStringFromTable(u"3");
    auto key4 = factory->NewStringFromTable(u"4");
    auto key_length = factory->NewStringFromTable(u"length");
    auto val0 = types::Object::Get(vm, array, key0);
    auto val1 = types::Object::Get(vm, array, key1);
    auto val3 = types::Object::Get(vm, array, key3);
    auto val4 = types::Object::Get(vm, array, key4);
    auto val_length = types::Object::Get(vm, array, key_length);
    ASSERT_TRUE(val0.IsString());
    ASSERT_TRUE(val1.IsString());
    ASSERT_TRUE(val3.IsString());
    ASSERT_TRUE(val4.IsString());
    ASSERT_TRUE(val_length.IsInt());
    EXPECT_EQ(u"a", val0.GetString()->GetString());
    EXPECT_EQ(u"b", val1.GetString()->GetString());
    EXPECT_EQ(u"d", val3.GetString()->GetString());
    EXPECT_EQ(u"e", val4.GetString()->GetString());
    EXPECT_EQ(6, val_length.GetInt());
  }

  {
    Parser parser(uR"(
var array1 = ['a', 'b', 'c'];
var array2 = ['d', 'e', 'f'];
var array3 = array1.concat(array2);
var array4 = array3.concat(42);
array4;
)");

    Interpreter interpreter;
    auto vm = interpreter.GetVM();
    auto factory = vm->GetObjectFactory();

    auto prog = parser.ParseProgram();
    ASSERT_TRUE(prog->IsProgram());

    auto comp = interpreter.Execute(prog);
    EXPECT_EQ(types::CompletionType::NORMAL, comp.GetType());
    ASSERT_TRUE(comp.GetValue().IsObject() && comp.GetValue().GetHeapObject()->IsJSArray());

    auto array = comp.GetValue().GetHeapObject()->AsJSArray();
    auto key0 = factory->NewStringFromTable(u"0");
    auto key1 = factory->NewStringFromTable(u"1");
    auto key3 = factory->NewStringFromTable(u"3");
    auto key6 = factory->NewStringFromTable(u"6");
    auto val0 = types::Object::Get(vm, array, key0);
    auto val1 = types::Object::Get(vm, array, key1);
    auto val3 = types::Object::Get(vm, array, key3);
    auto val6 = types::Object::Get(vm, array, key6);
    ASSERT_TRUE(val0.IsString());
    ASSERT_TRUE(val1.IsString());
    ASSERT_TRUE(val3.IsString());
    ASSERT_TRUE(val6.IsInt());
    EXPECT_EQ(u"a", val0.GetString()->GetString());
    EXPECT_EQ(u"b", val1.GetString()->GetString());
    EXPECT_EQ(u"d", val3.GetString()->GetString());
    EXPECT_EQ(42, val6.GetInt());
  }
}

TEST(JSArray, Join) {
  Parser parser(uR"(
var elements = ['Fire', 'Air', 'Water'];

// Fire,Air,Water
var r1 = elements.join();

// FireAirWater
var r2 = elements.join('');

// Fire-Air-Water
var r3 = elements.join('-');

r1 + r2 + r3;
)");

  Interpreter interpreter;

  auto prog = parser.ParseProgram();
  ASSERT_TRUE(prog->IsProgram());

  auto comp = interpreter.Execute(prog);
  EXPECT_EQ(types::CompletionType::NORMAL, comp.GetType());
  ASSERT_TRUE(comp.GetValue().IsString());
  EXPECT_EQ(u"Fire,Air,WaterFireAirWaterFire-Air-Water", comp.GetValue().GetString()->GetString());
}

TEST(JSArray, Pop) {
  Parser parser(uR"(
var plants = ['broccoli', 'cauliflower', 'cabbage', 'kale', 'tomato'];

plants.pop();
plants.pop();

plants.join(',');
)");

  Interpreter interpreter;

  auto prog = parser.ParseProgram();
  ASSERT_TRUE(prog->IsProgram());

  auto comp = interpreter.Execute(prog);
  EXPECT_EQ(types::CompletionType::NORMAL, comp.GetType());
  ASSERT_TRUE(comp.GetValue().IsString());
  EXPECT_EQ(u"broccoli,cauliflower,cabbage", comp.GetValue().GetString()->GetString());
}

TEST(JSArray, Push) {
  Parser parser(uR"(
var plants = ['broccoli', 'cauliflower', 'cabbage', 'kale', 'tomato'];

plants.pop();
plants.push('sunflower');

plants.join(',');
)");

  Interpreter interpreter;

  auto prog = parser.ParseProgram();
  ASSERT_TRUE(prog->IsProgram());

  auto comp = interpreter.Execute(prog);
  EXPECT_EQ(types::CompletionType::NORMAL, comp.GetType());
  ASSERT_TRUE(comp.GetValue().IsString());
  EXPECT_EQ(u"broccoli,cauliflower,cabbage,kale,sunflower", comp.GetValue().GetString()->GetString());
}

TEST(JSString, CharAt) {
  Parser parser(uR"(
var sentence = 'The quick brown fox jumps over the lazy dog.';
sentence.charAt(5);
)");

  Interpreter interpreter;

  auto prog = parser.ParseProgram();
  ASSERT_TRUE(prog->IsProgram());

  auto comp = interpreter.Execute(prog);
  EXPECT_EQ(types::CompletionType::NORMAL, comp.GetType());
  ASSERT_TRUE(comp.GetValue().IsString());
  EXPECT_EQ(u"u", comp.GetValue().GetString()->GetString());
}

TEST(JSString, Concat) {
  Parser parser(uR"(
var str1 = 'Hello';
var str2 = 'World';

str1.concat(', ', str2);
)");

  Interpreter interpreter;

  auto prog = parser.ParseProgram();
  ASSERT_TRUE(prog->IsProgram());

  auto comp = interpreter.Execute(prog);
  EXPECT_EQ(types::CompletionType::NORMAL, comp.GetType());
  ASSERT_TRUE(comp.GetValue().IsString());
  EXPECT_EQ(u"Hello, World", comp.GetValue().GetString()->GetString());
}

TEST(JSString, Indexof) {
  Parser parser(uR"(
var paragraph = "I think Ruth's dog is cuter than your dog!";

var searchTerm = 'dog';

paragraph.indexOf(searchTerm);
)");

  Interpreter interpreter;

  auto prog = parser.ParseProgram();
  ASSERT_TRUE(prog->IsProgram());

  auto comp = interpreter.Execute(prog);
  EXPECT_EQ(types::CompletionType::NORMAL, comp.GetType());
  ASSERT_TRUE(comp.GetValue().IsInt());
  EXPECT_EQ(15, comp.GetValue().GetInt());
}

TEST(JSBoolean, Construct) {
  Parser parser(uR"(
var flag = new Boolean();

flag;
)");

  Interpreter interpreter;

  auto prog = parser.ParseProgram();
  ASSERT_TRUE(prog->IsProgram());

  auto comp = interpreter.Execute(prog);
  EXPECT_EQ(types::CompletionType::NORMAL, comp.GetType());
  ASSERT_TRUE(comp.GetValue().IsObject() && comp.GetValue().GetHeapObject()->IsJSBoolean());
  EXPECT_EQ(false, comp.GetValue().GetHeapObject()->AsJSBoolean()->GetPrimitiveValue().GetBoolean());
}

TEST(JSNumber, Construct) {
  Parser parser(uR"(
var num = new Number(123);

num;
)");

  Interpreter interpreter;

  auto prog = parser.ParseProgram();
  ASSERT_TRUE(prog->IsProgram());

  auto comp = interpreter.Execute(prog);
  EXPECT_EQ(types::CompletionType::NORMAL, comp.GetType());
  ASSERT_TRUE(comp.GetValue().IsObject() && comp.GetValue().GetHeapObject()->IsJSNumber());
  EXPECT_EQ(123, comp.GetValue().GetHeapObject()->AsJSBoolean()->GetPrimitiveValue().GetInt());
}

