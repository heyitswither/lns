//
// Created by lorenzo on 12/5/17.
//
#include "defs.h"
#include <iostream>
using namespace lns;
class print_c : public callable{
public:
    const int arity() const override {
        return 1;
    }

    const std::string &name() const override {
        return S(print);
    }

    object *call(std::vector<object *> &args) override {
        std::cout << args[0]->str();
    }
};


extern "C" object* supplier(){
    return new print_c;
}