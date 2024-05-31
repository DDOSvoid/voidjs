#include "gtest/gtest.h"

#include "voidjs/types/js_value.h"
#include "voidjs/types/object_factory.h"
#include "voidjs/builtins/js_function.h"
#include "voidjs/builtins/js_array.h"
#include "voidjs/builtins/js_boolean.h"
#include "voidjs/interpreter/interpreter.h"

using namespace voidjs;

TEST(Quickjs, Loop) {
  Parser parser(uR"(
function assert(actual, expected, message) {
    if (arguments.length == 1)
        expected = true;

    if (actual === expected)
        return;

    if (actual !== null && expected !== null
    &&  typeof actual == 'object' && typeof expected == 'object'
    &&  actual.toString() === expected.toString())
        return;

    throw Error('assertion failed: got |' + actual + '|' +
                ', expected |' + expected + '|' +
                (message ? ' (' + message + ')' : ''));
}

function test_while()
{
    var i, c;
    i = 0;
    c = 0;
    while (i < 3) {
        c++;
        i++;
    }
    assert(c === 3);
}

function test_while_break()
{
    var i, c;
    i = 0;
    c = 0;
    while (i < 3) {
        c++;
        if (i == 1)
            break;
        i++;
    }
    assert(c === 2 && i === 1);
}

function test_do_while()
{
    var i, c;
    i = 0;
    c = 0;
    do {
        c++;
        i++;
    } while (i < 3);
    assert(c === 3 && i === 3);
}

function test_for()
{
    var i, c;
    c = 0;
    for(i = 0; i < 3; i++) {
        c++;
    }
    assert(c === 3 && i === 3);

    c = 0;
    for(var j = 0; j < 3; j++) {
        c++;
    }
    assert(c === 3 && j === 3);
}

function test_for_in()
{
    var i, tab, a, b;

    tab = [];
    for(i in {x:1, y: 2}) {
        tab.push(i);
    }
    tab.sort();
    assert(tab.toString(), "x,y", "for_in");

    /* prototype chain test */
    a = {x:2, y: 2, "1": 3};
    b = {"4" : 3 }; 
    Object.setPrototypeOf(a, b);
    tab = [];
    for(i in a) {
        tab.push(i);
    }
    // ES5.1 does not require fixed property order
    tab.sort();
    assert(tab.toString(), "1,4,x,y", "for_in");

    /* non enumerable properties hide enumerables ones in the
       prototype chain */
    a = {y: 2, "1": 3};
    Object.defineProperty(a, "x", { value: 1 });
    b = {"x" : 3 };
    Object.setPrototypeOf(a, b);
    tab = [];
    for(i in a) {
        tab.push(i);
    }
    // ES5.1 does not require fixed property order
    tab.sort();
    assert(tab.toString(), "1,y", "for_in");

    /* array optimization */
    a = [];
    for(i = 0; i < 10; i++)
        a.push(i);
    tab = [];
    for(i in a) {
        tab.push(i);
    }
    tab.sort();
    assert(tab.toString(), "0,1,2,3,4,5,6,7,8,9", "for_in");

    /* iterate with a field */
    a={x:0};
    tab = [];
    for(a.x in {x:1, y: 2}) {
        tab.push(a.x);
    }
    tab.sort();
    assert(tab.toString(), "x,y", "for_in");

    /* iterate with a variable field */
    a=[0];
    tab = [];
    for(a[0] in {x:1, y: 2}) {
        tab.push(a[0]);
    }
    tab.sort();
    assert(tab.toString(), "x,y", "for_in");

    /* variable definition in the for in */
    tab = [];
    for(var j in {x:1, y: 2}) {
        tab.push(j);
    }
    tab.sort();
    assert(tab.toString(), "x,y", "for_in");

    /* variable assigment in the for in */
    tab = [];
    for(var k = 2 in {x:1, y: 2}) {
        tab.push(k);
    }
    tab.sort();
    assert(tab.toString(), "x,y", "for_in");
}

function test_for_in2()
{
    var i, tab;
    tab = [];
    for(i in {x:1, y: 2, z:3}) {
        if (i === "y")
            continue;
        tab.push(i);
    }
    tab.sort();
    assert(tab.toString() == "x,z");
}

function test_for_break()
{
    var i, c;
    c = 0;
    L1: for(i = 0; i < 3; i++) {
        c++;
        if (i == 0)
            continue;
        while (1) {
            break L1;
        }
    }
    assert(c === 2 && i === 1);
}

function test_switch1()
{
    var i, a, s;
    s = "";
    for(i = 0; i < 3; i++) {
        a = "?";
        switch(i) {
        case 0:
            a = "a";
            break;
        case 1:
            a = "b";
            break;
        default:
            a = "c";
            break;
        }
        s += a;
    }
    assert(s === "abc" && i === 3);
}

function test_switch2()
{
    var i, a, s;
    s = "";
    for(i = 0; i < 4; i++) {
        a = "?";
        switch(i) {
        case 0:
            a = "a";
            break;
        case 1:
            a = "b";
            break;
        case 2:
            continue;
        default:
            a = "" + i;
            break;
        }
        s += a;
    }
    assert(s === "ab3" && i === 4);
}

function test_try_catch1()
{
    try {
        throw Error("hello");
    } catch (e) {
        assert(e.message, "hello", "catch");
        return;
    }
    assert(false, "catch");
}

function test_try_catch2()
{
    var a;
    try {
        a = 1;
    } catch (e) {
        a = 2;
    }
    assert(a, 1, "catch");
}

function test_try_catch3()
{
    var s;
    s = "";
    try {
        s += "t";
    } catch (e) {
        s += "c";
    } finally {
        s += "f";
    }
    assert(s, "tf", "catch");
}

function test_try_catch4()
{
    var s;
    s = "";
    try {
        s += "t";
        throw Error("c");
    } catch (e) {
        s += e.message;
    } finally {
        s += "f";
    }
    assert(s, "tcf", "catch");
}

function test_try_catch5()
{
    var s;
    s = "";
    for(;;) {
        try {
            s += "t";
            break;
            s += "b";
        } finally {
            s += "f";
        }
    }
    assert(s, "tf", "catch");
}

function test_try_catch6()
{
    function f() {
        try {
            s += 't';
            return 1;
        } finally {
            s += "f";
        }
    }
    var s = "";
    assert(f() === 1);
    assert(s, "tf", "catch6");
}

function test_try_catch7()
{
    var s;
    s = "";

    try {
        try {
            s += "t";
            throw Error("a");
        } finally {
            s += "f";
        }
    } catch(e) {
        s += e.message;
    } finally {
        s += "g";
    }
    assert(s, "tfag", "catch");
}

function test_try_catch8()
{
    var i, s;

    s = "";
    for(var i in {x:1, y:2}) {
        try {
            s += i;
            throw Error("a");
        } catch (e) {
            s += e.message;
        } finally {
            s += "f";
        }
    }
    assert(s, "xafyaf");
}

test_while();
test_while_break();
test_do_while();
test_for();
test_for_break();
test_switch1();
test_switch2();
test_for_in();
test_for_in2();

test_try_catch1();
test_try_catch2();
test_try_catch3();
test_try_catch4();
test_try_catch5();
test_try_catch6();
test_try_catch7();
test_try_catch8();

)");

  Interpreter interpreter;

  auto prog = parser.ParseProgram();
  ASSERT_TRUE(prog->IsProgram());

  auto comp = interpreter.Execute(prog);
  EXPECT_EQ(types::CompletionType::NORMAL, comp.GetType());
}

