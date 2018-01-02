//
// Created by lorenzo on 12/5/17.
//
#include "defs.h"
#include <string>
#include <iostream>

using namespace lns;
using namespace std;

class readln_c : public native_callable{
public:
    readln_c() : native_callable(0) {}

    const std::string name() const override {
        return string("readln");
    }

    shared_ptr <object> call(std::vector<shared_ptr < object>>

    &args)

    override {
        auto str = make_shared<string_o>();
        getline(cin,str->value);
        return str;
    }
};


extern "C" shared_ptr <object> supplier() {
    return make_shared<readln_c>();
}