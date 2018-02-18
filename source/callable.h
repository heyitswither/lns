//
// Created by lorenzo on 2/17/18.
//
#ifndef CPPLNS_CALLABLE_H
#define CPPLNS_CALLABLE_H

#include <vector>
#include "expr.h"
#include "stmt.h"
#include "primitives.h"

namespace lns {

    class callable : public object {
    public:
        callable() = delete;

        callable(bool is_native);

        virtual const parameter_declaration &arity() const = 0;

        virtual const std::string name() const = 0;

        virtual std::shared_ptr<object> call(std::vector<std::shared_ptr<object>> &args) = 0;

        bool operator==(const object &o) const override;

        std::string str() const override;

        std::shared_ptr<object> clone() const override;

    };

    class function_container : public object {
    public:
        explicit function_container(objtype type);

        virtual std::set<callable *> declare_natives() const = 0;
    };

    class native_callable : public callable {
    private:
        parameter_declaration &parameters;
    public:
        native_callable(int arity);

        const parameter_declaration &arity() const;
    };


}
#endif //CPPLNS_CALLABLE_H
