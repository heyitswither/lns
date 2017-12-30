//
// Created by lorenzo on 12/5/17.
//
#include "defs.h"
#include <string>
#include <iostream>

using namespace lns;
using namespace std;

class read_c : public native_callable{
public:

    read_c() : native_callable(0) {}

    const std::string &name() const override {
        return S(read);
    }

    object *call(std::vector<object *> &args) override {
        string_o* str = new string_o();
        cin >> str->value;
        return str;
    }
};


extern "C" object* supplier(){
    return new read_c;
}