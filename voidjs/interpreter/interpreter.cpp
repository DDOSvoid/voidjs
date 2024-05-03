#include "voidjs/interpreter/interpreter.h"

#include <cmath>
#include <sched.h>
#include <variant>
#include <iostream>
#include <functional>
#include <utility>

#include "voidjs/ir/ast.h"
#include "voidjs/ir/expression.h"
#include "voidjs/ir/literal.h"
#include "voidjs/ir/statement.h"
#include "voidjs/lexer/token_type.h"
#include "voidjs/types/heap_object.h"
#include "voidjs/types/js_type.h"
#include "voidjs/types/js_value.h"
#include "voidjs/types/object_factory.h"
#include "voidjs/types/lang_types/object.h"
#include "voidjs/types/lang_types/string.h"
#include "voidjs/types/lang_types/number.h"
#include "voidjs/types/spec_types/completion.h"
#include "voidjs/types/spec_types/property_descriptor.h"
#include "voidjs/types/spec_types/reference.h"
#include "voidjs/types/spec_types/lexical_environment.h"
#include "voidjs/types/spec_types/environment_record.h"
#include "voidjs/types/internal_types/property_map.h"
#include "voidjs/types/internal_types/internal_function.h"
#include "voidjs/builtins/builtin.h"
#include "voidjs/builtins/global_object.h"
#include "voidjs/builtins/js_object.h"
#include "voidjs/builtins/js_function.h"
#include "voidjs/builtins/js_error.h"
#include "voidjs/interpreter/execution_context.h"
#include "voidjs/interpreter/string_table.h"
#include "voidjs/utils/macros.h"

