//
// Created by lorenzo on 12/5/17.
//
#include "defs.h"
#include <string>
#include <iostream>

using namespace lns;
using namespace std;

class readln_c : public callable{
public:
    const int arity() const override {
        return 0;
    }

    const std::string &name() const override {
        return S(readln);
    }

    object *call(std::vector<object *> &args) override {
        string_o* str = new string_o();
        getline(cin,str->value);
        return str;
    }
};


extern "C" object* supplier(){
    return new readln_c;
}