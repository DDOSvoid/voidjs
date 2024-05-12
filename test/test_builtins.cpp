#include "gtest/gtest.h"

#include "voidjs/types/js_value.h"
#include "voidjs/types/object_factory.h"
#include "voidjs/builtins/js_function.h"
#include "voidjs/builtins/js_array.h"
#include "voidjs/builtins/js_boolean.h"
#include "voidjs/interpreter/interpreter.h"

using namespace voidjs;

TEST(JSObject, ObjectConstructorConstruct) {
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
  ASSERT_TRUE(comp.GetValue()->IsInt());
  EXPECT_EQ(42, comp.GetValue()->GetInt());
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
  ASSERT_TRUE(comp.GetValue()->IsInt());
  EXPECT_EQ(42, comp.GetValue()->GetInt());
}

TEST(JSObject, GetOwnPropertyNames) {
  Parser parser(uR"(
var obj = { 0: "a", 1: "b", 2: "c" };
var str = Object.getOwnPropertyNames(obj).join();
str;
)");

  Interpreter interpreter;

  auto prog = parser.ParseProgram();
  ASSERT_TRUE(prog->IsProgram());

  auto comp = interpreter.Execute(prog);
  EXPECT_EQ(types::CompletionType::NORMAL, comp.GetType());
  ASSERT_TRUE(comp.GetValue()->IsString());
  EXPECT_EQ(u"1,2,0", comp.GetValue()->GetString());
}

TEST(JSObject, Create) {
  Parser parser(uR"(
var person = {
  isHuman: false,
  printIntroduction: function () {
    return "My name is " + this.name + ". Am I human? " + this.isHuman;
  },
};

var me = Object.create(person);

me.name = 'Matthew';
me.isHuman = true;

me.printIntroduction();
)");

  Interpreter interpreter;

  auto prog = parser.ParseProgram();
  ASSERT_TRUE(prog->IsProgram());

  auto comp = interpreter.Execute(prog);
  EXPECT_EQ(types::CompletionType::NORMAL, comp.GetType());
  ASSERT_TRUE(comp.GetValue()->IsString());
  EXPECT_EQ(u"My name is Matthew. Am I human? true", comp.GetValue()->GetString());
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
    ASSERT_TRUE(comp.GetValue()->IsInt());
    EXPECT_EQ(42, comp.GetValue()->GetInt());
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
    ASSERT_TRUE(comp.GetValue()->IsInt());
    EXPECT_EQ(42, comp.GetValue()->GetInt()); 
  }
}

TEST(JSObject, DefineProperties) {
  {
    Parser parser(uR"(
var object1 = {};

Object.defineProperties(object1, {
  property1: {
    value: 42,
    writable: true,
  },
  property2: {},
});

object1.property1;

)");

    Interpreter interpreter;

    auto prog = parser.ParseProgram();
    ASSERT_TRUE(prog->IsProgram());

    auto comp = interpreter.Execute(prog);
    EXPECT_EQ(types::CompletionType::NORMAL, comp.GetType());
    ASSERT_TRUE(comp.GetValue()->IsInt());
    EXPECT_EQ(42, comp.GetValue()->GetInt());
  }

  {
    Parser parser(uR"(
var object1 = {};

Object.defineProperties(object1, {
  property1: {
    value: 42,
    writable: true,
  },
  property2: {},
});

object1.property1;

var object2 = {};

Object.defineProperties(object2, object1);

object2.property1;

)");

    Interpreter interpreter;

    auto prog = parser.ParseProgram();
    ASSERT_TRUE(prog->IsProgram());

    auto comp = interpreter.Execute(prog);
    EXPECT_EQ(types::CompletionType::NORMAL, comp.GetType());
    EXPECT_TRUE(comp.GetValue()->IsUndefined());
  }
}

TEST(JSObject, Seal) {
  Parser parser(uR"(
var object = {
  property: 42,
};

Object.seal(object);

object.property = 33;

// Cannot delete when sealed
// Throws an error in strict mode
delete object.property; 

object.property;
)");

  Interpreter interpreter;

  auto prog = parser.ParseProgram();
  ASSERT_TRUE(prog->IsProgram());

  auto comp = interpreter.Execute(prog);
  EXPECT_EQ(types::CompletionType::NORMAL, comp.GetType());
  ASSERT_TRUE(comp.GetValue()->IsInt());
  EXPECT_EQ(33, comp.GetValue()->GetInt());
}

