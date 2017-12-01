//
// Created by lorenzo on 11/30/17.
//

#include "environments.h"
#include "options.h"
#include "exceptions.h"
using namespace lns;
using namespace std;
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

const variable::operator=(variable &v) {
    this->value = v.value;
}

variable::variable(const bool isfinal, object *value) : isfinal(isfinal), value(value) {}
