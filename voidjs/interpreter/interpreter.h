#ifndef VOIDJS_INTERPRETER_INTERPRETER_H
#define VOIDJS_INTERPRETER_INTERPRETER_H

#include "voidjs/ir/ast.h"
#include "voidjs/ir/program.h"
#include "voidjs/ir/expression.h"
#include "voidjs/ir/statement.h"
#include "voidjs/ir/literal.h"
#include "voidjs/parser/parser.h"
#include "voidjs/types/js_value.h"
#include "voidjs/types/lang_types/null.h"
#include "voidjs/types/lang_types/boolean.h"
#include "voidjs/types/lang_types/number.h"
#include "voidjs/types/lang_types/string.h"

namespace voidjs {

class Interpreter {
 public:
  void Execute();

  types::Null* EvalNullLiteral(ast::AstNode*);
  types::Boolean* EvalBooleanLiteral(ast::AstNode*);
  types::Number* EvalNumericLiteral(ast::AstNode*);
  types::String* EvalStringLiteral(ast::AstNode*);
};

}  // namespace voidjs

#endif  // VOIDJS_INTERPRETER_INTERPRETER_H
