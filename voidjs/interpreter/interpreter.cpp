#include "voidjs/interpreter/interpreter.h"

#include <variant>
#include <iostream>
#include <functional>

#include "voidjs/ir/ast.h"
#include "voidjs/ir/expression.h"
#include "voidjs/ir/literal.h"
#include "voidjs/ir/statement.h"
#include "voidjs/lexer/token_type.h"
#include "voidjs/types/heap_object.h"
#include "voidjs/types/js_value.h"
#include "voidjs/types/object_factory.h"
#include "voidjs/types/lang_types/object.h"
#include "voidjs/types/spec_types/completion.h"
#include "voidjs/types/spec_types/reference.h"
#include "voidjs/types/spec_types/lexical_environment.h"
#include "voidjs/types/spec_types/environment_record.h"
#include "voidjs/interpreter/execution_context.h"

namespace voidjs {

using namespace ast;
using namespace types;

Completion Interpreter::Execute(AstNode* ast_node) {
  auto global_object = ObjectFactory::NewGlobalObject();
  auto global_env = LexicalEnvironment::NewObjectEnvironmentRecord(JSValue(global_object), nullptr);
  vm_ = new VM(global_object, global_env);
  EnterGlobalCode(ast_node);
  return EvalProgram(ast_node);
}

void Interpreter::EnterGlobalCode(AstNode* ast_node) {
  // 1. Initialize the execution context using the global code as described in 10.4.1.1.
  auto global_ctx = new ExecutionContext(vm_->GetGlobalEnv(), vm_->GetGlobalEnv(), vm_->GetGlobalObject());
  vm_->PushExecutionContext(global_ctx);

  // 2. Perform Declaration Binding Instantiation as described in 10.5 using the global code.
  DeclarationBindingInstantiation(ast_node);
}

void Interpreter::DeclarationBindingInstantiation(AstNode* ast_node) {
  // 1. Let env be the environment record component of the running execution context’s VariableEnvironment.
  auto env = vm_->GetExecutionContext()->GetVariableEnvironment()->GetEnvRec();

  // 2. If code is eval code, then let configurableBindings be true else let configurableBindings be false.
  // todo
  bool configurable_bindings = false;

  // 3. If code is strict mode code, then let strict be true else let strict be false.
  // todo
  auto strict = false;

  // 4. If code is function code, then
  // todo

  // 5. For each FunctionDeclaration f in code, in source text order do
  const auto& func_decls = std::invoke([](AstNode* ast_node) -> const FunctionDeclarations& {
    if (ast_node->IsProgram()) {
      return ast_node->AsProgram()->GetFunctionDeclarations();
    } else if ( ast_node->IsFunctionDeclaration()) {
      return ast_node->AsFunctionDeclaration()->GetFunctionDeclarations();
    } else {
      // ast_node must be FunctionExpression
      return ast_node->AsFunctionExpression()->GetFunctionDeclarations();
    }
  }, ast_node);
  
  for (auto func : func_decls) {
    // a. Let fn be the Identifier in FunctionDeclaration f.
    auto fn = func->GetName();

    // b. Let fo be the result of instantiating FunctionDeclaration f as described in Clause 13.
    // todo
  }

  // 6. Let argumentsAlreadyDeclared be the result of
  //    calling env’s HasBinding concrete method passing "arguments" as the argument
  // auto args_already_declared_ = env->HasBinding(ObjectFactory::NewString(u"arguments"));

  // 7. If code is function code and argumentsAlreadyDeclared is false, then
  // todo

  // 8. For each VariableDeclaration and VariableDeclarationNoIn d in code, in source text order do
  const auto& var_decls = std::invoke([](AstNode* ast_node) -> const VariableDeclarations& {
    if (ast_node->IsProgram()) {
      return ast_node->AsProgram()->GetVariableDeclarations();
    } else if ( ast_node->IsFunctionDeclaration()) {
      return ast_node->AsFunctionDeclaration()->GetVariableDeclarations();
    } else {
      // ast_node must be FunctionExpression
      return ast_node->AsFunctionExpression()->GetVariableDeclarations();
    }
  }, ast_node);

  for (auto var_decl : var_decls) {
    // a. Let dn be the Identifier in d.
    auto dn = var_decl->GetIdentifier()->AsIdentifier();
    auto dn_str = ObjectFactory::NewString(dn->GetName());

    // b. Let varAlreadyDeclared be the result of calling env’s HasBinding concrete method passing dn as the argument.
    auto var_already_declared = env->HasBinding(dn_str);

    // c. If varAlreadyDeclared is false, then
    if (!var_already_declared) {
      // i. Call env’s CreateMutableBinding concrete method
      //    passing dn and configurableBindings as the arguments.
      env->CreateMutableBinding(dn_str, configurable_bindings);

      // ii. Call env’s SetMutableBinding concrete method passing dn, undefined, and strict as the arguments.
      env->SetMutableBinding(dn_str, JSValue::Undefined(), strict);
    }
  }
}
  
// Eval Program
// Defined in ECMAScript 5.1 Chapter 14
Completion Interpreter::EvalProgram(AstNode *ast_node) {
  auto prog = ast_node->AsProgram();

  // 1. The code of this Program is strict mode code
  //    if the Directive Prologue (14.1) of its SourceElements contains a Use Strict Directive or
  //    if any of the conditions of 10.1.1 apply.
  //    If the code of this Program is strict mode code,
  //    SourceElements is evaluated in the following steps as strict mode code.
  //    Otherwise SourceElements is evaluated in the following steps as non-strict mode code.
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
    case AstNodeType::BLOCK_STATEMENT: {
      return EvalBlockStatement(stmt->AsBlockStatement());
    }
    case AstNodeType::EXPRESSION_STATEMENT: {
      return EvalExpressionStatement(stmt->AsExpressionStatement());
    }
    case AstNodeType::VARIABLE_STATEMENT: {
      return EvalVariableStatement(stmt->AsVariableStatement());
    }
    case AstNodeType::EMPTY_STATEMENT: {
      return EvalEmptyStatement(stmt->AsEmptyStatement());
    }
    case AstNodeType::IF_STATEMENT: {
      return EvalIfStatement(stmt->AsIfStatement());
    }
    case AstNodeType::DO_WHILE_STATEMENT: {
      return EvalDoWhileStatement(stmt->AsDoWhileStatement());
    }
    case AstNodeType::WHILE_STATEMENT: {
      return EvalWhileStatement(stmt->AsWhileStatement());
    }
    case AstNodeType::FOR_STATEMENT: {
      return EvalForStatement(stmt->AsForStatement());
    }
    case AstNodeType::FOR_IN_STATEMENT: {
      return EvalForInStatement(stmt->AsForInStatement());
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

// Eval VariableStatement
// Defined in ECMAScript 5.1 Chapter 12.2
Completion Interpreter::EvalVariableStatement(VariableStatement *var_stmt) {
  // 1. Evaluate VariableDeclarationList.
  // 2. Return (normal, empty, empty).
  EvalVariableDeclarationList(var_stmt->GetVariableDeclarations());
  return Completion(CompletionType::NORMAL);
}

// Eval EmptyStatement
// Defined in ECMAScript 5.1 Chapter 12.3
Completion Interpreter::EvalEmptyStatement(EmptyStatement *empty_stmt) {
  // 1. Return (normal, empty, empty).
  return Completion(CompletionType::NORMAL);
}

// Eval ExpressionStatement
// Defined in ECMAScript 5.1 Chapter 12.4
Completion Interpreter::EvalExpressionStatement(ExpressionStatement* expr_stmt) {
  // 1. Let exprRef be the result of evaluating Expression.
  auto expr_ref = EvalExpression(expr_stmt->GetExpression());

  // 2. Return (normal, GetValue(exprRef), empty).
  return Completion(CompletionType::NORMAL, GetValue(expr_ref));
}

// Eval IfStatement
// Defined in ECMAScript 5.1 Chapter 12.5
Completion Interpreter::EvalIfStatement(ast::IfStatement* if_stmt) {
  // IfStatement : if ( Expression ) Statement else Statement
  
  // 1. Let exprRef be the result of evaluating Expression.
  auto expr_ref = EvalExpression(if_stmt->GetCondition());

  // 2. If ToBoolean(GetValue(exprRef)) is true, then
  if (JSValue::ToBoolean(GetValue(expr_ref))) {
    // a. Return the result of evaluating the first Statement.
    return EvalStatement(if_stmt->GetConsequent());
  }
  // 3. Else
  else {
    // a. Return the result of evaluating the second Statement.
    if (if_stmt->GetAlternate()) {
      return EvalStatement(if_stmt->GetAlternate());
    } else {
      return Completion(CompletionType::NORMAL);
    }
  }
}

// Eval DoWhileStatement
// Defined in ECMAScript 5.1 Chapter 12.6.1
Completion Interpreter::EvalDoWhileStatement(DoWhileStatement* do_while_stmt) {
  // do Statement while ( Expression );
  
  // 1. Let V = empty.
  JSValue V;

  // 2. Let iterating be true.
  bool iterating = true;

  // 3. Repeat, while iterating is true
  while (iterating) {
    // a. Let stmt be the result of evaluating Statement.
    auto stmt = EvalStatement(do_while_stmt->GetBody());

    // b. If stmt.value is not empty, let V = stmt.value.
    if (!stmt.GetValue().IsEmpty()) {
      V = stmt.GetValue();
    }

    // c. If stmt.type is not continue ||
    //    stmt.target is not in the current label set, then
    if (stmt.GetType() != CompletionType::CONTINUE ||
        !vm_->GetExecutionContext()->HasLabel(stmt.GetTarget())) {
      // i. If stmt.type is break and stmt.target is in the current label set,
      //    return (normal, V, empty).
      if (stmt.GetType() == CompletionType::BREAK &&
          vm_->GetExecutionContext()->HasLabel(stmt.GetTarget())) {
        return Completion(CompletionType::NORMAL, V);
      }

      // ii. If stmt is an abrupt completion, return stmt.
      if (stmt.IsAbruptCompletion()) {
        return stmt;
      }
    }

    // d. Let exprRef be the result of evaluating Expression.
    auto expr_ref = EvalExpression(do_while_stmt->GetCondition());

    // e. If ToBoolean(GetValue(exprRef)) is false, set iterating to false.
    if (!JSValue::ToBoolean(GetValue(expr_ref))) {
      iterating = false;
    }
  }

  // 4. Return (normal, V, empty);
  return Completion(CompletionType::NORMAL, V);
}

// Eval WhileStatement
// Defined in ECMAScript 5.1 Chapter 12.7
Completion Interpreter::EvalWhileStatement(WhileStatement* while_stmt) {
  // IterationStatement : while ( Expression ) Statement
  
  // 1. Let V = empty.
  JSValue V;

  // 2. Repeat
  while (true) {
    // a. Let exprRef be the result of evaluating Expression.
    auto expr_ref = EvalExpression(while_stmt->GetCondition());

    // b. If ToBoolean(GetValue(exprRef)) is false, return (normal, V, empty).
    if (!JSValue::ToBoolean(GetValue(expr_ref))) {
      return Completion(CompletionType::NORMAL, V);
    }

    // c. Let stmt be the result of evaluating Statement.
    auto stmt = EvalStatement(while_stmt->GetBody());

    // d. If stmt.value is not empty, let V = stmt.value.
    if (!stmt.GetValue().IsEmpty()) {
      V = stmt.GetValue();
    }

    // e. If stmt.type is not continue ||
    //    stmt.target is not in the current label set, then
    if (stmt.GetType() != CompletionType::CONTINUE || 
        !vm_->GetExecutionContext()->HasLabel(stmt.GetTarget())) {
      // i. If stmt.type is break and stmt.target is in the current label set, then
      if (stmt.GetType() == CompletionType::BREAK &&
          vm_->GetExecutionContext()->HasLabel(stmt.GetTarget())) {
        // 1. Return (normal, V, empty).
        return Completion(CompletionType::NORMAL, V);
      }

      // ii. If stmt is an abrupt completion, return stmt.
      if (stmt.IsAbruptCompletion()) {
        return stmt;
      }
    }
  }
}

// Eval ForStatement
// Defind in ECMAScript 5.1 Chapter 12.6
Completion Interpreter::EvalForStatement(ForStatement *for_stmt) {
  // IterationStatement : for ( ExpressionNoInopt ; Expressionopt ; Expressionopt) Statement
  // IterationStatement : for ( var VariableDeclarationListNoIn ; Expressionopt ; Expressionopt ) Statement

  // 1. Evaluate VariableDeclarationListNoIn.
  if (auto init = for_stmt->GetInitializer()) {
    if (init->IsVariableStatement()) {
      EvalVariableStatement(init->AsVariableStatement());
    } else {
      EvalExpression(init->AsExpression());
    }
  }

  // 2. Let V = empty.
  JSValue V;

  // 3. Repeat
  while (true) {
    // a. If the first Expression is present, then
    if (for_stmt->GetCondition()) {
      // i. Let testExprRef be the result of evaluating the first Expression.
      auto test_expr_ref = EvalExpression(for_stmt->GetCondition());

      // ii. If ToBoolean(GetValue(testExprRef)) is false, return (normal, V, empty).
      if (!JSValue::ToBoolean(GetValue(test_expr_ref))) {
        return Completion(CompletionType::NORMAL, V);
      }
    }

    // b. Let stmt be the result of evaluating Statement.
    auto stmt = EvalStatement(for_stmt->GetBody());

    // c. If stmt.value is not empty, let V = stmt.value
    if (!stmt.GetValue().IsEmpty()) {
      V = stmt.GetValue();
    }

    // d. If stmt.type is break and stmt.target is in the current label set,
    //    return (normal, V, empty).
    if (stmt.GetType() == CompletionType::BREAK &&
        vm_->GetExecutionContext()->HasLabel(stmt.GetTarget())) {
      return Completion(CompletionType::NORMAL, V);
    }

    // e. If stmt.type is not continue ||
    //    stmt.target is not in the current label set, then
    if (stmt.GetType() != CompletionType::CONTINUE || 
        !vm_->GetExecutionContext()->HasLabel(stmt.GetTarget())) {
      // 1. If stmt is an abrupt completion, return stmt.
      if (stmt.IsAbruptCompletion()) {
        return stmt;
      }
    }

    // f. If the second Expression is present, then
    if (for_stmt->GetUpdate()) {
      // 1. Let incExprRef be the result of evaluating the second Expression.
      auto inc_expr_ref = EvalExpression(for_stmt->GetUpdate());

      // 2. Call GetValue(incExprRef). (This value is not used.)
      GetValue(inc_expr_ref);
    }
  }
}

// Eval ForInStatement
// Defined in ECMASCript 5.1 Chapter 12.6.4
Completion Interpreter::EvalForInStatement(ForInStatement *for_in_stmt) {
  // IterationStatement : for ( LeftHandSideExpression in Expression ) Statement
  // IterationStatement : for ( var VariableDeclarationNoIn in Expression ) Statement

  // 1. Let varName be the result of evaluating VariableDeclarationNoIn.
  JSValue var_name;
  if (for_in_stmt->GetLeft()->IsVariableDeclaraion()) {
    var_name = EvalVariableDeclaration(for_in_stmt->GetLeft()->AsVariableDeclaration());
  }

  // 2. Let exprRef be the result of evaluating the Expression.
  auto expr_ref = EvalExpression(for_in_stmt->GetRight());

  // 3. Let experValue be GetValue(exprRef).
  auto expr_val = GetValue(expr_ref);

  // 4. If experValue is null or undefined, return (normal, empty, empty).
  if (expr_val.IsNull() || expr_val.IsUndefined()) {
    return Completion(CompletionType::NORMAL);
  }

  // 5. Let obj be ToObject(experValue).
  auto obj = JSValue::ToObject(expr_val);

  // 6. Let V = empty.
  JSValue V;

  // 7. Repeat
  while (true) {
    // a. Let P be the name of the next property of obj whose [[Enumerable]] attribute is true.
    //    If there is no such property, return (normal, V, empty).
    // todo
  }
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
    case AstNodeType::IDENTIFIER: {
      return EvalIdentifier(expr->AsIdentifier());
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
        plref                                                   &&
        plref->IsStrictReference()                              &&
        std::get_if<EnvironmentRecord*>(&(plref->GetBase()))    &&
        (plref->GetReferencedName()->Equal(u"eval")     ||
         plref->GetReferencedName()->Equal(u"arguments"))) {
      // todo
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
        plref                                                   &&
        plref->IsStrictReference()                              &&
        std::get_if<EnvironmentRecord*>(&(plref->GetBase()))    &&
        (plref->GetReferencedName()->Equal(u"eval")     ||
         plref->GetReferencedName()->Equal(u"arguments"))) {
      // todo
    }

    // 5. Call PutValue(lref, r).
    PutValue(lref, r);

    // 6. Return r.
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
  if (JSValue::ToBoolean(GetValue(lref))) {
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
  return Reference(base_val, prop_name_str.GetHeapObject()->AsString(), strict);
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
  if (utils::IsDoubleWithinRangeInt32(num->GetDouble())) {
    return JSValue(num->GetInt32());
  } else {
    return JSValue(num->GetDouble());
  }
}

// Eval StringLiteral
// Defined in ECMAScript 5.1 Chapter 11.1
JSValue Interpreter::EvalStringLiteral(StringLiteral* str) {
  return JSValue(ObjectFactory::NewString(str->GetString()));
}

// Eval Identifier
// Defined in ECMAScript 5.1 Chapter 11.1.2
Reference Interpreter::EvalIdentifier(Identifier* ident) {
  return IdentifierResolution(ObjectFactory::NewString(ident->GetName()));
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

// Eval VariableDeclarationList
// Defined in ECMAScript 5.1 Chapter 12.2
void Interpreter::EvalVariableDeclarationList(const VariableDeclarations& decls) {
  for (auto decl : decls) {
    EvalVariableDeclaration(decl);
  }
}

// Eval VariableDeclaration
// Defined in ECMASCript 5.1 Chapter 12.2
JSValue Interpreter::EvalVariableDeclaration(VariableDeclaration* decl) {
  // 1. Let lhs be the result of evaluating Identifier as described in 11.1.2.
  auto lhs = EvalExpression(decl->GetIdentifier());

  // 2. Let rhs be the result of evaluating Initialiser.
  if (!decl->GetInitializer()) {
    return JSValue(ObjectFactory::NewString(decl->GetIdentifier()->AsIdentifier()->GetName()));
  }
  auto rhs = EvalExpression(decl->GetInitializer());

  // 3. Let value be GetValue(rhs).
  auto value = GetValue(rhs);

  // 4. Call PutValue(lhs, value).
  PutValue(lhs, value);

  // 5. Return a String value containing the same sequence of characters as in the Identifier.
  return JSValue(ObjectFactory::NewString(decl->GetIdentifier()->AsIdentifier()->GetName()));
}

// ApplyCompoundAssignment
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


// ApplyLogicalOperator
// Defind in ECMAScript 5.1 Chatper 11.11
// Note that the value produced by a && or || operator is not necessarily of type Boolean.
// The value produced will always be the value of one of the two operand expressions.
JSValue Interpreter::ApplyLogicalOperator(TokenType op, Expression* left, Expression* right) {
  if (op == TokenType::LOGICAL_AND) {
    // 1. Let lref be the result of evaluating LogicalANDExpression.
    auto lref = EvalExpression(left);
    
    // 2. Let lval be GetValue(lref).
    auto lval = GetValue(lref);
    
    // 3. If ToBoolean(lval) is false, return lval.
    if (!JSValue::ToBoolean(lval)) {
      return lval;
    }
    
    // 4. Let rref be the result of evaluating BitwiseORExpression.
    auto rref = EvalExpression(right);
    
    // 5. Return GetValue(rref).
    return GetValue(rref);
  } else {
    // op must be TokenType::Logical_OR

    // 1. Let lref be the result of evaluating LogicalORExpression.
    auto lref = EvalExpression(left);
    
    // 2. Let lval be GetValue(lref).
    auto lval = GetValue(lref);
    
    // 3. If ToBoolean(lval) is true, return lval.
    if (JSValue::ToBoolean(lval)) {
      return lval;
    }
    
    // 4. Let rref be the result of evaluating LogicalANDExpression.
    auto rref = EvalExpression(right);
    
    // 5. Return GetValue(rref).
    return GetValue(rref);
  }
}

// ApplyBitwiseOperator
// Defined in ECMAScript 5.1 Chapter 11.10
JSValue Interpreter::ApplyBitwiseOperator(TokenType op, Expression* left, Expression* right) {
  // 1. Let lref be the result of evaluating A.
  auto lref = EvalExpression(left);
  
  // 2. Let lval be GetValue(lref).
  auto lval = GetValue(lref);
  
  // 3. Let rref be the result of evaluating B.
  auto rref = EvalExpression(right);
  
  // 4. Let rval be GetValue(rref).
  auto rval = GetValue(rref);
  
  // 5. Let lnum be ToInt32(lval).
  auto lnum = JSValue::ToInt32(lval);
  
  // 6. Let rnum be ToInt32(rval).
  auto rnum = JSValue::ToInt32(rval);
  
  // 7. Return the result of applying the bitwise operator @ to lnum and rnum.
  //    The result is a signed 32 bit integer.
  if (op == TokenType::BIT_AND) {
    return JSValue(lnum & rnum);
  } else if (op == TokenType::BIT_XOR) {
    return JSValue(lnum ^ rnum);
  } else {
    // op must bt TokenType::BIT_OR
    return JSValue(lnum | rnum);
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
    case TokenType::SUB: {
      if (lval.IsInt() && rval.IsInt()) {
        return JSValue(lval.GetInt() - rval.GetInt());
      } else {
        return {};
      }
    }
    case TokenType::LESS_THAN: {
      if (lval.IsInt() && rval.IsInt()) {
        return JSValue(lval.GetInt() < rval.GetInt());
      } else {
        return {};
      }
    }
    default: {
      return {};
    }
  }
}

// ApplyUnaryOperator
// todo
JSValue Interpreter::ApplyUnaryOperator(TokenType op, JSValue val) {
  switch (op) {
    case TokenType::INC: {
      if (val.IsInt()) {
        return JSValue(val.GetInt() + 1);
      } else {
        return {};
      }
    }
    default: {
      return {};
    }
  }
}

// Identifier Resolution
// Defined in ECMAScript 5.1 Chapter 10.3.1
Reference Interpreter::IdentifierResolution(String* ident) {
  // 1. Let env be the running execution context’s LexicalEnvironment.
  auto env = vm_->GetExecutionContext()->GetLexicalEnvironment();

  // 2. If the syntactic production that is being evaluated is contained in a strict mode code,
  //    then let strict be true, else let strict be false.
  // todo
  auto strict = false;

  // 3. Return the result of calling GetIdentifierReference function passing env,
  //    Identifier, and strict as arguments.
  return LexicalEnvironment::GetIdentifierReference(
    vm_->GetExecutionContext()->GetLexicalEnvironment(), ident, strict);
}

// GetValue(V)
// Defined in ECMAScript 5.1 Chapter 8.7.1
JSValue Interpreter::GetValue(const std::variant<JSValue, Reference>& V) {
  // 1. If Type(V) is not Reference, return V.
  if (auto ptr = std::get_if<JSValue>(&V)) {
    return *ptr;
  }

  auto ref = std::get<Reference>(V);
  
  // 2. Let base be the result of calling GetBase(V).
  auto base = ref.GetBase();

  // 3. If IsUnresolvableReference(V), throw a ReferenceError exception.
  if (ref.IsUnresolvableReference()) {
    // todo
  }

  // 4. If IsPropertyReference(V), then
  if (ref.IsPropertyReference()) {
    // todo
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
    auto env = std::get<EnvironmentRecord*>(base);
    return env->GetBindingValue(ref.GetReferencedName(), ref.IsStrictReference());
  }
}

// PutValue
// Defined in ECMAScript 5.1 Chapter 8.7.2
void Interpreter::PutValue(const std::variant<JSValue, Reference>& V, JSValue W) {
  // 1. If Type(V) is not Reference, throw a ReferenceError exception.
  if (!std::get_if<Reference>(&V)) {
    // todo
  }

  auto ref = std::get_if<Reference>(&V);

  // 2. Let base be the result of calling GetBase(V).
  auto base = ref->GetBase();

  // 3. If IsUnresolvableReference(V), then
  if (ref->IsUnresolvableReference()) {
    // a. If IsStrictReference(V) is true, then
    if (ref->IsStrictReference()) {
      // i. Throw ReferenceError exception.
      // todo
    }
    // b. Call the [[Put]] internal method of the global object,
    //    passing GetReferencedName(V) for the property name,
    //    W for the value, and false for the Throw flag.
    else {
      auto global_obj = vm_->GetGlobalObject();
      global_obj->Put(JSValue(ref->GetReferencedName()), W, false);
    }
  }
  // 4. Else if IsPropertyReference(V), then
  else if (ref->IsPropertyReference()) {
    // a. If HasPrimitiveBase(V) is false,
    //    then let put be the [[Put]] internal method of base,
    //    otherwise let put be the special [[Put]] internal method defined below.
    // b. Call the put internal method using base as its this value,
    //    and passing GetReferencedName(V) for the property name,
    //    W for the value, and IsStrictReference(V) for the Throw flag.
    if (!ref->HasPrimitiveBase()) {
      auto base_obj = std::get<JSValue>(base).GetHeapObject()->AsObject();
      base_obj->Put(JSValue(ref->GetReferencedName()), W, ref->IsStrictReference());
    } else {
      auto base_prim = std::get<JSValue>(base);
      Put(base_prim, JSValue(ref->GetReferencedName()), W, ref->IsStrictReference());
    }

  }
  // 5. Else base must be a reference whose base is an environment record. So,
  else {
    // a. Call the SetMutableBinding (10.2.1) concrete method of base,
    //    passing GetReferencedName(V), W, and IsStrictReference(V) as arguments.
    auto base_env = std::get<EnvironmentRecord*>(base);
    base_env->SetMutableBinding(ref->GetReferencedName(), W, ref->IsStrictReference());
  }

  // 6. Return
}

// Put
// Defined in ECMAScript 5.1 Chapter 8.7.2
// used by PutValue when V is a property reference with primitive base value
void Interpreter::Put(JSValue base, JSValue P, JSValue W, bool Throw) {
  // 1. Let O be ToObject(base).
  auto O = JSValue::ToObject(base);

  // 2. If the result of calling the [[CanPut]] internal method of O with argument P is false, then
  if (!O->CanPut(P)) {
    // a. If Throw is true, then throw a TypeError exception.
    if (Throw) {
      // todo
    }
    // b. Else return.
    else {
      return ;
    }
  }

  // 3. Let ownDesc be the result of calling the [[GetOwnProperty]] internal method of O with argument P.
  auto own_desc = O->GetOwnProperty(P);

  // 4. If IsDataDescriptor(ownDesc) is true, then
  if (own_desc.IsDataDescriptor()) {
    // a. If Throw is true, then throw a TypeError exception.
    if (Throw) {
      // todo
    }
    // b. Else return.
    else {
      return ;
    }
  }

  // 5. Let desc be the result of calling the [[GetProperty]] internal method of O with argument P.
  //    This may be either an own or inherited accessor property descriptor or an inherited data property descriptor.
  auto desc = O->GetProperty(P);

  // 6. If IsAccessorDescriptor(desc) is true, then
  if (desc.IsAccessorDescriptor()) {
    // a. Let setter be desc.[[Set]] which cannot be undefined.
    auto setter = desc.GetSetter();

    // b. Call the [[Call]] internal method of setter providing base
    //    as the this value and an argument list containing only W.
    // todo
  }
  // 7. Else, this is a request to create an own property on the transient object O
  else {
    // a. If Throw is true, then throw a TypeError exception.
    if (Throw) {
      // todo
    }
  }

  // 8. Return.
}

}  // namespace voidjs
