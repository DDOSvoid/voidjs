#include "voidjs/ir/program.h"

#include "voidjs/ir/dumper.h"

namespace voidjs {
namespace ast {

void Program::Dump(Dumper* dumper) const {
  dumper->Dump({
      Dumper::DumperNode{"type", "Program"},
      Dumper::DumperNode{"statements", statements_},
    });
}

}  // namespace ast
}  // namespace voidjs
