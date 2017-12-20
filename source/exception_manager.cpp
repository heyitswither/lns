//
// Created by lorenzo on 12/18/17.
//

#include "exception_manager.h"

lns::exception_definition::exception_definition(const char * file, int line, const std::string &name, const std::set<std::string> &fields) : name(name), fields(fields), def_file(file), def_line(line){}

lns::exception_manager::exception_manager() : definitions(*new std::set<exception_definition *>()) {}

void lns::exception_manager::define(lns::exception_definition *def) {
    exception_definition *prev = this->get(def->name);
    if(prev != nullptr) throw *prev;
    definitions.insert(def);
}

lns::exception_definition *lns::exception_manager::get(const std::string &name) const {
    for(auto &item : definitions){
        if(item->name == name) return item;
    }
    return nullptr;
}
