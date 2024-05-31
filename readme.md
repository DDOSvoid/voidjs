# Voidjs

## Introduction

Voidjs is a JavaScript interpreter, it supports most of the ES5.1 specification, expect for Date and RegExp.

## Usage

### Installation

A CMakeLists.txt is provided to compile the interpreter on Linux/MacOS/Windows. Note that only 64 bit operating systems are supported.

### Run unit tests

Use the command below to run all unit tests.
```bash
./bin/unit_test
```

### Command line options

```bash
voidjs [options] [file]
```

Options are: 

--dump-ast 

Dump AST to JSON files.

## Acknowledgement

I've learned a lot from [Constellation/iv](https://github.com/Constellation/iv), [OpenHarmony/arkcompiler_ets_runtime ](https://gitee.com/openharmony/arkcompiler_ets_runtime), [V8](https://v8.dev/), [zhuzilin/es](https://github.com/zhuzilin/es).

Also, thanks a lot to the specification file [Annotated ECMAScript 5.1](https://es5.github.io/#x15.3.5.3).
