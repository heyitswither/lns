//
// Created by lorenzo on 11/30/17.
//

#include "parser.h"
#include "defs.h"
#include "exceptions.h"
#include "options.h"

using namespace lns;
using namespace std;
lns::object::object() {}

lns::object::object(lns::objtype t) : type(t) {}


lns::string_o::string_o(string value) : object(objtype::STRING_T), value(value) {}

string lns::string_o::str() const {
    return value;
}

bool lns::string_o::operator==(const lns::object &o) const {
    if (o.type != STRING_T) return false;
    return (value == dynamic_cast<const string_o *>(&o)->value);
}

bool lns::string_o::operator||(const lns::object &o) const {
    WRONG_OP('or')
}

lns::object *lns::string_o::operator!() const {
    WRONG_OP('not')
}

bool lns::string_o::operator&&(const lns::object &o) const {
    WRONG_OP('and');
}

lns::object *lns::string_o::operator+=(const lns::object &o) {
    this->value += o.str();
    return this;
}

lns::object *lns::string_o::operator-=(const lns::object &o) {
    WRONG_OP(-);
}

lns::object *lns::string_o::operator*=(const lns::object &o) {
    WRONG_OP(*);
}

lns::object *lns::string_o::operator/=(const lns::object &o) {
    WRONG_OP(/);
}

bool lns::string_o::operator>(const lns::object &o2) const {
    if (!check_type(STRING_T, *this, o2)) WRONG_OP(>);
    return this->value > dynamic_cast<const string_o &>(o2).value;
}

bool lns::string_o::operator>=(const lns::object &o2) const {
    if (!check_type(STRING_T, *this, o2)) WRONG_OP(>=);
    return this->value >= dynamic_cast<const string_o &>(o2).value;
}

bool lns::string_o::operator<=(const lns::object &o2) const {
    if (!check_type(STRING_T, *this, o2)) WRONG_OP(<=);
    return this->value <= dynamic_cast<const string_o &>(o2).value;
}

bool lns::string_o::operator<(const lns::object &o2) const {
    if (!check_type(STRING_T, *this, o2)) WRONG_OP(<);
    return this->value < dynamic_cast<const string_o &>(o2).value;
}

lns::object *lns::string_o::operator+(const lns::object &o2) const {
    return new string_o(this->value + o2.str());
}

lns::object *lns::string_o::operator-(const lns::object &o2) const {
    WRONG_OP(-);
}

lns::object *lns::string_o::operator*(const lns::object &o2) const {
    WRONG_OP(*);
}

lns::object *lns::string_o::operator/(const lns::object &o2) const {
    if(o2.type != NULL_T)
        return GET_DEFAULT_NULL();
    auto *map = new map_o();
    unsigned long i = 0;
    unsigned long current;
    string partial = this->value;
    while((current = partial.find(o2.str())) != string::npos){
        string& s = *new string();
        s+= std::to_string(i++);
        map->values[s] = new string_o(partial.substr(0, current));
        partial = partial.substr(current + 1,partial.size()-1);
    }
    string& s = *new string();
    s+= std::to_string(i++);
    map->values[s] = new string_o(move(partial));
    return map;
}

lns::object *lns::string_o::operator^(const lns::object &o2) const {
    WRONG_OP(^);
}

lns::object *lns::string_o::operator++() {
    WRONG_OP(++);
}

lns::object *lns::string_o::operator--() {
    WRONG_OP(--);
}

lns::object *lns::string_o::operator-() const {
    WRONG_OP(-(unary));
}


lns::number_o::number_o(double value) : object(objtype::NUMBER_T), value(value) {}

string lns::number_o::str() const {
    stringstream ss;
    ss << value;
    return *new string(ss.str());
}

bool lns::number_o::operator==(const lns::object &o) const {
    if (o.type != NUMBER_T) return false;
    return value == dynamic_cast<number_o *>(const_cast<object *>(&o))->value;
}

bool lns::number_o::operator&&(const lns::object &o) const {
    WRONG_OP('and')
}

bool lns::number_o::operator||(const lns::object &o) const {
    WRONG_OP('or')
}

