#include "voidjs/interpreter/interpreter.h"

#include <cmath>
#include <variant>
#include <iostream>
#include <functional>

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
#include "voidjs/types/spec_types/reference.h"
#include "voidjs/types/spec_types/lexical_environment.h"
#include "voidjs/types/spec_types/environment_record.h"
#include "voidjs/builtins/global_object.h"
#include "voidjs/builtins/js_object.h"
#include "voidjs/builtins/js_function.h"
#include "voidjs/interpreter/execution_context.h"
#include "voidjs/interpreter/string_table.h"

namespace voidjs {

using namespace ast;
using namespace types;

void Interpreter::Initialize() {
  vm_ = new VM{};

  auto string_table = new StringTable{};
  auto object_factory = new ObjectFactory(vm_, string_table);

  vm_->SetObjectFactory(object_factory);
  
  InitializeBuiltinObjects();
  auto global_env = LexicalEnvironment::NewObjectEnvironmentRecord(vm_, JSValue(vm_->GetGlobalObject()), nullptr);
  
  vm_->SetGlobalEnv(global_env);
}

Completion Interpreter::Execute(AstNode* ast_node) {
  EnterGlobalCode(ast_node);
  return EvalProgram(ast_node);
}

void Interpreter::InitializeBuiltinObjects() {
  auto factory = vm_->GetObjectFactory();
  
  // Initialize GlobalObject
  auto global_obj = factory->NewGlobalObject();
  vm_->SetGlobalObject(global_obj);

  // Initialize Object Prototype
  // The value of the [[Prototype]] internal property of the Object prototype object is null,
  // the value of the [[Class]] internal property is "Object",
  // and the initial value of the [[Extensible]] internal property is true.
  auto obj_proto = factory->NewEmptyObject(builtins::JSObject::SIZE)->AsJSObject();
  obj_proto->SetType(JSType::JS_OBJECT);
  obj_proto->SetClassType(ObjectClassType::OBJECT);
  obj_proto->SetPrototype(JSValue::Null());
  obj_proto->SetExtensible(true);

  // Initialzie Function Prototype
  // The value of the [[Prototype]] internal property of the Function prototype object is
  // the standard built-in Object prototype object (15.2.4).
  // The initial value of the [[Extensible]] internal property of the Function prototype object is true.
  // The Function prototype object does not have a valueOf property of its own;
  // however, it inherits the valueOf property from the Object prototype Object.
  auto func_proto = factory->NewEmptyObject(builtins::JSFunction::SIZE)->AsJSFunction();
  func_proto->SetType(JSType::JS_FUNCTION);
  func_proto->SetClassType(ObjectClassType::FUNCTION);
  func_proto->SetPrototype(JSValue(obj_proto));
  func_proto->SetExtensible(true);

  // Initialize Object Constructor
  // The value of the [[Prototype]] internal property of the Object constructor is
  // the standard built-in Function prototype object.
  // Besides the internal properties and the length property (whose value is 1),
  // the Object constructor has the following properties:
  auto obj_ctor = factory->NewEmptyObject(builtins::JSObject::SIZE)->AsJSObject();
  obj_ctor->SetType(JSType::JS_OBJECT);
  obj_ctor->SetClassType(ObjectClassType::OBJECT);
  obj_ctor->SetPrototype(JSValue(func_proto));
  
  // Initialize Function Constructor
  // The Function constructor is itself a Function object and its [[Class]] is "Function".
  // The value of the [[Prototype]] internal property of the Function constructor is
  // the standard built-in Function prototype object (15.3.4).
  // The value of the [[Extensible]] internal property of the Function constructor is true.
  auto func_ctor = factory->NewEmptyObject(builtins::JSFunction::SIZE)->AsJSFunction();
  func_ctor->SetType(JSType::JS_FUNCTION);
  func_ctor->SetClassType(ObjectClassType::FUNCTION);
  func_ctor->SetPrototype(JSValue(func_proto));
  func_ctor->SetExtensible(true);
}

void Interpreter::EnterGlobalCode(AstNode* ast_node) {
  // 1. Initialize the execution context using the global code as described in 10.4.1.1.
  auto global_ctx = new ExecutionContext(vm_->GetGlobalEnv(), vm_->GetGlobalEnv(), vm_->GetGlobalObject());
  vm_->PushExecutionContext(global_ctx);

  // 2. Perform Declaration Binding Instantiation as described in 10.5 using the global code.
  DeclarationBindingInstantiation(ast_node);
}

void Interpreter::DeclarationBindingInstantiation(AstNode* ast_node) {
  auto factory = vm_->GetObjectFactory();
  
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
    auto dn_str = factory->NewString(dn->GetName());

    // b. Let varAlreadyDeclared be the result of calling env’s HasBinding concrete method passing dn as the argument.
    auto var_already_declared = env->HasBinding(vm_, dn_str);

    // c. If varAlreadyDeclared is false, then
    if (!var_already_declared) {
      // i. Call env’s CreateMutableBinding concrete method
      //    passing dn and configurableBindings as the arguments.
      env->CreateMutableBinding(vm_, dn_str, configurable_bindings);

      // ii. Call env’s SetMutableBinding concrete method passing dn, undefined, and strict as the arguments.
      env->SetMutableBinding(vm_, dn_str, JSValue::Undefined(), strict);
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
    if (!JSValue::ToBoolean(vm_, GetValue(expr_ref))) {
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
    if (!JSValue::ToBoolean(vm_, GetValue(expr_ref))) {
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
      if (!JSValue::ToBoolean(vm_, GetValue(test_expr_ref))) {
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
  auto obj = JSValue::ToObject(vm_, expr_val);

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
  if (JSValue::ToBoolean(vm_, GetValue(lref))) {
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
  auto prop_name_str = JSValue::ToString(vm_, prop_name_val); 

  // 7. If the syntactic production that is being evaluated is contained in strict mode code,
  //    let strict be true, else let strict be false.
  // todo
  bool strict = false;

  // 8. Return a value of type Reference
  //    whose base value is baseValue and whose referenced name is propertyNameString,
  //    and whose strict mode flag is strict.
  return Reference(base_val, prop_name_str.GetHeapObject()->AsString(), strict);
}

// EvalObjectLiteral
// Defined in ECMAScript 5.1 Chapter 11.1.5
JSValue Interpreter::EvalObjectLiteral(ObjectLiteral* object) {
  // ObjectLiteral : { }
  // 1. Return a new object created as if by the expression
  //    new Object() where Object is the standard built-in constructor with that name.
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
    return JSValue(vm_->GetObjectFactory()->NewStringFromTable(decl->GetIdentifier()->AsIdentifier()->GetName()));
  }
  auto rhs = EvalExpression(decl->GetInitializer());

  // 3. Let value be GetValue(rhs).
  auto value = GetValue(rhs);

  // 4. Call PutValue(lhs, value).
  PutValue(lhs, value);

  // 5. Return a String value containing the same sequence of characters as in the Identifier.
  return JSValue(vm_->GetObjectFactory()->NewStringFromTable(decl->GetIdentifier()->AsIdentifier()->GetName()));
}

// ApplyCompoundAssignment
// todo
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
    
    // 2. Let lval be GetValue(lref).
    auto lval = GetValue(lref);
    
    // 3. If ToBoolean(lval) is false, return lval.
    if (!JSValue::ToBoolean(vm_, lval)) {
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
    if (JSValue::ToBoolean(vm_, lval)) {
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
  
  // 2. Let lval be GetValue(lref).
  auto lval = GetValue(lref);
  
  // 3. Let rref be the result of evaluating RelationalExpression.
  auto rref = EvalExpression(right);
  
  // 4. Let rval be GetValue(rref).
  auto rval = GetValue(rref);

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
    
    // 2. Let lval be GetValue(lref).
    auto lval = GetValue(lref);
    
    // 3. Let rref be the result of evaluating ShiftExpression.
    auto rref = EvalExpression(right);
    
    // 4. Let rval be GetValue(rref).
    auto rval = GetValue(rref);
    
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
    
    // 2. Let lval be GetValue(lref).
    auto lval = GetValue(lref);
    // 3. Let rref be the result of evaluating ShiftExpression.
    auto rref = EvalExpression(right);
    
    // 4. Let rval be GetValue(rref).
    auto rval = GetValue(rref);
    
    // 5. If Type(rval) is not Object, throw a TypeError exception.
    if (!rval.IsObject()) {
      // todo
    }
    
    // 6. If rval does not have a [[HasInstance]] internal method, throw a TypeError exception.
    // todo
    
    // 7. Return the result of calling the [[HasInstance]] internal method of rval with argument lval.
    // todo
  } else {
    // op must be TokenType::KEYWORD_IN
    
    // 1. Let lref be the result of evaluating RelationalExpression.
    auto lref = EvalExpression(left);
    
    // 2. Let lval be GetValue(lref).
    auto lval = GetValue(lref);
    
    // 3. Let rref be the result of evaluating ShiftExpression.
    auto rref = EvalExpression(right);
    
    // 4. Let rval be GetValue(rref).
    auto rval = GetValue(rref);
    
    // 5. If Type(rval) is not Object, throw a TypeError exception.
    if (!rval.IsObject()) {
      // todo
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
  
  // 2. Let lval be GetValue(lref).
  auto lval = GetValue(lref);
  
  // 3. Let rref be the result of evaluating AdditiveExpression.
  auto rref = EvalExpression(right);
  
  // 4. Let rval be GetValue(rref).
  auto rval = GetValue(rref);
  
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
    return JSValue(rnum >> shift_count);
  } else {
    // op must be TokenType::U_RIGHT_SHIFT

    // Return the result of performing a zero-filling right shift of lnum by shiftCount bits.
    // Vacated bits are filled with zero. The result is an unsigned 32-bit integer.
    return JSValue(JSValue::ToUint32(vm_, lval) >> rnum);
  }
}

// ApplyAdditiveOperator
JSValue Interpreter::ApplyAdditiveOperator(TokenType op, Expression* left, Expression* right) {
  // AdditiveExpression op MultiplicativeExpression

  if (op == TokenType::ADD) {
    // 1. Let lref be the result of evaluating AdditiveExpression.
    auto lref = EvalExpression(left);
    
    // 2. Let lval be GetValue(lref).
    auto lval = GetValue(lref);
    
    // 3. Let rref be the result of evaluating MultiplicativeExpression.
    auto rref = EvalExpression(right);
    
    // 4. Let rval be GetValue(rref).
    auto rval = GetValue(rref);
    
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
      
    // 2. Let lval be GetValue(lref).
    auto lval = GetValue(lref);
      
    // 3. Let rref be the result of evaluating MultiplicativeExpression.
    auto rref = EvalExpression(right);
      
    // 4. Let rval be GetValue(rref).
    auto rval = GetValue(rref);
      
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
  
  // 2. Let leftValue be GetValue(left).
  auto lval = GetValue(lref);
  
  // 3. Let right be the result of evaluating UnaryExpression.
  auto rref = EvalExpression(right);
  
  // 4. Let rightValue be GetValue(right).
  auto rval = GetValue(rref);
  
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
  auto ref = EvalExpression(expr);
  switch (op) {
    case TokenType::KEYWORD_DELETE: {
      // todo
      return {};
    }
    case TokenType::KEYWORD_VOID: {
      // 1. Call GetValue(expr)
      GetValue(ref);

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
      // todo
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
        // todo
      }

      // 2. Let oldValue be ToNumber(GetValue(expr)).
      auto old_val = JSValue::ToNumber(vm_, GetValue(ref));

      // 3. Let newValue be the result of adding the value 1 to oldValue,
      //    using the same rules as for the + operator (see 11.6.3).
      auto new_val = ++old_val;

      // 4. Call PutValue(expr, newValue).
      PutValue(ref, new_val);

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
        // todo
      }

      // 2. Let oldValue be ToNumber(GetValue(expr)).
      auto old_val = JSValue::ToNumber(vm_, GetValue(ref));

      // 3. Let newValue be the result of subtracting the value 1 to oldValue,
      //    using the same rules as for the + operator (see 11.6.3).
      auto new_val = --old_val;

      // 4. Call PutValue(expr, newValue).
      PutValue(ref, new_val);

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
    // todo
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
    vm_, 
    vm_->GetExecutionContext()->GetLexicalEnvironment(), ident, strict);
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
    return env->GetBindingValue(vm_, ref.GetReferencedName(), ref.IsStrictReference());
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
      Object::Put(vm_, global_obj, JSValue(ref->GetReferencedName()), W, false);
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
      Object::Put(vm_, base_obj, JSValue(ref->GetReferencedName()), W, ref->IsStrictReference());
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
    base_env->SetMutableBinding(vm_, ref->GetReferencedName(), W, ref->IsStrictReference());
  }

  // 6. Return
}

// Put
// Defined in ECMAScript 5.1 Chapter 8.7.2
// used by PutValue when V is a property reference with primitive base value
void Interpreter::Put(JSValue base, JSValue P, JSValue W, bool Throw) {
  // 1. Let O be ToObject(base).
  auto O = JSValue::ToObject(vm_, base);

  // 2. If the result of calling the [[CanPut]] internal method of O with argument P is false, then
  if (!Object::CanPut(vm_, O, P)) {
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
  auto own_desc = Object::GetOwnProperty(vm_, O, P);

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
      // todo
    }
  }

  // 8. Return.
}

}  // namespace voidjs