namespace voidjs {

using namespace ast;
using namespace types;
using namespace builtins;

void Interpreter::Initialize() {
  vm_ = new VM{this};

  auto string_table = new StringTable{};
  auto object_factory = new ObjectFactory{vm_, string_table};

  vm_->SetObjectFactory(object_factory);

  // Initialize builtin objects and set it for vm
  Builtin::Initialize(vm_);
  
  auto global_env = LexicalEnvironment::NewObjectEnvironmentRecord(vm_, JSValue(vm_->GetGlobalObject()), nullptr);
  
  vm_->SetGlobalEnv(global_env);
}

Completion Interpreter::Execute(AstNode* ast_node) {
  ExecutionContext::EnterGlobalCode(vm_, ast_node);
  return EvalProgram(ast_node);
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
  auto result = EvalSourceElements(prog->GetStatements());
  
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
    case AstNodeType::CONTINUE_STATEMENT: {
      return EvalContinueStatement(stmt->AsContinueStatement());
    }
    case AstNodeType::BREAK_STATEMENT: {
      return EvalBreakStatement(stmt->AsBreakStatement());
    }
    case AstNodeType::RETURN_STATEMENT: {
      return EvalRetrunStatement(stmt->AsReturnStatement());
    }
    case AstNodeType::WITH_STATEMENT: {
      return EvalWithStatement(stmt->AsWithStatement());
    }
    case AstNodeType::SWITCH_STATEMENT: {
      return EvalSwitchStatement(stmt->AsSwitchStatement());
    }
    case AstNodeType::LABELLED_STATEMENT: {
      return EvalLabelledStatement(stmt->AsLabelledStatement());
    }
    case AstNodeType::THROW_STATEMENT: {
      return EvalThrowStatement(stmt->AsThrowStatement());
    }
    case AstNodeType::TRY_STATEMENT: {
      return EvalTryStatement(stmt->AsTryStatement());
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
  RETURN_COMPLETION_IF_HAS_EXCEPTION(vm_);

  // 2. Return (normal, GetValue(exprRef), empty).
  return Completion(CompletionType::NORMAL, GetValue(expr_ref));
}

// Eval IfStatement
// Defined in ECMAScript 5.1 Chapter 12.5
Completion Interpreter::EvalIfStatement(ast::IfStatement* if_stmt) {
  // IfStatement : if ( Expression ) Statement else Statement
  
  // 1. Let exprRef be the result of evaluating Expression.
  auto expr_ref = EvalExpression(if_stmt->GetCondition());
  RETURN_COMPLETION_IF_HAS_EXCEPTION(vm_);

  // 2. If ToBoolean(GetValue(exprRef)) is true, then
  if (JSValue::ToBoolean(vm_, GetValue(expr_ref))) {
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

  vm_->GetExecutionContext()->EnterIteration();
  
  // 1. Let V = empty.
  JSValue V;

  // 2. Let iterating be true.
  bool iterating = true;

  // 3. Repeat, while iterating is true
  while (iterating) {
    // a. Let stmt be the result of evaluating Statement.
    auto stmt = EvalStatement(do_while_stmt->GetBody());
    RETURN_COMPLETION_AND_EXIT_ITERATION_IF_HAS_EXCEPTION(vm_);

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
        vm_->GetExecutionContext()->ExitIteration();
        return Completion(CompletionType::NORMAL, V);
      }

      // ii. If stmt is an abrupt completion, return stmt.
      if (stmt.IsAbruptCompletion()) {
        vm_->GetExecutionContext()->ExitIteration();
        return stmt;
      }
    }

    // d. Let exprRef be the result of evaluating Expression.
    auto expr_ref = EvalExpression(do_while_stmt->GetCondition());
    RETURN_COMPLETION_AND_EXIT_ITERATION_IF_HAS_EXCEPTION(vm_);

    // e. If ToBoolean(GetValue(exprRef)) is false, set iterating to false.
    if (!JSValue::ToBoolean(vm_, GetValue(expr_ref))) {
      iterating = false;
    }
  }

  vm_->GetExecutionContext()->ExitIteration();

  // 4. Return (normal, V, empty);
  return Completion(CompletionType::NORMAL, V);
}

// Eval WhileStatement
// Defined in ECMAScript 5.1 Chapter 12.7
Completion Interpreter::EvalWhileStatement(WhileStatement* while_stmt) {
  // IterationStatement : while ( Expression ) Statement

  vm_->GetExecutionContext()->EnterIteration();
  
  // 1. Let V = empty.
  JSValue V;

  // 2. Repeat
  while (true) {
    // a. Let exprRef be the result of evaluating Expression.
    auto expr_ref = EvalExpression(while_stmt->GetCondition());
    RETURN_COMPLETION_AND_EXIT_ITERATION_IF_HAS_EXCEPTION(vm_);

    // b. If ToBoolean(GetValue(exprRef)) is false, return (normal, V, empty).
    if (!JSValue::ToBoolean(vm_, GetValue(expr_ref))) {
      vm_->GetExecutionContext()->ExitIteration();
      return Completion(CompletionType::NORMAL, V);
    }

    // c. Let stmt be the result of evaluating Statement.
    auto stmt = EvalStatement(while_stmt->GetBody());
    RETURN_COMPLETION_AND_EXIT_ITERATION_IF_HAS_EXCEPTION(vm_);

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
        vm_->GetExecutionContext()->ExitIteration();
        return Completion(CompletionType::NORMAL, V);
      }

      // ii. If stmt is an abrupt completion, return stmt.
      if (stmt.IsAbruptCompletion()) {
        vm_->GetExecutionContext()->ExitIteration();
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

  vm_->GetExecutionContext()->EnterIteration();

  // 1. Evaluate VariableDeclarationListNoIn.
  if (auto init = for_stmt->GetInitializer()) {
    if (init->IsVariableStatement()) {
      EvalVariableStatement(init->AsVariableStatement());
    } else {
      EvalExpression(init->AsExpression());
    }
  }
  RETURN_COMPLETION_AND_EXIT_ITERATION_IF_HAS_EXCEPTION(vm_);

  // 2. Let V = empty.
  JSValue V;

  // 3. Repeat
  while (true) {
    // a. If the first Expression is present, then
    if (for_stmt->GetCondition()) {
      // i. Let testExprRef be the result of evaluating the first Expression.
      auto test_expr_ref = EvalExpression(for_stmt->GetCondition());
      RETURN_COMPLETION_AND_EXIT_ITERATION_IF_HAS_EXCEPTION(vm_);

      // ii. If ToBoolean(GetValue(testExprRef)) is false, return (normal, V, empty).
      if (!JSValue::ToBoolean(vm_, GetValue(test_expr_ref))) {
        vm_->GetExecutionContext()->ExitIteration();
        return Completion(CompletionType::NORMAL, V);
      }
    }

    // b. Let stmt be the result of evaluating Statement.
    auto stmt = EvalStatement(for_stmt->GetBody());
    RETURN_COMPLETION_AND_EXIT_ITERATION_IF_HAS_EXCEPTION(vm_);

    // c. If stmt.value is not empty, let V = stmt.value
    if (!stmt.GetValue().IsEmpty()) {
      V = stmt.GetValue();
    }

    // d. If stmt.type is break and stmt.target is in the current label set,
    //    return (normal, V, empty).
    if (stmt.GetType() == CompletionType::BREAK &&
        vm_->GetExecutionContext()->HasLabel(stmt.GetTarget())) {
      vm_->GetExecutionContext()->ExitIteration();
      return Completion(CompletionType::NORMAL, V);
    }

    // e. If stmt.type is not continue ||
    //    stmt.target is not in the current label set, then
    if (stmt.GetType() != CompletionType::CONTINUE || 
        !vm_->GetExecutionContext()->HasLabel(stmt.GetTarget())) {
      // 1. If stmt is an abrupt completion, return stmt.
      if (stmt.IsAbruptCompletion()) {
        vm_->GetExecutionContext()->ExitIteration();
        return stmt;
      }
    }

    // f. If the second Expression is present, then
    if (for_stmt->GetUpdate()) {
      // 1. Let incExprRef be the result of evaluating the second Expression.
      auto inc_expr_ref = EvalExpression(for_stmt->GetUpdate());
      RETURN_COMPLETION_AND_EXIT_ITERATION_IF_HAS_EXCEPTION(vm_);

      // 2. Call GetValue(incExprRef). (This value is not used.)
      GetValue(inc_expr_ref);
      RETURN_COMPLETION_AND_EXIT_ITERATION_IF_HAS_EXCEPTION(vm_);
    }
  }
}

// Eval ForInStatement
// Defined in ECMASCript 5.1 Chapter 12.6.4
Completion Interpreter::EvalForInStatement(ForInStatement *for_in_stmt) {
  // IterationStatement : for ( LeftHandSideExpression in Expression ) Statement
  // IterationStatement : for ( var VariableDeclarationNoIn in Expression ) Statement

  vm_->GetExecutionContext()->EnterIteration();

  // 1. Let varName be the result of evaluating VariableDeclarationNoIn.
  JSValue var_name;
  if (for_in_stmt->GetLeft()->IsVariableDeclaraion()) {
    var_name = EvalVariableDeclaration(for_in_stmt->GetLeft()->AsVariableDeclaration());
  }
  RETURN_COMPLETION_AND_EXIT_ITERATION_IF_HAS_EXCEPTION(vm_);

  // 2. Let exprRef be the result of evaluating the Expression.
  auto expr_ref = EvalExpression(for_in_stmt->GetRight());
  RETURN_COMPLETION_AND_EXIT_ITERATION_IF_HAS_EXCEPTION(vm_);

  // 3. Let experValue be GetValue(exprRef).
  auto expr_val = GetValue(expr_ref);
  RETURN_COMPLETION_AND_EXIT_ITERATION_IF_HAS_EXCEPTION(vm_);

  // 4. If experValue is null or undefined, return (normal, empty, empty).
  if (expr_val.IsNull() || expr_val.IsUndefined()) {
    vm_->GetExecutionContext()->ExitIteration();
    return Completion(CompletionType::NORMAL);
  }

  // 5. Let obj be ToObject(experValue).
  auto obj = JSValue::ToObject(vm_, expr_val);
  RETURN_COMPLETION_AND_EXIT_ITERATION_IF_HAS_EXCEPTION(vm_);

  // 6. Let V = empty.
  JSValue V;

  // 7. Repeat
  while (true) {
    // a. Let P be the name of the next property of obj whose [[Enumerable]] attribute is true.
    //    If there is no such property, return (normal, V, empty).
    // todo
  }
}

// EvalContinueStatement
// Defined in ECMAScript 5.1 Chapter 12.7
Completion Interpreter::EvalContinueStatement(ContinueStatement* cont_stmt) {
  // A program is considered syntactically incorrect if either of the following is true:
  //   1. The program contains a continue statement without the optional Identifier,
  //      which is not nested, directly or indirectly (but not crossing function boundaries), within an IterationStatement.
  //   2. The program contains a continue statement with the optional Identifier,
  //      where Identifier does not appear in the label set of an enclosing (but not crossing function boundaries) IterationStatement.
  if (auto ident = cont_stmt->GetIdentifier();
      !ident && !vm_->GetExecutionContext()->InIteration() ||
      ident && !vm_->GetExecutionContext()->HasLabel(ident->AsIdentifier()->GetName())) {
    THROW_SYNTAX_ERROR_AND_RETURN_VALUE(vm_, u"Incorrect use of continue.", Completion{});
  }

  auto ident = cont_stmt->GetIdentifier();

  // A ContinueStatement without an Identifier is evaluated as follows:
  // 1. Return (continue, empty, empty).
  if (!ident) {
    return Completion{CompletionType::CONTINUE};
  }
  // A ContinueStatement with the optional Identifier is evaluated as follows:
  // 1.Return (continue, empty, Identifier).
  else {
    return Completion{CompletionType::CONTINUE, JSValue{}, ident->AsIdentifier()->GetName()};
  }
}

// EvalBreakStatement
// Defined in ECMAScript 5.1 Chapter 12.8
Completion Interpreter::EvalBreakStatement(BreakStatement* break_stmt) {
  // A program is considered syntactically incorrect if either of the following is true:
  //   1. The program contains a break statement without the optional Identifier,
  //      which is not nested, directly or indirectly (but not crossing function boundaries),
  //      within an IterationStatement or a SwitchStatement.
  //   2. The program contains a break statement with the optional Identifier,
  //      where Identifier does not appear in the label set of an enclosing
  //      (but not crossing function boundaries) Statement.
  if (auto ident = break_stmt->GetIdentifier();
      !ident && !vm_->GetExecutionContext()->InIteration() && !vm_->GetExecutionContext()->InSwitch() ||
      ident && !vm_->GetExecutionContext()->HasLabel(ident->AsIdentifier()->GetName())) {
    THROW_SYNTAX_ERROR_AND_RETURN_VALUE(vm_, u"Incorrect use of break.", Completion{});
  }

  auto ident = break_stmt->GetIdentifier();

  // A BreakStatement without an Identifier is evaluated as follows:
  // 1. Return (break, empty, empty).
  if (!ident) {
    return Completion{CompletionType::BREAK};
  } 
  // A BreakStatement with an Identifier is evaluated as follows:
  // 1. Return (break, empty, Identifier).
  else {
    return Completion{CompletionType::BREAK, JSValue{}, ident->AsIdentifier()->GetName()};
  }
}

// EvalReturnStatement
// Defined in ECMAScript 5.1 Chapter 12.9
Completion Interpreter::EvalRetrunStatement(ReturnStatement* return_stmt) {
  // ReturnStatement : return [no LineTerminator here] Expressionopt ;

  // An ECMAScript program is considered syntactically incorrect if
  // it contains a return statement that is not within a FunctionBody.
  // A return statement causes a function to cease execution and return a value to the caller.
  // If Expression is omitted, the return value is undefined. Otherwise, the return value is the value of Expression.
  // todo

  // 1. If the Expression is not present, return (return, undefined, empty).
  if (!return_stmt->GetExpression()) {
    return Completion{CompletionType::RETURN, JSValue::Undefined()};
  }
  
  // 2. Let exprRef be the result of evaluating Expression.
  auto expr_ref = EvalExpression(return_stmt->GetExpression());
  RETURN_COMPLETION_IF_HAS_EXCEPTION(vm_);
  
  // 3. Return (return, GetValue(exprRef), empty).
  auto val = GetValue(expr_ref);
  RETURN_COMPLETION_IF_HAS_EXCEPTION(vm_);

  return Completion{CompletionType::RETURN, val};
}

// EvalWithStatement
// Defined in ECMAScript 5.1 Chapter 12.10
// todo
Completion Interpreter::EvalWithStatement(WithStatement* with_stmt) {
  // WithStatement : with ( Expression ) Statement

  // 1. Let val be the result of evaluating Expression.
  auto val = EvalExpression(with_stmt->GetContext());
  RETURN_COMPLETION_IF_HAS_EXCEPTION(vm_);
  
  // 2. Let obj be ToObject(GetValue(val)).
  auto obj = JSValue::ToObject(vm_, GetValue(val));
  RETURN_COMPLETION_IF_HAS_EXCEPTION(vm_);
  
  // 3. Let oldEnv be the running execution context’s LexicalEnvironment.
  auto old_env = vm_->GetExecutionContext()->GetLexicalEnvironment();
  
  // 4. Let newEnv be the result of calling NewObjectEnvironment passing obj and oldEnv as the arguments
  auto new_env = LexicalEnvironment::NewObjectEnvironmentRecord(vm_, JSValue(obj), old_env);
  
  // 5. Set the provideThis flag of newEnv to true.
  new_env->SetProvideThis(true);
  
  // 6. Set the running execution context’s LexicalEnvironment to newEnv.
  vm_->GetExecutionContext()->SetLexicalEnvironment(new_env);
  
  // 7. Let C be the result of evaluating Statement but if an exception is thrown during the evaluation,
  //    let C be (throw, V, empty), where V is the exception. (Execution now proceeds as if no exception were thrown.)
  Completion C;
  C = EvalStatement(with_stmt->GetBody());
  if (vm_->HasException()) {
    C = Completion{CompletionType::THROW, JSValue(vm_->GetException())};
    vm_->ClearException();
  }
  
  // 8. Set the running execution context’s Lexical Environment to oldEnv.
  vm_->GetExecutionContext()->SetLexicalEnvironment(old_env);
  
  // 9. Return C.
  return C;
}

// EvalSwitchStatement
// Defined in ECMAScript 5.1 Chapter 12.11
Completion Interpreter::EvalSwitchStatement(SwitchStatement* switch_stmt) {
  // SwitchStatement : switch ( Expression ) CaseBlock

  vm_->GetExecutionContext()->EnterSwitch();
  
  // 1. Let exprRef be the result of evaluating Expression.
  auto expr_ref = EvalExpression(switch_stmt->GetDiscriminant());
  RETURN_COMPLETION_AND_EXIT_SWITCH_IF_HAS_EXCEPTION(vm_);
  
  // 2. Let R be the result of evaluating CaseBlock, passing it GetValue(exprRef) as a parameter.
  auto R = EvalCaseBlock(switch_stmt->GetCaseClauses(), GetValue(expr_ref));
  RETURN_COMPLETION_AND_EXIT_SWITCH_IF_HAS_EXCEPTION(vm_);
  
  // 3. If R.type is break and R.target is in the current label set, return (normal, R.value, empty).
  if (R.GetType() == CompletionType::BREAK &&
      vm_->GetExecutionContext()->HasLabel(R.GetTarget())) {
    return Completion{CompletionType::NORMAL, R.GetValue()};
  }
  
  vm_->GetExecutionContext()->ExitSwitch();
  
  // 4. Return R.
  return R;
}

// EvalLabelledStatement
// Defined in ECMAScript 5.1 Chapter 12.12
Completion Interpreter::EvalLabelledStatement(LabelledStatement* label_stmt) {
  // LabelledStatement : Identifier : Statement
  
  // An ECMAScript program is considered syntactically incorrect
  // if it contains a LabelledStatement that is enclosed by a LabelledStatement with the same Identifier as label.
  // This does not apply to labels appearing within the body of a FunctionDeclaration that
  // is nested, directly or indirectly, within a labelled statement.
  // THROW_SYNTAX_ERROR_AND_RETURN_VALUE(vm_, u"Incorrect use of label.", Completion{});

  auto label = label_stmt->GetLabel()->AsIdentifier()->GetName();
  vm_->GetExecutionContext()->AddLabel(label);

  // The production Identifier : Statement is evaluated by
  // adding Identifier to the label set of Statement and then evaluating Statement.
  // If the LabelledStatement itself has a non-empty label set,
  // these labels are also added to the label set of Statement before evaluating it.
  // If the result of evaluating Statement is (break, V, L) where L is equal to Identifier, the production results in (normal, V, empty).
  auto ret = EvalStatement(label_stmt->GetBody());
  RETURN_COMPLETION_IF_HAS_EXCEPTION(vm_);
  if (ret.GetType() == CompletionType::BREAK && ret.GetTarget() == label) {
    vm_->GetExecutionContext()->DeleteLabel(label);
    return Completion{CompletionType::NORMAL, ret.GetValue()};
  }
  
  vm_->GetExecutionContext()->DeleteLabel(label);
  
  return ret;
}

// EvalThrowStatement
// Defined in ECMAScript 5.1 Chapter 12.13
Completion Interpreter::EvalThrowStatement(ThrowStatement* throw_stmt) {
  // ThrowStatement : throw [no LineTerminator here] Expression ;

  // 1. Let exprRef be the result of evaluating Expression.
  auto expr_ref = EvalExpression(throw_stmt->GetExpression());
  RETURN_COMPLETION_IF_HAS_EXCEPTION(vm_);

  // 2. Return (throw, GetValue(exprRef), empty).
  auto val = GetValue(expr_ref);
  RETURN_COMPLETION_IF_HAS_EXCEPTION(vm_);

  return Completion{CompletionType::THROW, val};
}

// EvalTryStatement
// Defined in ECMAScript 5.1 Chapter 12.14
Completion Interpreter::EvalTryStatement(TryStatement* try_stmt) {
  auto factory = vm_->GetObjectFactory();
  
  // TryStatement : try Block Catch
  // TryStatement : try Block Finally
  // TryStatement : try Block Catch Finally
  
  // 1. Let B be the result of evaluating Block.
  Completion C;
  auto B = EvalStatement(try_stmt->GetBody());
    
  // 2. If B.type is not throw, return B.
  if (B.GetType() == CompletionType::THROW) {
    // a. Let C be the result of evaluating Catch with parameter B.
    if (try_stmt->GetCatchBlock()) {
      vm_->ClearException();
      C = EvalCatch(try_stmt->GetCatchName(), try_stmt->GetCatchBlock(), B.GetValue());
      RETURN_COMPLETION_IF_HAS_EXCEPTION(vm_);
    }
  }
  // 3. Else, B.type is not throw,
  else {
    // a. Let C be B.
    C = B;
  }

  // 4. Let F be the result of evaluating Finally.
  if (!try_stmt->GetFinallyBlock()) {
    return C;
  }
  auto F = EvalStatement(try_stmt->GetFinallyBlock());
  RETURN_COMPLETION_IF_HAS_EXCEPTION(vm_);

  // 5. If F.type is normal, return C.
  if (F.GetType() == CompletionType::NORMAL) {
    return C;
  }

  // 6. Return F.
  return F;
}

// EvalDebuggerStatement
// Defined in ECMAScript 5.1 Chapter 12.15
// todo
Completion Interpreter::EvalDebuggerStatement(DebuggerStatement* debug_stmt) {
  // DebuggerStatement : debugger ;

  // 1. If an implementation defined debugging facility is available and enabled, then
  // a. Perform an implementation defined debugging action.
  // b. Let result be an implementation defined Completion value.
  // 2. Else
  // a. Let result be (normal, empty, empty).
  // 3. Return result.

  return Completion{CompletionType::NORMAL};
}

// EvalFunctionDeclaration
// Defined in ECMAScript 5.1 Chapter 13
void Interpreter::EvalFunctionDeclaration(AstNode* ast_node) {
  // FunctionDeclaration was instantiated during Declaration Binding instantiation
  // Builtin::InstantiatingFunctionDeclaration(vm_, ast_node, vm_->GetExecutionContext()->GetLexicalEnvironment(), false);
  return ;
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
    case AstNodeType::NEW_EXPRESSION: {
      return EvalNewExpression(expr->AsNewExpression());
    }
    case AstNodeType::CALL_EXPRESSION: {
      return EvalCallExpression(expr->AsCallExpression());
    }
    case AstNodeType::FUNCTION_EXPRESSION: {
      return EvalFunctionExpression(expr->AsFunctionExpression());
    }
    case AstNodeType::OBJECT_LITERAL: {
      return EvalObjectLiteral(expr->AsObjectLiteral());
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
    auto ref = EvalExpression(expr);
    RETURN_VALUE_IF_HAS_EXCEPTION(vm_, JSValue{});
    
    val = GetValue(ref);
    RETURN_VALUE_IF_HAS_EXCEPTION(vm_, JSValue{});
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
    RETURN_VALUE_IF_HAS_EXCEPTION(vm_, JSValue{});

    // 2. Let rref be the result of evaluating AssignmentExpression.
    auto rref = EvalExpression(assign_expr->GetRight());
    RETURN_VALUE_IF_HAS_EXCEPTION(vm_, JSValue{});

    // 3. Let rval be GetValue(rref).
    auto rval = GetValue(rref);
    RETURN_VALUE_IF_HAS_EXCEPTION(vm_, JSValue{});

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
      if (plref->GetReferencedName()->Equal(u"eval")) {
        THROW_SYNTAX_ERROR_AND_RETURN_VALUE(vm_, u"Incorrect use of eval.", JSValue{});
      } else {
        THROW_SYNTAX_ERROR_AND_RETURN_VALUE(vm_, u"Incorrect use of arguments.", JSValue{});
      }
    }

    // 5. Call PutValue(lref, rval).
    PutValue(lref, rval);
    RETURN_VALUE_IF_HAS_EXCEPTION(vm_, JSValue{});

    // 6. Return rval.
    return rval;
  } else {
    // AssignmentExpression : LeftHandSideExpression @= AssignmentExpression
      
    // 1. Let lref be the result of evaluating LeftHandSideExpression.
    auto lref = EvalExpression(assign_expr->GetLeft());
    RETURN_VALUE_IF_HAS_EXCEPTION(vm_, JSValue{});

    // 2. Let lval be GetValue(lref).
    auto lval = GetValue(lref);
    RETURN_VALUE_IF_HAS_EXCEPTION(vm_, JSValue{});

    // 3. Let rref be the result of evaluating AssignmentExpression.
    auto rref = EvalExpression(assign_expr->GetRight());
    RETURN_VALUE_IF_HAS_EXCEPTION(vm_, JSValue{});

    // 4. Let rval be GetValue(rref).
    auto rval = GetValue(rref);
    RETURN_VALUE_IF_HAS_EXCEPTION(vm_, JSValue{});

    // 5. Let r be the result of applying operator @ to lval and rval.
    JSValue r = ApplyCompoundAssignment(assign_expr->GetOperator(), lval, rval);
    RETURN_VALUE_IF_HAS_EXCEPTION(vm_, JSValue{});

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
      if (plref->GetReferencedName()->Equal(u"eval")) {
        THROW_SYNTAX_ERROR_AND_RETURN_VALUE(vm_, u"Incorrect use of eval.", JSValue{});
      } else {
        THROW_SYNTAX_ERROR_AND_RETURN_VALUE(vm_, u"Incorrect use of arguments.", JSValue{});
      }
    }

    // 5. Call PutValue(lref, r).
    PutValue(lref, r);
    RETURN_VALUE_IF_HAS_EXCEPTION(vm_, JSValue{});

    // 6. Return r.
    return r;
  }
}

