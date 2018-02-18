//
// Created by lorenzo on 31/08/17.
//

#ifndef CPPLNS_DEFS_H
#define CPPLNS_DEFS_H

#include <string>
#include <cstring>
#include <fstream>
#include <vector>
#include <memory>

#define NATIVE_RUNTIME_EXC(msg) throw runtime_exception(__FILE__,__LINE__,*new std::string(msg));
#define STR(S) *new string(S)

#define DCAST(a, b) (dynamic_cast<a>(b))
#define DCAST_ASN(a, b, c) a = DCAST(b,c)
#define DCAST_ASNCHK(a, b, c) ((DCAST_ASN(a,b,c)) != nullptr)
#define DCAST_CHK(b, c) (DCAST(b,c) != nullptr)

#define LNS_INSTALL_LOCATION "/lns/"
#define LNS_LIBRARY_LOCATION LNS_INSTALL_LOCATION "lib/"

#define DEBUGGER_WATCHES_FILENAME "..##DEBUG_WATCH##.."

#define CHARSTR(c) string(1,c)

#define CODE_LEAK_ERROR (-250)


namespace lns {

    class expr;

    const char *best_file_path(const char *filename);

    class stack_call {
    public:
        const char *filename;
        const int line;
        const std::string method;
        const bool native;

        stack_call(const char *filename, const int line, const std::string method, const bool native);
    };

    const char *concat(std::initializer_list<std::__cxx11::string> ss);

    inline bool file_exists(const char *name);

    class parameter {
    public:
        std::string name;
        bool optional;
        std::shared_ptr<expr> default_value;

        parameter(std::string name, bool optional, std::shared_ptr<expr> default_value);
    };

    class parameter_declaration {
    public:
        parameter_declaration(int i);

        std::vector<parameter> &parameters;

        parameter_declaration();

        explicit parameter_declaration(std::vector<parameter> &parameters);

        const int required() const;

        const int optional() const;
    };

}

#endif //CPPLNS_DEFS_H