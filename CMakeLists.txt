cmake_minimum_required(VERSION 3.8)
project(CppLNS)
set(CMAKE_CXX_STANDARD 11)

#set(CMAKE_VERBOSE_MAKEFILE ON)

set(SOURCE_FILES source/lns.cpp source/errors.h source/exceptions.h source/scanner.h source/utils.h source/parser.h source/options.h source/interpreter.h source/debug.h source/commands.h source/commands.cpp source/debug.cpp source/utils.cpp source/errors.cpp source/exceptions.cpp source/interpreter.cpp source/parser.cpp source/scanner.cpp source/stmt.h source/expr.h source/options.cpp source/signals.cpp source/signals.h source/error_message.h source/update.cpp source/update.h source/revision.h source/token.cpp source/token.h source/primitives.cpp source/primitives.h source/callable.cpp source/callable.h source/environments.cpp source/environments.h)
set(CMAKE_CXX_FLAGS "-Wall -fpermissive -rdynamic -Wl,--no-as-needed -ldl")
add_executable(CppLNS ${SOURCE_FILES})