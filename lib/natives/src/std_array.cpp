//
// Created by lorenzo on 12/17/17.
//

#include "defs.h"
using namespace lns;
class array_c : public callable{
public:
    const int arity() const override {
        return 0;
    }

    const std::string &name() const override {
        return S(array);
    }

    object *call(std::vector<object *> &args) override {
        return new array_o;
    }
};


extern "C" object* supplier(){
    return new array_c;
}

