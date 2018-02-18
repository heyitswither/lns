//
// Created by lorenzo on 12/5/17.
//
#include "utils.h"
#include "callable.h"
#include <string>
#include <iostream>
#include <chrono>

using namespace lns;
using namespace std;
using namespace chrono;

class time_c : public native_callable {
public:

    time_c() : native_callable(0) {}

    const std::string name() const override {
        return std::string("unix_timestamp");
    }

    shared_ptr <object> call(std::vector<shared_ptr < object>>

    &args)

    override {
        return make_shared<number_o>(duration_cast<milliseconds>(system_clock::now().time_since_epoch()).count());
    }
};


extern "C" shared_ptr <object> supplier() {
    return make_shared<time_c>();
}