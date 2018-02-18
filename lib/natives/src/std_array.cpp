//
// Created by lorenzo on 12/17/17.
//

#include "utils.h"
#include "callable.h"
using namespace lns;
using namespace std;
class array_c : public native_callable{
public:
    array_c() : native_callable(0) {}

    const std::string name() const override {
        return string("array");
    }

    shared_ptr <object> call(std::vector <shared_ptr<object>> &args) override {
        return make_shared<array_o>();
    }
};


extern "C" shared_ptr <object> supplier() {
    return make_shared<array_c>();
}