lns::object *lns::number_o::operator!() const {
    WRONG_OP('not')
}

bool lns::number_o::operator>(const lns::object &o2) const {
    if (!check_type(NUMBER_T, *this, o2)) WRONG_OP(>);
    return this->value > dynamic_cast<const number_o &>(o2).value;
}

bool lns::number_o::operator>=(const lns::object &o2) const {
    if (!check_type(NUMBER_T, *this, o2)) WRONG_OP(>=);
    return this->value >= dynamic_cast<const number_o &>(o2).value;
}

bool lns::number_o::operator<=(const lns::object &o2) const {
    if (!check_type(NUMBER_T, *this, o2)) WRONG_OP(<=);
    return this->value >= dynamic_cast<const number_o &>(o2).value;
}

bool lns::number_o::operator<(const lns::object &o2) const {
    if (!check_type(NUMBER_T, *this, o2)) WRONG_OP(<);
    return this->value < dynamic_cast<const number_o &>(o2).value;
}

lns::object *lns::number_o::operator+=(const lns::object &o) {
    if (!check_type(NUMBER_T, *this, o)) WRONG_OP(+=);
    this->value += dynamic_cast<const number_o &>(o).value;
    return this;
}

lns::object *lns::number_o::operator-=(const lns::object &o) {
    if (!check_type(NUMBER_T, *this, o)) WRONG_OP(-=);
    this->value -= dynamic_cast<const number_o &>(o).value;
    return this;
}

lns::object *lns::number_o::operator*=(const lns::object &o) {
    if (!check_type(NUMBER_T, *this, o)) WRONG_OP(*=);
    this->value *= dynamic_cast<const number_o &>(o).value;
    return this;
}

lns::object *lns::number_o::operator/=(const lns::object &o) {
    if (!check_type(NUMBER_T, *this, o)) WRONG_OP(/=);
    this->value /= dynamic_cast<const number_o &>(o).value;
    return this;
}

lns::object *lns::number_o::operator+(const lns::object &o2) const {
    if (!check_type(NUMBER_T, *this, o2)) WRONG_OP(+);
    return new number_o(this->value + dynamic_cast<const number_o &>(o2).value);
}

lns::object *lns::number_o::operator-(const lns::object &o2) const {
    if (!check_type(NUMBER_T, *this, o2)) WRONG_OP(-);
    return new number_o(this->value - dynamic_cast<const number_o &>(o2).value);
}

lns::object *lns::number_o::operator*(const lns::object &o2) const {
    if (!check_type(NUMBER_T, *this, o2)) WRONG_OP(+);
    return new number_o(this->value * dynamic_cast<const number_o &>(o2).value);
}

lns::object *lns::number_o::operator/(const lns::object &o2) const {
    if (!check_type(NUMBER_T, *this, o2)) WRONG_OP(+);
    return new number_o(this->value / dynamic_cast<const number_o &>(o2).value);
}

lns::object *lns::number_o::operator^(const lns::object &o2) const {
    if (!check_type(NUMBER_T, *this, o2)) WRONG_OP(+);
    return new number_o(pow(this->value, dynamic_cast<const number_o &>(o2).value));
}

lns::object *lns::number_o::operator-() const {
    return new number_o(this->value);
}

lns::object *lns::number_o::operator++() {
    ++this->value;
    return new number_o(value - 1);
}

lns::object *lns::number_o::operator--() {
    --this->value;
    return new number_o(value + 1);
}


lns::bool_o::bool_o(bool value) : object(objtype::BOOL_T), value(value) {}

bool lns::bool_o::operator==(const lns::object &o) const {
    if (o.type != BOOL_T) return false;
    return value == dynamic_cast<const bool_o &>(o).value;
}

bool lns::bool_o::operator&&(const lns::object &o) const {
    if (!check_type(BOOL_T, *this, o)) WRONG_OP('and')
    return this->value && dynamic_cast<const bool_o &>(o).value;
}

bool lns::bool_o::operator||(const lns::object &o) const {
    if (!check_type(BOOL_T, *this, o)) WRONG_OP('or')
    return this->value || dynamic_cast<const bool_o &>(o).value;
}

