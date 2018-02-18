//
// Created by lorenzo on 12/5/17.
//
#include "utils.h"
#include "callable.h"
#include <string>
#include <iostream>

using namespace lns;
using namespace std;

class read_c : public native_callable{
public:

    read_c() : native_callable(0) {}

    const std::string name() const override {
        return std::string("read");
    }

    shared_ptr <object> call(std::vector<shared_ptr < object>>

    &args)

    override {
        auto str = make_shared<string_o>();
        cin >> str->value;
        return str;
    }
};


extern "C" shared_ptr <object> supplier() {
    return make_shared<read_c>();
}