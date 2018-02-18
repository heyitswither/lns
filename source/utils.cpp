//
// Created by lorenzo on 11/30/17.
//
#include "utils.h"

using namespace lns;
using namespace std;

stack_call::stack_call(const char *filename, const int line, const string method, const bool native) : filename(
        filename), line(line),
                                                                                                       method(method),
                                                                                                       native(native) {}

const char *lns::best_file_path(const char *filename) {
    if (file_exists(filename)) return filename;
    auto buf = (char *) malloc(1024 * sizeof(char));
    *buf = '\0';
    strcat(buf, LNS_LIBRARY_LOCATION);
    strcat(buf, filename);
    if (file_exists(buf)) return buf;
    strcat(buf, ".lns");
    if (file_exists(buf)) return buf;
    return filename;
}

const char *lns::concat(initializer_list<string> ss) {
    string s;
    for (auto &s1 : ss) {
        s.append(s1);
    }
    char *ret = (char *) malloc(sizeof(char) * s.size() + 1);
    strcpy(ret, s.c_str());
    return ret;
}

inline bool lns::file_exists(const char *name) {
    ifstream f(name);
    return f.good();
}


lns::parameter_declaration::parameter_declaration(std::vector<parameter> &parameters) : parameters(parameters) {}

lns::parameter_declaration::parameter_declaration() : parameters(*new std::vector<lns::parameter>()) {}

const int lns::parameter_declaration::required() const {
    int required = 0;
    for (auto &item : parameters) {
        if (item.optional) break;
        ++required;
    }
    return required;
}

const int lns::parameter_declaration::optional() const {
    return parameters.size() - required();
}

parameter_declaration::parameter_declaration(int nr) : parameter_declaration() {
    for (int i = 0; i < nr; i++)
        parameters.push_back(parameter(string(std::to_string(i)), false, std::shared_ptr<expr>()));
}

parameter::parameter(string name, bool optional, std::shared_ptr<expr> default_value) : name(name), optional(optional),
                                                                                        default_value(default_value) {}
