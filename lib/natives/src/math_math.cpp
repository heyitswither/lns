//
// Created by lorenzo on 12/3/17.
//
#include "defs.h"
#include <cmath>

#define S(val) STR(#val)

using namespace lns;
using namespace std;

class log10_c : public native_callable{
public:
    log10_c() : native_callable(1) {}

    const std::string name() const override {
        return string("log10");
    }

    shared_ptr <object> call(std::vector<shared_ptr < object>>

    &args)

    override {
        number_o *nr = dynamic_cast<number_o *>(args[0].get());
        if (nr == nullptr) return make_shared<null_o>();
        return make_shared<number_o>(log10(nr->value));
    }
};
class log2_c : public native_callable{
public:
    log2_c() : native_callable(1) {}

    const std::string name() const override {
        return string("log2");
    }

    shared_ptr <object> call(std::vector<shared_ptr < object>>

    &args)

    override {
        number_o *nr = dynamic_cast<number_o *>(args[0].get());
        if (nr == nullptr) return make_shared<null_o>();
        return make_shared<number_o>(log2(nr->value));
    }
};
class ln_c : public native_callable{
public:
    ln_c() : native_callable(1) {}

    const std::string name() const override {
        return string("ln");
    }

    shared_ptr <object> call(std::vector<shared_ptr < object>>

    &args)

    override {
        number_o *nr = dynamic_cast<number_o *>(args[0].get());
        if (nr == nullptr) return make_shared<null_o>();
        return make_shared<number_o>(log(nr->value));
    }
};
class sin_c : public native_callable{
public:
    sin_c() : native_callable(1) {}

    const std::string name() const override {
        return string("sin");
    }

    shared_ptr <object> call(std::vector<shared_ptr < object>>

    &args)

    override {
        number_o *nr = dynamic_cast<number_o *>(args[0].get());
        if (nr == nullptr) return make_shared<null_o>();
        return make_shared<number_o>(sin(nr->value));
    }
};
class cos_c : public native_callable{
public:
    cos_c() : native_callable(1) {}

    const std::string name() const override {
        return string("cos");
    }

    shared_ptr <object> call(std::vector<shared_ptr < object>>

    &args)

    override {
        number_o *nr = dynamic_cast<number_o *>(args[0].get());
        if (nr == nullptr) return make_shared<null_o>();
        return make_shared<number_o>(cos(nr->value));
    }
};
class tan_c : public native_callable{
public:
    tan_c() : native_callable(1) {}

    const std::string name() const override {
        return string("tan");
    }

    shared_ptr <object> call(std::vector<shared_ptr < object>>

    &args)

    override {
        number_o *nr = dynamic_cast<number_o *>(args[0].get());
        if (nr == nullptr) return make_shared<null_o>();
        return make_shared<number_o>(tan(nr->value));
    }
};
class asin_c : public native_callable{
public:
    asin_c() : native_callable(1) {}

    const std::string name() const override {
        return string("asin");
    }

    shared_ptr <object> call(std::vector<shared_ptr < object>>

    &args)

    override {
        number_o *nr = dynamic_cast<number_o *>(args[0].get());
        if (nr == nullptr) return make_shared<null_o>();
        return make_shared<number_o>(asin(nr->value));
    }
};
class acos_c : public native_callable{
public:
    acos_c() : native_callable(1) {}

    const std::string name() const override {
        return string("acos");
    }

    shared_ptr <object> call(std::vector<shared_ptr < object>>

    &args)

    override {
        number_o *nr = dynamic_cast<number_o *>(args[0].get());
        if (nr == nullptr) return make_shared<null_o>();
        return make_shared<number_o>(acos(nr->value));
    }
};
class atan_c : public native_callable{
public:
    atan_c() : native_callable(1) {}

    const std::string name() const override {
        return string("atan");
    }

    shared_ptr <object> call(std::vector<shared_ptr < object>>

    &args)

    override {
        number_o *nr = dynamic_cast<number_o *>(args[0].get());
        if (nr == nullptr) return make_shared<null_o>();
        return make_shared<number_o>(atan(nr->value));
    }
};
class sqrt_c : public native_callable{
public:
    sqrt_c() : native_callable(1) {}

    const std::string name() const override {
        return string("sqrt");
    }

    shared_ptr <object> call(std::vector<shared_ptr < object>>

    &args)

    override {
        number_o *nr = dynamic_cast<number_o *>(args[0].get());
        if (nr == nullptr) return make_shared<null_o>();
        return make_shared<number_o>(sqrt(nr->value));
    }
};
class cbrt_c : public native_callable{
public:
    cbrt_c() : native_callable(1) {}

    const std::string name() const override {
        return string("cbrt");
    }

    shared_ptr <object> call(std::vector<shared_ptr < object>>

    &args)

    override {
        number_o *nr = dynamic_cast<number_o *>(args[0].get());
        if (nr == nullptr) return make_shared<null_o>();
        return make_shared<number_o>(cbrt(nr->value));
    }
};
class ceil_c : public native_callable{
public:
    ceil_c() : native_callable(1) {}

    const std::string name() const override {
        return string("ceil");
    }

    shared_ptr <object> call(std::vector<shared_ptr < object>>

    &args)

    override {
        number_o *nr = dynamic_cast<number_o *>(args[0].get());
        if (nr == nullptr) return make_shared<null_o>();
        return make_shared<number_o>(ceil(nr->value));
    }
};
class floor_c : public native_callable{
public:
    floor_c() : native_callable(1) {}

    const std::string name() const override {
        return string("floor");
    }

    shared_ptr <object> call(std::vector<shared_ptr < object>>

    &args)

    override {
        number_o *nr = dynamic_cast<number_o *>(args[0].get());
        if (nr == nullptr) return make_shared<null_o>();
        return make_shared<number_o>(floor(nr->value));
    }
};

shared_ptr <object> gen() {
    auto ctx = make_shared<context>(nullptr);
    ctx->define(S(log10), make_shared<log10_c>(), true, visibility::V_GLOBAL, __FILE__);
    ctx->define(S(log2), make_shared<log2_c>(), true, visibility::V_GLOBAL, __FILE__);
    ctx->define(S(ln), make_shared<ln_c>(), true, visibility::V_GLOBAL, __FILE__);
    ctx->define(S(sin), make_shared<sin_c>(), true, visibility::V_GLOBAL, __FILE__);
    ctx->define(S(cos), make_shared<cos_c>(), true, visibility::V_GLOBAL, __FILE__);
    ctx->define(S(tan), make_shared<tan_c>(), true, visibility::V_GLOBAL, __FILE__);
    ctx->define(S(asin), make_shared<asin_c>(), true, visibility::V_GLOBAL, __FILE__);
    ctx->define(S(acos), make_shared<acos_c>(), true, visibility::V_GLOBAL, __FILE__);
    ctx->define(S(atan), make_shared<atan_c>(), true, visibility::V_GLOBAL, __FILE__);
    ctx->define(S(sqrt), make_shared<sqrt_c>(), true, visibility::V_GLOBAL, __FILE__);
    ctx->define(S(cbrt), make_shared<cbrt_c>(), true, visibility::V_GLOBAL, __FILE__);
    ctx->define(S(ceil), make_shared<ceil_c>(), true, visibility::V_GLOBAL, __FILE__);
    ctx->define(S(floor), make_shared<floor_c>(), true, visibility::V_GLOBAL, __FILE__);
    return ctx;
}

extern "C" shared_ptr <object> supplier() {
    return gen();
}