//
// Created by lorenzo on 12/5/17.
//
#include "lib_files_common.h"

using namespace lns;
using namespace std;


class open_c : public native_callable {
public:
    open_c() : native_callable(2) {}

    const std::string name() const override {
        return string("open");
    }

    shared_ptr <object> call(std::vector<shared_ptr < object>>

    &args)

    override {
        errno = 0;
        auto DCAST_ASN(name, string_o * , args[0].get());
        auto DCAST_ASN(mode, string_o * , args[1].get());
        if (name == nullptr || mode == nullptr)
            return make_shared<null_o>();
        else {
            auto ptr = fopen(name->value.c_str(), mode->value.c_str());
            if (ptr == nullptr) return make_shared<null_o>();
            return make_shared<file_ptr_o>(ptr);
        }
    }
};


extern "C" shared_ptr <object> supplier() {
    return make_shared<open_c>();
}