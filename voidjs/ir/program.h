#ifndef VOIDJS_IR_PROGRAM_H
#define VOIDJS_IR_PROGRAM_H

#include "voidjs/ir/ast.h"

namespace voidjs {
namespace ast {

class Program : public AstNode {
 public:
  explicit Program(Statements statements, bool is_strict)
    : AstNode(AstNodeType::PROGRAM),
      statements_(std::move(statements)), is_strict_(is_strict)
  {}

  const Statements& GetStatements() const { return statements_; }
  
  bool IsStrict() const { return is_strict_; }
  
 private:
  Statements statements_;
  bool is_strict_;
};

}  // namespace ast
}  // namespace voidjs

#endif  // VOIDJS_IR_PROGRAM_H