lns::object *lns::bool_o::operator!() const {
    return new bool_o(!this->value);
}

bool lns::bool_o::operator>(const lns::object &o2) const {
    WRONG_OP(>)
}

bool lns::bool_o::operator>=(const lns::object &o2) const {
    WRONG_OP(>=)
}

bool lns::bool_o::operator<=(const lns::object &o2) const {
    WRONG_OP(<=)
}

bool lns::bool_o::operator<(const lns::object &o2) const {
    WRONG_OP(<)
}

lns::object *lns::bool_o::operator+=(const lns::object &o) {
    WRONG_OP(+=)
}

lns::object *lns::bool_o::operator-=(const lns::object &o) {
    WRONG_OP(-=)
}

lns::object *lns::bool_o::operator*=(const lns::object &o) {
    WRONG_OP(*=)
}

lns::object *lns::bool_o::operator/=(const lns::object &o) {
    WRONG_OP(/=)
}

lns::object *lns::bool_o::operator+(const lns::object &o2) const {
    WRONG_OP(+)
}

lns::object *lns::bool_o::operator-(const lns::object &o2) const {
    WRONG_OP(-)
}

lns::object *lns::bool_o::operator*(const lns::object &o2) const {
    WRONG_OP(*)
}

lns::object *lns::bool_o::operator/(const lns::object &o2) const {
    WRONG_OP(/)
}

lns::object *lns::bool_o::operator^(const lns::object &o2) const {
    WRONG_OP(^)
}

lns::object *lns::bool_o::operator-() const {
    WRONG_OP(-)
}

lns::object *lns::bool_o::operator++() {
    WRONG_OP(++)
}

lns::object *lns::bool_o::operator--() {
    WRONG_OP(--)
}

lns::null_o::null_o() : object(NULL_T) {}

string lns::null_o::str() const {
    return "null";
}

bool lns::null_o::operator==(const lns::object &o) const {
    return o.type == NULL_T;
}

bool lns::null_o::operator&&(const lns::object &o) const {
    NULL_OP(&&)
}

bool lns::null_o::operator||(const lns::object &o) const {
    NULL_OP(||)
}

lns::object *lns::null_o::operator!() const {
    NULL_OP(!)
}

bool lns::null_o::operator>(const lns::object &o2) const {
    NULL_OP(>)
}

bool lns::null_o::operator>=(const lns::object &o2) const {
    NULL_OP(>=)
}

bool lns::null_o::operator<=(const lns::object &o2) const {
    NULL_OP(<=)
}

bool lns::null_o::operator<(const lns::object &o2) const {
    NULL_OP(<)
}

lns::object *lns::null_o::operator+=(const lns::object &o) {
    NULL_OP(+=)
}

lns::object *lns::null_o::operator-=(const lns::object &o) {
    NULL_OP(-=)
}

lns::object *lns::null_o::operator*=(const lns::object &o) {
    NULL_OP(*=)
}

lns::object *lns::null_o::operator/=(const lns::object &o) {
    NULL_OP(/=)
}

lns::object *lns::null_o::operator+(const lns::object &o2) const {
    NULL_OP(+)
}

lns::object *lns::null_o::operator-(const lns::object &o2) const {
    NULL_OP(-)
}

lns::object *lns::null_o::operator*(const lns::object &o2) const {
    NULL_OP(*)
}

lns::object *lns::null_o::operator/(const lns::object &o2) const {
    NULL_OP(/)
}

lns::object *lns::null_o::operator^(const lns::object &o2) const {
    NULL_OP(^)
}

lns::object *lns::null_o::operator-() const {
    NULL_OP(-)
}

lns::object *lns::null_o::operator++() {
    NULL_OP(++)
}

lns::object *lns::null_o::operator--() {
    NULL_OP(--)
}

bool map_o::operator==(const object &o) const {
    if (o.type != MAP_T) return false;
    return values == dynamic_cast<map_o *>(const_cast<object *>(&o))->values;
}

bool map_o::operator&&(const object &o) const {
    WRONG_OP('and')
}