// Eval ConditionalExpression
// Defined in ECMAScript 5.1 Chapter 11.12
std::variant<JSValue, Reference> Interpreter::EvalConditionalExpression(ConditionalExpression* cond_expr) {
  // ConditionalExpression : LogicalORExpression ? AssignmentExpression : AssignmentExpression
  
  // 1. Let lref be the result of evaluating LogicalORExpression.
  auto lref = EvalExpression(cond_expr->GetConditional());
  RETURN_VALUE_IF_HAS_EXCEPTION(vm_, JSValue{});

  // 2. If ToBoolean(GetValue(lref)) is true, then
  if (JSValue::ToBoolean(vm_, GetValue(lref))) {
    // a. Let trueRef be the result of evaluating the first AssignmentExpression.
    auto true_ref = EvalExpression(cond_expr->GetConsequent());
    RETURN_VALUE_IF_HAS_EXCEPTION(vm_, JSValue{});

    // b. Return GetValue(trueRef).
    return true_ref;
  } else {
    // a. Let falseRef be the result of evaluating the second AssignmentExpression.
    auto false_ref = EvalExpression(cond_expr->GetAlternate());
    RETURN_VALUE_IF_HAS_EXCEPTION(vm_, JSValue{});

    // b. Return GetValue(falseRef).
    return false_ref;
  }
}

// Eval BinaryExpression
std::variant<JSValue, Reference> Interpreter::EvalBinaryExpression(BinaryExpression* binary_expr) {
  auto op = binary_expr->GetOperator();
  auto left = binary_expr->GetLeft();
  auto right = binary_expr->GetRight();
  switch (auto op = binary_expr->GetOperator()) {
    case TokenType::LOGICAL_OR:
    case TokenType::LOGICAL_AND: {
      return ApplyLogicalOperator(op, left, right);
    }
    case TokenType::BIT_OR:
    case TokenType::BIT_AND:
    case TokenType::BIT_NOT: {
      return ApplyBitwiseOperator(op, left, right);
    }
    case TokenType::EQUAL:
    case TokenType::NOT_EQUAL:
    case TokenType::STRICT_EQUAL:
    case TokenType::NOT_STRICT_EQUAL: {
      return ApplyEqualityOperator(op, left, right);
    }
    case TokenType::LESS_THAN:
    case TokenType::GREATER_THAN:
    case TokenType::LESS_EQUAL:
    case TokenType::GREATER_EQUAL: {
      return ApplyRelationalOperator(op, left, right);
    }
    case TokenType::LEFT_SHIFT:
    case TokenType::RIGHT_SHIFT:
    case TokenType::U_RIGHT_SHIFT: {
      return ApplyShiftOperator(op, left, right);
    }
    case TokenType::ADD:
    case TokenType::SUB: {
      return ApplyAdditiveOperator(op, left, right);
    }
    case TokenType::MUL:
    case TokenType::DIV:
    case TokenType::MOD: {
      return ApplyMultiplicativeOperator(op, left, right);
    }
    default: {
      // unreachable branch
      // todo
      return {};
    }
  }
}

// Eval UnaryExpression
// Defined in ECMAScript 5.1 Chapter 11.4
std::variant<JSValue, Reference> Interpreter::EvalUnaryExpression(UnaryExpression* unary_expr) {
  return ApplyUnaryOperator(unary_expr->GetOperator(), unary_expr->GetExpression());
}

// Eval PostfixExpression
// Defined in ECMAScript 5.1 Chapter 11.3
std::variant<JSValue, Reference> Interpreter::EvalPostfixExpression(PostfixExpression* post_expr) {
  return ApplyPostfixOperator(post_expr->GetOperator(), post_expr->GetExpression());
}

// Eval MemberExpression
// Defined in ECMAScript 5.1 Chapter 11.2
std::variant<JSValue, Reference> Interpreter::EvalMemberExpression(MemberExpression* mem_expr) {
  // MemberExpression : MemberExpression [ Expression ]
  // MemberExpression : MemberExpression . IdentifierName
  
  // 1. Let baseReference be the result of evaluating MemberExpression.
  auto base_ref = EvalExpression(mem_expr->GetObject());
  RETURN_VALUE_IF_HAS_EXCEPTION(vm_, JSValue{});

  // 2. Let baseValue be GetValue(baseReference).
  auto base_val = GetValue(base_ref);
  RETURN_VALUE_IF_HAS_EXCEPTION(vm_, JSValue{});

  // 3. Let propertyNameReference be the result of evaluating Expression.
  // todo
  auto prop_name_ref =
    std::invoke([*this](Expression* expr, bool is_dot) mutable -> std::variant<JSValue, Reference> {
    if (is_dot) {
      auto factory = vm_->GetObjectFactory();
      return JSValue(factory->NewStringFromTable(expr->AsIdentifier()->GetName()));
    } else {
      return EvalExpression(expr);
    }
  }, mem_expr->GetProperty(), mem_expr->IsDot());
  RETURN_VALUE_IF_HAS_EXCEPTION(vm_, JSValue{});

  // 4. Let propertyNameValue be GetValue(propertyNameReference).
  auto prop_name_val = GetValue(prop_name_ref);
  RETURN_VALUE_IF_HAS_EXCEPTION(vm_, JSValue{});

  // 5. Call CheckObjectCoercible(baseValue).
  JSValue::CheckObjectCoercible(vm_, base_val);
  RETURN_VALUE_IF_HAS_EXCEPTION(vm_, JSValue{});

  // 6. Let propertyNameString be ToString(propertyNameValue).
  auto prop_name_str = JSValue::ToString(vm_, prop_name_val); 

  // 7. If the syntactic production that is being evaluated is contained in strict mode code,
  //    let strict be true, else let strict be false.
  // todo
  bool strict = false;

  // 8. Return a value of type Reference
  //    whose base value is baseValue and whose referenced name is propertyNameString,
  //    and whose strict mode flag is strict.
  return Reference(base_val, prop_name_str, strict);
}

// EvalNewExpression
// Defined in ECMAScript 5.1 Chapter 11.2.2
std::variant<JSValue, types::Reference> Interpreter::EvalNewExpression(ast::NewExpression* new_expr) {
  // NewExpression : new NewExpression
  // MemberExpression : new MemberExpression Arguments

  // 1. Let ref be the result of evaluating NewExpression.
  auto ref = EvalExpression(new_expr->GetConstructor());
  RETURN_VALUE_IF_HAS_EXCEPTION(vm_, JSValue{});
  
  // 2. Let constructor be GetValue(ref).
  auto ctor = GetValue(ref);
  RETURN_VALUE_IF_HAS_EXCEPTION(vm_, JSValue{});

  // 3. Let argList be the result of evaluating Arguments,
  //    producing an internal list of argument values (11.2.4).
  auto arg_list = EvalArgumentList(new_expr->GetArguments());
  RETURN_VALUE_IF_HAS_EXCEPTION(vm_, JSValue{});
  
  // 4. If Type(constructor) is not Object, throw a TypeError exception.
  if (!ctor.IsObject()) {
    THROW_TYPE_ERROR_AND_RETURN_VALUE(
      vm_, u"Cannot use new on values that aren't Object.", JSValue{});
  }
  
  // 5. If constructor does not implement the [[Construct]] internal method, throw a TypeError exception.
  // 6. Return the result of calling the [[Construct]] internal method on constructor,
  //    providing the list argList as the argument values.
  return Object::Construct(vm_, ctor.GetHeapObject()->AsObject(), arg_list);
}

// EvalCallExpression
// Defined in ECMAScript 5.1 Chapter 11.2.3
std::variant<JSValue, Reference> Interpreter::EvalCallExpression(CallExpression* call_expr) {
  // CallExpression : MemberExpression Arguments

  // 1. Let ref be the result of evaluating MemberExpression.
  auto ref = EvalExpression(call_expr->GetCallee());
  RETURN_VALUE_IF_HAS_EXCEPTION(vm_, JSValue{});
  
  // 2. Let func be GetValue(ref).
  auto func = GetValue(ref);
  RETURN_VALUE_IF_HAS_EXCEPTION(vm_, JSValue{});
  
  // 3. Let argList be the result of evaluating Arguments, producing an internal list of argument values (see 11.2.4).
  auto arg_list = EvalArgumentList(call_expr->GetArguments());
  RETURN_VALUE_IF_HAS_EXCEPTION(vm_, JSValue{});
  
  // 4. If Type(func) is not Object, throw a TypeError exception.
  if (!func.IsObject()) {
    THROW_SYNTAX_ERROR_AND_RETURN_VALUE(
      vm_, u"Function call on non-object value.", JSValue{});
  }
  
  // 5. If IsCallable(func) is false, throw a TypeError exception.
  if (!func.IsCallable()) {
    THROW_SYNTAX_ERROR_AND_RETURN_VALUE(
      vm_, u"Function call on non-callable value.", JSValue{});
  }

  JSValue this_value {};
  
  // 6. If Type(ref) is Reference, then
  if (auto pref = std::get_if<Reference>(&ref)) {
    // a. If IsPropertyReference(ref) is true, then
    if (pref->IsPropertyReference()) {
      // i. Let thisValue be GetBase(ref).
      this_value = std::get<JSValue>(pref->GetBase());
    }
    // b. Else, the base of ref is an Environment Record
    else {
      // i. Let thisValue be the result of calling the ImplicitThisValue concrete method of GetBase(ref).
      this_value = EnvironmentRecord::ImplicitThisValue(vm_, std::get<EnvironmentRecord*>(pref->GetBase()));
    }
  }
  // 7. Else, Type(ref) is not Reference.
  else {
    // a. Let thisValue be undefined.
    this_value = JSValue::Undefined();
  }
  
  // 8. Return the result of calling the [[Call]] internal method on func,
  //    providing thisValue as the this value and providing the list argList as the argument values.
  return Object::Call(vm_, func.GetHeapObject()->AsObject(), this_value, arg_list);
}