TEST(Quickjs, Language) {
  Parser parser(uR"(
function assert(actual, expected, message) {
    if (arguments.length == 1)
        expected = true;

    if (actual === expected)
        return;
    
    if (actual !== null && expected !== null
    &&  typeof actual == 'object' && typeof expected == 'object'
    &&  actual.toString() === expected.toString())
        return;
    
    throw Error('assertion failed: got |' + actual + '|' +
                ', expected |' + expected + '|' +
                (message ? ' (' + message + ')' : ''));
}

/*----------------*/

function test_op1()
{
    var r, a;
    r = 1 + 2;
    assert(r, 3, "1 + 2 === 3");

    r = 1 - 2;
    assert(r, -1, "1 - 2 === -1");

    r = -1;
    assert(r, -1, "-1 === -1");

    r = +2;
    assert(r, 2, "+2 === 2");

    r = 2 * 3;
    assert(r, 6, "2 * 3 === 6");

    r = 4 / 2;
    assert(r, 2, "4 / 2 === 2");

    r = 4 % 3;
    assert(r, 1, "4 % 3 === 3");

    r = 4 << 2;
    assert(r, 16, "4 << 2 === 16");

    r = 1 << 0;
    assert(r, 1, "1 << 0 === 1");

    r = 1 << 31;
    assert(r, -2147483648, "1 << 31 === -2147483648");
    
    r = 1 << 32;
    assert(r, 1, "1 << 32 === 1");
    
    r = (1 << 31) < 0;
    assert(r, true, "(1 << 31) < 0 === true");

    r = -4 >> 1;
    assert(r, -2, "-4 >> 1 === -2");

    r = -4 >>> 1;
    assert(r, 0x7ffffffe, "-4 >>> 1 === 0x7ffffffe");

    r = 1 & 1;
    assert(r, 1, "1 & 1 === 1");

    r = 0 | 1;
    assert(r, 1, "0 | 1 === 1");

    r = 1 ^ 1;
    assert(r, 0, "1 ^ 1 === 0");

    r = ~1;
    assert(r, -2, "~1 === -2");

    r = !1;
    assert(r, false, "!1 === false");

    assert((1 < 2), true, "(1 < 2) === true");

    assert((2 > 1), true, "(2 > 1) === true");

    assert(('b' > 'a'), true, "('b' > 'a') === true");
}

function test_cvt()
{
    assert((NaN | 0) === 0);
    assert((Infinity | 0) === 0);
    assert(((-Infinity) | 0) === 0);
    assert(("12345" | 0) === 12345);
    assert(("0x12345" | 0) === 0x12345);
    assert(((4294967296 * 3 - 4) | 0) === -4);
    
    assert(("12345" >>> 0) === 12345);
    assert(("0x12345" >>> 0) === 0x12345);
    assert((NaN >>> 0) === 0);
    assert((Infinity >>> 0) === 0);
    assert(((-Infinity) >>> 0) === 0);
    assert(((4294967296 * 3 - 4) >>> 0) === (4294967296 - 4));
}

function test_eq()
{
    assert(null == undefined);
    assert(undefined == null);
    assert(true == 1);
    assert(0 == false);
    assert("" == 0);
    assert("123" == 123);
    assert("122" != 123);
    assert((new Number(1)) == 1);
    assert(2 == (new Number(2)));
    assert((new String("abc")) == "abc");
    assert({} != "abc");
}

function test_inc_dec()
{
    var a, r;
    
    a = 1;
    r = a++;
    assert(r === 1 && a === 2, true, "++");

    a = 1;
    r = ++a;
    assert(r === 2 && a === 2, true, "++");

    a = 1;
    r = a--;
    assert(r === 1 && a === 0, true, "--");

    a = 1;
    r = --a;
    assert(r === 0 && a === 0, true, "--");

    a = {x:true};
    a.x++;
    assert(a.x, 2, "++");

    a = {x:true};
    a.x--;
    assert(a.x, 0, "--");

    a = [true];
    a[0]++;
    assert(a[0], 2, "++");
    
    a = {x:true};
    r = a.x++;
    assert(r === 1 && a.x === 2, true, "++");
    
    a = {x:true};
    r = a.x--;
    assert(r === 1 && a.x === 0, true, "--");
    
    a = [true];
    r = a[0]++;
    assert(r === 1 && a[0] === 2, true, "++");
    
    a = [true];
    r = a[0]--;
    assert(r === 1 && a[0] === 0, true, "--");
}

function F(x)
{
    this.x = x;
}

function test_op2()
{
    var a, b;
    a = new Object;
    a.x = 1;
    assert(a.x, 1, "new");
    b = new F(2);
    assert(b.x, 2, "new");

    a = {x : 2};
    assert(("x" in a), true, "in");
    assert(("y" in a), false, "in");

    a = {};
    assert((a instanceof Object), true, "instanceof");
    assert((a instanceof String), false, "instanceof");

    assert((typeof 1), "number", "typeof");
    assert((typeof Object), "function", "typeof");
    assert((typeof null), "object", "typeof");
    assert((typeof unknown_var), "undefined", "typeof");
    
    a = {x: 1, if: 2, async: 3};
    assert(a.if === 2);
    assert(a.async === 3);
}

function test_delete()
{
    var a, err;

    a = {x: 1, y: 1};
    assert((delete a.x), true, "delete");
    assert(("x" in a), false, "delete");
    
    /* the following are not tested by test262 */
    assert(delete "abc"[100], true);
}

function test_prototype()
{
    var f = function f() { };
    assert(f.prototype.constructor, f, "prototype");
}

function test_arguments()
{
    function f2() {
        assert(arguments.length, 2, "arguments");
        assert(arguments[0], 1, "arguments");
        assert(arguments[1], 3, "arguments");
    }
    f2(1, 3);
}

function test_object_literal()
{
    var a = { get: 2, set: 3, async: 4 };
    assert(a.get, 2);
    assert(a.set, 3);
    assert(a.async, 4);
}

function test_function_expr_name()
{
    var f;

    /* non strict mode test : assignment to the function name silently
       fails */
    
    f = function myfunc() {
        myfunc = 1;
        return myfunc;
    };
    assert(f(), f);

    // eval unimplemented
    // f = function myfunc() {
    //     eval("myfunc = 1");
    //     return myfunc;
    // };
    // assert(f(), f);
}

test_op1();
test_cvt();
test_eq();
test_inc_dec();
test_op2();
test_delete();
test_prototype();
test_arguments();
test_object_literal();
test_function_expr_name();

)");

  Interpreter interpreter;

  auto prog = parser.ParseProgram();
  ASSERT_TRUE(prog->IsProgram());

  auto comp = interpreter.Execute(prog);
  EXPECT_EQ(types::CompletionType::NORMAL, comp.GetType());
}