bool map_o::operator||(const object &o) const {
    WRONG_OP('or')
}

object *map_o::operator!() const {
    WRONG_OP('not')
}

bool map_o::operator>(const object &o2) const {
    WRONG_OP(>)
}

bool map_o::operator>=(const object &o2) const {
    WRONG_OP(>=)
}

bool map_o::operator<=(const object &o2) const {
    WRONG_OP(<=)
}

bool map_o::operator<(const object &o2) const {
    WRONG_OP(<)
}

object *map_o::operator+=(const object &o) {
    WRONG_OP(+=)
}

object *map_o::operator-=(const object &o) {
    WRONG_OP(-=)
}

object *map_o::operator*=(const object &o) {
    WRONG_OP(*=)
}

object *map_o::operator/=(const object &o) {
    WRONG_OP(/=)
}

object *map_o::operator+(const object &o2) const {
    WRONG_OP(+)
}

object *map_o::operator-(const object &o2) const {
    WRONG_OP(-)
}

object *map_o::operator*(const object &o2) const {
    WRONG_OP(*)
}

object *map_o::operator/(const object &o2) const {
    WRONG_OP(/)
}

object *map_o::operator^(const object &o2) const {
    WRONG_OP(^)
}

object *map_o::operator-() const {
    WRONG_OP(-)
}

object *map_o::operator++() {
    WRONG_OP(++)
}

object *map_o::operator--() {
    WRONG_OP(--)
}

map_o::map_o() : object(MAP_T) {}

string map_o::str() const {
    stringstream ss;
    ss << "{";
    int i = 0;
    for (auto p : values) {
        if (++i != 1) ss << ", ";
        ss << "\"" << p.first << "\" : ";
        if (p.second->type == STRING_T) ss << "\"";
        ss << p.second->str();
        if (p.second->type == STRING_T) ss << "\"";
    }
    ss << "}";
    return ss.str();
}

const bool map_o::contains_key(string s) {
    return values.find(s) != values.end();
}

token::token(token_type type, string lexeme, object &literal, const char *filename, int line) : type(type),
                                                                                                lexeme(std::move(
                                                                                                        lexeme)),
                                                                                                literal(literal),
                                                                                                filename(filename),
                                                                                                line(line) {}

string token::format() const {
    stringstream ss;
    ss << "type: " << KEYWORDS_S_VALUES[type] << ", lexeme: " << lexeme << ", literal: |";
    string str = literal.str();
    ss << str;
    ss << "|, file: \"" << filename << "\", line:" << line;
    return *new string(ss.str());
}

stack_call::stack_call(const char *filename, const int line, const string &method, const bool native) : filename(filename), line(line),
                                                                                     method(method) , native(native){}

return_signal::return_signal(object *value, const token &keyword) : value(value), keyword(keyword) {}

const char *lns::return_signal::what() const throw(){
    return exception::what();
}

const char *lns::break_signal::what() const throw(){
    return exception::what();
}

break_signal::break_signal(const token &keyword) : keyword(keyword) {}

continue_signal::continue_signal(const token &keyword) : keyword(keyword) {}

const char *continue_signal::what() const throw(){
    return exception::what();
}

string context::str() const {
    return *new string("<context>");
}

bool context::operator==(const object &o) const {
    return false;
}

bool context::operator&&(const object &o) const {
    WRONG_OP(&&)
}

bool context::operator||(const object &o) const {
    WRONG_OP(||)
}

object *context::operator!() const {
    WRONG_OP(!)
}

bool context::operator>(const object &o2) const {
    WRONG_OP(>)
}

bool context::operator>=(const object &o2) const {
    WRONG_OP(>=)
}

bool context::operator<=(const object &o2) const {
    WRONG_OP(<=)
}

bool context::operator<(const object &o2) const {
    WRONG_OP(<)
}

object *context::operator+=(const object &o) {
    WRONG_OP(+=)
}

object *context::operator-=(const object &o) {
    WRONG_OP(-=)
}

object *context::operator*=(const object &o) {
    WRONG_OP(*=)
}

object *context::operator/=(const object &o) {
    WRONG_OP(/=)
}

