//
// Created by lorenzo on 12/5/17.
//
#include "lib_files_common.h"
#include <cstdio>

using namespace lns;
using namespace std;

class strerr_c : public native_callable {
public:
    strerr_c() : native_callable(0) {}

    const std::string name() const override {
        return string("strerr_c");
    }

    shared_ptr <object> call(std::vector<shared_ptr < object>>

    &args)

    override {
        return make_shared<string_o>(std::string(strerror(errno)));
    }
};


extern "C" shared_ptr <object> supplier() {
    return make_shared<strerr_c>();
}