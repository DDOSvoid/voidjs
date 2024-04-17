#include <variant>

#include "voidjs/interpreter/interpreter.h"
#include "voidjs/ir/ast.h"
#include "voidjs/lexer/token_type.h"
#include "voidjs/types/js_value.h"
#include "voidjs/types/spec_types/completion.h"
#include "voidjs/types/spec_types/reference.h"

namespace voidjs {

using namespace ast;
using namespace types;

void Interpreter::Execute() {
  
}

// Eval Program
// Defined in ECMAScript 5.1 Chapter 14
Completion Interpreter::EvalProgram(ast::AstNode *ast_node) {
  auto prog = ast_node->AsProgram();

  // 1. strict mode
  bool is_strict = prog->IsStrict();

  // 2. If SourceElements is not present, return (normal, empty, empty).
  if (prog->GetStatements().empty()) {
    return Completion(CompletionType::NORMAL);
  }

  // 3. Let progCxt be a new execution context for global code as described in 10.4.1.

  // 4. Let result be the result of evaluating SourceElements.
  Completion result;
  {
    // 1. Let headResult be the result of evaluating SourceElements.
    const auto& stmts = prog->GetStatements();
    auto stmt = stmts.front();
    Completion head_result;
    if (stmt->IsFunctionDeclaration()) {
      // head_result = EvalFunctionDeclaration(stmt);
    } else {
      head_result = EvalStatement(stmt);
    }
    
    // 2. If headResult is an abrupt completion, return headResult
    // 3. Let tailResult be result of evaluating SourceElement.
    // 4. If tailResult.value is empty, let V = headResult.value, otherwise let V = tailResult.value.
    Completion tail_result;
    for (std::size_t i = 1; i < stmts.size(); ++i) {
      if (head_result.IsAbruptCompletion()) {
        break;
      }
      if (auto stmt = stmts[i]; stmt->IsFunctionDeclaration()) {
        // tail_result = EvalFunctionDeclaration(stmt);
      } else {
        tail_result = EvalStatement(stmt);
      }
      head_result = Completion(
        tail_result.GetType(),
        tail_result.GetValue().IsEmpty() ? head_result.GetValue() : tail_result.GetValue(),
        tail_result.GetTarget());
    }

    // 5. Return (tailResult.type, V, tailResult.target)
    result = head_result;
  }

  // 5. Exit the execution context progCxt.

  // 6. Return result
  return result;
}

// Eval Statement
// Defined in ECMAScript 5.1 Chapter 12
Completion Interpreter::EvalStatement(ast::AstNode* ast_node) {
  switch (ast_node->GetType()) {
    case ast::AstNodeType::EXPRESSION_STATEMENT: {
      return EvalExpressionStatement(ast_node);
    }
    default: {
      return Completion();
    }
  }
}

// Eval ExpressionStatement
// Defined in ECMAScript 5.1 Chapter 12.4
Completion Interpreter::EvalExpressionStatement(AstNode* ast_node) {
  // 1. Let exprRef be the result of evaluating Expression.
  auto expr_ref = EvalExpression(ast_node);

  // 2. Return (normal, GetValue(exprRef), empty).
  return Completion(CompletionType::NORMAL, GetValue(expr_ref));
}

// Eval Expression
// Defined in ECMAScript 5.1 Chapter 11.14
std::variant<JSValue, Reference> Interpreter::EvalExpression(AstNode* ast_node) {
  if (ast_node->IsSequenceExpression()) {
    // 1. Let lref be the result of evaluating Expression.
    // 2. Call GetValue(lref).
    // 3. Let rref be the result of evaluating AssignmentExpression.
    // 4. Return GetValue(rref).
    const auto& exprs = ast_node->AsSequenceExpression()->GetExpressions();
    JSValue value;
    for (auto expr : exprs) {
      value = GetValue(EvalExpression(expr));
    }
    return value;
  } else {
    return EvalAssignmentExpression(ast_node);
  }
}

// Eval AssignmentExpression
// Defined in ECMAScript 5.1 Chapter 11.13
std::variant<JSValue, Reference> Interpreter::EvalAssignmentExpression(AstNode *ast_node) {
  if (ast_node->IsAssignmentExpression()) {
    auto assign_expr = ast_node->AsAssignmentExpression();

    if (assign_expr->GetOperator() == TokenType::ASSIGN) {
      // AssignmentExpression : LeftHandSideExpression = AssignmentExpression
      
      // 1. Let lref be the result of evaluating LeftHandSideExpression.
      auto lref = EvalLeftHandSideExpression(assign_expr->GetLeft());

      // 2. Let rref be the result of evaluating AssignmentExpression.
      auto rref = EvalAssignmentExpression(assign_expr->GetRight());

      // 3. Let rval be GetValue(rref).
      auto rval = GetValue(rref);

      // 4. Throw a SyntaxError exception if the following conditions are all true:
      //      Type(lref) is Reference is true
      //      IsStrictReference(lref) is true
      //      Type(GetBase(lref)) is Environment Record
      //      GetReferencedName(lref) is either "eval" or "arguments"
      if (auto plref = std::get_if<Reference>(&lref);
          plref                                                &&
          plref->IsStrictReference()                           &&
          std::get_if<EnvironmentRecord*>(&(plref->GetBase())) &&
          (plref->GetReferencedName() == u"eval"               ||
           plref->GetReferencedName() == u"arguments")) {
        
      }

      // 5. Call PutValue(lref, rval).
      PutValue(lref, rval);

      // 6. Return rval.
      return rval;
    } else {
      // AssignmentExpression : LeftHandSideExpression @= AssignmentExpression
      
      // 1. Let lref be the result of evaluating LeftHandSideExpression.
      auto lref = EvalLeftHandSideExpression(assign_expr->GetLeft());

      // 2. Let lval be GetValue(lref).
      auto lval = GetValue(lref);

      // 3. Let rref be the result of evaluating AssignmentExpression.
      auto rref = EvalAssignmentExpression(assign_expr->GetRight());

      // 4. Let rval be GetValue(rref).
      auto rval = GetValue(rref);

      // 5. Let r be the result of applying operator @ to lval and rval.
      JSValue r = ApplyCompoundAssignment(assign_expr->GetOperator(), lval, rval);

      // 4. Throw a SyntaxError exception if the following conditions are all true:
      //      Type(lref) is Reference is true
      //      IsStrictReference(lref) is true
      //      Type(GetBase(lref)) is Environment Record
      //      GetReferencedName(lref) is either "eval" or "arguments"
      if (auto plref = std::get_if<Reference>(&lref);
          plref                                                &&
          plref->IsStrictReference()                           &&
          std::get_if<EnvironmentRecord*>(&(plref->GetBase())) &&
          (plref->GetReferencedName() == u"eval"               ||
           plref->GetReferencedName() == u"arguments")) {
        
      }

      // 5. Call PutValue(lref, rval).
      PutValue(lref, rval);

      // 6. Return rval.
      return rval;
    }
  } else {
    return EvalConditionalExpression(ast_node);
  }
}

std::variant<JSValue, Reference> Interpreter::EvalConditionalExpression(AstNode *ast_node) {
  if (ast_node->IsConditionalExpression()) {
    // ConditionalExpression : LogicalORExpression ? AssignmentExpression : AssignmentExpression
    auto cond_expr = ast_node->AsConditionalExpression();

    // 1. Let lref be the result of evaluating LogicalORExpression.
    auto lref = EvalBinaryExpression(cond_expr->GetConditional());

    // 2. If ToBoolean(GetValue(lref)) is true, then
    if (true) {
      // a. Let trueRef be the result of evaluating the first AssignmentExpression.
      auto true_ref = EvalAssignmentExpression(cond_expr->GetConsequent());

      // b. Return GetValue(trueRef).
      return true_ref;
    } else {
      // a. Let falseRef be the result of evaluating the second AssignmentExpression.
      auto false_ref = EvalAssignmentExpression(cond_expr->GetAlternate());

      // b. Return GetValue(falseRef).
      return false_ref;
    }
  } else {
    return EvalBinaryExpression(ast_node);
  }
}

std::variant<JSValue, Reference> Interpreter::EvalBinaryExpression(AstNode *ast_node) {
  
}

std::variant<JSValue, Reference> Interpreter::EvalLeftHandSideExpression(AstNode *ast_node) {
  
}

std::variant<JSValue, Reference> Interpreter::EvalPrimaryExpression(AstNode* ast_node) {
  switch (ast_node->GetType()) {
    case ast::AstNodeType::THIS: {
      return JSValue();
    }
    case ast::AstNodeType::IDENTIFIER: {
      return JSValue();
    }
    case ast::AstNodeType::NULL_LITERAL: {
      return EvalNullLiteral(ast_node);
    }
    case ast::AstNodeType::BOOLEAN_LITERAL: {
      return EvalBooleanLiteral(ast_node);
    }
    case ast::AstNodeType::NUMERIC_LITERAL: {
      return EvalNumericLiteral(ast_node); 
    }
    case ast::AstNodeType::STRING_LITERAL: {
      return EvalStringLiteral(ast_node);
    }
    case ast::AstNodeType::ARRAY_LITERAL: {
      return JSValue();
    }
    case ast::AstNodeType::OBJECT_LITERAL: {
      return JSValue();
    }
    default: {
      return EvalExpression(ast_node);
    }
  }
}

JSValue Interpreter::EvalNullLiteral(AstNode* ast_node) {
  return JSValue::Null();
}

JSValue Interpreter::EvalBooleanLiteral(AstNode* ast_node) {
  return JSValue(ast_node->AsBooleanLiteral()->GetBoolean());
}

JSValue Interpreter::EvalNumericLiteral(AstNode* ast_node) {
  auto literal = ast_node->AsNumericLiteral();
  if (literal->IsInteger()) {
    return JSValue(literal->GetNumber<std::int32_t>());
  } else {
    return JSValue(literal->GetNumber<double>());
  }
}

JSValue Interpreter::EvalStringLiteral(AstNode* ast_node) {
  auto str = String::New(ast_node->AsStringLiteral()->GetString());
  return JSValue(str);
}

// Apply Compound Assignment
JSValue Interpreter::ApplyCompoundAssignment(TokenType op, JSValue lval, JSValue rval) {
  switch (op) {
    case TokenType::ADD_ASSIGN: {
      if (lval.IsInt() && rval.IsInt()) {
        return JSValue(lval.GetInt() + rval.GetInt());
      } else {
        return {};
      }
    }
    default: {
      return {};
    }
  }
}

// GetValue(V)
// Defined in ECMAScript 5.1 Chapter 8.7.1
JSValue Interpreter::GetValue(const std::variant<JSValue, Reference>& V) {
  // 1. If Type(V) is not Reference, return V.
  if (auto ptr = std::get_if<JSValue>(&V)) {
    return *ptr;
  }
  
  // 2. Let base be the result of calling GetBase(V).
  auto base = std::get_if<Reference>(&V)->GetBase();

  // 3. If IsUnresolvableReference(V), throw a ReferenceError exception.
  if (std::get_if<Reference>(&V)->IsUnresolvableReference()) {
    
  }

  // 4. If IsPropertyReference(V), then
  if (std::get_if<Reference>(&V)->IsPropertyReference()) {
    // a. If HasPrimitiveBase(V) is false,
    //    then let get be the [[Get]] internal method of base,
    //    otherwise let get be the special [[Get]] internal method defined below.

    // b. Return the result of calling the get internal method
    //    using base as its this value, and passing GetReferencedName(V) for the argument.

  }
  // 5. Else, base must be an environment record.
  else { 
    // a. Return the result of calling the GetBindingValue concrete method of
    //    base passing GetReferencedName(V) and IsStrictReference(V) as arguments.
    
  }
}

void Interpreter::PutValue(const std::variant<JSValue, Reference>& V, JSValue W) {
  // 1. If Type(V) is not Reference, throw a ReferenceError exception.
  if (std::get_if<Reference>(&V)) {
    
  }

  auto ref = std::get_if<Reference>(&V);

  // 2. Let base be the result of calling GetBase(V).
  auto base = ref->GetBase();

  // 3. If IsUnresolvableReference(V), then
  if (ref->IsUnresolvableReference()) {
    // a. If IsStrictReference(V) is true, then
    if (ref->IsStrictReference()) {
      // i. Throw ReferenceError exception.
      
    }
    // b. Call the [[Put]] internal method of the global object,
    //    passing GetReferencedName(V) for the property name,
    //    W for the value, and false for the Throw flag.
    else {
      
    }
  }
  // 4. Else if IsPropertyReference(V), then
  else if (ref->IsPropertyReference()) {
    // a. If HasPrimitiveBase(V) is false,
    // then let put be the [[Put]] internal method of base,
    // otherwise let put be the special [[Put]] internal method defined below.
    if (ref->HasPrimitiveBase()) {
      
    } else {
    }

    // b. Call the put internal method using base as its this value,
    // and passing GetReferencedName(V) for the property name,
    // W for the value, and IsStrictReference(V) for the Throw flag.
  }
  // 5. Else base must be a reference whose base is an environment record. So,
  else {
    // a. Call the SetMutableBinding (10.2.1) concrete method of base,
    //    passing GetReferencedName(V), W, and IsStrictReference(V) as arguments.
  }

  // 6. Return
}

}  // namespace voidjs