object *context::operator+(const object &o2) const {
    WRONG_OP(+)
}

object *context::operator-(const object &o2) const {
    WRONG_OP(-)
}

object *context::operator*(const object &o2) const {
    WRONG_OP(*)
}

object *context::operator/(const object &o2) const {
    WRONG_OP(/)
}

object *context::operator^(const object &o2) const {
    WRONG_OP(^)
}

object *context::operator-() const {
    WRONG_OP(-)
}

object *context::operator++() {
    WRONG_OP(++)
}

object *context::operator--() {
    WRONG_OP(--)
}

bool lns::runtime_environment::contains_key(const std::string name) {
    return values.find(name) != values.end();
}

lns::runtime_environment::runtime_environment() : enclosing(nullptr), values(*new std::map<std::string, variable>()) {}

lns::runtime_environment::runtime_environment(runtime_environment *enc) : enclosing(enc),
                                                                          values(*new std::map<std::string, variable>) {}

object *runtime_environment::get(const token &name) {
    if (contains_key(name.lexeme)) {
        variable s = values[name.lexeme];
        return s.value;
    }
    if (enclosing != nullptr) return enclosing->get(name);
    if (permissive) { return lns::GET_DEFAULT_NULL(); }
    string &s = *new string();
    s += "'" + name.lexeme + "' is undefined";
    throw runtime_exception(name, s);
}


void runtime_environment::define(const token &name, object *o, bool is_final, bool is_global) {
    if (contains_key(name.lexeme)) {
        string &s = *new string();
        s += "variable " + name.lexeme + " is already defined";
        throw runtime_exception(name, s);
    }

    if (is_global) {
        if (enclosing != nullptr) {
            return enclosing->define(name, o, is_final, is_global);
        }
    }

    values.insert(std::pair<string, variable>(name.lexeme, *new variable(is_final, o)));
}

object* lns::GET_DEFAULT_NULL() {
    return new null_o();
}

void runtime_environment::assign(const token &name, token_type op, object *obj) {
    if (contains_key(name.lexeme)) {
        if (values[name.lexeme].isfinal) {
            string &s = *new string();
            s += "variable " + name.lexeme + " is final";
            throw runtime_exception(name, s);
        }
        //values[name.lexeme]->isfinal = false;
        try {
            variable v = values[name.lexeme];
            switch (op) {
                case EQUAL:
                    values[name.lexeme].value = obj;
                    break;
                case PLUS_EQUALS:
                    *v.value += *obj;
                    break;
                case MINUS_EQUALS:
                    *v.value -= *obj;
                    break;
                case STAR_EQUALS:
                    *v.value *= *obj;
                    break;
                case SLASH_EQUALS:
                    *v.value /= *obj;
                    break;
            }
        } catch (string &s) {
            throw runtime_exception(name, s);
        }
        return;
    }
    if (enclosing != nullptr) {
        return enclosing->assign(name, op, obj);
    }
    if (permissive) { return define(name, obj, false, false); }
    string &s = *new string();
    s += "undefined variable '" + name.lexeme + "'";
    throw runtime_exception(name, s);
}

bool runtime_environment::is_valid_object_type(objtype objtype) {
    switch (objtype) {
        case NUMBER_T:
        case STRING_T:
        case BOOL_T:
        case NULL_T:
        case CALLABLE_T:
        case MAP_T:
            return true;
    }
    return false;
}

object *runtime_environment::get_map_field(const token &map_name, string key) {
    map_o *map = dynamic_cast<map_o *>(get(map_name));
    if (map == nullptr) {
        string &s = *new string();
        s += "variable '" + map_name.lexeme + "' is not a map";
        throw runtime_exception(map_name, s);
    } else {
        if (!map->contains_key(key)) return new null_o();
        return map->values[key];
    }
}

