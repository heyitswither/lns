//
// Created by lorenzo on 31/08/17.
//
#ifndef CPPLNS_EXCEPTIONS_H
#define CPPLNS_EXCEPTIONS_H

#include <exception>
#include <string>
#include "defs.h"
#include "cstring"

#define FATAL_ERROR_S   "\n\033[1;31mFatal error\033[0m"
#define PARSE_ERROR_S   "\n\033[1;31mParse error\033[0m"
#define RUNTIME_ERROR_S "\n\033[1;31mRuntime error\033[0m"

namespace lns {

    const char *createDescription(std::string s);

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
