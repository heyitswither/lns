//
// Created by lorenzo on 12/5/17.
//
#include "defs.h"
using namespace lns;
class exit_c : public native_callable{
public:
    exit_c() : native_callable(1) {}

    const std::string &name() const override {
        return S(exitc);
    }

    object *call(std::vector<object *> &args) override {
        number_o *n = dynamic_cast<number_o *>(args[0]);
        std::exit(n == nullptr ? 0 : n->value);
    }
};


extern "C" object* supplier(){
    return new exit_c;
}