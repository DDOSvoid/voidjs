#include <iostream>
#include <fstream>
#include <sstream>

#include "voidjs/gc/js_handle_scope.h"
#include "voidjs/interpreter/interpreter.h"
#include "voidjs/interpreter/vm.h"
#include "voidjs/ir/ast.h"
#include "voidjs/ir/program.h"
#include "voidjs/parser/parser.h"
#include "voidjs/utils/helper.h"

std::string ReadFile(char* filename) {
  std::ifstream file{filename};
  std::stringstream buffer;
  buffer << file.rdbuf();
  std::string file_content = buffer.str();
  return file_content;
}

void ExecuteFile(char* filename) {
  std::u16string source = voidjs::utils::U8StrToU16Str(ReadFile(filename));

  voidjs::Parser parser{source};
  voidjs::ast::Program* program = parser.ParseProgram();
  if (!program) {
    return ;
  }
  
  voidjs::Interpreter interpreter;
  voidjs::VM* vm = interpreter.GetVM();
  voidjs::JSHandleScope top_handle_scope{vm};

  interpreter.Execute(program);
}

int main(int argc, char* argv[]) {
  if (argc < 2) {
    std::cout << "No filename supplied." << "\n";
  } else {
    ExecuteFile(argv[1]);
  } 
  return 0; 
}
