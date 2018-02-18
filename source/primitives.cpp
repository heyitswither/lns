//
// Created by lorenzo on 2/17/18.
//

#include "primitives.h"

using namespace lns;
using namespace std;

lns::object::object() : type(object_type::UNSPECIFIED) {}

lns::object::object(lns::objtype t) : type(t) {}

const char *lns::INVALID_OP(const char *OP, const lns::object_type t1, const lns::object_type *t2) {
    auto *buf = (char *) malloc(sizeof(char) * 64);
    *buf = '\0';
    strcat(buf, "Operator '");
    strcat(buf, OP);
    strcat(buf, t2 == nullptr ? "' is not applicable to type '" : "' is not applicable to types '");
    strcat(buf, type_to_string(t1));
    if (t2 != nullptr) {
        strcat(buf, "' and '");
        strcat(buf, type_to_string(*t2));
    }
    strcat(buf, "'");
    return buf;
}

const char *lns::type_to_string(object_type t) {
    switch (t) {
        case NUMBER_T:
            return "number";
        case STRING_T:
            return "string";
        case BOOL_T:
            return "bool";
        case NULL_T:
            return "null";
        case ARRAY_T:
            return "array";
        case CALLABLE_T:
            return "callable";
        case CONTEXT_T:
            return "context";
        case NATIVE_CALLABLE_T:
            return "native_callable";
        case EXCEPTION_T:
            return "exception";
        case INSTANCE_T:
            return "object";
    }
    return "unknown";
}

lns::string_o::string_o(string value) : object(objtype::STRING_T), value(std::move(value)) {}

string lns::string_o::str() const {
    return value;
}

bool lns::string_o::operator==(const lns::object &o) const {
    return value == o.str();
}

void lns::string_o::operator+=(const lns::object &o) {
    this->value += o.str();
}

bool lns::string_o::operator>(const lns::object &o) const {
    if (!check_type(STRING_T, *this, o)) WRONG_OP(>);
    return this->value > dynamic_cast<const string_o &>(o).value;
}

bool lns::string_o::operator>=(const lns::object &o) const {
    if (!check_type(STRING_T, *this, o)) WRONG_OP(>=);
    return this->value >= dynamic_cast<const string_o &>(o).value;
}

bool lns::string_o::operator<=(const lns::object &o) const {
    if (!check_type(STRING_T, *this, o)) WRONG_OP(<=);
    return this->value <= dynamic_cast<const string_o &>(o).value;
}

bool lns::string_o::operator<(const lns::object &o) const {
    if (!check_type(STRING_T, *this, o)) WRONG_OP(<);
    return this->value < dynamic_cast<const string_o &>(o).value;
}

shared_ptr<object> lns::string_o::operator+(const lns::object &o) const {
    return make_shared<string_o>(this->value + o.str());
}

shared_ptr<object> lns::string_o::operator/(const lns::object &o) const {
    if (o.type != STRING_T)
        return make_shared<null_o>();
    auto map = make_shared<array_o>();
    unsigned long i = 0;
    unsigned long current;
    string partial = this->value;
    while ((current = partial.find(o.str())) != string::npos) {
        map->values[i++] = make_shared<string_o>(partial.substr(0, current));
        partial = partial.substr(current + 1, partial.size() - 1);
    }
    map->values[i++] = make_shared<string_o>(partial);
    return map;
}

shared_ptr<object> string_o::clone() const {
    return make_shared<string_o>(this->value);
}

string_o::string_o() : object(STRING_T) {}

shared_ptr<object> number_o::clone() const {
    return make_shared<number_o>(this->value);
}

shared_ptr<object> null_o::clone() const {
    return make_shared<null_o>();
}

shared_ptr<object> bool_o::clone() const {
    return make_shared<bool_o>(this->value);
}

shared_ptr<object> array_o::clone() const {
    auto map = make_shared<array_o>();
    map->values = this->values;
    return map;
}

lns::number_o::number_o(double value) : object(objtype::NUMBER_T), value(value) {}

string lns::number_o::str() const {
    stringstream ss;
    ss << value;
    return ss.str();
}

bool lns::number_o::operator==(const lns::object &o) const {
    if (o.type != NUMBER_T) return false;
    return value == dynamic_cast<number_o *>(const_cast<object *>(&o))->value;
}

bool lns::number_o::operator>(const lns::object &o) const {
    if (!check_type(NUMBER_T, *this, o)) WRONG_OP(>);
    return this->value > dynamic_cast<const number_o &>(o).value;
}

bool lns::number_o::operator>=(const lns::object &o) const {
    if (!check_type(NUMBER_T, *this, o)) WRONG_OP(>=);
    return this->value >= dynamic_cast<const number_o &>(o).value;
}

bool lns::number_o::operator<=(const lns::object &o) const {
    if (!check_type(NUMBER_T, *this, o)) WRONG_OP(<=);
    return this->value >= dynamic_cast<const number_o &>(o).value;
}

bool lns::number_o::operator<(const lns::object &o) const {
    if (!check_type(NUMBER_T, *this, o)) WRONG_OP(<);
    return this->value < dynamic_cast<const number_o &>(o).value;
}

void lns::number_o::operator+=(const lns::object &o) {
    if (!check_type(NUMBER_T, *this, o))WRONG_OP(+=);
    this->value += dynamic_cast<const number_o &>(o).value;
}

void lns::number_o::operator-=(const lns::object &o) {
    if (!check_type(NUMBER_T, *this, o)) WRONG_OP(-=);
    this->value -= dynamic_cast<const number_o &>(o).value;
}

