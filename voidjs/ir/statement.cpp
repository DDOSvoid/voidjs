#include "voidjs/ir/statement.h"

#include "voidjs/ir/ast.h"
#include "voidjs/ir/dumper.h"

namespace voidjs {
namespace ast {

void BlockStatement::Dump(Dumper* dumper) const {
  dumper->Dump({
      Dumper::DumperNode{"type", "BlockStatement"},
      Dumper::DumperNode{"statements", statements_},
    });
}

void VariableStatement::Dump(Dumper* dumper) const {
  dumper->Dump({
      Dumper::DumperNode{"type", "VariableStatement"},
      Dumper::DumperNode{"variable_declarations", declarations_},
    });
}

void EmptyStatement::Dump(Dumper* dumper) const {
  dumper->Dump({
      Dumper::DumperNode{"type", "EmptyStatement"},
    });
}

void ExpressionStatement::Dump(Dumper* dumper) const {
  dumper->Dump({
      Dumper::DumperNode{"type", "ExpressionStatement"},
      Dumper::DumperNode{"expression", expression_},
    });
}

void IfStatement::Dump(Dumper* dumper) const {
  dumper->Dump({
      Dumper::DumperNode{"type", "IfStatement"},
      Dumper::DumperNode{"condition", condition_},
      Dumper::DumperNode{"consequent", consequent_},
      Dumper::DumperNode{"alternate", Dumper::NullableAstNode{alternate_}}
    });
}

void DoWhileStatement::Dump(Dumper* dumper) const {
  dumper->Dump({
      Dumper::DumperNode{"type", "DoWhileStatement"},
      Dumper::DumperNode{"condition", condition_},
      Dumper::DumperNode{"body", body_}
    });
}

void WhileStatement::Dump(Dumper* dumper) const {
  dumper->Dump({
      Dumper::DumperNode{"type", "WhileStatement"},
      Dumper::DumperNode{"condition", condition_},
      Dumper::DumperNode{"body", body_}
    });
}

void ForStatement::Dump(Dumper* dumper) const {
  dumper->Dump({
      Dumper::DumperNode{"type", "ForStatement"},
      Dumper::DumperNode{"initializer", Dumper::NullableAstNode{initializer_}},
      Dumper::DumperNode{"condition", Dumper::NullableAstNode{condition_}},
      Dumper::DumperNode{"update", Dumper::NullableAstNode{update_}},
      Dumper::DumperNode{"body", body_},
    });
}

void ForInStatement::Dump(Dumper* dumper) const {
  dumper->Dump({
      Dumper::DumperNode{"type", "ForInStatement"},
      Dumper::DumperNode{"left", left_},
      Dumper::DumperNode{"right", right_},
      Dumper::DumperNode{"body", body_},
    });
}

void ContinueStatement::Dump(Dumper* dumper) const {
  dumper->Dump({
      Dumper::DumperNode{"type", "ContinueStatement"},
      Dumper::DumperNode{"identifier", Dumper::NullableAstNode{identifier_}},
    });
}

void BreakStatement::Dump(Dumper* dumper) const {
  dumper->Dump({
      Dumper::DumperNode{"type", "BreakStatement"},
      Dumper::DumperNode{"identifier", Dumper::NullableAstNode{identifier_}},
    });
}

void ReturnStatement::Dump(Dumper* dumper) const {
  dumper->Dump({
      Dumper::DumperNode{"type", "ReturnStatement"},
      Dumper::DumperNode{"expression", Dumper::NullableAstNode{expression_}},
    });
}

void WithStatement::Dump(Dumper* dumper) const {
  dumper->Dump({
      Dumper::DumperNode{"type", "WithStatement"},
      Dumper::DumperNode{"context", context_},
      Dumper::DumperNode{"body", body_},
    });
}

void SwitchStatement::Dump(Dumper* dumper) const {
  dumper->Dump({
      Dumper::DumperNode{"type", "SwitchStatement"},
      Dumper::DumperNode{"discriminant", discriminant_},
      Dumper::DumperNode{"case_clauses", case_clauses_},
    });
}

void LabelledStatement::Dump(Dumper* dumper) const {
  dumper->Dump({
      Dumper::DumperNode{"type", "LabelledStatement"},
      Dumper::DumperNode{"label", label_},
      Dumper::DumperNode{"body", body_},
    });
}

void ThrowStatement::Dump(Dumper* dumper) const {
  dumper->Dump({
      Dumper::DumperNode{"type", "ThrowStatement"},
      Dumper::DumperNode{"expression", expression_},
    });
}

void TryStatement::Dump(Dumper* dumper) const {
  dumper->Dump({
      Dumper::DumperNode{"type", "TryStatement"},
      Dumper::DumperNode{"body", body_},
      Dumper::DumperNode{"catch_name", catch_name_},
      Dumper::DumperNode{"catch_block", catch_block_},
      Dumper::DumperNode{"finally_block", Dumper::NullableAstNode{finally_block_}},
    });
}

void DebuggerStatement::Dump(Dumper* dumper) const {
  dumper->Dump({
      Dumper::DumperNode{"type", "DebuggerStatement"},
    });
}

void VariableDeclaration::Dump(Dumper* dumper) const {
  dumper->Dump({
      Dumper::DumperNode{"type", "VariableDeclaration"},
      Dumper::DumperNode{"identifier", identifier_},
      Dumper::DumperNode{"initalizer", Dumper::NullableAstNode{initializer_}},
    });
}

void CaseClause::Dump(Dumper* dumper) const {
  dumper->Dump({
      Dumper::DumperNode{"type", "CaseClause"},
      Dumper::DumperNode{"condition", condition_},
      Dumper::DumperNode{"statements", statements_},
    });
}

void FunctionDeclaration::Dump(Dumper* dumper) const {
  dumper->Dump({
      Dumper::DumperNode{"type", "FunctionDeclaration"},
      Dumper::DumperNode{"name", name_},
      Dumper::DumperNode{"parameters", parameters_},
      Dumper::DumperNode{"statements", statements_},
    });
}

}  // namespace ast
}  // namespace voidjs