TEST(JSObject, Freeze) {
  Parser parser(uR"(
var obj = {
  prop: 42,
};

Object.freeze(obj);

obj.prop = 33;
// Throws an error in strict mode

obj.prop;
)");

  Interpreter interpreter;

  auto prog = parser.ParseProgram();
  ASSERT_TRUE(prog->IsProgram());

  auto comp = interpreter.Execute(prog);
  EXPECT_EQ(types::CompletionType::NORMAL, comp.GetType());
  ASSERT_TRUE(comp.GetValue()->IsInt());
  EXPECT_EQ(42, comp.GetValue()->GetInt());
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
  ASSERT_TRUE(comp.GetValue()->IsString());
  EXPECT_EQ(u"Successfully catch.", comp.GetValue()->GetString()); 
}

TEST(JSObject, isSealed) {
  Parser parser(uR"(
var object = {
  property: 42,
};

var count = 0;

count += !Object.isSealed(object);

Object.seal(object);

count += Object.isSealed(object);

)");

  Interpreter interpreter;

  auto prog = parser.ParseProgram();
  ASSERT_TRUE(prog->IsProgram());

  auto comp = interpreter.Execute(prog);
  EXPECT_EQ(types::CompletionType::NORMAL, comp.GetType());
  ASSERT_TRUE(comp.GetValue()->IsInt());
  EXPECT_EQ(2, comp.GetValue()->GetInt()); 
}

TEST(JSObject, isFrozen) {
  Parser parser(uR"(
var count = 0;

// 一个新对象是默认是可扩展的，所以它也是非冻结的。
count += !Object.isFrozen({}); // false

// 一个不可扩展的空对象同时也是一个冻结对象。
var vacuouslyFrozen = Object.preventExtensions({});
count += Object.isFrozen(vacuouslyFrozen); // true

// 一个非空对象默认也是非冻结的。
var oneProp = { p: 42 };
count += !Object.isFrozen(oneProp); // false

// 即使令对象不可扩展，它也不会被冻结，因为属性仍然是可配置的（而且可写的）。
Object.preventExtensions(oneProp);
count += !Object.isFrozen(oneProp); // false

// 此时，如果删除了这个属性，则它会成为一个冻结对象。
delete oneProp.p;
count += Object.isFrozen(oneProp); // true

// 一个不可扩展的对象，拥有一个不可写但可配置的属性，则它仍然是非冻结的。
var nonWritable = { e: "plep" };
Object.preventExtensions(nonWritable);
Object.defineProperty(nonWritable, "e", {
  writable: false,
}); // 令其不可写
count += !Object.isFrozen(nonWritable); // false

// 把这个属性改为不可配置，会让这个对象成为冻结对象。
Object.defineProperty(nonWritable, "e", {
  configurable: false,
}); // 令其不可配置
count += Object.isFrozen(nonWritable); // true

// 一个不可扩展的对象，拥有一个不可配置但可写的属性，则它也是非冻结的。
var nonConfigurable = { release: "the kraken!" };
Object.preventExtensions(nonConfigurable);
Object.defineProperty(nonConfigurable, "release", {
  configurable: false,
});
count += !Object.isFrozen(nonConfigurable); // false

// 把这个属性改为不可写，会让这个对象成为冻结对象。
Object.defineProperty(nonConfigurable, "release", {
  writable: false,
});
count += Object.isFrozen(nonConfigurable); // true

// 一个不可扩展的对象，拥有一个访问器属性，则它仍然是非冻结的。
var accessor = {
  get food() {
    return "yum";
  },
};
Object.preventExtensions(accessor);
count += !Object.isFrozen(accessor); // false

// 把这个属性改为不可配置，会让这个对象成为冻结对象。
Object.defineProperty(accessor, "food", {
  configurable: false,
});
count += Object.isFrozen(accessor); // true

)");

  Interpreter interpreter;

  auto prog = parser.ParseProgram();
  ASSERT_TRUE(prog->IsProgram());

  auto comp = interpreter.Execute(prog);
  EXPECT_EQ(types::CompletionType::NORMAL, comp.GetType());
  ASSERT_TRUE(comp.GetValue()->IsInt());
  EXPECT_EQ(11, comp.GetValue()->GetInt()); 
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
  ASSERT_TRUE(comp.GetValue()->IsInt());
  EXPECT_EQ(3, comp.GetValue()->GetInt()); 
}

