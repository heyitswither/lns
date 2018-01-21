//
// Created by lorenzo on 12/5/17.
//
#include "lib_files_common.h"
#include "defs.h"
#include <cstdio>

using namespace lns;
using namespace std;

class close_c : public native_callable {
public:
    close_c() : native_callable(1) {}

    const std::string name() const override {
        return string("close");
    }

    shared_ptr <object> call(std::vector<shared_ptr < object>>

    &args)

    override {
        auto DCAST_ASN(file, file_ptr_o * , args[0].get());
        if (file == nullptr)
            return make_shared<null_o>();
        int c = fclose(file->ptr);
        if (c != EOF)
            return make_shared<bool_o>(true);
        return make_shared<bool_o>(false);
    }
};


extern "C" shared_ptr <object> supplier() {
    return make_shared<close_c>();
}