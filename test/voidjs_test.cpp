#include "gtest/gtest.h"

#include "voidjs/types/js_value.h"
#include "voidjs/types/object_factory.h"
#include "voidjs/builtins/js_function.h"
#include "voidjs/builtins/js_array.h"
#include "voidjs/builtins/js_boolean.h"
#include "voidjs/interpreter/interpreter.h"

using namespace voidjs;

TEST(Voidjs, QuickSelect) {
  Parser parser(uR"(
// code from https://github.com/TheAlgorithms/JavaScript/blob/master/Data-Structures/Array/QuickSelect.js
function QuickSelect(items, kth) {
  // eslint-disable-line no-unused-vars
  if (kth < 1 || kth > items.length) {
    throw new RangeError('Index Out of Bound');
  }
  return RandomizedSelect(items, 0, items.length - 1, kth);
}
function RandomizedSelect(items, left, right, i) {
  if (left === right) return items[left];
  var pivotIndex = RandomizedPartition(items, left, right);
  var k = pivotIndex - left + 1;
  if (i === k) return items[pivotIndex];
  if (i < k) return RandomizedSelect(items, left, pivotIndex - 1, i);
  return RandomizedSelect(items, pivotIndex + 1, right, i - k);
}
function RandomizedPartition(items, left, right) {
  var rand = getRandomInt(left, right);
  Swap(items, rand, right);
  return Partition(items, left, right);
}
function Partition(items, left, right) {
  var x = items[right];
  var pivotIndex = left - 1;
  for (var j = left; j < right; j++) {
    if (items[j] <= x) {
      pivotIndex++;
      Swap(items, pivotIndex, j);
    }
  }
  Swap(items, pivotIndex + 1, right);
  return pivotIndex + 1;
}
function getRandomInt(min, max) {
  return Math.floor(Math.random() * (max - min + 1)) + min;
}
function Swap(arr, x, y) {
  var _ref = [arr[y], arr[x]];
  arr[x] = _ref[0];
  arr[y] = _ref[1];
}

var array = [18, 29, 30, 32, 5, 20, 26, 24, 15, 46, 36, 43, 42, 47, 21, 34, 0, 33, 41, 23, 25, 8, 28, 3, 13, 6, 17, 10, 39, 1];
QuickSelect(array, 5) == 6;
)");

  Interpreter interpreter;

  auto prog = parser.ParseProgram();
  ASSERT_TRUE(prog->IsProgram());

  auto comp = interpreter.Execute(prog);
  EXPECT_EQ(types::CompletionType::NORMAL, comp.GetType());
  ASSERT_TRUE(comp.GetValue()->IsBoolean());
  EXPECT_EQ(true, comp.GetValue()->GetBoolean());
}

