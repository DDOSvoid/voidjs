#include "voidjs/ir/dumper.h"

#include <initializer_list>
#include <ios>
#include <string>
#include <variant>

#include "voidjs/lexer/token.h"
#include "voidjs/lexer/token_type.h"
#include "voidjs/ir/ast.h"
#include "voidjs/ir/program.h"
#include "voidjs/utils/helper.h"

namespace voidjs {
namespace ast {

Dumper::Dumper(Program* program) {
  DumpAstNode(program);
}

void Dumper::Dump(const DumperNode& node) {
  ss_ << '\n';
  AddIndent();
  DumpString(node.GetKey());
  ss_ << ": ";
  
  if (std::holds_alternative<TokenType>(node.GetValue())) {
    DumpTokenType(std::get<TokenType>(node.GetValue()));
  } else if (std::holds_alternative<AstNode*>(node.GetValue())) {
    DumpAstNode(std::get<AstNode*>(node.GetValue()));
  } else if (std::holds_alternative<bool>(node.GetValue())) {
    DumpBoolean(std::get<bool>(node.GetValue()));
  } else if (std::holds_alternative<double>(node.GetValue())) {
    DumpNumber(std::get<double>(node.GetValue()));
  } else if (std::holds_alternative<const char*>(node.GetValue())) {
    DumpString(std::get<const char*>(node.GetValue()));
  } else if (std::holds_alternative<std::string>(node.GetValue())) {
    DumpString(std::get<std::string>(node.GetValue()));
  } else if (std::holds_alternative<std::vector<AstNode*>>(node.GetValue())) {
    DumpAstNodes(std::get<std::vector<AstNode*>>(node.GetValue()));
  } else if (std::holds_alternative<NullableAstNode>(node.GetValue())) {
  }
}

void Dumper::Dump(std::initializer_list<DumperNode> nodes) {
  bool first = false;
  for (const auto& node : nodes) {
    if (first) {
      ss_ << ',';
    }
    
    Dump(node);
    
    first = true;
  }
}

void Dumper::DumpTokenType(TokenType type) {
  ss_ << '"' << Token::TokenTypeToString(type) << '"';
}

void Dumper::DumpString(const char* string) {
  ss_ << '"' << string << '"';
}

void Dumper::DumpString(std::string string) {
  ss_ << '"' << string << '"';
}

void Dumper::DumpString(std::u16string string) {
  ss_ << '"' << utils::U16StrToU8Str(std::u16string{string}) << '"';
}

void Dumper::DumpNumber(int number) {
  ss_ << number;
}

void Dumper::DumpNubmer(double number) {
  ss_ << number;
}

void Dumper::DumpBoolean(bool boolean) {
  ss_ << std::boolalpha << boolean;
}

void Dumper::DumpAstNode(AstNode* ast_node) {
  ss_ << '{';
  ++indent_;

  ast_node->Dump(this);

  ss_ << '\n';
  --indent_;
  AddIndent();
  ss_ << '}';
}

void Dumper::DumpAstNodes(std::vector<AstNode*> ast_nodes) {
  ss_ << '[';
  ++indent_;

  bool first = false;
  for (auto ast_node : ast_nodes) {
    if (first) {
      ss_ << ',';
    }
    
    ss_ << '\n';
    AddIndent();
    
    DumpAstNode(ast_node);
    
    first = true;
  }

  ss_ << '\n';
  --indent_;
  AddIndent();
  ss_ << ']';
}

void Dumper::AddIndent() {
  for (int i = 0; i < 2 * indent_; ++i) {
    ss_ << ' ';
  }
}

}  // namespace ast
}  // namespace voidjs
