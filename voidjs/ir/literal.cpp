#include "voidjs/ir/literal.h"

#include "voidjs/ir/ast.h"
#include "voidjs/ir/dumper.h"
#include "voidjs/utils/helper.h"

namespace voidjs {
namespace ast {

void NullLiteral::Dump(Dumper* dumper) const {
  dumper->Dump({
      Dumper::DumperNode{"type", "NullLiteral"}
    });
}

void BooleanLiteral::Dump(Dumper* dumper) const {
  dumper->Dump({
      Dumper::DumperNode{"type", "BooleanLiteral"},
      Dumper::DumperNode{"boolean", boolean_}
    });
}

void NumericLiteral::Dump(Dumper* dumper) const {
  dumper->Dump({
      Dumper::DumperNode{"type", "NumericLiteral"},
      Dumper::DumperNode{"number", number_},
    });
}

void StringLiteral::Dump(Dumper* dumper) const {
  dumper->Dump({
      Dumper::DumperNode{"type", "StringLiteral"},
      Dumper::DumperNode{"string", string_},
    });
}

}  // namespace ast
}  // namespace voidjs
