//
// Created by lorenzo on 11/30/17.
//
#include "exceptions.h"

#define CMD_USAGE "- lns [script] [options...]"

using namespace lns;
using namespace std;

const char *lns::unknown_option_exception::what() const throw(){
    string s = FATAL_ERROR_S;
    s += ": unknown option '";
    s += option;
    s += "'.";
    return concat({s});
}

lns::unknown_option_exception::unknown_option_exception(string o) : option(o) {}

const char *lns::memory_leak_exception::what() const throw(){
    return concat({FATAL_ERROR_S, ": a memory leak occurred during execution."});
}

const char *lns::wrong_usage_exception::what() const throw(){
    return concat({
                          FATAL_ERROR_S, ": wrong usage. Usage:\n        " CMD_USAGE});
}

lns::file_not_found_exception::file_not_found_exception(string f) : filename(f) {}

const char *lns::file_not_found_exception::what() const throw(){
    return concat({FATAL_ERROR_S, ": file ", filename, " doesn't exist or is inaccessible."});
}

const char *lns::invalid_operator_exception::what() const throw(){
    return concat({"Invalid operator '" + op + "'" + " for type " + type + "."});
}

lns::invalid_operator_exception::invalid_operator_exception(string op, string type) : op(op), type(type) {}

const char *lns::parse_exception::what() const throw(){
    return "\nParsing concluded with errors.";
}

const char *lns::cmd_processor_unavailable::what() const throw() {
    return concat({FATAL_ERROR_S, ": couldn't call system(), command processor unavailable."});
}

return_signal::return_signal(std::shared_ptr<object> value, const token *keyword) : value(value), keyword(keyword) {}

const char *lns::return_signal::what() const throw() {
    return exception::what();
}

const char *lns::break_signal::what() const throw() {
    return exception::what();
}

break_signal::break_signal(const token *keyword) : keyword(keyword) {}

continue_signal::continue_signal(const token *keyword) : keyword(keyword) {}

const char *continue_signal::what() const throw() {
    return exception::what();
}
//runtime_environment::~runtime_environment() {}


lns::runtime_exception::runtime_exception(const lns::token *token, std::string m) : native_throw(false), message(m),
                                                                                    token(token) {}

const char *lns::runtime_exception::what() const throw() {
    return message.c_str();
}

runtime_exception::runtime_exception(const char *filename, int line, std::string message) : native_throw(true),
                                                                                            message(message),
                                                                                            token(new lns::token(
                                                                                                    UNRECOGNIZED,
                                                                                                    STR(""),
                                                                                                    make_shared<null_o>(),
                                                                                                    filename, line)) {}

incode_exception::incode_exception(const lns::token *token, std::string name,
                                   const std::map<std::string, std::shared_ptr<object>> fields)
        : lns::runtime_exception(token, name), lns::object(EXCEPTION_T), fields(fields), calls_bypassed(0) {}

std::shared_ptr<object> incode_exception::get(std::string &key) {
    if (fields.find(key) != fields.end()) return this->fields[key];
    return nullptr;
}

bool incode_exception::operator==(const object &o) const {
    return this == &o;
}

std::string incode_exception::str() const {
    stringstream s;
    s << "<exception@" << static_cast<const void *>(this) << ", name: \"" << message << "\">";
    return s.str();
}

shared_ptr<object> incode_exception::clone() const {
    return make_shared<incode_exception>(token, message, fields);
}

const char *incode_exception::what() const throw() {
    string &s = *new string();
    s += "unhandled exception: \"";
    s += message;
    s += "\"";
    if (this->fields.find("message") != this->fields.end()) {
        s += " (message = \"";
        s += fields.at("message")->str();
        s += "\")";
    }
    return s.c_str();
}