//
// Created by lorenzo on 12/3/17.
//
#include "defs.h"
#include <cmath>

using namespace lns;
using namespace std;

class log10_c : public native_callable{
public:
    log10_c() : native_callable(1) {}

    const std::string &name() const override {
        return S(log10);
    }

    object *call(std::vector<object *> &args) override {
        number_o* nr = dynamic_cast<number_o*>(args[0]);
        if(nr == nullptr) return new null_o();
        return new number_o(log10(nr->value));
    }
};
class log2_c : public native_callable{
public:
    log2_c() : native_callable(1) {}

    const std::string &name() const override {
        return S(log2);
    }

    object *call(std::vector<object *> &args) override {
        number_o* nr = dynamic_cast<number_o*>(args[0]);
        if(nr == nullptr) return new null_o();
        return new number_o(log2(nr->value));
    }
};
class ln_c : public native_callable{
public:
    ln_c() : native_callable(1) {}

    const std::string &name() const override {
        return S(ln);
    }

    object *call(std::vector<object *> &args) override {
        number_o* nr = dynamic_cast<number_o*>(args[0]);
        if(nr == nullptr) return new null_o();
        return new number_o(log(nr->value));
    }
};
class sin_c : public native_callable{
public:
    sin_c() : native_callable(1) {}

    const std::string &name() const override {
        return S(sin);
    }

    object *call(std::vector<object *> &args) override {
        number_o* nr = dynamic_cast<number_o*>(args[0]);
        if(nr == nullptr) return new null_o();
        return new number_o(sin(nr->value));
    }
};
class cos_c : public native_callable{
public:
    cos_c() : native_callable(1) {}

    const std::string &name() const override {
        return S(cos);
    }

    object *call(std::vector<object *> &args) override {
        number_o* nr = dynamic_cast<number_o*>(args[0]);
        if(nr == nullptr) return new null_o();
        return new number_o(cos(nr->value));
    }
};
class tan_c : public native_callable{
public:
    tan_c() : native_callable(1) {}

    const std::string &name() const override {
        return S(tan);
    }

    object *call(std::vector<object *> &args) override {
        number_o* nr = dynamic_cast<number_o*>(args[0]);
        if(nr == nullptr) return new null_o();
        return new number_o(tan(nr->value));
    }
};
class asin_c : public native_callable{
public:
    asin_c() : native_callable(1) {}

    const std::string &name() const override {
        return S(asin);
    }

    object *call(std::vector<object *> &args) override {
        number_o* nr = dynamic_cast<number_o*>(args[0]);
        if(nr == nullptr) return new null_o();
        return new number_o(asin(nr->value));
    }
};
class acos_c : public native_callable{
public:
    acos_c() : native_callable(1) {}

    const std::string &name() const override {
        return S(acos);
    }

    object *call(std::vector<object *> &args) override {
        number_o* nr = dynamic_cast<number_o*>(args[0]);
        if(nr == nullptr) return new null_o();
        return new number_o(acos(nr->value));
    }
};
class atan_c : public native_callable{
public:
    atan_c() : native_callable(1) {}

    const std::string &name() const override {
        return S(atan);
    }

    object *call(std::vector<object *> &args) override {
        number_o* nr = dynamic_cast<number_o*>(args[0]);
        if(nr == nullptr) return new null_o();
        return new number_o(atan(nr->value));
    }
};
class sqrt_c : public native_callable{
public:
    sqrt_c() : native_callable(1) {}

    const std::string &name() const override {
        return S(sqrt);
    }

    object *call(std::vector<object *> &args) override {
        number_o* nr = dynamic_cast<number_o*>(args[0]);
        if(nr == nullptr) return new null_o();
        return new number_o(sqrt(nr->value));
    }
};
class cbrt_c : public native_callable{
public:
    cbrt_c() : native_callable(1) {}

    const std::string &name() const override {
        return S(cbrt);
    }

    object *call(std::vector<object *> &args) override {
        number_o* nr = dynamic_cast<number_o*>(args[0]);
        if(nr == nullptr) return new null_o();
        return new number_o(cbrt(nr->value));
    }
};
class ceil_c : public native_callable{
public:
    ceil_c() : native_callable(1) {}

    const std::string &name() const override {
        return S(ceil);
    }

    object *call(std::vector<object *> &args) override {
        number_o* nr = dynamic_cast<number_o*>(args[0]);
        if(nr == nullptr) return new null_o();
        return new number_o(ceil(nr->value));
    }
};
class floor_c : public native_callable{
public:
    floor_c() : native_callable(1) {}

    const std::string &name() const override {
        return S(floor);
    }

    object *call(std::vector<object *> &args) override {
        number_o* nr = dynamic_cast<number_o*>(args[0]);
        if(nr == nullptr) return new null_o();
        return new number_o(sqrt(nr->value));
    }
};

object* gen(){
    context* ctx = new context(nullptr);
    ctx->define(S(log10),new log10_c, true, visibility::V_GLOBAL, __FILE__);
    ctx->define(S(log2),new log2_c, true, visibility::V_GLOBAL, __FILE__);
    ctx->define(S(ln),new ln_c, true, visibility::V_GLOBAL, __FILE__);
    ctx->define(S(sin),new sin_c, true, visibility::V_GLOBAL, __FILE__);
    ctx->define(S(cos),new cos_c, true, visibility::V_GLOBAL, __FILE__);
    ctx->define(S(tan),new tan_c, true, visibility::V_GLOBAL, __FILE__);
    ctx->define(S(asin),new asin_c, true, visibility::V_GLOBAL, __FILE__);
    ctx->define(S(acos),new acos_c, true, visibility::V_GLOBAL, __FILE__);
    ctx->define(S(atan),new atan_c, true, visibility::V_GLOBAL, __FILE__);
    ctx->define(S(sqrt),new sqrt_c, true, visibility::V_GLOBAL, __FILE__);
    ctx->define(S(cbrt),new cbrt_c, true, visibility::V_GLOBAL, __FILE__);
    ctx->define(S(ceil),new ceil_c, true, visibility::V_GLOBAL, __FILE__);
    ctx->define(S(floor),new floor_c, true, visibility::V_GLOBAL, __FILE__);
    return ctx;
}

extern "C" object* supplier(){
    return gen();
}