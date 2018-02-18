//
// Created by lorenzo on 2/17/18.
//

#include "callable.h"

callable::callable(bool is_native) : object(is_native ? NATIVE_CALLABLE_T : CALLABLE_T) {}

bool callable::operator==(const object &o) const {
    return this == &o;
}

string callable::str() const {
    stringstream s;
    s << "<" << (this->type == NATIVE_CALLABLE_T ? "native_" : "") << "callable@" << static_cast<const void *>(this)
      << ", name: \"" << this->name() << "\">";
    return s.str();
}

std::shared_ptr<object> callable::clone() const {
    return nullptr;
}

function_container::function_container(objtype type) : object(type) {}

native_callable::native_callable(int arity) : callable(true), parameters(*new parameter_declaration(arity)) {}

const parameter_declaration &native_callable::arity() const {
    return parameters;
}
