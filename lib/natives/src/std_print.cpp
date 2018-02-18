//
// Created by lorenzo on 12/5/17.
//
#include "utils.h"
#include "callable.h"
#include <cstdio>
using namespace lns;
using namespace std;
class print_c : public native_callable{
public:
    print_c() : native_callable(1) {}

    const std::string name() const override {
        return string("print");
    }

    shared_ptr <object> call(std::vector<shared_ptr < object>>

    &args)

    override {
        printf(args[0]->str().c_str());
        return make_shared<null_o>();
    }
};


extern "C" shared_ptr <object> supplier() {
    return make_shared<print_c>();
}