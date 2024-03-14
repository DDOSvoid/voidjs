#ifndef VOIDJS_UTILS_ERROR_H
#define VOIDJS_UTILS_ERROR_H

#include <exception>
#include <string>

namespace voidjs {
namespace utils {

enum class ErrorType {
  GENERIC,
  SYNTAX_ERROR,
  TYPE_ERROR,
};

class Error : public std::exception {
 public:
  Error() = default;
  Error(ErrorType type, std::string message)
    : type_(type), message_(std::move(message)) {}
  ~Error() override = default;

  Error(const Error&) = default;
  Error& operator=(const Error&) = default;

  const char *what() const noexcept override { return message_.c_str(); }

  ErrorType GetType() const { return type_; }
  const std::string &GetMessage() const { return message_; }

 private:
  ErrorType type_ {ErrorType::GENERIC};
  std::string message_;
};

}  // namespace voidjs
}  // namespace utils

#endif  // VOIDJS_UTILS_ERROR_H
