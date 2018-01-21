//
// Created by lorenzo on 12/5/17.
//
#include "lib_files_common.h"
#include "defs.h"
#include <cstdio>
#include <csignal>

using namespace lns;
using namespace std;

class getc_c : public native_callable {
public:
    getc_c() : native_callable(1) {}

    const std::string name() const override {
        return string("getc");
    }

    shared_ptr <object> call(std::vector<shared_ptr < object>>

    &args)

    override {
        auto DCAST_ASN(file, file_ptr_o * , args[0].get());
        if (file == nullptr)
            return make_shared<null_o>();

        int c = fgetc(file->ptr);
        if (c != EOF)
            return make_shared<string_o>(CHARSTR((char) c));
        return make_shared<null_o>();
    }
};


extern "C" shared_ptr <object> supplier() {
    return make_shared<getc_c>();
}