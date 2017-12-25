//
// Created by lorenzo on 11/30/17.
//
#include "exceptions.h"

#define CMD_USAGE "- lns [script] [options...]"
using namespace std;
const char *lns::createDescription(initializer_list<string> ss) {
    string s;
    for(auto &s1 : ss){
        s.append(s1);
    }
    s.append("\n");
    char *ret = (char *) malloc(sizeof(char) * s.size());
    strcpy(ret, s.c_str());
    return ret;
}

const char *lns::unknown_option_exception::what() const throw(){
    string s = FATAL_ERROR_S;
    s += ": unknown option '";
    s += option;
    s += "'.";
    return createDescription({s});
}

lns::unknown_option_exception::unknown_option_exception(string o) : option(o) {}

const char *lns::memory_leak_exception::what() const throw(){
    return createDescription({FATAL_ERROR_S,": a memory leak occurred during execution."});
}

const char *lns::wrong_usage_exception::what() const throw(){
    return createDescription({
            FATAL_ERROR_S,": wrong usage. Usage:\n        " CMD_USAGE});
}

lns::file_not_found_exception::file_not_found_exception(string f) : filename(f) {}

const char *lns::file_not_found_exception::what() const throw(){
    return createDescription({"File ", filename," doesn't exist or is inaccessible."});
}

const char *lns::invalid_operator_exception::what() const throw(){
    return createDescription({"Invalid operator '" + op + "'" + " for type " + type + "."});
}

lns::invalid_operator_exception::invalid_operator_exception(string op, string type) : op(op), type(type) {}

const char *lns::parse_exception::what() const throw(){
    return createDescription({"\nParsing concluded with errors."});
}
