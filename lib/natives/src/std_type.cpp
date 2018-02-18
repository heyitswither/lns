//
// Created by lorenzo on 12/5/17.
//
#include "utils.h"
#include "callable.h"
#include "interpreter.h"
using namespace lns;
using namespace std;

class type_c : public native_callable{
public:
    type_c() : native_callable(1) {}

    const std::string name() const override {
        return string("type");
    }

    shared_ptr <object> call(std::vector <shared_ptr<object>> &args) override {
        object *o = args[0].get();
        auto type = make_shared<string_o>();
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
                type->value = "exception_definition";
                break;
            case EXCEPTION_T:
                type->value = "exception";
                break;
            case CLASS_DEFINITION_T:
                type->value = "class_definition";
                break;
            case INSTANCE_T:
                type->value = DCAST(instance_o * , o)->class_->str();
                break;
            default:
                type->value = "unknown";
                break;
        }
        return type;
    }
};


extern "C" shared_ptr <object> supplier() {
    return make_shared<type_c>();
}