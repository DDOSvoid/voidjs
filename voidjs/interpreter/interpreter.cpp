#include <variant>

#include "voidjs/interpreter/interpreter.h"
#include "voidjs/ir/ast.h"
#include "voidjs/ir/expression.h"
#include "voidjs/ir/literal.h"
#include "voidjs/lexer/token_type.h"
#include "voidjs/types/js_value.h"
#include "voidjs/types/object_factory.h"
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
Completion Interpreter::EvalStatement(Statement* stmt) {
  switch (stmt->GetType()) {
    case ast::AstNodeType::EXPRESSION_STATEMENT: {
      return EvalExpressionStatement(stmt->AsExpressionStatement());
    }
    default: {
      return Completion();
    }
  }
}

// Eval BlockStatement
// Defined in ECMAScript 5.1 Chapter 12.1
Completion Interpreter::EvalBlockStatement(BlockStatement* block_stmt) {
  const auto& stmts = block_stmt->GetStatements();
  // Block: { }
  
  // 1. Return (normal, empty, empty)
  if (stmts.empty()) {
    return Completion(CompletionType::NORMAL);
  }

  // Block : { StatementList }

  // 1. Return the result of evaluating StatementList.
  return EvalStatementList(stmts);
}

// Eval ExpressionStatement
// Defined in ECMAScript 5.1 Chapter 12.4
Completion Interpreter::EvalExpressionStatement(ExpressionStatement* expr_stmt) {
  // 1. Let exprRef be the result of evaluating Expression.
  auto expr_ref = EvalExpression(expr_stmt->GetExpression());

  // 2. Return (normal, GetValue(exprRef), empty).
  return Completion(CompletionType::NORMAL, GetValue(expr_ref));
}

// Eval Expression
// Defined in ECMAScript 5.1 Chapter 11.14
std::variant<JSValue, Reference> Interpreter::EvalExpression(Expression* expr) {
  switch (expr->GetType()) {
    case AstNodeType::SEQUENCE_EXPRESSION: {
      return EvalSequenceExpression(expr->AsSequenceExpression());
    }
    case AstNodeType::ASSIGNMENT_EXPRESSION: {
      return EvalAssignmentExpression(expr->AsAssignmentExpression());
    }
    case AstNodeType::CONDITIONAL_EXPRESSION: {
      return EvalConditionalExpression(expr->AsConditionalExpression());
    }
    case AstNodeType::BINARY_EXPRESSION: {
      return EvalBinaryExpression(expr->AsBinaryExpression());
    }
    case AstNodeType::UNARY_EXPRESSION: {
      return EvalUnaryExpression(expr->AsUnaryExpression());
    }
    case AstNodeType::POSTFIX_EXPRESSION: {
      return EvalPostfixExpression(expr->AsPostfixExpression());
    }
    case AstNodeType::MEMBER_EXPRESSION: {
      return EvalMemberExpression(expr->AsMemberExpression());
    }
    case AstNodeType::NULL_LITERAL: {
      return EvalNullLiteral(expr->AsNullLiteral());
    }
    case AstNodeType::BOOLEAN_LITERAL: {
      return EvalBooleanLiteral(expr->AsBooleanLiteral());
    }
    case AstNodeType::NUMERIC_LITERAL: {
      return EvalNumericLiteral(expr->AsNumericLiteral());
    }
    case AstNodeType::STRING_LITERAL: {
      return EvalStringLiteral(expr->AsStringLiteral());
    }
    default: {
      return JSValue{};
    }
  }
}

// Eval SequenceExpression
// Defined in ECMAScript 5.1 Chapter 11.14
std::variant<JSValue, Reference> Interpreter::EvalSequenceExpression(SequenceExpression* seq_expr) {
  // Expression : Expression , AssignmentExpression
  
  // 1. Let lref be the result of evaluating Expression.
  // 2. Call GetValue(lref).
  // 3. Let rref be the result of evaluating AssignmentExpression.
  // 4. Return GetValue(rref).
  const auto& exprs = seq_expr->GetExpressions();
  JSValue val;
  for (auto expr : exprs) {
    val = GetValue(EvalExpression(expr));
  }
  return val;
}

