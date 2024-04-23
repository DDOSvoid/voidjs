#ifndef VOIDJS_IR_PROGRAM_H
#define VOIDJS_IR_PROGRAM_H

#include "voidjs/ir/ast.h"

namespace voidjs {
namespace ast {

class VariableDeclaration;

class Program : public AstNode {
 public:
  explicit Program(Statements statements, bool is_strict,
                   VariableDeclarations var_decls,
                   AstNodes func_decls)
    : AstNode(AstNodeType::PROGRAM),
      statements_(std::move(statements)), is_strict_(is_strict),
      variable_declarations_(std::move(var_decls)),
      function_declarations_(std::move(func_decls))
  {}

  const Statements& GetStatements() const { return statements_; }
  const VariableDeclarations& GetVariableDeclarations() const { return variable_declarations_; }
  const AstNodes& GetFunctionDeclarations() const { return function_declarations_; }
  
  bool IsStrict() const { return is_strict_; }
  
 private:
  Statements statements_;
  bool is_strict_;
  
  VariableDeclarations variable_declarations_;
  AstNodes function_declarations_;
};

}  // namespace ast
}  // namespace voidjs

#endif  // VOIDJS_IR_PROGRAM_H