TEST(Voidjs, Trie) {
  Parser parser(uR"(
function _typeof(o) { "@babel/helpers - typeof"; return _typeof = "function" == typeof Symbol && "symbol" == typeof Symbol.iterator ? function (o) { return typeof o; } : function (o) { return o && "function" == typeof Symbol && o.constructor === Symbol && o !== Symbol.prototype ? "symbol" : typeof o; }, _typeof(o); }
function _defineProperties(target, props) { for (var i = 0; i < props.length; i++) { var descriptor = props[i]; descriptor.enumerable = descriptor.enumerable || false; descriptor.configurable = true; if ("value" in descriptor) descriptor.writable = true; Object.defineProperty(target, _toPropertyKey(descriptor.key), descriptor); } }
function _createClass(Constructor, protoProps, staticProps) { if (protoProps) _defineProperties(Constructor.prototype, protoProps); if (staticProps) _defineProperties(Constructor, staticProps); Object.defineProperty(Constructor, "prototype", { writable: false }); return Constructor; }
function _toPropertyKey(t) { var i = _toPrimitive(t, "string"); return "symbol" == _typeof(i) ? i : i + ""; }
function _toPrimitive(t, r) { if ("object" != _typeof(t) || !t) return t; var e = t[Symbol.toPrimitive]; if (void 0 !== e) { var i = e.call(t, r || "default"); if ("object" != _typeof(i)) return i; throw new TypeError("@@toPrimitive must return a primitive value."); } return ("string" === r ? String : Number)(t); }
function _classCallCheck(instance, Constructor) { if (!(instance instanceof Constructor)) { throw new TypeError("Cannot call a class as a function"); } }
var TrieNode = /*#__PURE__*/_createClass(function TrieNode(key, parent) {
  "use strict";

  _classCallCheck(this, TrieNode);
  this.key = key;
  this.count = 0;
  this.children = Object.create(null);
  if (parent === undefined) {
    this.parent = null;
  } else {
    this.parent = parent;
  }
});
var Trie = /*#__PURE__*/function () {
  "use strict";

  function Trie() {
    _classCallCheck(this, Trie);
    // create only root with null key and parent
    this.root = new TrieNode(null, null);
  }

  // Recursively finds the occurrence of all words in a given node
  return _createClass(Trie, [{
    key: "insert",
    value: function insert(word) {
      if (typeof word !== 'string') return;
      if (word === '') {
        this.root.count += 1;
        return;
      }
      var node = this.root;
      var len = word.length;
      var i;
      for (i = 0; i < len; i++) {
        if (node.children[word.charAt(i)] === undefined) {
          node.children[word.charAt(i)] = new TrieNode(word.charAt(i), node);
        }
        node = node.children[word.charAt(i)];
      }
      node.count += 1;
    }
  }, {
    key: "findPrefix",
    value: function findPrefix(word) {
      if (typeof word !== 'string') return null;
      var node = this.root;
      var len = word.length;
      var i;
      // After end of this loop node will be at desired prefix
      for (i = 0; i < len; i++) {
        if (node.children[word.charAt(i)] === undefined) return null; // No such prefix exists
        node = node.children[word.charAt(i)];
      }
      return node;
    }
  }, {
    key: "remove",
    value: function remove(word, count) {
      if (typeof word !== 'string') return;
      if (typeof count !== 'number') count = 1;else if (count <= 0) return;

      // for empty string just delete count of root
      if (word === '') {
        if (this.root.count >= count) this.root.count -= count;else this.root.count = 0;
        return;
      }
      var child = this.root;
      var len = word.length;
      var i, key;
      // child: node which is to be deleted
      for (i = 0; i < len; i++) {
        key = word.charAt(i);
        if (child.children[key] === undefined) return;
        child = child.children[key];
      }

      // Delete no of occurrences specified
      if (child.count >= count) child.count -= count;else child.count = 0;

      // If some occurrences are left we don't delete it or else
      // if the object forms some other objects prefix we don't delete it
      // For checking an empty object
      // https://stackoverflow.com/questions/679915/how-do-i-test-for-an-empty-javascript-object
      if (child.count <= 0 && Object.keys(child.children).length && child.children.constructor === Object) {
        child.parent.children[child.key] = undefined;
      }
    }
  }, {
    key: "findAllWords",
    value: function findAllWords(prefix) {
      var output = [];
      // find the node with provided prefix
      var node = this.findPrefix(prefix);
      // No such prefix exists
      if (node === null) return output;
      Trie.findAllWords(node, prefix, output);
      return output;
    }
  }, {
    key: "contains",
    value: function contains(word) {
      // find the node with given prefix
      var node = this.findPrefix(word);
      // No such word exists
      return node !== null && node.count !== 0;
    }
  }, {
    key: "findOccurrences",
    value: function findOccurrences(word) {
      // find the node with given prefix
      var node = this.findPrefix(word);
      // No such word exists
      if (node === null) return 0;
      return node.count;
    }
  }], [{
    key: "findAllWords",
    value: function findAllWords(root, word, output) {
      if (root === null) return;
      if (root.count > 0) {
        if (_typeof(output) === 'object') {
          output.push({
            word: word,
            count: root.count
          });
        }
      }
      var key;
      for (key in root.children) {
        word += key;
        this.findAllWords(root.children[key], word, output);
        word = word.slice(0, -1);
      }
    }
  }]);
}();

var count = 0; 

var root = new Trie();
root.insert("aab");
root.insert("aba");
root.insert("a");
root.insert('abc');
root.insert("bac");
root.insert("bba");
root.insert("bab");
root.insert("bba");

count += root.contains("a");
count += root.contains("abc")

count += root.findOccurrences("bba") == 2;

root.remove("bba");

count += root.findOccurrences("bba") == 1; 
)");

  Interpreter interpreter;

  auto prog = parser.ParseProgram();
  ASSERT_TRUE(prog->IsProgram());

  auto comp = interpreter.Execute(prog);
  EXPECT_EQ(types::CompletionType::NORMAL, comp.GetType());
  ASSERT_TRUE(comp.GetValue()->IsInt());
  EXPECT_EQ(4, comp.GetValue()->GetInt());
}

