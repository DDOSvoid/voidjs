#include <iostream>
#include <fstream>
#include <sstream>
#include <functional>
#include <map>

#include "voidjs/ir/ast.h"
#include "voidjs/ir/program.h"
#include "voidjs/ir/dumper.h"
#include "voidjs/parser/parser.h"
#include "voidjs/types/heap_object.h"
#include "voidjs/types/js_value.h"
#include "voidjs/types/object_factory.h"
#include "voidjs/builtins/js_error.h"
#include "voidjs/gc/js_handle.h"
#include "voidjs/gc/js_handle_scope.h"
#include "voidjs/interpreter/interpreter.h"
#include "voidjs/interpreter/vm.h"
#include "voidjs/utils/helper.h"

std::string ReadFile(char* filename) {
  std::ifstream file{filename};
  std::stringstream buffer;
  buffer << file.rdbuf();
  std::string file_content = buffer.str();
  return file_content;
}

void ExecuteFile(char* filename) {
  using namespace voidjs;
  
  std::u16string source = voidjs::utils::U8StrToU16Str(ReadFile(filename));

  Parser parser{source};
  ast::Program* program = parser.ParseProgram();
  if (!program) {
    return ;
  }
  
  Interpreter interpreter;
  VM* vm = interpreter.GetVM();
  JSHandleScope top_handle_scope{vm};

  types::Completion comp = interpreter.Execute(program);
  if (vm->HasException()) {
    JSHandle<types::String> msg = types::Object::Call(vm, vm->GetObjectFactory()->NewInternalFunction(voidjs::builtins::JSError::ToString),
                                                      vm->GetException().As<voidjs::JSValue>(), {}).As<types::String>();
    std::cout << utils::U16StrToU8Str(std::u16string{msg->GetString()}) << std::endl;
  }
}

void DumpAst(char* filename) {
  std::u16string source = voidjs::utils::U8StrToU16Str(ReadFile(filename));

  voidjs::Parser parser{source};
  voidjs::ast::Program* program = parser.ParseProgram();
  if (!program) {
    return ;
  }

  voidjs::ast::Dumper dumper{program};
  std::cout << dumper.GetString() << std::endl;
}

void ParseCommandAndExecute(int argc, char* argv[]) {
  if (argc < 2) {
    std::cout << "No filename supplied." << "\n";
    return ;
  }

  std::map<std::string, std::function<void(char*)>> commands = {
    {"dump-ast", DumpAst},
  };

  char* filename = argv[argc - 1];

  for (int i = 1; i + 1 < argc; ++i) {
    std::size_t len = std::strlen(argv[i]);
    if (len < 2 || argv[i][0] != '-' || argv[i][1] != '-') {
      continue;
    }

    if (auto iter = commands.find(std::string{argv[i] + 2, len - 2});
        iter != commands.end()) {
      (iter->second)(filename);
      return ;
    }
  }

  ExecuteFile(filename);
}

int main(int argc, char* argv[]) {
  ParseCommandAndExecute(argc, argv);
  return 0; 
}