// EvalFunctionExpression
// Defined in ECMAScript 5.1 Chapter 13
JSValue Interpreter::EvalFunctionExpression(FunctionExpression* func_expr) {
  // FunctionExpression : function ( FormalParameterListopt ) { FunctionBody }
  // FunctionExpression : function Identifier ( FormalParameterListopt ) { FunctionBody }
  
  // todo
  if (!func_expr->GetName()) {
    return JSValue{Builtin::InstantiatingFunctionDeclaration(
        vm_, func_expr, vm_->GetExecutionContext()->GetLexicalEnvironment(), false)};
  }
  auto ident = vm_->GetObjectFactory()->NewStringFromTable(
    func_expr->GetName()->AsIdentifier()->GetName());

  // 1. Let funcEnv be the result of calling NewDeclarativeEnvironment passing
  //    the running execution context’s Lexical Environment as the argument
  auto func_env = LexicalEnvironment::NewDeclarativeEnvironmentRecord(
    vm_, vm_->GetExecutionContext()->GetLexicalEnvironment());
  
  // 2. Let envRec be funcEnv’s environment record.
  auto env_rec = func_env->GetEnvRec()->AsDeclarativeEnvironmentRecord();
  
  // 3. Call the CreateImmutableBinding(N) concrete method of envRec passing the String value of Identifier as the argument.
  DeclarativeEnvironmentRecord::CreateImmutableBinding(vm_, env_rec, ident);
  
  // 4. Let closure be the result of creating a new Function object as specified in 13.2
  //    with parameters specified by FormalParameterListopt and body specified by FunctionBody.
  //    Pass in funcEnv as the Scope. Pass in true as the Strict flag if
  //    the FunctionExpression is contained in strict code or if its FunctionBody is strict code.
  // todo
  auto closure = Builtin::InstantiatingFunctionDeclaration(vm_, func_expr, func_env, false);
  
  // 5. Call the InitializeImmutableBinding(N,V) concrete method of envRec passing the String value of Identifier and closure as the arguments.
  DeclarativeEnvironmentRecord::InitializeImmutableBinding(vm_, env_rec, ident, JSValue{closure});
  
  // 6. Return closure.
  return JSValue{closure};
}

// EvalArgumentList
// Defined in ECMAScript 5.1 Chapter 11.2.4
std::vector<JSValue> Interpreter::EvalArgumentList(const ast::Expressions& exprs) {
  std::vector<JSValue> vals;
  for (auto expr : exprs) {
    auto ref = EvalExpression(expr);
    RETURN_VALUE_IF_HAS_EXCEPTION(vm_, {});

    auto val = GetValue(ref);
    RETURN_VALUE_IF_HAS_EXCEPTION(vm_, {});
    
    vals.push_back(val);
  }
  return vals;
}

// EvalCaseBlock
// Defined in ECMAScript 5.1 Chapter 12.11
Completion Interpreter::EvalCaseBlock(const CaseClauses& cases, JSValue input) {
  // CaseBlock : { CaseClausesopt }
  // CaseBlock : { CaseClausesoptDefaultClause CaseClausesopt }
  
  // 1. Let V = empty.
  JSValue V {};
  
  // 2. Let A be the list of CaseClause items in the first CaseClauses, in source text order.
  // 3. Let B be the list of CaseClause items in the second CaseClauses, in source text order.
  auto it = cases.begin();
  
  // 4. Let found be false.
  bool found {false};
  
  // 5. Repeat letting C be in order each CaseClause in A
  while (it != cases.end() && !(*it)->IsDefault()) {
    // a. If found is false, then
    if (!found) {
      // i. Let clauseSelector be the result of evaluating C.
      auto ref = EvalExpression((*it)->GetCondition());
      RETURN_COMPLETION_IF_HAS_EXCEPTION(vm_);
      
      auto clause_selector = GetValue(ref);
      RETURN_COMPLETION_IF_HAS_EXCEPTION(vm_);
      
      // ii. If input is equal to clauseSelector as defined by the === operator, then set found to true.
      found = StrictEqualityComparison(clause_selector, input);
    }
    
    // b. If found is true, then
    if (found) {
      // i. If C has a StatementList, then
      if (!(*it)->GetStatements().empty()) {
        // 1. Evaluate C’s StatementList and let R be the result.
        auto R = EvalStatementList((*it)->GetStatements());
        RETURN_COMPLETION_IF_HAS_EXCEPTION(vm_);
        
        // 2. If R.value is not empty, then let V = R.value.
        if (!R.GetValue().IsEmpty()) {
          V = R.GetValue();
        }
        
        // 3. R is an abrupt completion, then return (R.type,V,R.target).
        if (R.IsAbruptCompletion()) {
          return Completion{R.GetType(), V, R.GetTarget()};
        }
      }
    }
    ++it;
  }

  CaseClause* default_clause {nullptr};
  if (it != cases.end() && (*it)->IsDefault()) {
    default_clause = *it;
    ++it;
  }
  
  // 6. Let foundInB be false.
  bool found_in_B {false};
  
  // 7. If found is false, then
  if (!found) {
    // a. Repeat, while foundInB is false and all elements of B have not been processed
    while (it != cases.end() && !found_in_B) {
      // i. Let C be the next CaseClause in B.
      // ii. Let clauseSelector be the result of evaluating C.
      auto ref = EvalExpression((*it)->GetCondition());
      RETURN_COMPLETION_IF_HAS_EXCEPTION(vm_);
      
      auto clause_selector = GetValue(ref);
      RETURN_COMPLETION_IF_HAS_EXCEPTION(vm_);
      
      // iii. If input is equal to clauseSelector as defined by the === operator, then
      if (StrictEqualityComparison(clause_selector, input)) {
        // 1. Set foundInB to true.
        found_in_B = true;
        
        // 2. If C has a StatementList, then
        if (!(*it)->GetStatements().empty()) {
          // 1. Evaluate C’s StatementList and let R be the result.
          auto R = EvalStatementList((*it)->GetStatements());
          RETURN_COMPLETION_IF_HAS_EXCEPTION(vm_);
          
          // 2. If R.value is not empty, then let V = R.value.
          if (!R.GetValue().IsEmpty()) {
            V = R.GetValue();
          }
        
          // 3. R is an abrupt completion, then return (R.type,V,R.target).
          if (R.IsAbruptCompletion()) {
            return Completion{R.GetType(), V, R.GetTarget()};
          }
        }
      }
      
      ++it;
    }
  }
  
  // 8. If foundInB is false and the DefaultClause has a StatementList, then
  if (!found_in_B && !default_clause->GetStatements().empty()) {
    // a. Evaluate the DefaultClause’s StatementList and let R be the result.
    auto R = EvalStatementList(default_clause->GetStatements());
    RETURN_COMPLETION_IF_HAS_EXCEPTION(vm_);
    
    // b. If R.value is not empty, then let V = R.value.
    if (!R.GetValue().IsEmpty()) {
      V = R.GetValue();
    }
    
    // c. If R is an abrupt completion, then return (R.type, V, R.target).
    if (R.IsAbruptCompletion()) {
      return Completion{R.GetType(), V, R.GetTarget()};
    }
  }
  
  // 9. Repeat (Note that if step 7.a.i has been performed this loop does not start at the beginning of B)
  while (it != cases.end()) {
    // a. Let C be the next CaseClause in B. If there is no such CaseClause, return (normal, V, empty).
    // b. If C has a StatementList, then
    if (!(*it)->GetStatements().empty()) {
      // i. Evaluate C’s StatementList and let R be the result.
      auto R = EvalStatementList((*it)->GetStatements());
      RETURN_COMPLETION_IF_HAS_EXCEPTION(vm_);
      
      // ii. If R.value is not empty, then let V = R.value.
      if (!R.GetValue().IsEmpty()) {
        V = R.GetValue();
      }
    
      // iii. If R is an abrupt completion, then return (R.type, V, R.target).
      if (R.IsAbruptCompletion()) {
        return Completion{R.GetType(), V, R.GetTarget()};
      }
    }

    ++it;
  }

  return Completion{CompletionType::NORMAL, V};
}

// EvalCatch
// Defined in ECMAScript 5.1 Chapter 12.14
Completion Interpreter::EvalCatch(Expression* catch_name, Statement* catch_block, JSValue C) {
  auto factory = vm_->GetObjectFactory();
  
  // 1. Let C be the parameter that has been passed to this production.
  // 2. Let oldEnv be the running execution context’s LexicalEnvironment.
  auto old_env = vm_->GetExecutionContext()->GetLexicalEnvironment();
    
  // 3. Let catchEnv be the result of calling NewDeclarativeEnvironment passing oldEnv as the argument.
  auto catch_env = LexicalEnvironment::NewDeclarativeEnvironmentRecord(vm_, old_env);
    
  // 4. Call the CreateMutableBinding concrete method of catchEnv passing the Identifier String value as the argument.
  auto ident_name = factory->NewStringFromTable(catch_name->AsIdentifier()->GetName());
  EnvironmentRecord::CreateMutableBinding(vm_, catch_env->GetEnvRec(), ident_name, false);
    
  // 5. Call the SetMutableBinding concrete method of catchEnv passing the Identifier, C,
  //    and false as arguments. Note that the last argument is immaterial in this situation.
  EnvironmentRecord::SetMutableBinding(vm_, catch_env->GetEnvRec(), ident_name, C, false);
    
  // 6. Set the running execution context’s LexicalEnvironment to catchEnv.
  vm_->GetExecutionContext()->SetLexicalEnvironment(catch_env);
    
  // 7. Let B be the result of evaluating Block.
  auto B = EvalStatement(catch_block);
  RETURN_COMPLETION_IF_HAS_EXCEPTION(vm_);
  
  // 8. Set the running execution context’s LexicalEnvironment to oldEnv.
  vm_->GetExecutionContext()->SetLexicalEnvironment(old_env);
    
  // 9. Return B.
  return B;
}

// EvalObjectLiteral
// Defined in ECMAScript 5.1 Chapter 11.1.5
JSValue Interpreter::EvalObjectLiteral(ObjectLiteral* object) {
  auto factory = vm_->GetObjectFactory();
  
  const auto& props = object->GetProperties();
  
  // ObjectLiteral : { }
  // 1. Return a new object created as if by the expression
  //    new Object() where Object is the standard built-in constructor with that name.
  if (props.empty()) {
    return JSValue(JSObject::Construct(vm_, JSValue{}));
  }

  // ObjectLiteral : { PropertyNameAndValueList }
  // 1. Return the result of evaluating PropertyNameAndValueList.
  return EvalPropertyNameAndValueList(props);
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
  if (utils::CanDoubleConvertToInt32(num->GetDouble())) {
    return JSValue(num->GetInt32());
  } else {
    return JSValue(num->GetDouble());
  }
}

// Eval StringLiteral
// Defined in ECMAScript 5.1 Chapter 11.1
JSValue Interpreter::EvalStringLiteral(StringLiteral* str) {
  return JSValue(vm_->GetObjectFactory()->NewStringFromTable(str->GetString()));
}

// Eval Identifier
// Defined in ECMAScript 5.1 Chapter 11.1.2
Reference Interpreter::EvalIdentifier(Identifier* ident) {
  return IdentifierResolution(vm_->GetObjectFactory()->NewStringFromTable(ident->GetName()));
}

// EvalSourceElements
// Defined in ECMAScript 5.1 Chapter 14
Completion Interpreter::EvalSourceElements(const Statements &stmts) {
  // 1. Let headResult be the result of evaluating SourceElements.
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
    head_result = Completion{
      tail_result.GetType(),
      tail_result.GetValue().IsEmpty() ? head_result.GetValue() : tail_result.GetValue(),
      tail_result.GetTarget()};
  }

  // 5. Return (tailResult.type, V, tailResult.target)
  return head_result;
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

    // If an exception was thrown, return (throw, V, empty) where V is the exception.
    // (Execution now proceeds as if no exception were thrown.)
    // todo
    RETURN_COMPLETION_IF_HAS_EXCEPTION(vm_);

    sl = Completion{s.GetType(), s.GetValue().IsEmpty() ? sl.GetValue() : s.GetValue(), s.GetTarget()};

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
    RETURN_VOID_IF_HAS_EXCEPTION(vm_);
  }
}

// Eval VariableDeclaration
// Defined in ECMASCript 5.1 Chapter 12.2
JSValue Interpreter::EvalVariableDeclaration(VariableDeclaration* decl) {
  // 1. Let lhs be the result of evaluating Identifier as described in 11.1.2.
  auto lhs = EvalExpression(decl->GetIdentifier());
  RETURN_VALUE_IF_HAS_EXCEPTION(vm_, JSValue{});

  // 2. Let rhs be the result of evaluating Initialiser.
  if (!decl->GetInitializer()) {
    return JSValue(vm_->GetObjectFactory()->NewStringFromTable(decl->GetIdentifier()->AsIdentifier()->GetName()));
  }
  auto rhs = EvalExpression(decl->GetInitializer());
  RETURN_VALUE_IF_HAS_EXCEPTION(vm_, JSValue{});

  // 3. Let value be GetValue(rhs).
  auto value = GetValue(rhs);
  RETURN_VALUE_IF_HAS_EXCEPTION(vm_, JSValue{});

  // 4. Call PutValue(lhs, value).
  PutValue(lhs, value);
  RETURN_VALUE_IF_HAS_EXCEPTION(vm_, JSValue{});

  // 5. Return a String value containing the same sequence of characters as in the Identifier.
  return JSValue(vm_->GetObjectFactory()->NewStringFromTable(decl->GetIdentifier()->AsIdentifier()->GetName()));
}