TEST(JSObject, Keys) {
  Parser parser(uR"(
var myObj = Object.create({}, {
  getFoo: {
    value: function value() {
      return this.foo;
    }
  }
});
myObj.foo = 1;
Object.keys(myObj).join();
)");

  Interpreter interpreter;

  auto prog = parser.ParseProgram();
  ASSERT_TRUE(prog->IsProgram());

  auto comp = interpreter.Execute(prog);
  EXPECT_EQ(types::CompletionType::NORMAL, comp.GetType());
  ASSERT_TRUE(comp.GetValue()->IsString());
  EXPECT_EQ(u"foo", comp.GetValue()->GetString()); 
}

TEST(JSObject, ToString) {
  Parser parser(uR"(
)");

  Interpreter interpreter;

  auto prog = parser.ParseProgram();
  ASSERT_TRUE(prog->IsProgram());

  auto comp = interpreter.Execute(prog);
  EXPECT_EQ(types::CompletionType::NORMAL, comp.GetType());
}

TEST(JSObject, ToLocaleString) {
  Parser parser(uR"(
)");

  Interpreter interpreter;

  auto prog = parser.ParseProgram();
  ASSERT_TRUE(prog->IsProgram());

  auto comp = interpreter.Execute(prog);
  EXPECT_EQ(types::CompletionType::NORMAL, comp.GetType());
}


TEST(JSObject, ValueOf) {
  Parser parser(uR"(
var obj = { foo: 1 };
obj.valueOf() === obj; // true
)");

  Interpreter interpreter;

  auto prog = parser.ParseProgram();
  ASSERT_TRUE(prog->IsProgram());

  auto comp = interpreter.Execute(prog);
  EXPECT_EQ(types::CompletionType::NORMAL, comp.GetType());
  ASSERT_TRUE(comp.GetValue()->IsBoolean());
  EXPECT_EQ(true, comp.GetValue()->GetBoolean());
}

TEST(JSObject, HasOwnProperty) {
  Parser parser(uR"(
var fruits = ["Apple", "Banana", "Watermelon", "Orange"];
var count = 0; 
count += fruits.hasOwnProperty(3); // 返回 true
count += !fruits.hasOwnProperty(4); // 返回 false
)");

  Interpreter interpreter;

  auto prog = parser.ParseProgram();
  ASSERT_TRUE(prog->IsProgram());

  auto comp = interpreter.Execute(prog);
  EXPECT_EQ(types::CompletionType::NORMAL, comp.GetType());
  ASSERT_TRUE(comp.GetValue()->IsInt());
  EXPECT_EQ(2, comp.GetValue()->GetInt());
}

TEST(JSObject, IsPrototypeOf) {
  Parser parser(uR"(
function Foo() {}
function Bar() {}

Bar.prototype = Object.create(Foo.prototype);

var bar = new Bar();

var count = 0;

count += Foo.prototype.isPrototypeOf(bar);

count += Bar.prototype.isPrototypeOf(bar);
)");

  Interpreter interpreter;

  auto prog = parser.ParseProgram();
  ASSERT_TRUE(prog->IsProgram());

  auto comp = interpreter.Execute(prog);
  EXPECT_EQ(types::CompletionType::NORMAL, comp.GetType());
  ASSERT_TRUE(comp.GetValue()->IsInt());
  EXPECT_EQ(2, comp.GetValue()->GetInt());
}

TEST(JSObject, PropertyIsEnumerable) {
  Parser parser(uR"(
var object = {};
var array = [];
object.property = 42;
array[0] = 42;

var count = 0; 

count += object.propertyIsEnumerable('property');

count += array.propertyIsEnumerable(0);

count += array.propertyIsEnumerable('length');
)");

  Interpreter interpreter;

  auto prog = parser.ParseProgram();
  ASSERT_TRUE(prog->IsProgram());

  auto comp = interpreter.Execute(prog);
  EXPECT_EQ(types::CompletionType::NORMAL, comp.GetType());
  ASSERT_TRUE(comp.GetValue()->IsInt());
  EXPECT_EQ(2, comp.GetValue()->GetInt());
}

