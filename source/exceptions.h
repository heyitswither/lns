//
// Created by lorenzo on 31/08/17.
//
#ifndef CPPLNS_EXCEPTIONS_H
#define CPPLNS_EXCEPTIONS_H

#include <exception>
#include <string>
#include "utils.h"
#include "options.h"
#include "token.h"
#include <initializer_list>

#define FATAL_ERROR_UNFORMATTED   "Fatal error"
#define PARSE_ERROR_UNFORMATTED   "Parse error"
#define RUNTIME_ERROR_UNFORMATTED "Runtime error"

#define FATAL_ERROR_FORMATTED   "\033[1;31m" FATAL_ERROR_UNFORMATTED   "\033[0m"
#define PARSE_ERROR_FORMATTED   "\033[1;31m" PARSE_ERROR_UNFORMATTED   "\033[0m"
#define RUNTIME_ERROR_FORMATTED "\033[1;31m" RUNTIME_ERROR_UNFORMATTED "\033[0m"

#define FATAL_ERROR_S   (lns::no_format ? (FATAL_ERROR_UNFORMATTED) : (FATAL_ERROR_FORMATTED))
#define PARSE_ERROR_S   (lns::no_format ? (PARSE_ERROR_UNFORMATTED) : (PARSE_ERROR_FORMATTED))
#define RUNTIME_ERROR_S (lns::no_format ? (RUNTIME_ERROR_UNFORMATTED) : (RUNTIME_ERROR_FORMATTED))

namespace lns {

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

    class cmd_processor_unavailable : public std::exception {
        const char *what() const throw() override;
    };

    class return_signal : public std::exception {
    public:
        std::shared_ptr<lns::object> value;
        const token *keyword;

        return_signal(std::shared_ptr<object> value, const token *keyword);

        const char *what() const throw() override;
    };

    class break_signal : public std::exception {
    public:
        const token *keyword;

        explicit break_signal(const token *keyword);

        const char *what() const throw() override;
    };

    class continue_signal : public std::exception {
    public:
        const token *keyword;

        const char *what() const throw() override;

        explicit continue_signal(const token *keyword);
    };

    class runtime_exception : public std::exception {
    public:
        std::string message;
        bool native_throw;
        const lns::token *token;

        runtime_exception(const char *filename, int line, std::string message);

        runtime_exception(const lns::token *token, std::string m);

        const char *what() const throw() override;
    };

    class incode_exception : public lns::runtime_exception, public lns::object {
    private:
        std::map<std::string, std::shared_ptr<object> > fields;
    public:
        int calls_bypassed;

        incode_exception(const lns::token *token, std::string m,
                         const std::map<std::string, std::shared_ptr<object>> fields);

        std::shared_ptr<object> get(std::string &key);

        bool operator==(const object &o) const override;

        std::string str() const override;

        std::shared_ptr<object> clone() const override;

        const char *what() const throw() override;
    };

}

#endif //CPPLNS_EXCEPTIONS_H