void lns::number_o::operator*=(const lns::object &o) {
    if (!check_type(NUMBER_T, *this, o)) WRONG_OP(*=);
    this->value *= dynamic_cast<const number_o &>(o).value;
}

void lns::number_o::operator/=(const lns::object &o) {
    if (!check_type(NUMBER_T, *this, o)) WRONG_OP(/=);
    this->value /= dynamic_cast<const number_o &>(o).value;
}

shared_ptr<object> lns::number_o::operator+(const lns::object &o) const {
    if (!check_type(NUMBER_T, *this, o)) if (!(o.type == STRING_T)) WRONG_OP(+);
    if (o.type == STRING_T)
        return make_shared<string_o>(this->str() + o.str());
    return make_shared<number_o>(this->value + dynamic_cast<const number_o &>(o).value);
}

shared_ptr<object> lns::number_o::operator-(const lns::object &o) const {
    if (!check_type(NUMBER_T, *this, o)) WRONG_OP(-);
    return make_shared<number_o>(this->value - dynamic_cast<const number_o &>(o).value);
}

shared_ptr<object> lns::number_o::operator*(const lns::object &o) const {
    if (!check_type(NUMBER_T, *this, o)) WRONG_OP(+);
    return make_shared<number_o>(this->value * dynamic_cast<const number_o &>(o).value);
}

shared_ptr<object> lns::number_o::operator/(const lns::object &o) const {
    if (!check_type(NUMBER_T, *this, o)) WRONG_OP(+);
    return make_shared<number_o>(this->value / dynamic_cast<const number_o &>(o).value);
}

shared_ptr<object> lns::number_o::operator^(const object &o) const {
    if (!check_type(NUMBER_T, *this, o)) WRONG_OP(+);
    return make_shared<number_o>(pow(this->value, dynamic_cast<const number_o &>(o).value));
}

shared_ptr<object> lns::number_o::operator-() const {
    return make_shared<number_o>(-this->value);
}

shared_ptr<object> lns::number_o::operator++() {
    return make_shared<number_o>(++this->value);
}

shared_ptr<object> lns::number_o::operator--() {
    return make_shared<number_o>(--this->value);
}

shared_ptr<object> lns::number_o::operator++(int) {
    return make_shared<number_o>(this->value++);
}

shared_ptr<object> lns::number_o::operator--(int) {
    return make_shared<number_o>(this->value++);
}

lns::bool_o::bool_o(bool value) : object(objtype::BOOL_T), value(value) {}

bool lns::bool_o::operator==(const lns::object &o) const {
    if (o.type != BOOL_T) return false;
    return value == dynamic_cast<const bool_o &>(o).value;
}

bool lns::bool_o::operator&&(const lns::object &o) const {
    if (!check_type(BOOL_T, *this, o)) WRONG_OP(and)
    return this->value && dynamic_cast<const bool_o &>(o).value;
}

bool lns::bool_o::operator||(const lns::object &o) const {
    if (!check_type(BOOL_T, *this, o)) WRONG_OP(or)
    return this->value || dynamic_cast<const bool_o &>(o).value;
}

shared_ptr<object> lns::bool_o::operator!() const {
    return make_shared<bool_o>(!this->value);
}


shared_ptr<object> lns::bool_o::operator+(const lns::object &o) const {
    if (o.type == STRING_T) return make_shared<string_o>(this->str() + o.str());
    WRONG_OP(+)
}


lns::null_o::null_o() : object(NULL_T) {}

string lns::null_o::str() const {
    return "null";
}

bool lns::null_o::operator==(const lns::object &o) const {
    return o.type == NULL_T;
}


bool array_o::operator==(const object &o) const {
    if (o.type != ARRAY_T) return false;
    return values == dynamic_cast<array_o *>(const_cast<object *>(&o))->values;
}


shared_ptr<object> array_o::operator+(const object &o) const {
    if (o.type == STRING_T) return make_shared<string_o>(this->str() + o.str());
    WRONG_OP(+)
}


array_o::array_o() : object(ARRAY_T) {}

string array_o::str() const {
    stringstream ss;
    ss << "{";
    int i = 0;
    for (auto p : values) {
        if (++i != 1) ss << ", ";
        ss << p.first << " : ";
        if (p.second->type == STRING_T) ss << "\"";
        ss << p.second->str();
        if (p.second->type == STRING_T) ss << "\"";
    }
    ss << "}";
    return ss.str();
}

const bool array_o::contains_key(double s) {
    return values.find(s) != values.end();
}

inline bool lns::check_type(object_type type, const object &o1, const object &o) {
    return (o1.type == type && o.type == type);
}

lns::exception_definition::exception_definition(const char *file, int line, const std::string &name,
                                                const std::set<std::string> &fields) : object(EXCEPTION_DEFINITION_T),
                                                                                       name(name), fields(fields),
                                                                                       def_file(file), def_line(line) {}

string exception_definition::str() const {
    stringstream s;
    s << "<exception_definition@" << static_cast<const void *>(this) << ", name: \"" << name << "\">";
    return s.str();
}

bool exception_definition::operator==(const object &o) const {
    if (o.type != EXCEPTION_DEFINITION_T) return false;
    const exception_definition *e;
    if (DCAST_ASNCHK(e, const exception_definition *, &o))
        return this->def_file == e->def_file && this->def_line == e->def_line;
    return false;
}

shared_ptr<object> exception_definition::clone() const {
    return nullptr;
}