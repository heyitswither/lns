//
// Created by lorenzo on 31/08/17.
//

#ifndef CPPLNS_ERRORS_H
#define CPPLNS_ERRORS_H
#include "defs.h"
#include "exceptions.h"

#define EXIT_SUCCESS 0
#define FATAL_ERROR 1
#define PARSE_ERROR 2
#define RUNTIME_ERROR 3
#define UNHANDLED_EXCEPTION 4


/*  SCANNER ERRORS  */
#define UNTERMINATED(what) "unterminated " what
#define INVALID_ESCAPE_CHARACTER(c) createDescription({"Invalid escape character '", c, "'"})
#define UNEXPECTED_CHARACTER(c) createDescription({"Unexpected character '", c, "'"})
#define UNTERMINATED_BLOCK_COMMENT "unterminated block comment"

/* PARSER ERRORS */
#define USE_NOT_ALLOWED "'use' statements are not allowed at this location"
#define EXPECTED(what) "expected " what
#define EXPECTED_AFTER(what, after) "expected " what " after " after
#define EXPECTED_IN(what,in) "expected " what " in " in
#define CAN_ONLY_CONTAIN(a,b) a " can only contain " b
#define EXPTOCLOSE(what, line) createDescription({"expected 'end' to close ", what ," (opened at line ", std::to_string(line), ")"})
#define ACCESS_SPEC_NOT_ALLOWED(key,for_) "Access specifier '" key "' is not allowed for " for_ " (--permissive)"
#define CONSTRUCTOR_OUTSIDE_OF_CLASS "constructor outside of class"
#define INVALID_STATEMENT "invalid statement"
#define INVALID_ASSIGNMENT_TARGET "invalid assignment target"
#define WITHOUT(what,without) what " without " without
#define MISSING_CLOSING(what) "missing closing " what
#define USE_FILE_NOT_FOUND "use: file was not found or is inaccessible"
#define DUPLICATE_ACCESS_SPEC(spec) "duplicate access specifier: '" spec "'"
#define CONFILICTING_SPECS(a,b) "conflicting access specifiers '" a "' and '" b "'"
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
