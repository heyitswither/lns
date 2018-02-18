//
// Created by lorenzo on 12/5/17.
//
#include "utils.h"
#include "callable.h"
using namespace lns;
using namespace std;
class exit_c : public native_callable{
public:
    exit_c() : native_callable(1) {}

    const std::string name() const override {
        return string("exitc");
    }

    shared_ptr <object> call(std::vector <shared_ptr<object>> &args) override {
        number_o *n = dynamic_cast<number_o *>(args[0].get());
        std::exit(n == nullptr ? 0 : n->value);
    }
};


extern "C" shared_ptr <object> supplier() {
    return make_shared<exit_c>();
}