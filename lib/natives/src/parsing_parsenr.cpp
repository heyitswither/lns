#include "utils.h"
#include "callable.h"
#include <sstream>
#include <iostream>
using namespace lns;
using namespace std;

class parse_c : public native_callable {
public:

    parse_c() : native_callable(1) {}

    const std::string name() const override {
        return string("parse");
    }

    shared_ptr <object> call(std::vector<shared_ptr < object>>

    &args)

    override {
        string_o *str;
        if (DCAST_ASNCHK(str, string_o * , args[0].get())) {
            std::stringstream ss(str->value);
            auto nr = make_shared<number_o>(0);
            ss >> nr->value;
            if (ss.fail()) return make_shared<null_o>();
            return nr;
        }
        return make_shared<null_o>();
    }
};

extern "C" shared_ptr <object> supplier() {
    return make_shared<parse_c>();
}
