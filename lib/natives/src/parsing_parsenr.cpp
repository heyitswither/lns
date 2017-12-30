#include "defs.h"
#include <sstream>

using namespace lns;

class parse_c : public native_callable {
public:

    parse_c() : native_callable(1) {}

    const std::string &name() const override {
        return S(parse);
    }

    object *call(std::vector<object *> &args) override {
        string_o *str;
        if (DCAST_ASNCHK(str, string_o * , args[0])) {
            std::stringstream ss(str->value);
            number_o *nr = new number_o(0);
            ss >> nr->value;
            if (ss.fail()) return new null_o();
            return nr;
        }
        return new null_o();
    }
};

extern "C" object *supplier() {
    return new parse_c;
}
