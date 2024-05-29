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

TEST(JSArray, ToString) {
  Parser parser(uR"(
var array = [1, 2, 'a', '1a'];

array.toString();
)");

  Interpreter interpreter;

  auto prog = parser.ParseProgram();
  ASSERT_TRUE(prog->IsProgram());

  auto comp = interpreter.Execute(prog);
  EXPECT_EQ(types::CompletionType::NORMAL, comp.GetType());
  ASSERT_TRUE(comp.GetValue()->IsString());
  EXPECT_EQ(u"1,2,a,1a", comp.GetValue()->GetString());
}

TEST(JSArray, ToLocaleString) {
  Parser parser(uR"(
)");

  Interpreter interpreter;

  auto prog = parser.ParseProgram();
  ASSERT_TRUE(prog->IsProgram());

  auto comp = interpreter.Execute(prog);
  EXPECT_EQ(types::CompletionType::NORMAL, comp.GetType());
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
  {
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
  
  {
    Parser parser(uR"(
var elements = [5, 4, , , 1];
elements.join();
)");

    Interpreter interpreter;

    auto prog = parser.ParseProgram();
    ASSERT_TRUE(prog->IsProgram());

    auto comp = interpreter.Execute(prog);
    EXPECT_EQ(types::CompletionType::NORMAL, comp.GetType());
    ASSERT_TRUE(comp.GetValue()->IsString());
    EXPECT_EQ(u"5,4,,,1", comp.GetValue()->GetString());
  }
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

TEST(JSArray, Reverse) {
  Parser parser(uR"(
var array = [1, , , 4, 5];
array.reverse().toString()
)");

  Interpreter interpreter;

  auto prog = parser.ParseProgram();
  ASSERT_TRUE(prog->IsProgram());

  auto comp = interpreter.Execute(prog);
  EXPECT_EQ(types::CompletionType::NORMAL, comp.GetType());
  ASSERT_TRUE(comp.GetValue()->IsString());
  EXPECT_EQ(u"5,4,,,1", comp.GetValue()->GetString());
}

TEST(JSArray, Shift) {
  Parser parser(uR"(
var array = [1, 2, 3];

var count = 0;

var first_elem = array.shift();

count += first_elem === 1;

count += array.toString() === "2,3";
)");

  Interpreter interpreter;

  auto prog = parser.ParseProgram();
  ASSERT_TRUE(prog->IsProgram());

  auto comp = interpreter.Execute(prog);
  EXPECT_EQ(types::CompletionType::NORMAL, comp.GetType());
  ASSERT_TRUE(comp.GetValue()->IsInt());
  EXPECT_EQ(2, comp.GetValue()->GetInt());
}

TEST(JSArray, Slice) {
  {
    Parser parser(uR"(
var animals = ['ant', 'bison', 'camel', 'duck', 'elephant'];

animals.slice(2).toString();
)");

    Interpreter interpreter;

    auto prog = parser.ParseProgram();
    ASSERT_TRUE(prog->IsProgram());

    auto comp = interpreter.Execute(prog);
    EXPECT_EQ(types::CompletionType::NORMAL, comp.GetType());
    ASSERT_TRUE(comp.GetValue()->IsString());
    EXPECT_EQ(u"camel,duck,elephant", comp.GetValue()->GetString());
  }

  {
    Parser parser(uR"(
[1, 2, , 4, 5].slice(1, 4).toString();
)");

    Interpreter interpreter;

    auto prog = parser.ParseProgram();
    ASSERT_TRUE(prog->IsProgram());

    auto comp = interpreter.Execute(prog);
    EXPECT_EQ(types::CompletionType::NORMAL, comp.GetType());
    ASSERT_TRUE(comp.GetValue()->IsString());
    EXPECT_EQ(u"2,,4", comp.GetValue()->GetString());
  }
}

TEST(JSArray, Sort) {
  {
    Parser parser(uR"(
var months = ['March', 'Jan', 'Feb', 'Dec'];
months.sort().toString();
)");

    Interpreter interpreter;

    auto prog = parser.ParseProgram();
    ASSERT_TRUE(prog->IsProgram());

    auto comp = interpreter.Execute(prog);
    EXPECT_EQ(types::CompletionType::NORMAL, comp.GetType());
    ASSERT_TRUE(comp.GetValue()->IsString());
    EXPECT_EQ(u"Dec,Feb,Jan,March", comp.GetValue()->GetString());
  }

  {
    Parser parser(uR"(
var numberArray = [40, 1, 5, 200];
// 根据 value 排序
numberArray.sort(function (a, b) { return a - b; }).toString();
)");

    Interpreter interpreter;

    auto prog = parser.ParseProgram();
    ASSERT_TRUE(prog->IsProgram());

    auto comp = interpreter.Execute(prog);
    EXPECT_EQ(types::CompletionType::NORMAL, comp.GetType());
    ASSERT_TRUE(comp.GetValue()->IsString());
    EXPECT_EQ(u"1,5,40,200", comp.GetValue()->GetString());
  }

  {
    Parser parser(uR"(
var months = ['March', , , , 'Jan', 'Feb', 'Dec'];
months.sort().toString();
)");

    Interpreter interpreter;

    auto prog = parser.ParseProgram();
    ASSERT_TRUE(prog->IsProgram());

    auto comp = interpreter.Execute(prog);
    EXPECT_EQ(types::CompletionType::NORMAL, comp.GetType());
    ASSERT_TRUE(comp.GetValue()->IsString());
    EXPECT_EQ(u"Dec,Feb,Jan,March,,,", comp.GetValue()->GetString());
  }
}

TEST(JSArray, ForEach) {
  {
    Parser parser(uR"(
var arraySparse = [1, 3, /* empty */, 7];
var numCallbackRuns = 0;
var sum = 0;

arraySparse.forEach(function (element) {
  sum += element;
  numCallbackRuns++;
});

sum == 11 && numCallbackRuns == 3;
)");

    Interpreter interpreter;

    auto prog = parser.ParseProgram();
    ASSERT_TRUE(prog->IsProgram());

    auto comp = interpreter.Execute(prog);
    EXPECT_EQ(types::CompletionType::NORMAL, comp.GetType());
    ASSERT_TRUE(comp.GetValue()->IsBoolean());
    EXPECT_EQ(true, comp.GetValue()->GetBoolean());
  }

  {
    Parser parser(uR"(
var copy = function copy(obj) {
  var copy = Object.create(Object.getPrototypeOf(obj));
  var propNames = Object.getOwnPropertyNames(obj);
  propNames.forEach(function (name) {
    var desc = Object.getOwnPropertyDescriptor(obj, name);
    Object.defineProperty(copy, name, desc);
  });
  return copy;
};
var obj1 = {
  a: 1,
  b: 2
};
var obj2 = copy(obj1); // 现在 obj2 看起来和 obj1 一模一样了

obj1.a == obj2.a && obj1.b == obj2.b;
)");

    Interpreter interpreter;

    auto prog = parser.ParseProgram();
    ASSERT_TRUE(prog->IsProgram());

    auto comp = interpreter.Execute(prog);
    EXPECT_EQ(types::CompletionType::NORMAL, comp.GetType());
    ASSERT_TRUE(comp.GetValue()->IsBoolean());
    EXPECT_EQ(true, comp.GetValue()->GetBoolean());
  }
}

TEST(JSArray, Map) {
  {
    Parser parser(uR"(
var numbers = [1, 4, 9];
var doubles = numbers.map(function (num) { return num * 2; });
doubles.join();
)");

    Interpreter interpreter;

    auto prog = parser.ParseProgram();
    ASSERT_TRUE(prog->IsProgram());

    auto comp = interpreter.Execute(prog);
    EXPECT_EQ(types::CompletionType::NORMAL, comp.GetType());
    ASSERT_TRUE(comp.GetValue()->IsString());
    EXPECT_EQ(u"2,8,18", comp.GetValue()->GetString());
  }

  {
    Parser parser(uR"(
[1, , 3].map(function (x) {
    return x * 2;
  }).join();
)");

    Interpreter interpreter;

    auto prog = parser.ParseProgram();
    ASSERT_TRUE(prog->IsProgram());

    auto comp = interpreter.Execute(prog);
    EXPECT_EQ(types::CompletionType::NORMAL, comp.GetType());
    ASSERT_TRUE(comp.GetValue()->IsString());
    EXPECT_EQ(u"2,,6", comp.GetValue()->GetString());
  }
}

TEST(JSArray, Filter) {
  Parser parser(uR"(
function isBigEnough(value) {
  return value >= 10;
}

var filtered = [12, 5, 8, 130, 44].filter(isBigEnough);
filtered.join();
)");

  Interpreter interpreter;

  auto prog = parser.ParseProgram();
  ASSERT_TRUE(prog->IsProgram());

  auto comp = interpreter.Execute(prog);
  EXPECT_EQ(types::CompletionType::NORMAL, comp.GetType());
  ASSERT_TRUE(comp.GetValue()->IsString());
  EXPECT_EQ(u"12,130,44", comp.GetValue()->GetString());
}

TEST(JSString, ToString) {
  Parser parser(uR"(
var x = new String("Hello World");

x.toString();
)");

  Interpreter interpreter;

  auto prog = parser.ParseProgram();
  ASSERT_TRUE(prog->IsProgram());

  auto comp = interpreter.Execute(prog);
  EXPECT_EQ(types::CompletionType::NORMAL, comp.GetType());
  ASSERT_TRUE(comp.GetValue()->IsString());
  EXPECT_EQ(u"Hello World", comp.GetValue()->GetString());
}

TEST(JSString, ValueOf) {
  Parser parser(uR"(
var x = new String("Hello World");

x.valueOf();
)");

  Interpreter interpreter;

  auto prog = parser.ParseProgram();
  ASSERT_TRUE(prog->IsProgram());

  auto comp = interpreter.Execute(prog);
  EXPECT_EQ(types::CompletionType::NORMAL, comp.GetType());
  ASSERT_TRUE(comp.GetValue()->IsString());
  EXPECT_EQ(u"Hello World", comp.GetValue()->GetString());
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

TEST(JSString, LastIndexOf) {
  Parser parser(uR"(
var count = 0;

count += "canal".lastIndexOf("a") == 3; 
count += "canal".lastIndexOf("a", 2) == 1;
count += "canal".lastIndexOf("a", 0) == -1;
count += "canal".lastIndexOf("x") == -1;
count += "canal".lastIndexOf("c", -5) == 0;
count += "canal".lastIndexOf("c", 0) == 0; 
count += "canal".lastIndexOf("") == 5;
count += "canal".lastIndexOf("", 2) == 2;

count;
)");

  Interpreter interpreter;

  auto prog = parser.ParseProgram();
  ASSERT_TRUE(prog->IsProgram());

  auto comp = interpreter.Execute(prog);
  EXPECT_EQ(types::CompletionType::NORMAL, comp.GetType());
  ASSERT_TRUE(comp.GetValue()->IsInt());
  EXPECT_EQ(8, comp.GetValue()->GetInt());
}

TEST(JSString, Slice) {
  Parser parser(uR"(
var str = "The morning is upon us."; // str1 的长度是 23

var count = 0; 

count += str.slice(1, 8) == "he morn";
count += str.slice(4, -2) == "morning is upon u";
count += str.slice(12) == "is upon us.";
count += str.slice(30) == "";

count;
)");

  Interpreter interpreter;

  auto prog = parser.ParseProgram();
  ASSERT_TRUE(prog->IsProgram());

  auto comp = interpreter.Execute(prog);
  EXPECT_EQ(types::CompletionType::NORMAL, comp.GetType());
  ASSERT_TRUE(comp.GetValue()->IsInt());
  EXPECT_EQ(4, comp.GetValue()->GetInt());
}

TEST(JSString, Substring) {
  Parser parser(uR"(
var anyString = "Mozilla";

var count = 0; 

count += anyString.substring(0, 1) == "M";
count += anyString.substring(1, 0) == 'M';

count += anyString.substring(0, 6) == 'Mozill';

count += anyString.substring(4) == 'lla';
count += anyString.substring(4, 7) == 'lla';
count += anyString.substring(7, 4) == 'lla';

count += anyString.substring(0, 7) == 'Mozilla';
count += anyString.substring(0, 10) == 'Mozilla';
)");

  Interpreter interpreter;

  auto prog = parser.ParseProgram();
  ASSERT_TRUE(prog->IsProgram());

  auto comp = interpreter.Execute(prog);
  EXPECT_EQ(types::CompletionType::NORMAL, comp.GetType());
  ASSERT_TRUE(comp.GetValue()->IsInt());
  EXPECT_EQ(8, comp.GetValue()->GetInt());
}

TEST(JSString, ToLowerCase) {
  Parser parser(uR"(
"ALPHABET".toLowerCase();
)");

  Interpreter interpreter;

  auto prog = parser.ParseProgram();
  ASSERT_TRUE(prog->IsProgram());

  auto comp = interpreter.Execute(prog);
  EXPECT_EQ(types::CompletionType::NORMAL, comp.GetType());
  ASSERT_TRUE(comp.GetValue()->IsString());
  EXPECT_EQ(u"alphabet", comp.GetValue()->GetString());
}

TEST(JSString, ToUpperCase) {
  Parser parser(uR"(
"alphabet".toUpperCase();
)");

  Interpreter interpreter;

  auto prog = parser.ParseProgram();
  ASSERT_TRUE(prog->IsProgram());

  auto comp = interpreter.Execute(prog);
  EXPECT_EQ(types::CompletionType::NORMAL, comp.GetType());
  ASSERT_TRUE(comp.GetValue()->IsString());
  EXPECT_EQ(u"ALPHABET", comp.GetValue()->GetString());
}

TEST(JSString, Trim) {
  Parser parser(uR"(
var greeting = '   Hello world!   ';
greeting.trim();
)");

  Interpreter interpreter;

  auto prog = parser.ParseProgram();
  ASSERT_TRUE(prog->IsProgram());

  auto comp = interpreter.Execute(prog);
  EXPECT_EQ(types::CompletionType::NORMAL, comp.GetType());
  ASSERT_TRUE(comp.GetValue()->IsString());
  EXPECT_EQ(u"Hello world!", comp.GetValue()->GetString());
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

TEST(JSBoolean, ToString) {
  Parser parser(uR"(
var flag1 = new Boolean(true);
var flag2 = new Boolean(1);

var count = 0;
count += flag1.toString() == "true";
count += flag2.toString() == 'true';
count;
)");

  Interpreter interpreter;

  auto prog = parser.ParseProgram();
  ASSERT_TRUE(prog->IsProgram());

  auto comp = interpreter.Execute(prog);
  EXPECT_EQ(types::CompletionType::NORMAL, comp.GetType());
  ASSERT_TRUE(comp.GetValue()->IsInt());
  EXPECT_EQ(2, comp.GetValue()->GetInt());
}

TEST(JSBoolean, ValueOf) {
  Parser parser(uR"(
)");

  Interpreter interpreter;

  auto prog = parser.ParseProgram();
  ASSERT_TRUE(prog->IsProgram());

  auto comp = interpreter.Execute(prog);
  EXPECT_EQ(types::CompletionType::NORMAL, comp.GetType());
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

TEST(JSNumber, ToString) {
  Parser parser(uR"(
var count = 0;
count += (17).toString() == "17";
count += (17.2).toString() == "17.2";
count;
)");

  Interpreter interpreter;

  auto prog = parser.ParseProgram();
  ASSERT_TRUE(prog->IsProgram());

  auto comp = interpreter.Execute(prog);
  EXPECT_EQ(types::CompletionType::NORMAL, comp.GetType());
  ASSERT_TRUE(comp.GetValue()->IsInt());
  EXPECT_EQ(2, comp.GetValue()->GetInt());
}

TEST(JSNumber, ValueOf) {
  Parser parser(uR"(
)");

  Interpreter interpreter;

  auto prog = parser.ParseProgram();
  ASSERT_TRUE(prog->IsProgram());

  auto comp = interpreter.Execute(prog);
  EXPECT_EQ(types::CompletionType::NORMAL, comp.GetType());
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

TEST(JSMath, Atan) {
  Parser parser(uR"(
function DoubleEqual(x, y) {
  return Math.abs(x - y) < 1e-7;
}

var count = 0; 

count += DoubleEqual(Math.atan(1), 0.7853981633974483);
count += DoubleEqual(Math.atan(0), 0);
)");

  Interpreter interpreter;

  auto prog = parser.ParseProgram();
  ASSERT_TRUE(prog->IsProgram());

  auto comp = interpreter.Execute(prog);
  EXPECT_EQ(types::CompletionType::NORMAL, comp.GetType());
  ASSERT_TRUE(comp.GetValue()->IsInt());
  EXPECT_DOUBLE_EQ(2, comp.GetValue()->GetInt());
}

TEST(JSMath, Atan2) {
  Parser parser(uR"(
function DoubleEqual(x, y) {
  return Math.abs(x - y) < 1e-7;
}

var count = 0; 

count += DoubleEqual(Math.atan2(90, 15), 1.4056476493802699);
count += DoubleEqual(Math.atan2(15, 90), 0.16514867741462683);
count += DoubleEqual(Math.atan2(0, -0), Math.PI);
count += DoubleEqual(Math.atan2(0, 0), 0);
count += DoubleEqual(Math.atan2(0, -1), Math.PI)
count += DoubleEqual(Math.atan2(0, 1), 0);
count += DoubleEqual(Math.atan2(-0, 1), -0);
count += DoubleEqual(Math.atan2(-1, 0), -Math.PI / 2);
count += DoubleEqual(Math.atan2(-1, -0), -Math.PI / 2);
count += DoubleEqual(Math.atan2(1, 0), Math.PI / 2);
count += DoubleEqual(Math.atan2(1, -0), Math.PI / 2);
count += DoubleEqual(Math.atan2(1, -Infinity), Math.PI);
count += DoubleEqual(Math.atan2(-1, -Infinity), -Math.PI);
count += DoubleEqual(Math.atan2(1, +Infinity), 0);
count += DoubleEqual(Math.atan2(-1, +Infinity), -0);
count += DoubleEqual(Math.atan2(+Infinity, 34234), Math.PI / 2);
count += DoubleEqual(Math.atan2(-Infinity, 2342), -Math.PI / 2);
count += DoubleEqual(Math.atan2(+Infinity, -Infinity), 3 * Math.PI / 4);
count += DoubleEqual(Math.atan2(-Infinity, -Infinity), -3 * Math.PI / 4);
count += DoubleEqual(Math.atan2(+Infinity, +Infinity), Math.PI / 4);
count += DoubleEqual(Math.atan2(-Infinity, +Infinity), -Math.PI / 4);
)");

  Interpreter interpreter;

  auto prog = parser.ParseProgram();
  ASSERT_TRUE(prog->IsProgram());

  auto comp = interpreter.Execute(prog);
  EXPECT_EQ(types::CompletionType::NORMAL, comp.GetType());
  ASSERT_TRUE(comp.GetValue()->IsInt());
  EXPECT_DOUBLE_EQ(21, comp.GetValue()->GetInt());
}

TEST(JSMath, Ceil) {
  Parser parser(uR"(
function DoubleEqual(x, y) {
  if (x == Infinity && y == Infinity   || 
      x == -Infinity && y == -Infinity || 
      isNaN(x) && isNaN(y)) { 
    return true;
  }
  return Math.abs(x - y) < 1e-7;
}

var count = 0;

count += DoubleEqual(Math.ceil(-Infinity), -Infinity);
count += DoubleEqual(Math.ceil(-7.004), -7);
count += DoubleEqual(Math.ceil(-4), -4);
count += DoubleEqual(Math.ceil(-0.95), -0);
count += DoubleEqual(Math.ceil(-0), -0);
count += DoubleEqual(Math.ceil(0), 0);
count += DoubleEqual(Math.ceil(0.95), 1);
count += DoubleEqual(Math.ceil(4), 4);
count += DoubleEqual(Math.ceil(7.004), 8);
count += DoubleEqual(Math.ceil(Infinity), Infinity);
)");

  Interpreter interpreter;

  auto prog = parser.ParseProgram();
  ASSERT_TRUE(prog->IsProgram());

  auto comp = interpreter.Execute(prog);
  EXPECT_EQ(types::CompletionType::NORMAL, comp.GetType());
  ASSERT_TRUE(comp.GetValue()->IsInt());
  EXPECT_DOUBLE_EQ(10, comp.GetValue()->GetInt());
}

TEST(JSMath, Cos) {
  Parser parser(uR"(
function DoubleEqual(x, y) {
  if (x == Infinity && y == Infinity   || 
      x == -Infinity && y == -Infinity || 
      isNaN(x) && isNaN(y)) { 
    return true;
  }
  return Math.abs(x - y) < 1e-7;
}

var count = 0;

count += DoubleEqual(Math.cos(0), 1);
count += DoubleEqual(Math.cos(1), 0.5403023058681398);
count += DoubleEqual(Math.cos(Math.PI), -1)
count += DoubleEqual(Math.cos(2 * Math.PI), 1);
)");

  Interpreter interpreter;

  auto prog = parser.ParseProgram();
  ASSERT_TRUE(prog->IsProgram());

  auto comp = interpreter.Execute(prog);
  EXPECT_EQ(types::CompletionType::NORMAL, comp.GetType());
  ASSERT_TRUE(comp.GetValue()->IsInt());
  EXPECT_DOUBLE_EQ(4, comp.GetValue()->GetInt());
}

TEST(JSMath, Exp) {
  Parser parser(uR"(
function DoubleEqual(x, y) {
  if (x == Infinity && y == Infinity   || 
      x == -Infinity && y == -Infinity || 
      isNaN(x) && isNaN(y)) { 
    return true;
  }
  return Math.abs(x - y) < 1e-7;
}

var count = 0;

count += DoubleEqual(Math.exp(-1), 0.36787944117144233);
count += DoubleEqual(Math.exp(0), 1)
count += DoubleEqual(Math.exp(1), Math.E);
)");

  Interpreter interpreter;

  auto prog = parser.ParseProgram();
  ASSERT_TRUE(prog->IsProgram());

  auto comp = interpreter.Execute(prog);
  EXPECT_EQ(types::CompletionType::NORMAL, comp.GetType());
  ASSERT_TRUE(comp.GetValue()->IsInt());
  EXPECT_DOUBLE_EQ(3, comp.GetValue()->GetInt());
}

TEST(JSMath, Floor) {
  Parser parser(uR"(
function DoubleEqual(x, y) {
  if (x == Infinity && y == Infinity   || 
      x == -Infinity && y == -Infinity || 
      isNaN(x) && isNaN(y)) { 
    return true;
  }
  return Math.abs(x - y) < 1e-7;
}

var count = 0;

count += DoubleEqual(Math.floor(-Infinity), -Infinity)
count += DoubleEqual(Math.floor(-45.95), -46)
count += DoubleEqual(Math.floor(-45.05), -46)
count += DoubleEqual(Math.floor(-0), -0)
count += DoubleEqual(Math.floor(0), 0)
count += DoubleEqual(Math.floor(4), 4)
count += DoubleEqual(Math.floor(45.05), 45)
count += DoubleEqual(Math.floor(45.95), 45)
count += DoubleEqual(Math.floor(Infinity), Infinity)
)");

  Interpreter interpreter;

  auto prog = parser.ParseProgram();
  ASSERT_TRUE(prog->IsProgram());

  auto comp = interpreter.Execute(prog);
  EXPECT_EQ(types::CompletionType::NORMAL, comp.GetType());
  ASSERT_TRUE(comp.GetValue()->IsInt());
  EXPECT_DOUBLE_EQ(9, comp.GetValue()->GetInt());
}

TEST(JSMath, Log) {
  Parser parser(uR"(
function DoubleEqual(x, y) {
  if (x == Infinity && y == Infinity   || 
      x == -Infinity && y == -Infinity ||
      isNaN(x) && isNaN(y)) { 
    return true;
  }
  return Math.abs(x - y) < 1e-7;
}

var count = 0;

count += DoubleEqual(Math.log(-1), NaN)
count += DoubleEqual(Math.log(0), -Infinity)
count += DoubleEqual(Math.log(1), 0)
count += DoubleEqual(Math.log(10), 2.302585092994046)
)");

  Interpreter interpreter;

  auto prog = parser.ParseProgram();
  ASSERT_TRUE(prog->IsProgram());

  auto comp = interpreter.Execute(prog);
  EXPECT_EQ(types::CompletionType::NORMAL, comp.GetType());
  ASSERT_TRUE(comp.GetValue()->IsInt());
  EXPECT_DOUBLE_EQ(4, comp.GetValue()->GetInt());
}

TEST(JSMath, Max) {
  Parser parser(uR"(
function DoubleEqual(x, y) {
  if (x == Infinity && y == Infinity   || 
      x == -Infinity && y == -Infinity ||
      isNaN(x) && isNaN(y)) { 
    return true;
  }
  return Math.abs(x - y) < 1e-7;
}

var count = 0;

count += DoubleEqual(Math.max(10, 20), 20);
count += DoubleEqual(Math.max(-10, -20), -10);
count += DoubleEqual(Math.max(-10, 20), 20);

function getMaxOfArray(numArray) {
  return Math.max.apply(null, numArray);
}

count += DoubleEqual(getMaxOfArray([1, 2, 3, 4]), 4);
)");

  Interpreter interpreter;

  auto prog = parser.ParseProgram();
  ASSERT_TRUE(prog->IsProgram());

  auto comp = interpreter.Execute(prog);
  EXPECT_EQ(types::CompletionType::NORMAL, comp.GetType());
  ASSERT_TRUE(comp.GetValue()->IsInt());
  EXPECT_DOUBLE_EQ(4, comp.GetValue()->GetInt());
}

TEST(JSMath, Min) {
  Parser parser(uR"(
function DoubleEqual(x, y) {
  if (x == Infinity && y == Infinity   || 
      x == -Infinity && y == -Infinity ||
      isNaN(x) && isNaN(y)) { 
    return true;
  }
  return Math.abs(x - y) < 1e-7;
}

var count = 0;

count += DoubleEqual(Math.min(10, 20), 10);
count += DoubleEqual(Math.min(-10, -20), -20);
count += DoubleEqual(Math.min(-10, 20), -10);

function getMinOfArray(numArray) {
  return Math.min.apply(null, numArray);
}

count += DoubleEqual(getMinOfArray([1, 2, 3, 4]), 1);
)");

  Interpreter interpreter;

  auto prog = parser.ParseProgram();
  ASSERT_TRUE(prog->IsProgram());

  auto comp = interpreter.Execute(prog);
  EXPECT_EQ(types::CompletionType::NORMAL, comp.GetType());
  ASSERT_TRUE(comp.GetValue()->IsInt());
  EXPECT_DOUBLE_EQ(4, comp.GetValue()->GetInt());
}

TEST(JSMath, Pow) {
  Parser parser(uR"(
function DoubleEqual(x, y) {
  if (x == Infinity && y == Infinity   || 
      x == -Infinity && y == -Infinity ||
      isNaN(x) && isNaN(y)) { 
    return true;
  }
  return Math.abs(x - y) < 1e-7;
}

var count = 0;

count += DoubleEqual(Math.pow(7, 3), 343);
count += DoubleEqual(Math.pow(4, 0.5), 2);
count += DoubleEqual(Math.pow(7, -2), 0.02040816326530612);
count += DoubleEqual(Math.pow(-7, 0.5), NaN);
)");

  Interpreter interpreter;

  auto prog = parser.ParseProgram();
  ASSERT_TRUE(prog->IsProgram());

  auto comp = interpreter.Execute(prog);
  EXPECT_EQ(types::CompletionType::NORMAL, comp.GetType());
  ASSERT_TRUE(comp.GetValue()->IsInt());
  EXPECT_DOUBLE_EQ(4, comp.GetValue()->GetInt());
}

TEST(JSMath, Random) {
  Parser parser(uR"(

var count = 0;

function getRandomInt(max) {
  return Math.floor(Math.random() * max);
}

var number = getRandomInt(3);

count += number == 0 || number == 1 || number == 2;
count += getRandomInt(1) == 0;
)");

  Interpreter interpreter;

  auto prog = parser.ParseProgram();
  ASSERT_TRUE(prog->IsProgram());

  auto comp = interpreter.Execute(prog);
  EXPECT_EQ(types::CompletionType::NORMAL, comp.GetType());
  ASSERT_TRUE(comp.GetValue()->IsInt());
  EXPECT_DOUBLE_EQ(2, comp.GetValue()->GetInt());
}

TEST(JSMath, Round) {
  Parser parser(uR"(
var count = 0;

count += Math.round(20.49) == 20;
count += Math.round(20.5) == 21;
count += Math.round(-20.5) == -20;
count += Math.round(-20.51) == -21;
)");

  Interpreter interpreter;

  auto prog = parser.ParseProgram();
  ASSERT_TRUE(prog->IsProgram());

  auto comp = interpreter.Execute(prog);
  EXPECT_EQ(types::CompletionType::NORMAL, comp.GetType());
  ASSERT_TRUE(comp.GetValue()->IsInt());
  EXPECT_DOUBLE_EQ(4, comp.GetValue()->GetInt());
}

TEST(JSMath, Sin) {
  Parser parser(uR"(
function DoubleEqual(x, y) {
  if (x == Infinity && y == Infinity   || 
      x == -Infinity && y == -Infinity ||
      isNaN(x) && isNaN(y)) { 
    return true;
  }
  return Math.abs(x - y) < 1e-7;
}

var count = 0;

count += DoubleEqual(Math.sin(0), 0);
count += DoubleEqual(Math.sin(1), 0.8414709848078965)
count += DoubleEqual(Math.sin(Math.PI / 2), 1);
)");

  Interpreter interpreter;

  auto prog = parser.ParseProgram();
  ASSERT_TRUE(prog->IsProgram());

  auto comp = interpreter.Execute(prog);
  EXPECT_EQ(types::CompletionType::NORMAL, comp.GetType());
  ASSERT_TRUE(comp.GetValue()->IsInt());
  EXPECT_DOUBLE_EQ(3, comp.GetValue()->GetInt());
}

TEST(JSMath, Sqrt) {
  Parser parser(uR"(
function DoubleEqual(x, y) {
  if (x == Infinity && y == Infinity   || 
      x == -Infinity && y == -Infinity ||
      isNaN(x) && isNaN(y)) { 
    return true;
  }
  return Math.abs(x - y) < 1e-7;
}

var count = 0;

count += DoubleEqual(Math.sqrt(9), 3);
count += DoubleEqual(Math.sqrt(2), 1.414213562373095);
count += DoubleEqual(Math.sqrt(1), 1);
count += DoubleEqual(Math.sqrt(0), 0)
count += DoubleEqual(Math.sqrt(-1), NaN);
count += DoubleEqual(Math.sqrt(-0), -0);
)");

  Interpreter interpreter;

  auto prog = parser.ParseProgram();
  ASSERT_TRUE(prog->IsProgram());

  auto comp = interpreter.Execute(prog);
  EXPECT_EQ(types::CompletionType::NORMAL, comp.GetType());
  ASSERT_TRUE(comp.GetValue()->IsInt());
  EXPECT_DOUBLE_EQ(6, comp.GetValue()->GetInt());
}

TEST(JSMath, Tan) {
  Parser parser(uR"(
function DoubleEqual(x, y) {
  if (x == Infinity && y == Infinity   || 
      x == -Infinity && y == -Infinity ||
      isNaN(x) && isNaN(y)) { 
    return true;
  }
  return Math.abs(x - y) < 1e-7;
}

function getTanDeg(deg) {
  var rad = (deg * Math.PI) / 180;
  return Math.tan(rad);
}

var count = 0;

count += DoubleEqual(getTanDeg(90), Math.tan(Math.PI / 2));
)");

  Interpreter interpreter;

  auto prog = parser.ParseProgram();
  ASSERT_TRUE(prog->IsProgram());

  auto comp = interpreter.Execute(prog);
  EXPECT_EQ(types::CompletionType::NORMAL, comp.GetType());
  ASSERT_TRUE(comp.GetValue()->IsInt());
  EXPECT_DOUBLE_EQ(1, comp.GetValue()->GetInt());
}

TEST(JSError, ErrorConstructorConstruct) {
  Parser parser(uR"(
var error = new Error('I was constructed via the "new" keyword!');
try {
  throw error;
} catch (e) {
  e.message;
}
)");

  Interpreter interpreter;

  auto prog = parser.ParseProgram();
  ASSERT_TRUE(prog->IsProgram());

  auto comp = interpreter.Execute(prog);
  EXPECT_EQ(types::CompletionType::NORMAL, comp.GetType());
  ASSERT_TRUE(comp.GetValue()->IsString());
  EXPECT_EQ(u"I was constructed via the \"new\" keyword!", comp.GetValue()->GetString());
}

TEST(JSError, ErrorConstructorCall) {
  Parser parser(uR"(
var error = Error('I was constructed using a function call.');
error.message;
)");

  Interpreter interpreter;

  auto prog = parser.ParseProgram();
  ASSERT_TRUE(prog->IsProgram());

  auto comp = interpreter.Execute(prog);
  EXPECT_EQ(types::CompletionType::NORMAL, comp.GetType());
  ASSERT_TRUE(comp.GetValue()->IsString());
  EXPECT_EQ(u"I was constructed using a function call.", comp.GetValue()->GetString());
}

TEST(JSError, ToString) {
  Parser parser(uR"(
var count = 0;

count += new Error("fatal error").toString() == "Error: fatal error";

var e = new Error('fatal error');
e.name = '';
count += e.toString() == "fatal error";

e = new Error('fatal error');
e.name = '';
e.message = '';
count += e.toString() == "Error";

e = new Error('fatal error');
e.name = '';
e.message = undefined;
count += e.toString() == '';

e = new Error('fatal error');
e.name = '你好';
e.message = undefined;
count += e.toString() == "你好";
)");

  Interpreter interpreter;

  auto prog = parser.ParseProgram();
  ASSERT_TRUE(prog->IsProgram());

  auto comp = interpreter.Execute(prog);
  EXPECT_EQ(types::CompletionType::NORMAL, comp.GetType());
  ASSERT_TRUE(comp.GetValue()->IsInt());
  EXPECT_EQ(4, comp.GetValue()->GetInt());
}

TEST(Arguments, Test) {
  Parser parser(uR"(
var count = 0;

function add(a, b, c) {
  count += arguments[0] == a;
  count += arguments[1] == b;
  count += arguments[2] == c;
  return a + b + c; 
}

add(1, 2, 3);

count;
)");

  Interpreter interpreter;

  auto prog = parser.ParseProgram();
  ASSERT_TRUE(prog->IsProgram());

  auto comp = interpreter.Execute(prog);
  EXPECT_EQ(types::CompletionType::NORMAL, comp.GetType());
  ASSERT_TRUE(comp.GetValue()->IsInt());
  EXPECT_EQ(3, comp.GetValue()->GetInt());
}
