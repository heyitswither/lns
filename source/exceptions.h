//
// Created by lorenzo on 31/08/17.
//
#ifndef CPPLNS_EXCEPTIONS_H
#define CPPLNS_EXCEPTIONS_H

#include <exception>
#include <string>
#include "defs.h"
#include "cstring"
#include "options.h"
#include <initializer_list>
#define FATAL_ERROR_UNFORMATTED   "Fatal error"
#define PARSE_ERROR_UNFORMATTED   "Parse error"
#define RUNTIME_ERROR_UNFORMATTED "Runtime error"

#define FATAL_ERROR_FORMATTED   "\033[1;31m" FATAL_ERROR_UNFORMATTED   "\033[0m"
#define PARSE_ERROR_FORMATTED   "\033[1;31m" PARSE_ERROR_UNFORMATTED   "\033[0m"
#define RUNTIME_ERROR_FORMATTED "\033[1;31m" RUNTIME_ERROR_UNFORMATTED "\033[0m"

#define FATAL_ERROR_S   (lns::no_format ? FATAL_ERROR_UNFORMATTED : FATAL_ERROR_FORMATTED)
#define PARSE_ERROR_S   (lns::no_format ? PARSE_ERROR_UNFORMATTED : PARSE_ERROR_FORMATTED)
#define RUNTIME_ERROR_S (lns::no_format ? RUNTIME_ERROR_UNFORMATTED : RUNTIME_ERROR_FORMATTED)

namespace lns {

    const char *createDescription(std::initializer_list<std::string> ss);

    class unknown_option_exception : public std::exception {
        std::string option;
    public:
        explicit unknown_option_exception(std::string o);

        const char *what() const throw() override;
    };

    class memory_leak_exception : public std::exception {
    public:
        const char *what() const throw() override;
    };

    class wrong_usage_exception : public std::exception {
    public:
        const char *what() const throw() override;
    };

    class file_not_found_exception : public std::exception {
    private:
        std::string filename;
    public:
        explicit file_not_found_exception(std::string f);

        const char *what() const throw() override;
    };

    class invalid_operator_exception : public std::exception {
    private:
        std::string op;
        std::string type;
    public:
        invalid_operator_exception(std::string op, std::string type);

        const char *what() const throw() override;
    };

    class not_implemented_exception : public std::exception {
    };

    class parse_exception : public std::exception {
        const char *what() const throw() override;
    };
}
#endif //CPPLNS_EXCEPTIONS_H
