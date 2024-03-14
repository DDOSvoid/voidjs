#ifndef VOIDJS_IR_EXPRESSION_H
#define VOIDJS_IR_EXPRESSION_H

#include "voidjs/ir/ast.h"

namespace voidjs {
namespace ast {

// Expression
// Defined in ECMAScript 5.1 Chapter 11
// Expression in ES 5.1 can be broadly categorized as follows: 
// Primary Expression
// Left-Hand-Side Expression
// Postfix Expression
// Unary Expression
// Multiplicative Expression --> member new call
// Additive Expression
// Shift Expression
// Relational Expression
// Equality Expression
// Bitwise Expression --> and or xor
// Conditional Expression
// Assignment Expression
class Expression : public AstNode {
};


}  // namespace voidjs
}  // namespace ast

#endif  // VOIDJS_IR_EXPRESSION_H
