#include "voidjs/ir/ast.h"

namespace voidjs {
namespace ast {

bool AstNode::IsStatement() const {
  switch (type_) {
    case AstNodeType::STATEMENT:
    case AstNodeType::BLOCK_STATEMENT:
    case AstNodeType::VARIABLE_STATEMENT:
    case AstNodeType::EMPTY_STATEMENT:
    case AstNodeType::EXPRESSION_STATEMENT:
    case AstNodeType::IF_STATEMENT:
    case AstNodeType::DO_WHILE_STATEMENT:
    case AstNodeType::WHILE_STATEMENT:
    case AstNodeType::FOR_STATEMENT:
    case AstNodeType::FOR_IN_STATEMENT:
    case AstNodeType::CONTINUE_STATEMENT:
    case AstNodeType::BREAK_STATEMENT:
    case AstNodeType::RETURN_STATEMENT: {
      return true;
    }
    default: {
      return false;
    }
  }
}

bool AstNode::IsExpression() const {
  switch (type_) {
    case AstNodeType::NEW_EXPRESSION:
    case AstNodeType::CALL_EXPRESSION:
    case AstNodeType::MEMBER_EXPRESSION:
    case AstNodeType::POSTFIX_EXPRESSION:
    case AstNodeType::UNARY_EXPRESSION:
    case AstNodeType::BINARY_EXPRESSION:
    case AstNodeType::CONDITIONAL_EXPRESSION:
    case AstNodeType::ASSIGNMENT_EXPRESSION:
    case AstNodeType::SEQUENCE_EXPRESSION: {
      return true;
    }
    default: {
      return false;
    }
  }
}

bool AstNode::IsLeftHandSideExpression() const {
  switch (type_) {
    case AstNodeType::NEW_EXPRESSION:
    case AstNodeType::CALL_EXPRESSION:
    case AstNodeType::MEMBER_EXPRESSION:
    case AstNodeType::IDENTIFIER: {
      return true;
    }
    default: {
      return false;
    }
  }
}

}  // namespace ast
}  // namespace voidjs
