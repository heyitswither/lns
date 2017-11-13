//
// Created by lorenzo on 31/08/17.
//
#ifndef CPPLNS_EXCEPTIONS_H
#define CPPLNS_EXCEPTIONS_H

#include <exception>
#include <string>
#include <string.h>
#include "defs.h"
#define FATAL_ERROR   "\n\033[1;31mFatal error\033[0m"
#define PARSE_ERROR   "\n\033[1;31mParse error\033[0m"
#define RUNTIME_ERROR "\n\033[1;31mRuntime error\033[0m"
using namespace std;
    const char* createDescription(string s){
        s.append("\n");
        char* ret = (char*)malloc(s.size());
        strcpy(ret,s.c_str());
        return ret;
    }
class unknown_option_exception : public std::exception{
    string option;
public:
    unknown_option_exception(string o) : option(o){}

    virtual const char* what() const throw(){
        string s = FATAL_ERROR ": unknown option '";
        s+=option;
        s+="'.";
        return createDescription(s);
    }
};
class memory_leak_exception : public std::exception{
public:
    virtual const char* what() const throw(){
        return createDescription(FATAL_ERROR ": a memory leak occurred during execution.");
    }
};
class wrong_usage_exception : public std::exception{
public:
    virtual const char* what() const throw(){
        return createDescription(FATAL_ERROR ": wrong usage. Usage:\n        - lns [script] [-p|q-s|-S|--permissive|-t|-o]");
    }
};
class file_not_found_exception : public std::exception{
private:
    string filename;
public:
    file_not_found_exception(string f) : filename(f) {}
    virtual const char* what() const throw(){
        return createDescription("File " + filename + " doesn't exist or is inaccessible.");
    }
};
class invalid_operator_exception : public std::exception{
private:
    string op;
    string type;
public:
    invalid_operator_exception(string op, string type) : op(op), type(type){}
    virtual const char* what() const throw(){
        return createDescription("Invalid operator '" + op + "'" + " for type " + type + ".");
    }
};
class not_implemented_exception : public std::exception{};
class parse_exception : public std::exception{
    virtual const char* what() const throw(){
        return createDescription("\nParsing concluded with errors.");
    }
};
class runtime_exception : public std::exception{
public:
    string& message;
    const lns::token& token;
    runtime_exception(const lns::token& token,string& m) : message(m), token(token){}
    virtual const char* what() const throw(){
        return message.c_str();
    }
};

#endif //CPPLNS_EXCEPTIONS_H
