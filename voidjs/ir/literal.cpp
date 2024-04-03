#include "voidjs/ir/literal.h"
#include "voidjs/ir/ast.h"

namespace voidjs {
namespace ast {

bool Literal::GetBoolean() const {
  return AsBooleanLiteral()->GetBoolean();
}

std::int32_t Literal::GetInt() const {
  return AsNumericLiteral()->GetNumber<std::int32_t>();
}

double Literal::GetDouble() const {
  return AsNumericLiteral()->GetNumber<double>();
}

std::u16string_view Literal::GetString() const {
  return AsStringLiteral()->GetString();
}

}  // namespace ast
}  // namespace voidjs