TEST(JSFunction, FunctionConstructorCall) {
  Parser parser(uR"(
var add = Function('a', 'b', 'return a + b;');
add(1, 2);
)");

  Interpreter interpreter;

  auto prog = parser.ParseProgram();
  ASSERT_TRUE(prog->IsProgram());

  auto comp = interpreter.Execute(prog);
  EXPECT_EQ(types::CompletionType::NORMAL, comp.GetType());
  ASSERT_TRUE(comp.GetValue()->IsInt());
  EXPECT_EQ(3, comp.GetValue()->GetInt());
}

TEST(JSFunction, FunctionConstructorConstruct) {
  Parser parser(uR"(
var add = new Function('a', 'b', 'return a + b;');
add(1, 2);
)");

  Interpreter interpreter;

  auto prog = parser.ParseProgram();
  ASSERT_TRUE(prog->IsProgram());

  auto comp = interpreter.Execute(prog);
  EXPECT_EQ(types::CompletionType::NORMAL, comp.GetType());
  ASSERT_TRUE(comp.GetValue()->IsInt());
  EXPECT_EQ(3, comp.GetValue()->GetInt());
}

TEST(JSFunction, ToString) {
  Parser parser(uR"(
)");

  Interpreter interpreter;

  auto prog = parser.ParseProgram();
  ASSERT_TRUE(prog->IsProgram());

  auto comp = interpreter.Execute(prog);
  EXPECT_EQ(types::CompletionType::NORMAL, comp.GetType());
}

TEST(JSFunction, Apply) {
  Parser parser(uR"(
var array = ["a", "b"];

var elements = [0, 1, 2];

array.push.apply(array, elements);

array.join();
)");

  Interpreter interpreter;

  auto prog = parser.ParseProgram();
  ASSERT_TRUE(prog->IsProgram());

  auto comp = interpreter.Execute(prog);
  EXPECT_EQ(types::CompletionType::NORMAL, comp.GetType());
  ASSERT_TRUE(comp.GetValue()->IsString());
  EXPECT_EQ(u"a,b,0,1,2", comp.GetValue()->GetString());
}

TEST(JSFunction, Call) {
  Parser parser(uR"(
function Product(name, price) {
  this.name = name;
  this.price = price;
}

function Food(name, price) {
  Product.call(this, name, price);
  this.category = 'food';
}

new Food('cheese', 5).name;
)");

  Interpreter interpreter;

  auto prog = parser.ParseProgram();
  ASSERT_TRUE(prog->IsProgram());

  auto comp = interpreter.Execute(prog);
  EXPECT_EQ(types::CompletionType::NORMAL, comp.GetType());
  ASSERT_TRUE(comp.GetValue()->IsString());
  EXPECT_EQ(u"cheese", comp.GetValue()->GetString());
}

