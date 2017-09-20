cmake_minimum_required(VERSION 3.8)
project(CppLNS)
set(CMAKE_CXX_STANDARD 17)

set(SOURCE_FILES main.cpp errors.h exceptions.h scanner.h defs.h expr.h stmt.h parser.h options.h interpreter.h environments.h)
set(CMAKE_CXX_FLAGS "-Wall -fpermissive")
add_executable(CppLNS ${SOURCE_FILES})