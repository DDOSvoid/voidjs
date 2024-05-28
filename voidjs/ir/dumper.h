#ifndef VOIDJS_IR_DUMPER_H
#define VOIDJS_IR_DUMPER_H

#include <cstdint>
#include <string>
#include <variant>
#include <sstream>
#include <iostream>

#include "voidjs/lexer/token_type.h"
#include "voidjs/ir/ast.h"
#include "voidjs/utils/helper.h"

namespace voidjs {
namespace ast {

class Dumper {
 public:
  class NullableAstNode {
   public:
    explicit NullableAstNode(AstNode* ast_node)
      : ast_node_(ast_node)
    {}
    
   private:
    AstNode* ast_node_{nullptr};
  };
  
  class DumperNode {
   public:
    using ValueType = std::variant<TokenType, AstNode*, bool, double, const char*,
                                   std::string, std::vector<AstNode*>, NullableAstNode>;

    DumperNode(const char* key, TokenType type)
      : key_(key), value_(type)
    {}

    DumperNode(const char* key, AstNode* ast_node)
      : key_(key), value_(ast_node)
    {}

    DumperNode(const char* key, bool boolean)
      : key_(key), value_(boolean)
    {}

    DumperNode(const char* key, double number)
      : key_(key), value_(number)
    {}

    DumperNode(const char* key, const char* string)
      : key_(key), value_(string)
    {}

    DumperNode(const char* key, std::u16string string)
      : key_(key), value_(utils::U16StrToU8Str(string))
    {}

    template <typename T>
    DumperNode(const char* key, const std::vector<T*>& ast_nodes)
      : key_(key)
    {
      std::vector<AstNode*> nodes;
      for (auto ast_node : ast_nodes) {
        nodes.push_back(reinterpret_cast<AstNode*>(ast_node));
      }
      value_ = std::move(nodes);
    }

    DumperNode(const char* key, NullableAstNode ast_node)
      : key_(key), value_(ast_node)
    {}

    const char* GetKey() const { return key_; }
    const ValueType& GetValue() const { return value_; }
    
   private:
    const char* key_;
    ValueType value_;
  };

  Dumper(Program* program);
  
  void Dump(const DumperNode& node);
  void Dump(std::initializer_list<DumperNode> nodes);

  void DumpTokenType(TokenType type);
  void DumpString(const char* string);
  void DumpString(std::string string);
  void DumpString(std::u16string string);
  void DumpNumber(int number);
  void DumpNubmer(double number);
  void DumpBoolean(bool boolean);
  void DumpAstNode(AstNode* ast_node);
  void DumpAstNodes(std::vector<AstNode*> ast_nodes);

  void AddIndent();

  std::string GetString() { return ss_.str(); }
  
  std::size_t indent_ {0};
  std::stringstream ss_;
}; 

}  // namespace ast
}  // namespace voidjs

#endif  // VOIDJS_IR_DUMPER_H
