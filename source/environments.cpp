//
// Created by lorenzo on 2/17/18.
//
#include "exceptions.h"
#include "environments.h"
#include "error_message.h"

string context::str() const {
    stringstream s;
    s << "<context@" << static_cast<const void *>(this) << ">";
    return s.str();
}

bool context::operator==(const object &o) const {
    return this == &o;
}

void context::define(const token *name, std::shared_ptr<object> o, bool is_final, visibility vis,
                     const char *def_file) {
    if (contains_key(name->lexeme))
        throw runtime_exception(name, VARIABLE_ALREADY_DEFINED(name->lexeme));
    define(name->lexeme, o, is_final, vis, def_file);
}

void context::define(const std::string &name, std::shared_ptr<object> o, bool is_final, visibility vis,
                     const char *def_file) {
    runtime_environment::define(name, o, is_final, vis, def_file);
}

shared_ptr<object> context::clone() const {
    return nullptr;
}

bool lns::runtime_environment::contains_key(const std::string name) {
    return values.find(name) != values.end();
}

lns::runtime_environment::runtime_environment() : enclosing(shared_ptr<runtime_environment>()),
                                                  values(std::map<std::string, variable>()) {}

lns::runtime_environment::runtime_environment(shared_ptr<runtime_environment> enc) : enclosing(enc),
                                                                                     values(std::map<std::string, variable>()) {}

shared_ptr<object> runtime_environment::get(const token *name, const char *accessing_file) {
    //cout << "runtime_environment::get " << name->lexeme << " from " << name->filename << ":" << to_string(name->line) << " at " << static_cast<const void *>(this) << endl;
    if (contains_key(name->lexeme)) {
        variable s = values[name->lexeme];
        if (s.visibility_ == V_LOCAL) {
            if (strcmp(s.def_file, accessing_file) != 0 && strcmp(DEBUGGER_WATCHES_FILENAME, accessing_file) != 0) {
                throw runtime_exception(name, VARIABLE_NOT_VISIBLE(name->lexeme));
            }
        }
        return s.value;
    }
    if (enclosing != nullptr) return enclosing->get(name, accessing_file);
    if (permissive) { return make_shared<null_o>(); }
    throw runtime_exception(name, VARIABLE_UNDEFINED(name->lexeme));
}


void runtime_environment::define(const token *name, shared_ptr<object> o, bool is_final, visibility vis,
                                 const char *def_file) {
    if (contains_key(name->lexeme)) {
        throw runtime_exception(name, VARIABLE_ALREADY_DEFINED(name->lexeme));
    }

    if (vis == V_GLOBAL) {
        if (enclosing != nullptr) {
            return enclosing->define(name, o, is_final, vis, def_file);
        }
    }
    define(name->lexeme, o, is_final, vis, def_file);
}

void
runtime_environment::assign(const token *name, token_type op, std::shared_ptr<object> obj, const char *assigning_file) {
    if (contains_key(name->lexeme)) {
        if (values[name->lexeme].is_final) {
            throw runtime_exception(name, VARIABLE_FINAL(name->lexeme));
        }
        //values[name.lexeme]->isfinal = false;
        try {
            variable v = values[name->lexeme];
            if (v.visibility_ == V_LOCAL) {
                if (strcmp(v.def_file, assigning_file) != 0 && strcmp(DEBUGGER_WATCHES_FILENAME, assigning_file) != 0) {
                    throw runtime_exception(name, VARIABLE_NOT_VISIBLE(name->lexeme));
                }
            }
            switch (op) {
                case EQUAL:
                    values[name->lexeme].value = obj;
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
        } catch (const char *s) {
            throw runtime_exception(name, s);
        }
        return;
    }
    if (enclosing != nullptr) {
        return enclosing->assign(name, op, obj, assigning_file);
    }
    if (permissive) { return define(name, obj, false, V_UNSPECIFIED, assigning_file); }
    throw runtime_exception(name, VARIABLE_UNDEFINED(name->lexeme));
}

bool runtime_environment::is_native(callable *ptr) {
    return natives.find(ptr) != natives.end();
}

void runtime_environment::reset() {
    values.clear();
    if (enclosing != nullptr) enclosing->reset();
}

void runtime_environment::define(const std::string &name, std::shared_ptr<object> o, bool is_final, visibility vis,
                                 const char *def_file) {
    values.insert(std::pair<string, lns::variable>(name, variable(vis, is_final, o, def_file)));
}

void runtime_environment::add_natives(const std::set<callable *> &natives) {
    this->natives.insert(natives.begin(), natives.end());
}

void runtime_environment::add_native(callable *ptr) {
    this->natives.insert(ptr);
}

bool runtime_environment::clear_var(const token *name) {
    return this->values.erase(name->lexeme) != 0;
}

std::set<callable *> context::declare_natives() const {
    std::set<callable *> natives;
    function_container *f;
    callable *clb;
    for (const auto &elem : this->values) {
        if (DCAST_ASNCHK(f, function_container*, elem.second.value.get())) {
            std::set<callable *> temp = f->declare_natives();
            natives.insert(temp.begin(), temp.end());
        } else if (elem.second.value->type == NATIVE_CALLABLE_T)
            natives.insert(DCAST(callable*, elem.second.value.get()));
    }
    return natives;
}

variable::variable() : value(new null_o()), visibility_(V_UNSPECIFIED), is_final(false), def_file("") {}

const variable &variable::operator=(const variable &v) {
    this->value = v.value;
    return *this;
}

variable::variable(lns::visibility vis, bool is_final, std::shared_ptr<object> value, const char *def_file)
        : visibility_(vis), value(value), is_final(is_final), def_file(def_file) {}
