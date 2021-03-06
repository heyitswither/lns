//
// Created by lorenzo on 11/30/17.
//

#include <iostream>
#include "errors.h"


using namespace std;
using namespace lns;
bool errors::had_parse_error = false;
bool errors::had_runtime_error = false;
int  errors::exit_status;
void errors::parse_error(const char *&file, int line, const char *message) {
    report(PARSE_ERROR_S, file, line, "", message);
}

void errors::runtime_error(lns::runtime_exception &error, std::vector<lns::stack_call *> &stack) {
    had_runtime_error = true;
    if (lns::silent_execution || lns::silent_full) return;
    lns::token *t = const_cast<token *>(error.token);
    auto *first = new lns::stack_call(t->filename, t->line, t->lexeme,error.native_throw);
    string stringstack = gen_stack_trace_desc(first, stack);
    cerr << RUNTIME_ERROR_S << " in file ";
    cerr << t->filename << ":" << t->line;
    cerr << ": ";
    cerr << error.what();
    cerr << ".\n";
    cerr << stringstack;
    exit_status = DCAST_CHK(lns::incode_exception*,&error) ? UNHANDLED_EXCEPTION : RUNTIME_ERROR;
}

string errors::gen_stack_trace_desc(lns::stack_call *first_call, std::vector<lns::stack_call *> &stack) {
    stringstream ss;
    if (stack.empty()) return string();
    lns::stack_call *current = nullptr;
    ss << "Stack trace (last calls first):\n";
    if (!first_call->native)
        ss << "       at token \"" + first_call->method + "\" (" + first_call->filename + ":" +
              to_string(first_call->line) + "), in\n";
    while (!stack.empty()) {
        current = stack.back();
        ss << "       ";
        if (current->native) ss << "in native ";
        ss << "function " << current->method << "(), called at " << current->filename << ":" << to_string(current->line)
           << " by\n";
        stack.pop_back();
    }
    string s = ss.str();
    s = s.substr(0, s.size() - 4) + ".\n";
    return s;
}

void errors::fatal_error(exception &e, int code) {
    cerr << e.what() << endl;
    cout << endl;
    exit(code);
}

void errors::report(const char *type, const char *filename, int line, const char *where, const char *message) {
    had_parse_error = true;
    errors::exit_status = PARSE_ERROR;
    if (lns::silent_full) return;
    cerr << type << " in file " << filename << ":" << line << (where == "" ? "" : ",") << where << ": " << message
         << "." << endl;
}