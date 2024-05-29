#include "voidjs/ir/expression.h"

#include "voidjs/ir/ast.h"
#include "voidjs/ir/dumper.h"

namespace voidjs {
namespace ast {

void NewExpression::Dump(Dumper* dumper) const {
  dumper->Dump({
      Dumper::DumperNode{"type", "NewExpression"},
      Dumper::DumperNode{"constructor", constructor_},
      Dumper::DumperNode{"arguments", arguments_},
    });
}

void CallExpression::Dump(Dumper* dumper) const {
  dumper->Dump({
      Dumper::DumperNode{"type", "CallExpression"},
      Dumper::DumperNode{"callee", callee_},
      Dumper::DumperNode{"arguments", arguments_},
    });
}

void MemberExpression::Dump(Dumper* dumper) const {
  dumper->Dump({
      Dumper::DumperNode{"type", "MemberExpression"},
      Dumper::DumperNode{"object", object_},
      Dumper::DumperNode{"property", property_},
    });
}

void PostfixExpression::Dump(Dumper* dumper) const {
  dumper->Dump({
      Dumper::DumperNode{"type", "PostfixExpression"},
      Dumper::DumperNode{"operator", operator_},
      Dumper::DumperNode{"expression", expression_},
    });
}

void UnaryExpression::Dump(Dumper* dumper) const {
  dumper->Dump({
      Dumper::DumperNode{"type", "UnaryExpression"},
      Dumper::DumperNode{"operator", operator_},
      Dumper::DumperNode{"expression", expression_},
    });
}

void BinaryExpression::Dump(Dumper* dumper) const {
  dumper->Dump({
      Dumper::DumperNode{"type", "BinaryExpression"},
      Dumper::DumperNode{"operator", operator_},
      Dumper::DumperNode{"left", left_},
      Dumper::DumperNode{"right", right_},
    });
}

void ConditionalExpression::Dump(Dumper* dumper) const {
  dumper->Dump({
      Dumper::DumperNode{"type", "ConditionalExpression"},
      Dumper::DumperNode{"condition", condition_},
      Dumper::DumperNode{"consequent", consequent_},
      Dumper::DumperNode{"alternate", alternate_},
    });
}

void AssignmentExpression::Dump(Dumper* dumper) const {
  dumper->Dump({
      Dumper::DumperNode{"type", "AssignmentExpression"},
      Dumper::DumperNode{"operator", operator_},
      Dumper::DumperNode{"left", left_},
      Dumper::DumperNode{"right", right_},
    });
}

void SequenceExpression::Dump(Dumper* dumper) const {
  dumper->Dump({
      Dumper::DumperNode{"type", "SequenceExpression"},
      Dumper::DumperNode{"expressions", expressions_},
    });
}

void This::Dump(Dumper* dumper) const {
  dumper->Dump({
      Dumper::DumperNode{"type", "This"},
    });
}

void FunctionExpression::Dump(Dumper* dumper) const {
  dumper->Dump({
      Dumper::DumperNode{"type", "FunctionExpression"},
      Dumper::DumperNode{"name", Dumper::NullableAstNode{name_}},
      Dumper::DumperNode{"parameters", parameters_},
      Dumper::DumperNode{"statements", statements_},
    });
}

void Identifier::Dump(Dumper* dumper) const {
  dumper->Dump({
      Dumper::DumperNode{"type", "Identifier"},
      Dumper::DumperNode{"name", name_},
    });
}

void ArrayLiteral::Dump(Dumper* dumper) const {
  dumper->Dump({
      Dumper::DumperNode{"type", "ArrayLiteral"},
      Dumper::DumperNode{"elements", elements_},
    });
}

void ObjectLiteral::Dump(Dumper* dumper) const {
  dumper->Dump({
      Dumper::DumperNode{"type", "ObjectLiteral"},
      Dumper::DumperNode{"properties", properties_},
    });
}

void Property::Dump(Dumper* dumper) const {
  const char* type_string = nullptr;
  if (type_ == PropertyType::INIT) {
    type_string = "init";
  } else if (type_ == PropertyType::GET) {
    type_string = "get";
  } else {
    // type_ must be PropertyType::SET
    type_string = "set";
  }
  
  dumper->Dump({
      Dumper::DumperNode{"type", "Property"},
      Dumper::DumperNode{"property_type", type_string},
      Dumper::DumperNode{"key", key_},
      Dumper::DumperNode{"value", value_},
    });
}

}  // namespace ast
}  // namespace voidjs
