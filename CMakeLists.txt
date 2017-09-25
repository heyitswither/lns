cmake_minimum_required(VERSION 3.8)
project(CppLNS)
set(CMAKE_CXX_STANDARD 17)

set(SOURCE_FILES source/main.cpp source/errors.h source/exceptions.h source/scanner.h source/defs.h source/expr.h source/stmt.h source/parser.h source/options.h source/interpreter.h source/environments.h)
set(CMAKE_CXX_FLAGS "-Wall -fpermissive")
add_executable(CppLNS ${SOURCE_FILES})