object *runtime_environment::assign_map_field(const token &name, const token_type op, string_o *key, object *value) {
    if (contains_key(name.lexeme)) {
        if (values[name.lexeme].isfinal) {
            string &s = *new string();
            s += "variable '" + name.lexeme + "' is final";
            throw runtime_exception(name, s);
        }
        variable o = values[name.lexeme];
        map_o *map;
        if ((map = dynamic_cast<map_o *>(o.value)) == nullptr) {
            string &s = *new string();
            s += "variable '" + name.lexeme + "' is not a map";
            throw runtime_exception(name, s);
        }
        try {
            switch (op) {
                case EQUAL:
                    map->values[key->value] = value;
                    break;
                case PLUS_EQUALS:
                    *(map->values[key->value]) += *value;
                    break;
                case MINUS_EQUALS:
                    *(map->values[key->value]) -= *value;
                    break;
                case STAR_EQUALS:
                    *(map->values[key->value]) *= *value;
                    break;
                case SLASH_EQUALS:
                    *(map->values[key->value]) /= *value;
                    break;
            }
            return map->values[key->value];
        } catch (string &s) {
            throw runtime_exception(name, s);
        }
    }
    if (enclosing != nullptr) {
        return enclosing->assign_map_field(name, op, key, value);
    }
    if (permissive) { return lns::GET_DEFAULT_NULL(); }
    string &s = *new string();
    s += "undefined variable '" + name.lexeme + "'";
    throw runtime_exception(name, s);
}

void runtime_environment::add_native(const string &s) {
    natives.insert(s);
}

bool runtime_environment::is_native(const string &basic_string) {
    return natives.find(basic_string) != natives.end();
}

object *runtime_environment::increment(const token &name) {
    if (contains_key(name.lexeme)) {
        variable s = values[name.lexeme];
        if (s.isfinal) {
            string &s = *new string();
            s += "variable '" + name.lexeme + "' is final";
            throw runtime_exception(name, s);
        }
        if (s.value->type != NUMBER_T) {
            string &s = *new string();
            s += "variable '" + name.lexeme + "' is not a number";
            throw runtime_exception(name, s);
        }
        number_o *n = dynamic_cast<number_o *>(s.value);
        ++n->value;
        return n;
    }
    if (enclosing != nullptr) return enclosing->increment(name);
    if (permissive) { return lns::GET_DEFAULT_NULL(); }
    string &s = *new string();
    s += "'" + name.lexeme + "' is undefined";
    throw runtime_exception(name, s);
}

object *runtime_environment::decrement(const token &name) {
    if (contains_key(name.lexeme)) {
        variable s = values[name.lexeme];
        if (s.isfinal) {
            string &s = *new string();
            s += "variable '" + name.lexeme + "' is final";
            throw runtime_exception(name, s);
        }
        if (s.value->type != NUMBER_T) {
            string &s = *new string();
            s += "variable '" + name.lexeme + "' is not a number";
            throw runtime_exception(name, s);
        }
        number_o *n = dynamic_cast<number_o *>(s.value);
        --n->value;
        return n;
    }
    if (enclosing != nullptr) return enclosing->increment(name);
    if (lns::permissive) { return lns::GET_DEFAULT_NULL(); }
    string &s = *new string();
    s += "'" + name.lexeme + "' is undefined";
    throw lns::runtime_exception(name, s);
}

void runtime_environment::reset() {
    values.clear();
    if (enclosing != nullptr) enclosing->reset();
}

variable::variable() : value(new null_o()), isfinal(false) {}

const variable& variable::operator=(const variable &v) {
    this->value = v.value;
    return *this;
}

variable::variable(const bool isfinal, object *value) : isfinal(isfinal), value(value) {}


lns::runtime_exception::runtime_exception(const lns::token &token, string &m) : message(m), token(token) {}

const char *lns::runtime_exception::what() const throw(){
    return message.c_str();
}

const char *lns::best_file_path(const char *filename) {
    ifstream rel_file(filename);
    auto buf = (char*) malloc(1024 * sizeof(char));
    if(rel_file.is_open()) return filename;
    *buf = '\0';
    strcat(buf,LNS_LIBRARY_LOCATION);
    strcat(buf,filename);
    ifstream path_file(buf);
    if(path_file.is_open()) return buf;
    strcat(buf,".lns");
    ifstream path_file2(buf);
    if(path_file2.is_open()) return buf;
    return filename;
}