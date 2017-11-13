//
// Created by lorenzo on 31/08/17.
//

#ifndef CPPLNS_ERRORS_H
#define CPPLNS_ERRORS_H
#include <exception>
#include <vector>
#include "exceptions.h"
#include "options.h"
#define EXIT_UNKNOWN -1
using namespace std;
    namespace errors{
        static bool had_parse_error = false;
        static bool had_runtime_error = false;
        void fatal_error(exception& e){
            cerr << e.what() << endl;
            cout << endl;
            exit(-1);
        }

        void report(const char* type, const char* filename, int line, const char* where, const char* message) {
            had_parse_error = true;
            if(lns::silent_full) return;
            cerr << type << " in file " << filename << ":" << line << (where == "" ? "" : ",") << where << ": " << message << "." << endl;
        }
        void parse_error(const char* &file, int line, const char* message) {
            report(PARSE_ERROR, file, line, "", message);
        }

        string& gen_stack_trace_desc(lns::stack_call *pCall, vector<lns::stack_call*>& vector);

        void runtime_error(runtime_exception& error, std::vector<lns::stack_call*>& stack){
            had_runtime_error = true;
            if(lns::silent_execution || lns::silent_full) return;
            lns::token t = move(error.token);
            lns::stack_call* first = new lns::stack_call(t.filename, t.line, t.lexeme);
            string& stringstack = gen_stack_trace_desc(first, stack);
            cerr << RUNTIME_ERROR << " in file ";
            cerr << t.filename;
            cerr << (stringstack.empty() ? (string(":") + to_string(t.line)).c_str() : "");
            cerr << ": ";
            cerr << error.what();
            cerr << ".\n";
            cerr<< stringstack;
            //printf("%s in file %s%s: %s.\n%s",RUNTIME_ERROR,error.token.filename,stringstack.empty() ? (":" + error.token.line) : "",error.what(),stringstack.c_str());
        }

        string& gen_stack_trace_desc(lns::stack_call *first_call, vector<lns::stack_call*>& stack) {
            string& s = *new std::string(""),s2 = *new std::string("");
            if(lns::prompt || stack.empty()) return s;
            string method;
            const char* filename;
            int line;
            lns::stack_call* current = nullptr;
            //cout << stack[2]->method << endl;
            while(!stack.empty()){
                current = move(stack.back());
                method = current->method;
                filename = current->filename;
                line = current->line;
                s2+="       function ";
                s2+= method;
                s2+= "(), called at ";
                s2+= filename;
                s2+= ":";
                s2+= to_string(line);
                s2+= " by\n";
                stack.pop_back();
            }
            s+="Stack trace (last calls first):\n";
            s+="       at token \""+ first_call->method + "\" (" + first_call->filename +":" + to_string(first_call->line) + "), in\n";
            s+=s2;
            s = s.substr(0,s.size()-4) + ".";
            return s;
        }
    }
#endif //CPPLNS_ERRORS_H