TEST(JSFunction, Bind) {
  Parser parser(uR"(
)");

  Interpreter interpreter;

  auto prog = parser.ParseProgram();
  ASSERT_TRUE(prog->IsProgram());

  auto comp = interpreter.Execute(prog);
  EXPECT_EQ(types::CompletionType::NORMAL, comp.GetType());
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
  ASSERT_TRUE(comp.GetValue()->IsString());
  EXPECT_EQ(u"1,2,3,4,5", comp.GetValue()->GetString());
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
  ASSERT_TRUE(comp.GetValue()->IsInt());
  EXPECT_EQ(4, comp.GetValue()->GetInt());
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
    ASSERT_TRUE(comp.GetValue()->IsObject() && comp.GetValue()->GetHeapObject()->IsJSArray());

    auto array = comp.GetValue().As<builtins::JSArray>();
    auto key0 = factory->NewString(u"0");
    auto key1 = factory->NewString(u"1");
    auto key3 = factory->NewString(u"3");
    auto key4 = factory->NewString(u"4");
    auto key_length = factory->NewString(u"length");
    auto val0 = types::Object::Get(vm, array, key0);
    auto val1 = types::Object::Get(vm, array, key1);
    auto val3 = types::Object::Get(vm, array, key3);
    auto val4 = types::Object::Get(vm, array, key4);
    auto val_length = types::Object::Get(vm, array, key_length);
    ASSERT_TRUE(val0->IsString());
    ASSERT_TRUE(val1->IsString());
    ASSERT_TRUE(val3->IsString());
    ASSERT_TRUE(val4->IsString());
    ASSERT_TRUE(val_length->IsInt());
    EXPECT_EQ(u"a", val0->GetString());
    EXPECT_EQ(u"b", val1->GetString());
    EXPECT_EQ(u"d", val3->GetString());
    EXPECT_EQ(u"e", val4->GetString());
    EXPECT_EQ(6, val_length->GetInt());
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
    ASSERT_TRUE(comp.GetValue()->IsObject() && comp.GetValue()->GetHeapObject()->IsJSArray());

    auto array = comp.GetValue().As<builtins::JSArray>();
    auto key0 = factory->NewString(u"0");
    auto key1 = factory->NewString(u"1");
    auto key3 = factory->NewString(u"3");
    auto key6 = factory->NewString(u"6");
    auto val0 = types::Object::Get(vm, array, key0);
    auto val1 = types::Object::Get(vm, array, key1);
    auto val3 = types::Object::Get(vm, array, key3);
    auto val6 = types::Object::Get(vm, array, key6);
    ASSERT_TRUE(val0->IsString());
    ASSERT_TRUE(val1->IsString());
    ASSERT_TRUE(val3->IsString());
    ASSERT_TRUE(val6->IsInt());
    EXPECT_EQ(u"a", val0->GetString());
    EXPECT_EQ(u"b", val1->GetString());
    EXPECT_EQ(u"d", val3->GetString());
    EXPECT_EQ(42, val6->GetInt());
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
  ASSERT_TRUE(comp.GetValue()->IsString());
  EXPECT_EQ(u"Fire,Air,WaterFireAirWaterFire-Air-Water", comp.GetValue()->GetString());
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
  ASSERT_TRUE(comp.GetValue()->IsString());
  EXPECT_EQ(u"broccoli,cauliflower,cabbage", comp.GetValue()->GetString());
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
  ASSERT_TRUE(comp.GetValue()->IsString());
  EXPECT_EQ(u"broccoli,cauliflower,cabbage,kale,sunflower", comp.GetValue()->GetString());
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
  ASSERT_TRUE(comp.GetValue()->IsString());
  EXPECT_EQ(u"u", comp.GetValue()->GetString());
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
  ASSERT_TRUE(comp.GetValue()->IsString());
  EXPECT_EQ(u"Hello, World", comp.GetValue()->GetString());
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
  ASSERT_TRUE(comp.GetValue()->IsInt());
  EXPECT_EQ(15, comp.GetValue()->GetInt());
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
  ASSERT_TRUE(comp.GetValue()->IsObject() && comp.GetValue()->GetHeapObject()->IsJSBoolean());
  EXPECT_EQ(false, comp.GetValue()->GetHeapObject()->AsJSBoolean()->GetPrimitiveValue().GetBoolean());
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
  ASSERT_TRUE(comp.GetValue()->IsObject() && comp.GetValue()->GetHeapObject()->IsJSNumber());
  EXPECT_EQ(123, comp.GetValue()->GetHeapObject()->AsJSBoolean()->GetPrimitiveValue().GetInt());
}

TEST(JSMath, Abs) {
  Parser parser(uR"(
Math.abs("-1"); 
)");

  Interpreter interpreter;

  auto prog = parser.ParseProgram();
  ASSERT_TRUE(prog->IsProgram());

  auto comp = interpreter.Execute(prog);
  EXPECT_EQ(types::CompletionType::NORMAL, comp.GetType());
  ASSERT_TRUE(comp.GetValue()->IsInt());
  EXPECT_EQ(1, comp.GetValue()->GetInt());
}

TEST(JSMath, Acos) {
  Parser parser(uR"(
Math.acos(0.5); // 1.0471975511965979
)");

  Interpreter interpreter;

  auto prog = parser.ParseProgram();
  ASSERT_TRUE(prog->IsProgram());

  auto comp = interpreter.Execute(prog);
  EXPECT_EQ(types::CompletionType::NORMAL, comp.GetType());
  ASSERT_TRUE(comp.GetValue()->IsDouble());
  EXPECT_DOUBLE_EQ(1.0471975511965979, comp.GetValue()->GetDouble());
}

TEST(JSMath, Asin) {
  Parser parser(uR"(
Math.asin(-1); // -1.5707963267948966 (-pi/2)
)");

  Interpreter interpreter;

  auto prog = parser.ParseProgram();
  ASSERT_TRUE(prog->IsProgram());

  auto comp = interpreter.Execute(prog);
  EXPECT_EQ(types::CompletionType::NORMAL, comp.GetType());
  ASSERT_TRUE(comp.GetValue()->IsDouble());
  EXPECT_DOUBLE_EQ(-1.5707963267948966, comp.GetValue()->GetDouble());
}
