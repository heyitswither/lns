//
// Created by lorenzo on 12/5/17.
//
#include "utils.h"
#include "callable.h"
#include <cstdlib>
#include <time.h>

#define NANOSECONDS 999999999

using namespace lns;
using namespace std;

struct timespec ns_to_struct(unsigned long ns) {
    struct timespec str;
    str.tv_sec = ns % NANOSECONDS;
    str.tv_nsec = ns - str.tv_sec * NANOSECONDS - 1;
    return str;
}

unsigned long struct_to_ns(struct timespec t) {
    return t.tv_sec * NANOSECONDS + t.tv_nsec;
}

class sleep_c : public native_callable {
public:

    sleep_c() : native_callable(1) {}

    const std::string name() const override {
        return std::string("__sleep");
    }

    shared_ptr <object> call(std::vector<shared_ptr < object>>

    &args)

    override {
        number_o *d;
        if (DCAST_ASNCHK(d, number_o * , args[0].get())) {
            struct timespec req = ns_to_struct(static_cast<unsigned long>(d->value)), rem;
            rem.tv_nsec = 0;
            rem.tv_nsec = 0;
            nanosleep(&req, &rem);
            return make_shared<number_o>(struct_to_ns(rem));
        }
        return make_shared<null_o>();
    }
};


extern "C" shared_ptr <object> supplier() {
    return make_shared<sleep_c>();
}