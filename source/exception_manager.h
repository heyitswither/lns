//
// Created by lorenzo on 12/18/17.
//

#ifndef CPPLNS_EXCEPTION_MANAGER_H
#define CPPLNS_EXCEPTION_MANAGER_H

#include "defs.h"
#include <set>

namespace lns {
    class exception_definition{
    public:
        const std::string &name;
        const std::set<std::string>& fields;
        const char* def_file;
        int def_line;
        exception_definition(const char* def_file, int def_line, const std::string& name, const std::set<std::string>& fields);
    };
    class exception_manager {
    public:
        std::set<lns::exception_definition *>& definitions;
        exception_manager();
        void define(lns::exception_definition* def);
        lns::exception_definition* get(const std::string& name) const;
    };
}
#endif //CPPLNS_EXCEPTION_MANAGER_H
