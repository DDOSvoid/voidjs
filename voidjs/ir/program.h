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
  
  void Append(Statement* stmt) {
    statements_.push_back(stmt);
  }
  
 private:
  Statements statements_;
};

}  // namespace ast
}  // namespace voidjs

#endif  // VOIDJS_IR_PROGRAM_H