TEST(Quickjs, Closure) {
  Parser parser(uR"(
function assert(actual, expected, message) {
    if (arguments.length == 1)
        expected = true;

    if (actual === expected)
        return;
    
    if (actual !== null && expected !== null
    &&  typeof actual == 'object' && typeof expected == 'object'
    &&  actual.toString() === expected.toString())
        return;
    
    throw Error('assertion failed: got |' + actual + '|' +
                ', expected |' + expected + '|' +
                (message ? ' (' + message + ')' : ''));
}

/*----------------*/

var log_str = "";

function log(str)
{
    log_str += str + ",";
}

function f(a, b, c)
{
    var x = 10;
    log("a="+a);
    function g(d) {
        function h() {
            log("d=" + d);
            log("x=" + x);
        }
        log("b=" + b);
        log("c=" + c);
        h();
    }
    g(4);
    return g;
}

var g1 = f(1, 2, 3);
g1(5);

assert(log_str, "a=1,b=2,c=3,d=4,x=10,b=2,c=3,d=5,x=10,", "closure1");

function test_closure1()
{
    function f2()
    {
        var val = 1;
        
        function set(a) {
            val = a;
        }
        function get(a) {
            return val;
        }
        return { "set": set, "get": get };
    }
    
    var obj = f2();
    obj.set(10);
    var r;
    r = obj.get();
    assert(r, 10, "closure2");
}

function test_closure2()
{
    var expr_func = function myfunc1(n) {
        function myfunc2(n) {
            return myfunc1(n - 1);
        }
        if (n == 0)
            return 0;
        else
            return myfunc2(n);
    };
    var r;
    r = expr_func(1);
    assert(r, 0, "expr_func");
}

function test_closure3()
{
    function fib(n)
    {
        if (n <= 0)
            return 0;
        else if (n == 1)
            return 1;
        else
            return fib(n - 1) + fib(n - 2);
    }

    var fib_func = function fib1(n)
    {
        if (n <= 0)
            return 0;
        else if (n == 1)
            return 1;
        else
            return fib1(n - 1) + fib1(n - 2);
    };

    assert(fib(6), 8, "fib");
    assert(fib_func(6), 8, "fib_func");
}

function test_with()
{
    var o1 = { x: "o1", y: "o1" };
    var x = "local";
    var z = "var_obj";
    with (o1) {
        assert(x === "o1");
        assert(eval("x") === "o1");
        var f = function () {
            o2 = { x: "o2" };
            with (o2) {
                assert(x === "o2");
                assert(y === "o1");
                assert(z === "var_obj");
                assert(eval("x") === "o2");
                assert(eval("y") === "o1");
                assert(eval("z") === "var_obj");
                assert(eval('eval("x")') === "o2");
            }
        };
        f();
    }
}

/* eval unimplemented
function test_eval_closure()
{
    var tab;

    tab = [];
    for(var i = 0; i < 3; i++) {
        eval('tab.push(function g1() { return i; })');
    }
    for(var i = 0; i < 3; i++) {
        assert(tab[i]() === i);
    }

    tab = [];
    for(var i = 0; i < 3; i++) {
        var f = function f() {
            eval('tab.push(function g2() { return i; })');
        };
        f();
    }
    for(var i = 0; i < 3; i++) {
        assert(tab[i]() === i);
    }
}
*/

test_closure1();
test_closure2();
test_closure3();
test_with();
// test_eval_closure();

)");

  Interpreter interpreter;

  auto prog = parser.ParseProgram();
  ASSERT_TRUE(prog->IsProgram());

  auto comp = interpreter.Execute(prog);
  EXPECT_EQ(types::CompletionType::NORMAL, comp.GetType());
}

