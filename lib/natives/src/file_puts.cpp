//
// Created by lorenzo on 12/5/17.
//
#include "lib_files_common.h"
#include "defs.h"
#include <cstdio>
#include <csignal>

using namespace lns;
using namespace std;

class puts_c : public native_callable {
public:
    puts_c() : native_callable(2) {}

    const std::string name() const override {
        return string("puts");
    }

    shared_ptr <object> call(std::vector<shared_ptr < object>>

    &args)

    override {
        auto DCAST_ASN(file, file_ptr_o * , args[0].get());
        if (file == nullptr)
            return make_shared<bool_o>(false);
        return make_shared<bool_o>(fputs(args[1]->str().c_str(), file->ptr) != EOF);
    }
};


extern "C" shared_ptr <object> supplier() {
    return make_shared<puts_c>();
}