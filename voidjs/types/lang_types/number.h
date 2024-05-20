#ifndef VOIDJS_TYPES_LANG_TYPES_NUMBER_H
#define VOIDJS_TYPES_LANG_TYPES_NUMBER_H

#include <limits>
#include <type_traits>
#include <iostream>

#include "voidjs/types/js_value.h"

namespace voidjs {
namespace types {

class Number : public JSValue {
 public:
  Number()
    : JSValue()
  {}

  explicit Number(std::int32_t number)
    : JSValue(number)
  {}

  explicit Number(std::uint32_t number)
    : JSValue(number)
  {}

  explicit Number(double number)
    : JSValue(number)
  {}

  explicit Number(bool number)
    : JSValue(number)
  {}

  explicit Number(JSValue value)
    : JSValue(value.GetRawData()) {
    // todo
    // assert value.IsNumber()
  }

  static Number NaN() { return Number{std::numeric_limits<double>::quiet_NaN()}; }

  Number operator+(Number number) const {
    if (IsInt() && number.IsInt()) {
      std::int64_t num0 = GetInt();
      std::int64_t num1 = number.GetInt();
      std::int64_t res = num0 + num1;
      if (res > std::numeric_limits<std::int32_t>::max() ||
          res < std::numeric_limits<std::int32_t>::min()) {
        return Number(static_cast<double>(res));
      }
      return Number(static_cast<std::int32_t>(res));
    }
    return Number(GetNumber() + number.GetNumber());
  }

  Number operator-(Number number) const {
    if (IsInt() && number.IsInt()) {
      std::int64_t num0 = GetInt();
      std::int64_t num1 = number.GetInt();
      std::int64_t res = num0 - num1;
      if (res > std::numeric_limits<std::int32_t>::max() ||
          res < std::numeric_limits<std::int32_t>::min()) {
        return Number(static_cast<double>(res));
      }
      return Number(static_cast<std::int32_t>(res));
    }
    return Number(GetNumber() - number.GetNumber());
  }

  Number operator*(Number number) const {
    if (IsInt() && number.IsInt()) {
      std::int64_t num0 = GetInt();
      std::int64_t num1 = number.GetInt();
      std::int64_t res = num0 * num1;
      if (res > std::numeric_limits<std::int32_t>::max() ||
          res < std::numeric_limits<std::int32_t>::min()) {
        return Number(static_cast<double>(res));
      }
      // we need to deal with -0 in JS,
      if (res == 0 && (num0 < 0 || num1 < 0)) {
        return Number(-0.0);
      }
      return Number(static_cast<std::int32_t>(res));
    }
    return Number(GetNumber() * number.GetNumber());
  }

  Number operator/(Number number) const {
    return Number(GetNumber() / number.GetNumber());
  }

  Number operator++() const {
    if (IsInt()) {
      int32_t num = GetInt();
      if (num == std::numeric_limits<std::int32_t>::max()) {
        return Number(static_cast<double>(num) + 1.0);
      } else {
        return Number(num + 1);
      }
    }
    return Number(GetDouble() + 1.0);
  }

  Number operator--() const {
    if (IsInt()) {
      int32_t num = GetInt();
      if (num == std::numeric_limits<std::int32_t>::min()) {
        return Number(static_cast<double>(num) - 1.0);
      }
      return Number(num - 1);
    }
    return Number(GetDouble() - 1.0);
  }

  bool operator==(const Number& other) const { return GetNumber() == other.GetNumber(); }
  bool operator!=(const Number& other) const { return GetNumber() != other.GetNumber(); }
};

}  // namespace types
}  // namespace voidjs

#endif  // VOIDJS_TYPES_LANGE_TYPES_NUMBER_H
