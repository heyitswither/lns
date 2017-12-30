//
// Created by lorenzo on 12/5/17.
//
#include "defs.h"

using namespace lns;
class type_c : public native_callable{
public:
    type_c() : native_callable(1) {}

    const std::string &name() const override {
        return S(type);
    }

    object *call(std::vector<object *> &args) override {
        object *o = args[0];
        string_o *type = new string_o("");
        switch (o->type) {
            case NUMBER_T:
                type->value = "number";
                break;
            case STRING_T:
                type->value = "string";
                break;
            case ARRAY_T:
                type->value = "array";
                break;
            case BOOL_T:
                type->value = "boolean";
                break;
            case NULL_T:
                type->value = "null";
                break;
            case CONTEXT_T:
                type->value = "context";
                break;
            case CALLABLE_T:
            case NATIVE_CALLABLE_T:
                type->value = "callable";
                break;
            case EXCEPTION_DEFINITION_T:
                type->value = "";
                break;
            case EXCEPTION_T:
                type->value = "exception";
                break;
            case CLASS:
                type->value = "class";
                break;
        }
        return type;
    }
};


extern "C" object* supplier(){
    return new type_c;
}