// EvalPropertyNameAndValueList
// Defined in ECMAScript 5.1 Chapter 11.1.5
JSValue Interpreter::EvalPropertyNameAndValueList(const ast::Properties& props) {
  auto factory = vm_->GetObjectFactory();
  
  // PropertyNameAndValueList : PropertyNameAndValueList , PropertyAssignment

  // 1. Let obj be the result of evaluating PropertyNameAndValueList.
  auto obj = JSObject::Construct(vm_, JSValue{});

  for (auto prop : props) {
    // 2. Let propId be the result of evaluating PropertyAssignment.
    auto [prop_id_name, prop_id_desc] = EvalPropertyAssignment(prop);
    RETURN_VALUE_IF_HAS_EXCEPTION(vm_, JSValue{});
    
    // 3. Let previous be the result of calling the [[GetOwnProperty]] internal method of obj with argument propId.name.
    auto previous = Object::GetOwnProperty(vm_, obj, prop_id_name);
    
    // 4. If previous is not undefined then throw a SyntaxError exception if any of the following conditions are true
    if (!previous.IsEmpty()) {
      // a. This production is contained in strict code and IsDataDescriptor(previous) is true and
      //    IsDataDescriptor(propId.descriptor) is true.
      // b. IsDataDescriptor(previous) is true and IsAccessorDescriptor(propId.descriptor) is true.
      // c. IsAccessorDescriptor(previous) is true and IsDataDescriptor(propId.descriptor) is true.
      // d. IsAccessorDescriptor(previous) is true and IsAccessorDescriptor(propId.descriptor) is true and
      //    either both previous and propId.descriptor have [[Get]] fields or
      //    both previous and propId.descriptor have [[Set]] fields
      // todo
    }

    // 5. Call the [[DefineOwnProperty]] internal method of obj with arguments propId.name, propId.descriptor, and false.
    Object::DefineOwnProperty(vm_, obj, prop_id_name, prop_id_desc, false);
    RETURN_VALUE_IF_HAS_EXCEPTION(vm_, JSValue{});
  }
  
  // 6. Return obj.
  return JSValue(obj);
}

// EvalPropertyAssignment
// Defined in ECMAScript 5.1 Chapter 11.1.5
std::pair<String*, PropertyDescriptor> Interpreter::EvalPropertyAssignment(ast::Property* prop) {
  auto factory = vm_->GetObjectFactory();
  
  if (prop->GetPropertyType() == PropertyType::INIT) {
    // PropertyAssignment : PropertyName : AssignmentExpression
    
    // 1. Let propName be the result of evaluating PropertyName.
    auto prop_name = std::invoke([vm = vm_, factory](Expression* name) {
      if (name->IsIdentifier()) {
        return factory->NewStringFromTable(name->AsIdentifier()->GetName());
      } else if (name->IsNumericLiteral()) {
        // todo
        return JSValue::NumberToString(vm, name->AsNumericLiteral()->GetNumber<double>());
      } else {
        // name.IsStringLiteral must be true
        return factory->NewStringFromTable(name->AsStringLiteral()->GetString());
      }
    }, prop->GetKey());
    
    // 2. Let exprValue be the result of evaluating AssignmentExpression.
    auto expr_value = EvalExpression(prop->GetValue());
    RETURN_VALUE_IF_HAS_EXCEPTION(vm_, std::make_pair(nullptr, PropertyDescriptor{}));
    
    // 3. Let propValue be GetValue(exprValue).
    auto prop_value = GetValue(expr_value);
    RETURN_VALUE_IF_HAS_EXCEPTION(vm_, std::make_pair(nullptr, PropertyDescriptor{}));
    
    // 4. Let desc be the Property Descriptor{[[Value]]: propValue, [[Writable]]: true, [[Enumerable]]: true, [[Configurable]]: true}
    auto desc = PropertyDescriptor(prop_value, true, true, true);
    
    // 5. Return Property Identifier (propName, desc).
    return std::make_pair(prop_name, desc);
  } else if (prop->GetPropertyType() == PropertyType::GET) {
    // PropertyAssignment : PropertyName : AssignmentExpression

    // 1. Let propName be the result of evaluating PropertyName.
    auto prop_name = EvalExpression(prop->GetKey());
    RETURN_VALUE_IF_HAS_EXCEPTION(vm_, std::make_pair(nullptr, PropertyDescriptor{}));
    
    // 2. Let closure be the result of creating a new Function object as specified in 13.2
    //    with an empty parameter list and body specified by FunctionBody.
    //    Pass in the LexicalEnvironment of the running execution context as the Scope.
    //    Pass in true as the Strict flag if the PropertyAssignment is contained in strict code or if its FunctionBody is strict code.
    // 3. Let desc be the Property Descriptor{[[Get]]: closure, [[Enumerable]]: true, [[Configurable]]: true}
    // 4. Return Property Identifier (propName, desc).
  } else {
    // PropertyAssignment : set PropertyName ( PropertySetParameterList ) { FunctionBody }

    // 1. Let propName be the result of evaluating PropertyName.
    // 2. Let closure be the result of creating a new Function object as specified in 13.2 with parameters specified by PropertySetParameterList and body specified by FunctionBody. Pass in the LexicalEnvironment of the running execution context as the Scope. Pass in true as the Strict flag if the PropertyAssignment is contained in strict code or if its FunctionBody is strict code.
    // 3. Let desc be the Property Descriptor{[[Set]]: closure, [[Enumerable]]: true, [[Configurable]]: true}
    // 4. Return Property Identifier (propName, desc).
  }
  
}