TEST(Voidjs, ConvexHullGraham) {
  Parser parser(uR"(
/**
 * Author: Arnab Ray ConvexHull using Graham Scan Wikipedia:
 * https://en.wikipedia.org/wiki/Graham_scan Given a set of points in the
 * plane. The Convex hull of the set is the smallest convex polygon that
 * contains all the points of it.
 */

function compare(a, b) {
  // Compare Function to Sort the points, a and b are points to compare
  if (a.x < b.x) return -1;
  if (a.x === b.x && a.y < b.y) return -1;
  return 1;
}
function orientation(a, b, c) {
  // Check orientation of Line(a,b) and Line(b,c)
  var alpha = (b.y - a.y) / (b.x - a.x);
  var beta = (c.y - b.y) / (c.x - b.x);

  // Clockwise
  if (alpha > beta) return 1;
  // Anticlockwise
  else if (beta > alpha) return -1;
  // Colinear
  return 0;
}
function convexHull(points) {
  var pointsLen = points.length;
  if (pointsLen <= 2) {
    throw new Error('Minimum of 3 points is required to form closed polygon!');
  }
  points.sort(compare);
  var p1 = points[0];
  var p2 = points[pointsLen - 1];

  // Divide Hull in two halves
  var upperPoints = [];
  var lowerPoints = [];
  upperPoints.push(p1);
  lowerPoints.push(p1);
  for (var i = 1; i < pointsLen; i++) {
    if (i === pointsLen - 1 || orientation(p1, points[i], p2) !== -1) {
      var upLen = upperPoints.length;
      while (upLen >= 2 && orientation(upperPoints[upLen - 2], upperPoints[upLen - 1], points[i]) === -1) {
        upperPoints.pop();
        upLen = upperPoints.length;
      }
      upperPoints.push(points[i]);
    }
    if (i === pointsLen - 1 || orientation(p1, points[i], p2) !== 1) {
      var lowLen = lowerPoints.length;
      while (lowLen >= 2 && orientation(lowerPoints[lowLen - 2], lowerPoints[lowLen - 1], points[i]) === 1) {
        lowerPoints.pop();
        lowLen = lowerPoints.length;
      }
      lowerPoints.push(points[i]);
    }
  }
  var hull = [];
  for (var _i = 1; _i < upperPoints.length - 1; _i++) {
    hull.push(upperPoints[_i]);
  }
  for (var _i2 = lowerPoints.length - 1; _i2 >= 0; _i2--) {
    hull.push(lowerPoints[_i2]);
  }
  return hull;
}

var points = [
   { x: 0, y: 3 },
   { x: 1, y: 1 },
   { x: 2, y: 2 },
   { x: 4, y: 4 },
   { x: 0, y: 0 },
   { x: 1, y: 2 },
   { x: 3, y: 1 },
   { x: 3, y: 3 }]

var convex = convexHull(points);

var count = 0; 
count += convex[0].x == 0 && convex[0].y == 3;
count += convex[1].x == 4 && convex[1].y == 4;
count += convex[2].x == 3 && convex[2].y == 1;
count += convex[3].x == 0 && convex[3].y == 0;
)");

  Interpreter interpreter;

  auto prog = parser.ParseProgram();
  ASSERT_TRUE(prog->IsProgram());

  auto comp = interpreter.Execute(prog);
  EXPECT_EQ(types::CompletionType::NORMAL, comp.GetType());
  ASSERT_TRUE(comp.GetValue()->IsInt());
  EXPECT_EQ(4, comp.GetValue()->GetInt());
}

TEST(Voidjs, KMP) {
  Parser parser(uR"(
// Implementing KMP Search Algorithm to search all the instances of pattern in
// given text
// Reference Book: Introduction to Algorithms, CLRS

// Explanation: https://www.topcoder.com/community/competitive-programming/tutorials/introduction-to-string-searching-algorithms/

var computeLPS = function computeLPS(pattern) {
  var lps = Array(pattern.length);
  lps[0] = 0;
  for (var i = 1; i < pattern.length; i++) {
    var matched = lps[i - 1];
    while (matched > 0 && pattern[i] !== pattern[matched]) {
      matched = lps[matched - 1];
    }
    if (pattern[i] === pattern[matched]) {
      matched++;
    }
    lps[i] = matched;
  }
  return lps;
};

/**
 * Returns all indices where pattern starts in text
 * @param {*} text a big text in which pattern string is to find
 * @param {*} pattern the string to find
 */
var KMPSearch = function KMPSearch(text, pattern) {
  if (!pattern || !text) {
    return []; // no results
  }

  // lps[i] = length of proper prefix of pattern[0]...pattern[i-1]
  //          which is also proper suffix of it
  var lps = computeLPS(pattern);
  var result = [];
  var matched = 0;
  for (var i = 0; i < text.length; i++) {
    while (matched > 0 && text[i] !== pattern[matched]) {
      matched = lps[matched - 1];
    }
    if (text[i] === pattern[matched]) {
      matched++;
    }
    if (matched === pattern.length) {
      result.push(i - pattern.length + 1);
      matched = lps[matched - 1];
    }
  }
  return result;
};

var result = KMPSearch("aabaaab", "ab");

var count = 0;
count += result.length == 2; 
count += result[0] == 1;
count += result[1] == 5; 
)");

  Interpreter interpreter;

  auto prog = parser.ParseProgram();
  ASSERT_TRUE(prog->IsProgram());

  auto comp = interpreter.Execute(prog);
  EXPECT_EQ(types::CompletionType::NORMAL, comp.GetType());
  ASSERT_TRUE(comp.GetValue()->IsInt());
  EXPECT_EQ(3, comp.GetValue()->GetInt());
}
