//
// Created by lorenzo on 12/5/17.
//
#include "defs.h"
#include <cstdio>
using namespace lns;
class print_c : public native_callable{
public:
    print_c() : native_callable(1) {}

    const std::string &name() const override {
        return S(print);
    }

    object *call(std::vector<object *> &args) override {
        printf(args[0]->str().c_str());
    }
};


extern "C" object* supplier(){
    return new print_c;
}