// ApplyCompoundAssignment
JSValue Interpreter::ApplyCompoundAssignment(TokenType op, JSValue lval, JSValue rval) {
  switch (op) {
    case TokenType::MUL_ASSIGN: {
      auto lnum = JSValue::ToNumber(vm_, lval);
      auto rnum = JSValue::ToNumber(vm_, rval);
      return JSValue(lnum * rnum);
    }
    case TokenType::DIV_ASSIGN: {
      auto lnum = JSValue::ToNumber(vm_, lval);
      auto rnum = JSValue::ToNumber(vm_, rval);
      return JSValue(lnum / rnum);
    }
    case TokenType::ADD_ASSIGN: {
      auto lprim = JSValue::ToPrimitive(vm_, lval, PreferredType::NUMBER);
      auto rprim = JSValue::ToPrimitive(vm_, rval, PreferredType::NUMBER);
    
      if (lprim.IsString() || rprim.IsString()) {
        return JSValue(String::Concat(vm_,
                                      lprim.GetHeapObject()->AsString(),
                                      rprim.GetHeapObject()->AsString()));
      } else {
        return JSValue::ToNumber(vm_, lprim) + JSValue::ToNumber(vm_, rprim);
      }
    }
    case TokenType::SUB_ASSIGN: {
      auto lnum = JSValue::ToNumber(vm_, lval);
      auto rnum = JSValue::ToNumber(vm_, rval);
      return JSValue(lnum - rnum);
    }
    case TokenType::LEFT_SHIFT_ASSIGN: {
      auto lnum = JSValue::ToInt32(vm_, lval);
      auto rnum = JSValue::ToUint32(vm_, rval);
      auto shift_count = rnum & 0x1F;
      
      return JSValue(lnum << shift_count);
    }
    case TokenType::RIGHT_SHIFT_ASSIGN: {
      auto lnum = JSValue::ToInt32(vm_, lval);
      auto rnum = JSValue::ToUint32(vm_, rval);
      auto shift_count = rnum & 0x1F;
      
      return JSValue(lnum >> shift_count);
    }
    case TokenType::U_RIGHT_SHIFT_ASSIGN: {
      auto lnum = JSValue::ToUint32(vm_, lval);
      auto rnum = JSValue::ToUint32(vm_, rval);
      auto shift_count = rnum & 0x1F;
      
      return JSValue(lnum >> shift_count);
    }
    case TokenType::BIT_AND_ASSIGN: {
      auto lnum = JSValue::ToInt32(vm_, lval);
      auto rnum = JSValue::ToInt32(vm_, rval);

      return JSValue(lnum & rnum);
    }
    case TokenType::BIT_XOR_ASSIGN: {
      auto lnum = JSValue::ToInt32(vm_, lval);
      auto rnum = JSValue::ToInt32(vm_, rval);

      return JSValue(lnum ^ rnum);
    }
    case TokenType::BIT_OR_ASSIGN: {
      auto lnum = JSValue::ToInt32(vm_, lval);
      auto rnum = JSValue::ToInt32(vm_, rval);

      return JSValue(lnum | rnum);
    }
    default: {
      // unreachable branch
      // todo
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
    RETURN_VALUE_IF_HAS_EXCEPTION(vm_, JSValue{});
    
    // 2. Let lval be GetValue(lref).
    auto lval = GetValue(lref);
    RETURN_VALUE_IF_HAS_EXCEPTION(vm_, JSValue{});
    
    // 3. If ToBoolean(lval) is false, return lval.
    if (!JSValue::ToBoolean(vm_, lval)) {
      return lval;
    }
    
    // 4. Let rref be the result of evaluating BitwiseORExpression.
    auto rref = EvalExpression(right);
    RETURN_VALUE_IF_HAS_EXCEPTION(vm_, JSValue{});
    
    // 5. Return GetValue(rref).
    return GetValue(rref);
  } else {
    // op must be TokenType::Logical_OR

    // 1. Let lref be the result of evaluating LogicalORExpression.
    auto lref = EvalExpression(left);
    RETURN_VALUE_IF_HAS_EXCEPTION(vm_, JSValue{});
    
    // 2. Let lval be GetValue(lref).
    auto lval = GetValue(lref);
    RETURN_VALUE_IF_HAS_EXCEPTION(vm_, JSValue{});
    
    // 3. If ToBoolean(lval) is true, return lval.
    if (JSValue::ToBoolean(vm_, lval)) {
      return lval;
    }
    
    // 4. Let rref be the result of evaluating LogicalANDExpression.
    auto rref = EvalExpression(right);
    RETURN_VALUE_IF_HAS_EXCEPTION(vm_, JSValue{});
    
    // 5. Return GetValue(rref).
    return GetValue(rref);
  }
}

// ApplyBitwiseOperator
// Defined in ECMAScript 5.1 Chapter 11.10
JSValue Interpreter::ApplyBitwiseOperator(TokenType op, Expression* left, Expression* right) {
  // 1. Let lref be the result of evaluating A.
  auto lref = EvalExpression(left);
  RETURN_VALUE_IF_HAS_EXCEPTION(vm_, JSValue{});
  
  // 2. Let lval be GetValue(lref).
  auto lval = GetValue(lref);
  RETURN_VALUE_IF_HAS_EXCEPTION(vm_, JSValue{});
  
  // 3. Let rref be the result of evaluating B.
  auto rref = EvalExpression(right);
  RETURN_VALUE_IF_HAS_EXCEPTION(vm_, JSValue{});
  
  // 4. Let rval be GetValue(rref).
  auto rval = GetValue(rref);
  RETURN_VALUE_IF_HAS_EXCEPTION(vm_, JSValue{});
  
  // 5. Let lnum be ToInt32(lval).
  auto lnum = JSValue::ToInt32(vm_, lval);
  
  // 6. Let rnum be ToInt32(rval).
  auto rnum = JSValue::ToInt32(vm_, rval);
  
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

// ApplyEqualityOperator
// Defined in ECMAScript 5.1 Chapter 11.9
JSValue Interpreter::ApplyEqualityOperator(TokenType op, Expression* left, Expression* right) {
  // EqualityExpression op RelationalExpression

  // 1. Let lref be the result of evaluating EqualityExpression.
  auto lref = EvalExpression(left);
  RETURN_VALUE_IF_HAS_EXCEPTION(vm_, JSValue{});
  
  // 2. Let lval be GetValue(lref).
  auto lval = GetValue(lref);
  RETURN_VALUE_IF_HAS_EXCEPTION(vm_, JSValue{});
  
  // 3. Let rref be the result of evaluating RelationalExpression.
  auto rref = EvalExpression(right);
  RETURN_VALUE_IF_HAS_EXCEPTION(vm_, JSValue{});
  
  // 4. Let rval be GetValue(rref).
  auto rval = GetValue(rref);
  RETURN_VALUE_IF_HAS_EXCEPTION(vm_, JSValue{});

  if (op == TokenType::EQUAL) {
    return JSValue(AbstractEqualityComparison(lval, rval));
  } else if (op == TokenType::NOT_EQUAL) {
    return JSValue(!AbstractEqualityComparison(lval, rval));
  } else if (op == TokenType::STRICT_EQUAL) {
    return JSValue(StrictEqualityComparison(lval, rval));
  } else {
    // op must be TokenType::NOT_STRICT_EQUAL
    return JSValue(!StrictEqualityComparison(lval, rval));
  }
}

// ApplyRelationalOperator
// Defined in ECMAScript 5.1 Chapter 11.8
JSValue Interpreter::ApplyRelationalOperator(TokenType op, Expression* left, Expression* right) {
  // RelationalExpression op ShiftExpression

  if (op != TokenType::KEYWORD_INSTANCEOF && op != TokenType::KEYWORD_IN) {
    // 1. Let lref be the result of evaluating RelationalExpression.
    auto lref = EvalExpression(left);
    RETURN_VALUE_IF_HAS_EXCEPTION(vm_, JSValue{});
    
    // 2. Let lval be GetValue(lref).
    auto lval = GetValue(lref);
    RETURN_VALUE_IF_HAS_EXCEPTION(vm_, JSValue{});
    
    // 3. Let rref be the result of evaluating ShiftExpression.
    auto rref = EvalExpression(right);
    RETURN_VALUE_IF_HAS_EXCEPTION(vm_, JSValue{});
    
    // 4. Let rval be GetValue(rref).
    auto rval = GetValue(rref);
    RETURN_VALUE_IF_HAS_EXCEPTION(vm_, JSValue{});
    
    // 5. Let r be the result of performing abstract relational comparison lval op rval. (see 11.8.5)
    if (op == TokenType::LESS_THAN) {
      auto r = AbstractRelationalComparison(lval, rval, true);
      return r.IsUndefined() ? JSValue::False() : r;
    } else if (op == TokenType::GREATER_THAN) {
      auto r = AbstractRelationalComparison(rval, lval, false);
      return r.IsUndefined() ? JSValue::False() : r;
    } else if (op == TokenType::LESS_EQUAL) {
      auto r = AbstractRelationalComparison(rval, lval, false);
      return r.IsTrue() || r.IsUndefined() ? JSValue::False() : JSValue::True();
    } else {
      // op must be TokenType::GREAT_EQUAL
      auto r = AbstractRelationalComparison(lval, rval, true);
      return r.IsTrue() || r.IsUndefined() ? JSValue::False() : JSValue::True();
    }
  } else if (op == TokenType::KEYWORD_INSTANCEOF) {
    // 1. Let lref be the result of evaluating RelationalExpression.
    auto lref = EvalExpression(left);
    RETURN_VALUE_IF_HAS_EXCEPTION(vm_, JSValue{});
    
    // 2. Let lval be GetValue(lref).
    auto lval = GetValue(lref);
    RETURN_VALUE_IF_HAS_EXCEPTION(vm_, JSValue{});
    
    // 3. Let rref be the result of evaluating ShiftExpression.
    auto rref = EvalExpression(right);
    RETURN_VALUE_IF_HAS_EXCEPTION(vm_, JSValue{});
    
    // 4. Let rval be GetValue(rref).
    auto rval = GetValue(rref);
    RETURN_VALUE_IF_HAS_EXCEPTION(vm_, JSValue{});
    
    // 5. If Type(rval) is not Object, throw a TypeError exception.
    if (!rval.IsObject()) {
      THROW_TYPE_ERROR_AND_RETURN_VALUE(
        vm_, u"instanceof cannot operate on primitive value.", JSValue::False());
    }
    
    // 6. If rval does not have a [[HasInstance]] internal method, throw a TypeError exception.
    // todo
    
    // 7. Return the result of calling the [[HasInstance]] internal method of rval with argument lval.
    // todo
  } else {
    // op must be TokenType::KEYWORD_IN
    
    // 1. Let lref be the result of evaluating RelationalExpression.
    auto lref = EvalExpression(left);
    RETURN_VALUE_IF_HAS_EXCEPTION(vm_, JSValue{});
    
    // 2. Let lval be GetValue(lref).
    auto lval = GetValue(lref);
    RETURN_VALUE_IF_HAS_EXCEPTION(vm_, JSValue{});
    
    // 3. Let rref be the result of evaluating ShiftExpression.
    auto rref = EvalExpression(right);
    RETURN_VALUE_IF_HAS_EXCEPTION(vm_, JSValue{});
    
    // 4. Let rval be GetValue(rref).
    auto rval = GetValue(rref);
    RETURN_VALUE_IF_HAS_EXCEPTION(vm_, JSValue{});
    
    // 5. If Type(rval) is not Object, throw a TypeError exception.
    if (!rval.IsObject()) {
      THROW_TYPE_ERROR_AND_RETURN_VALUE(
        vm_, u"in operator cannot operate on primitive value.", JSValue::False());
    }
    
    // 6. Return the result of calling the [[HasProperty]] internal method of rval with argument ToString(lval).
    // todo
  }
}

// ApplyShiftOperator
// Defind in ECMAScript 5.1 Chapter 11.7
JSValue Interpreter::ApplyShiftOperator(TokenType op, Expression* left, Expression* right) {
  // ShiftExpression op AdditiveExpression

  // 1. Let lref be the result of evaluating ShiftExpression.
  auto lref = EvalExpression(left);
  RETURN_VALUE_IF_HAS_EXCEPTION(vm_, JSValue{});
  
  // 2. Let lval be GetValue(lref).
  auto lval = GetValue(lref);
  RETURN_VALUE_IF_HAS_EXCEPTION(vm_, JSValue{});
  
  // 3. Let rref be the result of evaluating AdditiveExpression.
  auto rref = EvalExpression(right);
  RETURN_VALUE_IF_HAS_EXCEPTION(vm_, JSValue{});
  
  // 4. Let rval be GetValue(rref).
  auto rval = GetValue(rref);
  RETURN_VALUE_IF_HAS_EXCEPTION(vm_, JSValue{});
  
  // 5. Let lnum be ToInt32(lval).
  auto lnum = JSValue::ToInt32(vm_, lval);
  
  // 6. Let rnum be ToUint32(rval).
  auto rnum = JSValue::ToUint32(vm_, rval);

  // 7. Let shiftCount be the result of masking out all but the least significant 5 bits of rnum,
  //    that is, compute rnum & 0x1F.
  auto shift_count = rnum & 0x1F;

  if (op == TokenType::LEFT_SHIFT) {
    // Return the result of left shifting lnum by shiftCount bits.
    // The result is a signed 32-bit integer.
    return JSValue(lnum << shift_count);
  } else if (op == TokenType::RIGHT_SHIFT) {
    // Return the result of performing a sign-extending right shift of lnum by shiftCount bits.
    // The most significant bit is propagated. The result is a signed 32-bit integer.
    return JSValue(lnum >> shift_count);
  } else {
    // op must be TokenType::U_RIGHT_SHIFT

    // Return the result of performing a zero-filling right shift of lnum by shiftCount bits.
    // Vacated bits are filled with zero. The result is an unsigned 32-bit integer.
    // todo
    return JSValue(JSValue::ToUint32(vm_, lval) >> shift_count);
  }
}

// ApplyAdditiveOperator
JSValue Interpreter::ApplyAdditiveOperator(TokenType op, Expression* left, Expression* right) {
  // AdditiveExpression op MultiplicativeExpression

  if (op == TokenType::ADD) {
    // 1. Let lref be the result of evaluating AdditiveExpression.
    auto lref = EvalExpression(left);
    RETURN_VALUE_IF_HAS_EXCEPTION(vm_, JSValue{});
    
    // 2. Let lval be GetValue(lref).
    auto lval = GetValue(lref);
    RETURN_VALUE_IF_HAS_EXCEPTION(vm_, JSValue{});
    
    // 3. Let rref be the result of evaluating MultiplicativeExpression.
    auto rref = EvalExpression(right);
    RETURN_VALUE_IF_HAS_EXCEPTION(vm_, JSValue{});
    
    // 4. Let rval be GetValue(rref).
    auto rval = GetValue(rref);
    RETURN_VALUE_IF_HAS_EXCEPTION(vm_, JSValue{});
    
    // 5. Let lprim be ToPrimitive(lval).
    auto lprim = JSValue::ToPrimitive(vm_, lval, PreferredType::NUMBER);
    
    // 6. Let rprim be ToPrimitive(rval).
    auto rprim = JSValue::ToPrimitive(vm_, rval, PreferredType::NUMBER);
    
    // 7. If Type(lprim) is String or Type(rprim) is String, then
    if (lprim.IsString() || rprim.IsString()) {
      // a. Return the String that is the result of
      //    concatenating ToString(lprim) followed by ToString(rprim)
      return JSValue(String::Concat(vm_,
                                    lprim.GetHeapObject()->AsString(),
                                    rprim.GetHeapObject()->AsString()));
    }
    // 8. Return the result of applying the addition operation to ToNumber(lprim) and ToNumber(rprim).
    //    See the Note below 11.6.3 (under IEE754 rules).
    else {
      return JSValue::ToNumber(vm_, lprim) + JSValue::ToNumber(vm_, rprim);
    }
  } else {
    // op must be TokenType::SUB

    // 1. Let lref be the result of evaluating AdditiveExpression.
    auto lref = EvalExpression(left);
    RETURN_VALUE_IF_HAS_EXCEPTION(vm_, JSValue{});
      
    // 2. Let lval be GetValue(lref).
    auto lval = GetValue(lref);
    RETURN_VALUE_IF_HAS_EXCEPTION(vm_, JSValue{});
      
    // 3. Let rref be the result of evaluating MultiplicativeExpression.
    auto rref = EvalExpression(right);
    RETURN_VALUE_IF_HAS_EXCEPTION(vm_, JSValue{});
      
    // 4. Let rval be GetValue(rref).
    auto rval = GetValue(rref);
    RETURN_VALUE_IF_HAS_EXCEPTION(vm_, JSValue{});
      
    // 5. Let lnum be ToNumber(lval).
    auto lnum = JSValue::ToNumber(vm_, lval);
      
    // 6. Let rnum be ToNumber(rval).
    auto rnum = JSValue::ToNumber(vm_, rval);
      
    // 7. Return the result of applying the subtraction operation to lnum and rnum.
    //    See the note below 11.6.3 (under IEE754 rules).
    return lnum - rnum;
  }
}

// AppplyMultiplicativeOperator
// Defined in ECMAScript 5.1 Chapter 11.5
JSValue Interpreter::ApplyMultiplicativeOperator(TokenType op, Expression* left, Expression* right) {
  // MultiplicativeExpression * UnaryExpression

  // 1. Let left be the result of evaluating MultiplicativeExpression.
  auto lref = EvalExpression(left);
  RETURN_VALUE_IF_HAS_EXCEPTION(vm_, JSValue{});
  
  // 2. Let leftValue be GetValue(left).
  auto lval = GetValue(lref);
  RETURN_VALUE_IF_HAS_EXCEPTION(vm_, JSValue{});
  
  // 3. Let right be the result of evaluating UnaryExpression.
  auto rref = EvalExpression(right);
  RETURN_VALUE_IF_HAS_EXCEPTION(vm_, JSValue{});
  
  // 4. Let rightValue be GetValue(right).
  auto rval = GetValue(rref);
  RETURN_VALUE_IF_HAS_EXCEPTION(vm_, JSValue{});
  
  // 5. Let leftNum be ToNumber(leftValue).
  auto lnum = JSValue::ToNumber(vm_, lval);
  
  // 6. Let rightNum be ToNumber(rightValue).
  auto rnum = JSValue::ToNumber(vm_, rval);
  
  // 7. Return the result of applying the specified operation (*, /, or %) to leftNum and rightNum.
  //    See the Notes below 11.5.1, 11.5.2, 11.5.3.
  if (op == TokenType::MUL) {
    return lnum * rnum;
  } else if (op == TokenType::DIV) {
    return lnum / rnum;
  } else {
    // op must be TokenType::MOD
    if (lnum.IsInt() && rnum.IsInt()) {
      return JSValue(lnum.GetInt() % rnum.GetInt());
    } else {
      return JSValue(std::fmod(lnum.GetNumber(), rnum.GetNumber()));
    }
  }
}

// ApplyUnaryOperator
// todo
JSValue Interpreter::ApplyUnaryOperator(TokenType op, Expression* expr) {
  auto factory = vm_->GetObjectFactory();
  
  auto ref = EvalExpression(expr);
  RETURN_VALUE_IF_HAS_EXCEPTION(vm_, JSValue{});
  
  switch (op) {
    case TokenType::KEYWORD_DELETE: {
      // 1. Let ref be the result of evaluating UnaryExpression.
      // 2. If Type(ref) is not Reference, return true.
      if (!std::get_if<Reference>(&ref)) {
        return JSValue::True();
      }
      auto pref = std::get<Reference>(ref);
      
      // 3. If IsUnresolvableReference(ref) then,
      if (pref.IsUnresolvableReference()) {
        // a. If IsStrictReference(ref) is true, throw a SyntaxError exception.
        if (pref.IsStrictReference()) {
          THROW_SYNTAX_ERROR_AND_RETURN_VALUE(
            vm_, u"Cannot delete.", JSValue::False());
        }
        // b. Else, return true.
        else {
          return JSValue::True();
        }
      }
      
      // 4. If IsPropertyReference(ref) is true, then
      if (pref.IsPropertyReference()) {
        // a. Return the result of calling the [[Delete]] internal method on
        // ToObject(GetBase(ref)) providing GetReferencedName(ref) and IsStrictReference(ref) as the arguments.
        auto ret = Object::Delete(vm_, JSValue::ToObject(vm_, std::get<JSValue>(pref.GetBase())),
                                  pref.GetReferencedName(), pref.IsStrictReference());
        RETURN_VALUE_IF_HAS_EXCEPTION(vm_, JSValue{ret});
        return JSValue{ret};
      }
      // 5. Else, ref is a Reference to an Environment Record binding, so
      else {
        // a. If IsStrictReference(ref) is true, throw a SyntaxError exception.
        if (pref.IsStrictReference()) {
          THROW_SYNTAX_ERROR_AND_RETURN_VALUE(vm_, u"Cannot delete.", JSValue::False());
        }
        
        // b. Let bindings be GetBase(ref).
        auto bindings = std::get<EnvironmentRecord*>(pref.GetBase());
        
        // c. Return the result of calling the DeleteBinding concrete method of bindings,
        //    providing GetReferencedName(ref) as the argument.
        return JSValue{EnvironmentRecord::DeleteBinding(vm_, bindings, pref.GetReferencedName())};
      }
    }
    case TokenType::KEYWORD_VOID: {
      // 1. Call GetValue(expr)
      GetValue(ref);
      RETURN_VALUE_IF_HAS_EXCEPTION(vm_, JSValue{});

      // 2. Return undefined
      return JSValue::Undefined();
    }
    case TokenType::KEYWORD_TYPEOF: {
      // 1. If Type(val) is Reference, then
      if (auto pref = std::get_if<Reference>(&ref)) {
        // a. If IsUnresolvableReference(val) is true, return "undefined".
        if (pref->IsUnresolvableReference()) {
          return JSValue(vm_->GetObjectFactory()->NewStringFromTable(u"undefined"));
        }
        // b. Let val be GetValue(val).
      }
      auto val = GetValue(ref);
      
      // 4. Return a String determined by Type(val) according to Table 20.
      std::u16string_view str;
      if (val.IsUndefined()) {
        str = u"undefined";
      } else if (val.IsNull()) {
        str = u"null";
      } else if (val.IsBoolean()) {
        str = u"boolean";
      } else if (val.IsNumber()) {
        str = u"number";
      } else if (val.IsString()) {
        str = u"string";
      } else if (val.IsObject() && !val.GetHeapObject()->GetCallable()) {
        str = u"object";
      } else if (val.IsObject() && val.GetHeapObject()->GetCallable()) {
        str = u"function";
      }
      return JSValue{factory->NewStringFromTable(str)};
    }
    case TokenType::INC: {
      // 1. Throw a SyntaxError exception if the following conditions are all true:
      //      Type(expr) is Reference is true
      //      IsStrictReference(expr) is true
      //      Type(GetBase(expr)) is Environment Record
      //      GetReferencedName(expr) is either "eval" or "arguments"
      if (auto plref = std::get_if<Reference>(&ref);
          plref                                                   &&
          plref->IsStrictReference()                              &&
          std::get_if<EnvironmentRecord*>(&(plref->GetBase()))    &&
          (plref->GetReferencedName()->Equal(u"eval")     ||
           plref->GetReferencedName()->Equal(u"arguments"))) {
        if (plref->GetReferencedName()->Equal(u"eval")) {
          THROW_SYNTAX_ERROR_AND_RETURN_VALUE(vm_, u"Incorrect use of eval.", JSValue{});
        } else {
          THROW_SYNTAX_ERROR_AND_RETURN_VALUE(vm_, u"Incorrect use of arguments.", JSValue{});
        }
      }

      // 2. Let oldValue be ToNumber(GetValue(expr)).
      auto old_val = JSValue::ToNumber(vm_, GetValue(ref));

      // 3. Let newValue be the result of adding the value 1 to oldValue,
      //    using the same rules as for the + operator (see 11.6.3).
      auto new_val = ++old_val;

      // 4. Call PutValue(expr, newValue).
      PutValue(ref, new_val);
      RETURN_VALUE_IF_HAS_EXCEPTION(vm_, JSValue{});

      // 5. Return newValue.
      return new_val;
    }
    case TokenType::DEC: {
      // 1. Throw a SyntaxError exception if the following conditions are all true:
      //      Type(expr) is Reference is true
      //      IsStrictReference(expr) is true
      //      Type(GetBase(expr)) is Environment Record
      //      GetReferencedName(expr) is either "eval" or "arguments"
      if (auto plref = std::get_if<Reference>(&ref);
          plref                                                   &&
          plref->IsStrictReference()                              &&
          std::get_if<EnvironmentRecord*>(&(plref->GetBase()))    &&
          (plref->GetReferencedName()->Equal(u"eval")     ||
           plref->GetReferencedName()->Equal(u"arguments"))) {
        if (plref->GetReferencedName()->Equal(u"eval")) {
          THROW_SYNTAX_ERROR_AND_RETURN_VALUE(vm_, u"Incorrect use of eval.", JSValue{});
        } else {
          THROW_SYNTAX_ERROR_AND_RETURN_VALUE(vm_, u"Incorrect use of arguments.", JSValue{});
        }
      }

      // 2. Let oldValue be ToNumber(GetValue(expr)).
      auto old_val = JSValue::ToNumber(vm_, GetValue(ref));

      // 3. Let newValue be the result of subtracting the value 1 to oldValue,
      //    using the same rules as for the + operator (see 11.6.3).
      auto new_val = --old_val;

      // 4. Call PutValue(expr, newValue).
      PutValue(ref, new_val);
      RETURN_VALUE_IF_HAS_EXCEPTION(vm_, JSValue{});

      // 5. Return newValue.
      return new_val;
    }
    case TokenType::ADD: {
      // 1. Return ToNumber(GetValue(expr)).
      return JSValue::ToNumber(vm_, GetValue(ref));
    }
    case TokenType::SUB: {
      // 1. Let oldValue be ToNumber(GetValue(expr)).
      auto old_val = JSValue::ToNumber(vm_, GetValue(ref));
      
      // 2. If oldValue is NaN, return NaN.
      if (std::isnan(old_val.GetNumber())) {
        return old_val;
      }
      
      // 3. Return the result of negating oldValue;
      //    that is, compute a Number with the same magnitude but opposite sign.
      if (old_val.IsInt()) {
        return JSValue(-old_val.GetInt());
      } else {
        // old_val must be Double
        return JSValue(-old_val.GetDouble());
      }
    }
    case TokenType::BIT_NOT: {
      // 1. Let oldValue be ToInt32(GetValue(expr)).
      auto old_val = JSValue::ToInt32(vm_, GetValue(ref));
      
      // 2. Return the result of applying bitwise complement to oldValue.
      //    The result is a signed 32-bit integer.
      return JSValue(~old_val);
    }
    case TokenType::LOGICAL_AND: {
      // 1. Let oldValue be ToBoolean(GetValue(expr)).
      auto old_val = JSValue::ToBoolean(vm_, GetValue(ref));
      
      // 2. If oldValue is true, return false.
      // 3. Return true.
      return JSValue(old_val);
    }
    default: {
      return {};
    }
  }
}

// ApplyPostfixOperator
// Defined in ECMAScript 5.1 Chapter 11.3
JSValue Interpreter::ApplyPostfixOperator(TokenType op, Expression* expr) {
  // 1. Let lhs be the result of evaluating LeftHandSideExpression.
  auto lhs = EvalExpression(expr);
  RETURN_VALUE_IF_HAS_EXCEPTION(vm_, JSValue{});
  
  // 2. Throw a SyntaxError exception if the following conditions are all true:
  //      Type(lhs) is Reference is true
  //      IsStrictReference(lhs) is true
  //      Type(GetBase(lhs)) is Environment Record
  //      GetReferencedName(lhs) is either "eval" or "arguments"
  if (auto plref = std::get_if<Reference>(&lhs);
      plref                                                   &&
      plref->IsStrictReference()                              &&
      std::get_if<EnvironmentRecord*>(&(plref->GetBase()))    &&
      (plref->GetReferencedName()->Equal(u"eval")     ||
       plref->GetReferencedName()->Equal(u"arguments"))) {
    if (plref->GetReferencedName()->Equal(u"eval")) {
      THROW_SYNTAX_ERROR_AND_RETURN_VALUE(vm_, u"Incorrect use of eval.", JSValue{});
    } else {
      THROW_SYNTAX_ERROR_AND_RETURN_VALUE(vm_, u"Incorrect use of arguments.", JSValue{});
    }
  }
  
  // 3. Let oldValue be ToNumber(GetValue(lhs)).
  auto old_val = JSValue::ToNumber(vm_, GetValue(lhs));
  
  // 4. Let newValue be the result of adding the value 1 to oldValue, using the same rules as for the + operator (see 11.6.3).
  Number new_val;
  if (op == TokenType::INC) {
    new_val = ++old_val;
  } else {
    // op must be TokenType::DEC
    new_val = --old_val;
  }
  
  // 5. Call PutValue(lhs, newValue).
  PutValue(lhs, new_val);
  RETURN_VALUE_IF_HAS_EXCEPTION(vm_, JSValue{});
  
  // 6. Return oldValue.
  return old_val;
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
    vm_, vm_->GetExecutionContext()->GetLexicalEnvironment(), ident, strict);
}

// AbstractEqualityComparison
// Defined in ECMAScript 5.1 Chapter 11.9.3
bool Interpreter::AbstractEqualityComparison(JSValue x, JSValue y) {
  // 1. If Type(x) is the same as Type(y), then
  {
    // a. If Type(x) is Undefined, return true.
    if (x.IsUndefined() && y.IsUndefined()) {
      return true;
    }
    
    // b. If Type(x) is Null, return true.
    if (x.IsNull() && y.IsNull()) {
      return true;
    }
    
    // c.If Type(x) is Number, then
    if (x.IsNumber() && y.IsNumber()) {
      // i. If x is NaN, return false.
      // ii. If y is NaN, return false.
      if (std::isnan(x.GetNumber()) || std::isnan(y.GetNumber())) {
          return false;
      }
      
      // iii. If x is the same Number value as y, return true.
      // iv. If x is +0 and y is −0, return true.
      // v. If x is −0 and y is +0, return true.
      if (x.GetNumber() == y.GetNumber()) {
        return true;
      }
      
      // vi. Return false.
      return false;
    }
    
    // d. If Type(x) is String, then return true if x and y are exactly the same sequence of characters
    //    (same length and same characters in corresponding positions).
    //    Otherwise, return false.
    if (x.IsString() && y.IsString()) {
      return
        x.GetHeapObject()->AsString()->GetString() ==
        y.GetHeapObject()->AsString()->GetString();
    }
    
    // e. If Type(x) is Boolean, return true if x and y are both true or both false. Otherwise, return false.
    if (x.IsBoolean() && y.IsBoolean()) {
      return x.GetBoolean() == y.GetBoolean();
    }
    
    // f. Return true if x and y refer to the same object. Otherwise, return false.
    if (x.IsObject() && y.IsObject()) {
      return x.GetRawData() == y.GetRawData();
    }
  }

  // 2. If x is null and y is undefined, return true.
  if (x.IsNull() && y.IsUndefined()) {
    return true;
  }

  // 3. If x is undefined and y is null, return true.
  if (x.IsUndefined() && y.IsNull()) {
    return true;
  }

  // 4. If Type(x) is Number and Type(y) is String,
  //    return the result of the comparison x == ToNumber(y).
  if (x.IsNumber() && y.IsString()) {
    return AbstractEqualityComparison(x, JSValue::ToNumber(vm_, y));
  }

  // 5. If Type(x) is String and Type(y) is Number,
  //    return the result of the comparison ToNumber(x) == y.
  if (x.IsString() && y.IsNumber()) {
    return AbstractEqualityComparison(JSValue::ToNumber(vm_, x), y);
  }

  // 6. If Type(x) is Boolean, return the result of the comparison ToNumber(x) == y.
  if (x.IsBoolean()) {
    return AbstractEqualityComparison(JSValue::ToNumber(vm_, x), y);
  }

  // 7. If Type(y) is Boolean, return the result of the comparison x == ToNumber(y).
  if (y.IsBoolean()) {
    return AbstractEqualityComparison(x, JSValue::ToNumber(vm_, y));
  }

  // 8. If Type(x) is either String or Number and Type(y) is Object,
  //    return the result of the comparison x == ToPrimitive(y).
  if ((x.IsString() || x.IsNumber()) && y.IsObject()) {
    if (x.IsString()) {
      return AbstractEqualityComparison(x, JSValue::ToPrimitive(vm_, y, PreferredType::STRING));
    } else {
      return AbstractEqualityComparison(x, JSValue::ToPrimitive(vm_, y, PreferredType::NUMBER));
    }
  }

  // 9. If Type(x) is Object and Type(y) is either String or Number,
  //    return the result of the comparison ToPrimitive(x) == y.
  if (x.IsObject() && (y.IsString() || y.IsNumber())) {
    if (y.IsString()) {
      return AbstractEqualityComparison(JSValue::ToPrimitive(vm_, y, PreferredType::STRING), x);
    } else {
      return AbstractEqualityComparison(JSValue::ToPrimitive(vm_, y, PreferredType::NUMBER), x);
    }
  }

  // 10. Return false.
  return false;
}

// StrictEqualityComparision
bool Interpreter::StrictEqualityComparison(JSValue x, JSValue y) {
  // 1. If Type(x) is different from Type(y), return false.
  
  // 2. If Type(x) is Undefined, return true.
  if (x.IsUndefined() && y.IsUndefined()) {
    return true;
  }
  
  // 3. If Type(x) is Null, return true.
  if (x.IsNull() && y.IsNull()) {
    return true;
  }
  
  // 4. If Type(x) is Number, then
  if (x.IsNumber() && y.IsNumber()) {
    // a. If x is NaN, return false.
    // b. If y is NaN, return false.
    if (std::isnan(x.GetNumber()) || std::isnan(y.GetNumber())) {
      return false;
    }
    
    // c. If x is the same Number value as y, return true.
    // d. If x is +0 and y is −0, return true.
    // e. If x is −0 and y is +0, return true.
    // f. Return false.
    return x.GetNumber() == y.GetNumber();
  }
  
  // 5. If Type(x) is String,
  //    then return true if x and y are exactly the same sequence of characters
  //    (same length and same characters in corresponding positions);
  //    otherwise, return false.
  if (x.IsString() && y.IsString()) {
    return x.GetHeapObject()->AsString()->Equal(y.GetHeapObject()->AsString());
  }

  // 6. If Type(x) is Boolean, return true if x and y are both true or both false;
  //    otherwise, return false.
  if (x.IsBoolean() && y.IsBoolean()) {
    return x.GetBoolean() == y.GetBoolean();
  }

  // 7. Return true if x and y refer to the same object. Otherwise, return false.
  if (x.IsObject() && y.IsObject()) {
    return x.GetRawData() == y.GetRawData();
  }

  // If Type(x) is different from Type(y), return false.
  return false;
}

// AbstractRelationalComparison
// Defined in ECMAScript 5.1 Chapter 11.8.5
JSValue Interpreter::AbstractRelationalComparison(JSValue x, JSValue y, bool left_first) {
  // 1. If the LeftFirst flag is true, then
  JSValue px;
  JSValue py;
  if (left_first) {
    // a. Let px be the result of calling ToPrimitive(x, hint Number).
    px = JSValue::ToPrimitive(vm_, x, PreferredType::NUMBER);
    
    // b. Let py be the result of calling ToPrimitive(y, hint Number).
    py = JSValue::ToPrimitive(vm_, y, PreferredType::NUMBER);
  } 
  // 2.  Else the order of evaluation needs to be reversed to preserve left to right evaluation
  else {
    // a. Let py be the result of calling ToPrimitive(y, hint Number).
    py = JSValue::ToPrimitive(vm_, y, PreferredType::NUMBER);
    
    // b. Let px be the result of calling ToPrimitive(x, hint Number).
    px = JSValue::ToPrimitive(vm_, x, PreferredType::NUMBER);
  }

  // 3. If it is not the case that both Type(px) is String and Type(py) is String, then
  if (!px.IsString() || !py.IsString()) {
    // a. Let nx be the result of calling ToNumber(px).
    //    Because px and py are primitive values evaluation order is not important.
    auto nx = JSValue::ToNumber(vm_, px);
    
    // b. Let ny be the result of calling ToNumber(py).
    auto ny = JSValue::ToNumber(vm_, py);
    
    // c. If nx is NaN, return undefined.
    // d. If ny is NaN, return undefined.
    if (std::isnan(nx.GetNumber()) || std::isnan(ny.GetNumber())) {
      return JSValue::Undefined();
    }
    
    // e. If nx and ny are the same Number value, return false.
    // f. If nx is +0 and ny is −0, return false.
    // g. If nx is −0 and ny is +0, return false.
    if (nx.GetNumber() == ny.GetNumber()) {
      return JSValue::False();
    }
    
    // h. If nx is +∞, return false.
    // i. If ny is +∞, return true.
    // j. If ny is −∞, return false.
    // k. If nx is −∞, return true.
    if (std::isinf(nx.GetNumber()) && !std::signbit(nx.GetNumber())) {
      return JSValue::False();
    }
    if (std::isinf(ny.GetNumber()) && !std::signbit(ny.GetNumber())) {
      return JSValue::True();
    }
    if (std::isinf(ny.GetNumber()) && std::signbit(ny.GetNumber())) {
      return JSValue::False();
    }
    if (std::isinf(nx.GetNumber()) && std::signbit(nx.GetNumber())) {
      return JSValue::True();
    }
    
    // l. If the mathematical value of nx is less than the mathematical value of ny
    //    note that these mathematical values are both finite and not both zero
    //    return true. Otherwise, return false.
    return nx.GetNumber() < ny.GetNumber() ? JSValue::True() : JSValue::False();
  }
  // 4. Else, both px and py are Strings
  else {
    // a. If py is a prefix of px, return false.
    //    (A String value p is a prefix of String value q
    //    if q can be the result of concatenating p and some other String r.
    //    Note that any String is a prefix of itself, because r may be the empty String.)
    // b. If px is a prefix of py, return true.
    // c. Let k be the smallest nonnegative integer such that
    //    the character at position k within px is different from the character at position k within py.
    //    (There must be such a k, for neither String is a prefix of the other.)
    // d. Let m be the integer that is the code unit value for the character at position k within px.
    // e. Let n be the integer that is the code unit value for the character at position k within py.
    // f. If m < n, return true. Otherwise, return false.
    return
      px.GetHeapObject()->AsString()->GetString() <
      py.GetHeapObject()->AsString()->GetString()   ? JSValue::True() : JSValue::False();
  }
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
    THROW_REFERENCE_ERROR_AND_RETURN_VALUE(
      vm_, u"GetValue cannot work on unresolveable reference.", JSValue{});
  }

  // 4. If IsPropertyReference(V), then
  if (ref.IsPropertyReference()) {
    // a. If HasPrimitiveBase(V) is false,
    //    then let get be the [[Get]] internal method of base,
    //    otherwise let get be the special [[Get]] internal method defined below.
    // b. Return the result of calling the get internal method
    //    using base as its this value, and passing GetReferencedName(V) for the argument.
    auto base_val = std::get<JSValue>(base);
    if (!ref.HasPrimitiveBase()) {
      return Object::Get(vm_, base_val.GetHeapObject()->AsObject(), ref.GetReferencedName());
    } else {
      return GetUsedByGetValue(base_val, ref.GetReferencedName());
    }
  }
  // 5. Else, base must be an environment record.
  else { 
    // a. Return the result of calling the GetBindingValue concrete method of
    //    base passing GetReferencedName(V) and IsStrictReference(V) as arguments.
    auto env = std::get<EnvironmentRecord*>(base);
    return EnvironmentRecord::GetBindingValue(vm_, env, ref.GetReferencedName(), ref.IsStrictReference());
  }
}

