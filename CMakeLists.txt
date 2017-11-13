cmake_minimum_required(VERSION 3.8)
project(CppLNS)
set(CMAKE_CXX_STANDARD 17)

set(SOURCE_FILES source/lns.cpp source/errors.h source/exceptions.h source/scanner.h source/defs.h source/expr.h source/stmt.h source/parser.h source/options.h source/interpreter.h source/environments.h source/debug.h source/commands.h source/debugger_defs.h)
set(CMAKE_CXX_FLAGS "-Wall -fpermissive")
add_executable(CppLNS ${SOURCE_FILES})