// Eval AssignmentExpression
// Defined in ECMAScript 5.1 Chapter 11.13
// AssignmentExpression : LeftHandSideExpression = AssignmentExpression
std::variant<JSValue, Reference> Interpreter::EvalAssignmentExpression(AssignmentExpression* assign_expr) {
  if (assign_expr->GetOperator() == TokenType::ASSIGN) {
    // AssignmentExpression : LeftHandSideExpression = AssignmentExpression
      
    // 1. Let lref be the result of evaluating LeftHandSideExpression.
    auto lref = EvalExpression(assign_expr->GetLeft());

    // 2. Let rref be the result of evaluating AssignmentExpression.
    auto rref = EvalExpression(assign_expr->GetRight());

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
    auto lref = EvalExpression(assign_expr->GetLeft());

    // 2. Let lval be GetValue(lref).
    auto lval = GetValue(lref);

    // 3. Let rref be the result of evaluating AssignmentExpression.
    auto rref = EvalExpression(assign_expr->GetRight());

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
}

// Eval ConditionalExpression
// Defined in ECMAScript 5.1 Chapter 11.12
std::variant<JSValue, Reference> Interpreter::EvalConditionalExpression(ConditionalExpression* cond_expr) {
  // ConditionalExpression : LogicalORExpression ? AssignmentExpression : AssignmentExpression
  
  // 1. Let lref be the result of evaluating LogicalORExpression.
  auto lref = EvalExpression(cond_expr->GetConditional());

  // 2. If ToBoolean(GetValue(lref)) is true, then
  if (JSValue::ToBoolean(GetValue(lref)) == JSValue::True()) {
    // a. Let trueRef be the result of evaluating the first AssignmentExpression.
    auto true_ref = EvalExpression(cond_expr->GetConsequent());

    // b. Return GetValue(trueRef).
    return true_ref;
  } else {
    // a. Let falseRef be the result of evaluating the second AssignmentExpression.
    auto false_ref = EvalExpression(cond_expr->GetAlternate());

    // b. Return GetValue(falseRef).
    return false_ref;
  }
}

// Eval BinaryExpression
std::variant<JSValue, Reference> Interpreter::EvalBinaryExpression(BinaryExpression* binary_expr) {
  auto lval = GetValue(EvalExpression(binary_expr->GetLeft()));
  auto rval = GetValue(EvalExpression(binary_expr->GetRight()));
  return ApplyBinaryOperator(binary_expr->GetOperator(), lval, rval);
}

// Eval UnaryExpression
// Defined in ECMAScript 5.1 Chapter 11.4
std::variant<JSValue, Reference> Interpreter::EvalUnaryExpression(UnaryExpression* unary_expr) {
  auto val = GetValue(EvalExpression(unary_expr->GetExpression()));
  return ApplyUnaryOperator(unary_expr->GetOperator(), val);
}

// Eval PostfixExpression
// Defined in ECMAScript 5.1 Chapter 11.3
std::variant<JSValue, Reference> Interpreter::EvalPostfixExpression(PostfixExpression* post_expr) {
  
}

// Eval MemberExpression
// Defined in ECMAScript 5.1 Chapter 11.2
std::variant<JSValue, Reference> Interpreter::EvalMemberExpression(MemberExpression* mem_expr) {
  // MemberExpression : MemberExpression [ Expression ]
  // MemberExpression . IdentifierName
  
  // 1. Let baseReference be the result of evaluating MemberExpression.
  auto base_ref = EvalExpression(mem_expr->GetObject());

  // 2. Let baseValue be GetValue(baseReference).
  auto base_val = GetValue(base_ref);

  // 3. Let propertyNameReference be the result of evaluating Expression.
  auto prop_name_ref = EvalExpression(mem_expr->GetProperty());

  // 4. Let propertyNameValue be GetValue(propertyNameReference).
  auto prop_name_val = GetValue(prop_name_ref);

  // 5. Call CheckObjectCoercible(baseValue).
  base_val.CheckObjectCoercible();

  // 6. Let propertyNameString be ToString(propertyNameValue).
  auto prop_name_str = JSValue::ToString(prop_name_val); 

  // 7. If the syntactic production that is being evaluated is contained in strict mode code,
  //    let strict be true, else let strict be false.
  // todo
  bool strict = false;

  // 8. Return a value of type Reference
  //    whose base value is baseValue and whose referenced name is propertyNameString,
  //    and whose strict mode flag is strict.
  return Reference(base_val, prop_name_str.GetHeapObject()->AsString()->GetString(), strict);
}

// Eval NullLiteral
// Defined in ECMAScript 5.1 Chapter 11.1
JSValue Interpreter::EvalNullLiteral(NullLiteral* nul) {
  return JSValue::Null();
}

// Eval BooleanLiteral
// Defined in ECMAScript 5.1 Chapter 11.1
JSValue Interpreter::EvalBooleanLiteral(BooleanLiteral* boolean) {
  return JSValue(boolean->GetBoolean());
}

// Eval NumericLiteral
// Defined in ECMAScript 5.1 Chapter 11.1
JSValue Interpreter::EvalNumericLiteral(NumericLiteral* num) {
  if (num->IsInteger()) {
    return JSValue(num->GetNumber<std::int32_t>());
  } else {
    return JSValue(num->GetNumber<double>());
  }
}

// Eval StringLiteral
// Defined in ECMAScript 5.1 Chapter 11.1
JSValue Interpreter::EvalStringLiteral(StringLiteral* str) {
  return JSValue(ObjectFactory::NewString(str->GetString()));
}

// Apply Compound Assignment
// todo
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

// ApplyBinaryOperator
// todo
JSValue Interpreter::ApplyBinaryOperator(TokenType op, JSValue lval, JSValue rval) {
  switch (op) {
    case TokenType::ADD: {
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

// Eval StatementList
// Defined in ECMAScript 5.1 Chapter 12.1
Completion Interpreter::EvalStatementList(const Statements &stmts) {
  // StatementList :StatementList Statement
  
  // 1. Let sl be the result of evaluating StatementList.
  // 2. If sl is an abrupt completion, return sl.
  // 3. Let s be the result of evaluating Statement.
  // 4. If an exception was thrown, return (throw, V, empty) where V is the exception.
  //    (Execution now proceeds as if no exception were thrown.)
  // 5. If s.value is empty, let V = sl.value, otherwise let V = s.value.
  // 6. Return (s.type, V, s.target).
  Completion sl;
  
  for (auto stmt : stmts) {
    auto s = EvalStatement(stmt);
    
    if (s.GetType() == CompletionType::THROW) {
        return s;
    }

    sl = Completion(s.GetType(), s.GetValue().IsEmpty() ? sl.GetValue() : s.GetValue(), s.GetTarget());

    if (sl.IsAbruptCompletion()) {
      return sl;
    }
  }

  return sl;
}

// ApplyUnaryOperator
// todo
JSValue Interpreter::ApplyUnaryOperator(TokenType op, JSValue val) {
  return {};
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