// GetUsedByGetValue
// Defined in ECMAScript 5.1 Chapter 8.71.
JSValue Interpreter::GetUsedByGetValue(JSValue base, String* P) {
  // 1. Let O be ToObject(base).
  auto O = JSValue::ToObject(vm_, JSValue(base));
  
  // 2. Let desc be the result of calling the [[GetProperty]] internal method of O with property name P.
  auto desc = Object::GetProperty(vm_, O, P);
  
  // 3. If desc is undefined, return undefined.
  // todo
  if (desc.IsEmpty()) {
    return JSValue::Undefined();
  }
  
  // 4. If IsDataDescriptor(desc) is true, return desc.[[Value]].
  if (desc.IsDataDescriptor()) {
    return desc.GetValue();
  }
  
  // 5. Otherwise, IsAccessorDescriptor(desc) must be true so, let getter be desc.[[Get]].
  auto getter = desc.GetGetter();
  
  // 6. If getter is undefined, return undefined.
  if (getter.IsUndefined()) {
    return getter;
  }
  
  // 7. Return the result calling the [[Call]] internal method of getter providing base as the this value and providing no arguments.
  // todo
}

// PutValue
// Defined in ECMAScript 5.1 Chapter 8.7.2
void Interpreter::PutValue(const std::variant<JSValue, Reference>& V, JSValue W) {
  // 1. If Type(V) is not Reference, throw a ReferenceError exception.
  if (!std::get_if<Reference>(&V)) {
    THROW_REFERENCE_ERROR_AND_RETURN_VOID(vm_, u"PutValue cannot operate on non-Reference type");
  }

  auto ref = std::get_if<Reference>(&V);

  // 2. Let base be the result of calling GetBase(V).
  auto base = ref->GetBase();

  // 3. If IsUnresolvableReference(V), then
  if (ref->IsUnresolvableReference()) {
    // a. If IsStrictReference(V) is true, then
    if (ref->IsStrictReference()) {
      // i. Throw ReferenceError exception.
      THROW_REFERENCE_ERROR_AND_RETURN_VOID(
        vm_, u"PutValue cannot operate on Undefined value in strict mode");
    }
    // b. Call the [[Put]] internal method of the global object,
    //    passing GetReferencedName(V) for the property name,
    //    W for the value, and false for the Throw flag.
    else {
      auto global_obj = vm_->GetGlobalObject();
      Object::Put(vm_, global_obj, ref->GetReferencedName(), W, false);
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
      Object::Put(vm_, base_obj, ref->GetReferencedName(), W, ref->IsStrictReference());
    } else {
      auto base_prim = std::get<JSValue>(base);
      PutUsedByPutValue(base_prim, ref->GetReferencedName(), W, ref->IsStrictReference());
    }

  }
  // 5. Else base must be a reference whose base is an environment record. So,
  else {
    // a. Call the SetMutableBinding (10.2.1) concrete method of base,
    //    passing GetReferencedName(V), W, and IsStrictReference(V) as arguments.
    auto base_env = std::get<EnvironmentRecord*>(base);
    EnvironmentRecord::SetMutableBinding(vm_, base_env,ref->GetReferencedName(), W, ref->IsStrictReference());
  }

  // 6. Return
}

