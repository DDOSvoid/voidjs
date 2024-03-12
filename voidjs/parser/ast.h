#ifndef VOIDJS_AST_H
#define VOIDJS_AST_H

#include <vector>

namespace voidjs {
namespace ast {

class AstNode;
class Program;
class Statement;
class Expression;

class AstNode {
};

class Program : public AstNode {
 public:
  void Append(Statement* stmt) {
    statements_.push_back(stmt);
  }

  std::vector<Statement*>& Statements() {
    return statements_;
  }
  
 private:
  std::vector<Statement*> statements_;
};

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


// Statement
// Defined in ECMAScript 5.1 Chapter 12
// Statement ::
//   Block                       --> why not BlockStatement ???
//   VariableStatement
//   EmptyStatement
//   ExpressionStatement
//   IfStatement
//   IterationStatement
//   ContinueStatement
//   BreakStatement
//   ReturnStatement
//   WithStatement
//   LabelledStatement
//   SwitchStatement
//   ThrowStatement
//   TryStatement
//   DebuggerStatement
class Statement : public AstNode {
};

class BlockStatement : public Statement {
 public:
  void Append(Statement* stmt) {
    statements_.push_back(stmt);
  }

  std::vector<Statement*>& Statements() {
    return statements_;
  }
 private:
  std::vector<Statement*> statements_;
};


}  // namespace ast
}  // namespace voidjs

#endif  // VOIDJS_AST_H
