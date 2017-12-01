//
// Created by lorenzo on 31/08/17.
//

#ifndef CPPLNS_ERRORS_H
#define CPPLNS_ERRORS_H
#include "defs.h"
#include "exceptions.h"
#define EXIT_UNKNOWN (-1)
namespace errors {
    static bool had_parse_error = false;
    static bool had_runtime_error = false;

    void fatal_error(std::exception &e);

    void report(const char *type, const char *filename, int line, const char *where, const char *message);

    void parse_error(const char *&file, int line, const char *message);

    void runtime_error(lns::runtime_exception &error, std::vector<lns::stack_call *> &stack);

    std::string &gen_stack_trace_desc(lns::stack_call *first_call, std::vector<lns::stack_call *> &stack);

}
#endif //CPPLNS_ERRORS_H
