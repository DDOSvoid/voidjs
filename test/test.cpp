#include "gtest/gtest.h"

#include "voidjs/types/js_value.h"
#include "voidjs/types/object_factory.h"
#include "voidjs/builtins/js_function.h"
#include "voidjs/builtins/js_array.h"
#include "voidjs/builtins/js_boolean.h"
#include "voidjs/interpreter/interpreter.h"

using namespace voidjs;

TEST(Voidjs, Loop) {
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

TEST(Voidjs, Language) {
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


TEST(Voidjs, Closure) {
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