TEST(Quickjs, Builtins) {
  Parser parser(uR"(
function assert(actual, expected, message) {
    if (arguments.length == 1)
        expected = true;

    if (actual === expected)
        return;
    
    if (actual !== null && expected !== null
    &&  typeof actual == 'object' && typeof expected == 'object'
    &&  actual.toString() === expected.toString())
        return;
    
    throw Error('assertion failed: got |' + actual + '|' +
                ', expected |' + expected + '|' +
                (message ? ' (' + message + ')' : ''));
}

/*----------------*/

function my_func(a, b)
{
    return a + b;
}

function test_function()
{
    function f(a, b) {
        var i, tab = [];
        tab.push(this);
        for(i = 0; i < arguments.length; i++)
            tab.push(arguments[i]);
        return tab;
    }
    function constructor1(a) {
        this.x = a;
    }

    var r, g;
    
    r = my_func.call(null, 1, 2);
    assert(r, 3, "call");

    r = my_func.apply(null, [1, 2]);
    assert(r, 3, "apply");

    r = (function () { return 1; }).apply(null, undefined);
    assert(r, 1);
    
    r = new Function("a", "b", "return a + b;");
    assert(r(2,3), 5, "function");

    /* Function.prototype.bind unimplemented 
    g = f.bind(1, 2);
    assert(g.length, 1);
    assert(g(3), [1,2,3]);

    g = constructor1.bind(null, 1);
    r = new g();
    assert(r.x, 1);
    */
}

function test()
{
    "use strict";
    var r, a, b, c, err;

    r = Error("hello");
    assert(r.message, "hello", "Error");

    a = new Object();
    a.x = 1;
    assert(a.x, 1, "Object");

    assert(Object.getPrototypeOf(a), Object.prototype, "getPrototypeOf");
    Object.defineProperty(a, "y", { value: 3, writable: true, configurable: true, enumerable: true });
    assert(a.y, 3, "defineProperty");

    Object.defineProperty(a, "z", { get: function () { return 4; }, set: function(val) { this.z_val = val; }, configurable: true, enumerable: true });
    assert(a.z, 4, "get");
    a.z = 5;
    assert(a.z_val, 5, "set");
    
    a = { get z() { return 4; }, set z(val) { this.z_val = val; } };
    assert(a.z, 4, "get");
    a.z = 5;
    assert(a.z_val, 5, "set");

    b = Object.create(a);
    assert(Object.getPrototypeOf(b), a, "create");
    c = {u:2};
    /* XXX: refcount bug in 'b' instead of 'a' */
    Object.setPrototypeOf(a, c);
    assert(Object.getPrototypeOf(a), c, "setPrototypeOf");

    a = {};
    assert(a.toString(), "[object Object]", "toString");

    a = {x:1};
    assert(Object.isExtensible(a), true, "extensible");
    Object.preventExtensions(a);

    err = false;
    try {
        a.y = 2;
    } catch(e) {
        err = true;
    }
    assert(Object.isExtensible(a), false, "extensible");
    assert(typeof a.y, "undefined", "extensible");
    assert(err, true, "extensible");
}

function test_enum()
{
    var a, tab;
    a = {x:1,
         "18014398509481984": 1,
         "9007199254740992": 1,
         "9007199254740991": 1,
         "4294967296": 1,
         "4294967295": 1,
         y:1,
         "4294967294": 1,
         "1": 2};
    tab = Object.keys(a);
    tab.sort();
    assert(tab, ["1","18014398509481984", "4294967294", "4294967295", "4294967296", "9007199254740991", "9007199254740992","x","y"])
}

function test_array()
{
    "use strict";
    var a, err;

    a = [1, 2, 3];
    assert(a.length, 3, "array");
    assert(a[2], 3, "array1");

    a = new Array(10);
    assert(a.length, 10, "array2");

    a = new Array(1, 2);
    assert(a.length === 2 && a[0] === 1 && a[1] === 2, true, "array3");

    a = [1, 2, 3];
    a.length = 2;
    assert(a.length === 2 && a[0] === 1 && a[1] === 2, true, "array4");

    a = [];
    a[1] = 10;
    a[4] = 3;
    assert(a.length, 5);

    a = [1,2];
    a.length = 5;
    a[4] = 1;
    a.length = 4;
    assert(a[4] !== 1, true, "array5");

    a = [1,2];
    a.push(3,4);
    assert(a.join(), "1,2,3,4", "join");

    a = [1,2,3,4,5];
    Object.defineProperty(a, "3", { configurable: false });
    err = false;
    try {
        a.length = 2;
    } catch(e) {
        err = true;
    }
    assert(err && a.toString() === "1,2,3,4");
}

function test_string()
{
    var a;
    a = String("abc");
    assert(a.length, 3, "string");
    assert(a[1], "b", "string");
    assert(a.charCodeAt(1), 0x62, "string");
    assert(String.fromCharCode(65), "A", "string");
    assert(String.fromCharCode.apply(null, [65, 66, 67]), "ABC", "string");
    assert(a.charAt(1), "b");
    assert(a.charAt(-1), "");
    assert(a.charAt(3), "");
    
    a = "abcd";
    assert(a.substring(1, 3), "bc", "substring");
    a = String.fromCharCode(0x20ac);
    assert(a.charCodeAt(0), 0x20ac, "unicode");
    assert(a, "€", "unicode");
    assert(a, "\u20ac", "unicode");
    assert("a", "\x61", "unicode");

    assert("a".concat("b", "c"), "abc");

    assert("abcabc".indexOf("cab"), 2);
    assert("abcabc".indexOf("cab2"), -1);
    assert("abc".indexOf("c"), 2);

    assert("aaa".indexOf("a"), 0);
    assert("aaa".indexOf("a", NaN), 0);
    assert("aaa".indexOf("a", -Infinity), 0);
    assert("aaa".indexOf("a", -1), 0);
    assert("aaa".indexOf("a", -0), 0);
    assert("aaa".indexOf("a", 0), 0);
    assert("aaa".indexOf("a", 1), 1);
    assert("aaa".indexOf("a", 2), 2);
    assert("aaa".indexOf("a", 3), -1);
    assert("aaa".indexOf("a", 4), -1);
    assert("aaa".indexOf("a", Infinity), -1);

    assert("aaa".indexOf(""), 0);
    assert("aaa".indexOf("", NaN), 0);
    assert("aaa".indexOf("", -Infinity), 0);
    assert("aaa".indexOf("", -1), 0);
    assert("aaa".indexOf("", -0), 0);
    assert("aaa".indexOf("", 0), 0);
    assert("aaa".indexOf("", 1), 1);
    assert("aaa".indexOf("", 2), 2);
    assert("aaa".indexOf("", 3), 3);
    assert("aaa".indexOf("", 4), 3);
    assert("aaa".indexOf("", Infinity), 3);

    assert("aaa".lastIndexOf("a"), 2);
    assert("aaa".lastIndexOf("a", NaN), 2);
    assert("aaa".lastIndexOf("a", -Infinity), 0);
    assert("aaa".lastIndexOf("a", -1), 0);
    assert("aaa".lastIndexOf("a", -0), 0);
    assert("aaa".lastIndexOf("a", 0), 0);
    assert("aaa".lastIndexOf("a", 1), 1);
    assert("aaa".lastIndexOf("a", 2), 2);
    assert("aaa".lastIndexOf("a", 3), 2);
    assert("aaa".lastIndexOf("a", 4), 2);
    assert("aaa".lastIndexOf("a", Infinity), 2);

    assert("aaa".lastIndexOf(""), 3);
    assert("aaa".lastIndexOf("", NaN), 3);
    assert("aaa".lastIndexOf("", -Infinity), 0);
    assert("aaa".lastIndexOf("", -1), 0);
    assert("aaa".lastIndexOf("", -0), 0);
    assert("aaa".lastIndexOf("", 0), 0);
    assert("aaa".lastIndexOf("", 1), 1);
    assert("aaa".lastIndexOf("", 2), 2);
    assert("aaa".lastIndexOf("", 3), 3);
    assert("aaa".lastIndexOf("", 4), 3);
    assert("aaa".lastIndexOf("", Infinity), 3);

    assert("Abc123!".toLowerCase(), "abc123!");
    assert("Abc123!".toUpperCase(), "ABC123!");

    // String.prototype.split unimplemented
    return ;

    assert("a,b,c".split(","), ["a","b","c"]);
    assert(",b,c".split(","), ["","b","c"]);
    assert("a,b,".split(","), ["a","b",""]);

    assert("a,b,c".split(",").length, 3);
    assert(",b,c".split(",").length, 3);
    assert("a,b,".split(",").length, 3);

    assert("aaaa".split(), [ "aaaa" ]);
    // assert("aaaa".split(undefined, 0), [ ]);
    assert("aaaa".split(""), [ "a", "a", "a", "a" ]);
    assert("aaaa".split("", 0), [ ]);
    assert("aaaa".split("", 1), [ "a" ]);
    assert("aaaa".split("", 2), [ "a", "a" ]);
    assert("aaaa".split("a"), [ "", "", "", "", "" ]);
    assert("aaaa".split("a", 2), [ "", "" ]);
    assert("aaaa".split("aa"), [ "", "", "" ]);
    assert("aaaa".split("aa", 0), [ ]);
    assert("aaaa".split("aa", 1), [ "" ]);
    assert("aaaa".split("aa", 2), [ "", "" ]);
    assert("aaaa".split("aaa"), [ "", "a" ]);
    assert("aaaa".split("aaaa"), [ "", "" ]);
    assert("aaaa".split("aaaaa"), [ "aaaa" ]);
    assert("aaaa".split("aaaaa", 0), [  ]);
    assert("aaaa".split("aaaaa", 1), [ "aaaa" ]);

    assert("aaaa".split().length, 1);
    // assert("aaaa".split(undefined, 0).length, [ ]);
    assert("aaaa".split("").length, 4);
    assert("aaaa".split("", 0).length, 0);
    assert("aaaa".split("", 1).length, 1);
    assert("aaaa".split("", 2).length, 2);
    assert("aaaa".split("a").length, 5);
    assert("aaaa".split("a", 2).length, 2);
    assert("aaaa".split("aa").length, 3);
    assert("aaaa".split("aa", 0).length, 0);
    assert("aaaa".split("aa", 1).length, 1);
    assert("aaaa".split("aa", 2).length, 2);
    assert("aaaa".split("aaa").length, 2);
    assert("aaaa".split("aaaa").length, 2);
    assert("aaaa".split("aaaaa").length, 1);
    assert("aaaa".split("aaaaa", 0).length, 0);
    assert("aaaa".split("aaaaa", 1).length, 1);
}

function test_math()
{
    var a;
    a = 1.4;
    assert(Math.floor(a), 1);
    assert(Math.ceil(a), 2);
    //assert(Math.imul(0x12345678, 123), -1088058456);
    //assert(Math.fround(0.1), 0.10000000149011612);
    //assert(Math.hypot() == 0);
    //assert(Math.hypot(-2) == 2);
    //assert(Math.hypot(3, 4) == 5);
    //assert(Math.abs(Math.hypot(3, 4, 5) - 7.0710678118654755) <= 1e-15);
}

function test_number()
{
    // assert(parseInt("123"), 123);
    // assert(parseInt("  123r"), 123);
    // assert(parseInt("0x123"), 0x123);
    // assert(parseInt("0o123"), 0);
    // assert(parseFloat("0x1234"), 0);
    // assert(parseFloat("Infinity"), Infinity);
    // assert(parseFloat("-Infinity"), -Infinity);
    // assert(parseFloat("123.2"), 123.2);
    // assert(parseFloat("123.2e3"), 123200);

    // assert((25).toExponential(0), "3e+1");
    // assert((-25).toExponential(0), "-3e+1");
    // assert((2.5).toPrecision(1), "3");
    // assert((-2.5).toPrecision(1), "-3");
    // assert((1.125).toFixed(2), "1.13");
    // assert((-1.125).toFixed(2), "-1.13");
}

test();
test_function();
test_enum();
test_array();
test_string();
test_math();
test_number();
)");

  Interpreter interpreter;

  auto prog = parser.ParseProgram();
  ASSERT_TRUE(prog->IsProgram());

  auto comp = interpreter.Execute(prog);
  EXPECT_EQ(types::CompletionType::NORMAL, comp.GetType());
}
