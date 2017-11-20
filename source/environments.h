//
// Created by lorenzo on 10/09/17.
//

#ifndef CPPLNS_ENVIRONMENTS_H
#define CPPLNS_ENVIRONMENTS_H

#include "defs.h"
#include "expr.h"
#include "stmt.h"
#include <map>
#include <string>
#include <iterator>
#include <utility>
#include <set>

using namespace lns;
using namespace std;
namespace lns {
    class runtime_environment {
    private:
        struct comparator {
        public:
            bool operator()(const std::string s1, const std::string s2) {
                return s1.compare(s2) < 0;
            }
        };

        set <string> natives;
        runtime_environment *enclosing;
        map<string, variable> values;

        bool contains_key(const string name) {
            return values.find(name) != values.end();
        }

    public:
        explicit runtime_environment() : enclosing(nullptr), values(*new map<string, variable>()) {}

        explicit runtime_environment(runtime_environment *enc) : enclosing(enc), values(*new map<string, variable>) {}

        object *get(const token &name) {
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

        void define(const token &name, object *o, bool is_final, bool is_global) {
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

        void assign(const token &name,token_type op, object *obj) {
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
                            *v.value = *obj;
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
                    throw runtime_exception(name,s);
                }
                return;
            }
            if (enclosing != nullptr) {
                return enclosing->assign(name,op, obj);
            }
            if (permissive) { return define(name, obj, false, false); }
            string &s = *new string();
            s += "undefined variable '" + name.lexeme + "'";
            throw runtime_exception(name, s);
        }

        bool is_valid_object_type(objtype objtype) {
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

        object *get_map_field(const token &map_name, string key) {
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

        object *assign_map_field(const token &name,const token_type op,string_o *key, object *value) {
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
                    throw runtime_exception(name,s);
                }
            }
            if (enclosing != nullptr) {
                return enclosing->assign_map_field(name,op, key, value);
            }
            if (permissive) { return lns::GET_DEFAULT_NULL(); }
            string &s = *new string();
            s += "undefined variable '" + name.lexeme + "'";
            throw runtime_exception(name, s);
        }

        void add_native(const string &s) {
            natives.insert(s);
        }

        bool is_native(const string &basic_string) {
            return natives.find(basic_string) != natives.end();
        }

        object *increment(const token &name) {
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

        object *decrement(const token &name) {
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
            if (permissive) { return lns::GET_DEFAULT_NULL(); }
            string &s = *new string();
            s += "'" + name.lexeme + "' is undefined";
            throw runtime_exception(name, s);
        }

        void reset() {
            values.clear();
            if (enclosing != nullptr) enclosing->reset();
        }
    };

    class context : public runtime_environment, public object {
    public:
        context(runtime_environment *previous) : runtime_environment(previous), object(CONTEXT_T) {}

        string str() const override {
            return *new string("<context>");
        }
        
        bool operator==(const object &o) const override {
            return false;
        }
        bool operator&&(const object &o) const override {
            WRONG_OP(&&)
        }

        bool operator||(const object &o) const override {
            WRONG_OP(||)
        }

        object *operator!() const override {
            WRONG_OP(!)
        }

        bool operator>(const object &o2) const override {
            WRONG_OP(>)
        }

        bool operator>=(const object &o2) const override {
            WRONG_OP(>=)
        }

        bool operator<=(const object &o2) const override {
            WRONG_OP(<=)
        }

        bool operator<(const object &o2) const override {
            WRONG_OP(<)
        }

        object *operator+=(const object &o) override {
            WRONG_OP(+=)
        }

        object *operator-=(const object &o) override {
            WRONG_OP(-=)
        }

        object *operator*=(const object &o) override {
            WRONG_OP(*=)
        }

        object *operator/=(const object &o) override {
            WRONG_OP(/=)
        }

        object *operator+(const object &o2) const override {
            WRONG_OP(+)
        }

        object *operator-(const object &o2) const override {
            WRONG_OP(-)
        }

        object *operator*(const object &o2) const override {
            WRONG_OP(*)
        }

        object *operator/(const object &o2) const override {
            WRONG_OP(/)
        }

        object *operator^(const object &o2) const override {
            WRONG_OP(^)
        }

        object *operator-() const override {
            WRONG_OP(-)
        }

        object *operator++() override {
            WRONG_OP(++)
        }

        object *operator--() override {
            WRONG_OP(--)
        }
    };
}
#endif //CPPLNS_ENVIRONMENTS_H
