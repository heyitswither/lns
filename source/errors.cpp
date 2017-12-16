//
// Created by lorenzo on 11/30/17.
//

#include <iostream>
#include "errors.h"
#include "options.h"


using namespace std;
using namespace lns;
bool errors::had_parse_error = false;
bool errors::had_runtime_error = false;
void errors::parse_error(const char *&file, int line, const char *message) {
    report(PARSE_ERROR_S, file, line, "", message);
}

void errors::runtime_error(lns::runtime_exception &error, std::vector<lns::stack_call *> &stack) {
    had_runtime_error = true;
    if (lns::silent_execution || lns::silent_full) return;
    lns::token t = error.token;
    auto *first = new lns::stack_call(t.filename, t.line, t.lexeme,error.native_throw);
    string &stringstack = gen_stack_trace_desc(first, stack);
    cerr << RUNTIME_ERROR_S << " in file ";
    cerr << t.filename << ":" << t.line;
    cerr << ": ";
    cerr << error.what();
    cerr << ".\n";
    cerr << stringstack << endl;
    //printf("%s in file %s%s: %s.\n%s",RUNTIME_ERROR_S,error.token.filename,stringstack.empty() ? (":" + error.token.line) : "",error.what(),stringstack.c_str());
}

string &errors::gen_stack_trace_desc(lns::stack_call *first_call, std::vector<lns::stack_call *> &stack) {
    string &s = *new std::string(""), s2 = *new std::string("");
    if (stack.empty()) return s;
    string method;
    const char *filename;
    int line;
    lns::stack_call *current = nullptr;
    //cout << stack[2]->method << endl;
    while (!stack.empty()) {
        current = stack.back();
        method = current->method;
        filename = current->filename;
        line = current->line;
        s2 += "       ";
        if(current->native) s2+="in native ";
        s2 += "function ";
        s2 += method;
        s2 += "(), called at ";
        s2 += filename;
        s2 += ":";
        s2 += to_string(line);
        s2 += " by\n";
        stack.pop_back();
    }
    s += "Stack trace (last calls first):\n";
    if(!first_call->native)
        s += "       at token \"" + first_call->method + "\" (" + first_call->filename + ":" + to_string(first_call->line) + "), in\n";

    s += s2;
    s = s.substr(0, s.size() - 4) + ".";
    return s;
}

void errors::fatal_error(exception &e, int code) {
    cerr << e.what() << endl;
    cout << endl;
    exit(code);
}

void errors::report(const char *type, const char *filename, int line, const char *where, const char *message) {
    had_parse_error = true;
    if (lns::silent_full) return;
    cerr << type << " in file " << filename << ":" << line << (where == "" ? "" : ",") << where << ": " << message
         << "." << endl;
}