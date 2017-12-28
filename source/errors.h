//
// Created by lorenzo on 31/08/17.
//

#ifndef CPPLNS_ERRORS_H
#define CPPLNS_ERRORS_H

#include "defs.h"
#include "exceptions.h"
#include "error_message.h"

#define EXIT_SUCCESS 0
#define FATAL_ERROR 1
#define PARSE_ERROR 2
#define RUNTIME_ERROR 3
#define UNHANDLED_EXCEPTION 4

namespace errors {
    extern bool had_parse_error;
    extern int exit_status;
    extern bool had_runtime_error;

    void fatal_error(std::exception &e, int code);

    void report(const char *type, const char *filename, int line, const char *where, const char *message);

    void parse_error(const char *&file, int line, const char *message);

    void runtime_error(lns::runtime_exception &error, std::vector<lns::stack_call *> &stack);

    std::string &gen_stack_trace_desc(lns::stack_call *first_call, std::vector<lns::stack_call *> &stack);

}
#endif //CPPLNS_ERRORS_H