// PutUsedByPutValue
// Defined in ECMAScript 5.1 Chapter 8.7.2
// used by PutValue when V is a property reference with primitive base value
void Interpreter::PutUsedByPutValue(JSValue base, String* P, JSValue W, bool Throw) {
  // 1. Let O be ToObject(base).
  auto O = JSValue::ToObject(vm_, base);

  // 2. If the result of calling the [[CanPut]] internal method of O with argument P is false, then
  if (!Object::CanPut(vm_, O, P)) {
    // a. If Throw is true, then throw a TypeError exception.
    if (Throw) {
      THROW_TYPE_ERROR_AND_RETURN_VOID(
        vm_, u"PutUsedByPutValue fails when Object.CanPut returns false");
    }
    // b. Else return.
    else {
      return ;
    }
  }

  // 3. Let ownDesc be the result of calling the [[GetOwnProperty]] internal method of O with argument P.
  auto own_desc = Object::GetOwnProperty(vm_, O, P);

  // 4. If IsDataDescriptor(ownDesc) is true, then
  if (own_desc.IsDataDescriptor()) {
    // a. If Throw is true, then throw a TypeError exception.
    if (Throw) {
      THROW_TYPE_ERROR_AND_RETURN_VOID(
        vm_, u"PutUsedByPutValue fails when new property differs from the old one in type.");
    }
    // b. Else return.
    else {
      return ;
    }
  }

  // 5. Let desc be the result of calling the [[GetProperty]] internal method of O with argument P.
  //    This may be either an own or inherited accessor property descriptor or an inherited data property descriptor.
  auto desc = Object::GetProperty(vm_, O, P);

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
      THROW_TYPE_ERROR_AND_RETURN_VOID(
        vm_, u"PutUsedByPutValue fails to create an own property on the transient object");
    }
  }

  // 8. Return.
}

}  // namespace voidjs
