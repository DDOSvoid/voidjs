#ifndef VOIDJS_IR_PROGRAM_H
#define VOIDJS_IR_PROGRAM_H

#include "voidjs/ir/ast.h"

namespace voidjs {
namespace ast {

class Program : public AstNode {
 public:
  explicit Program(Statements statements)
    : AstNode(AstNodeType::PROGRAM), statements_(std::move(statements))
  {}

  bool IsProgram() const override { return true; }
  
 private:
  Statements statements_;
};

}  // namespace ast
}  // namespace voidjs

#endif  // VOIDJS_IR_PROGRAM_H
