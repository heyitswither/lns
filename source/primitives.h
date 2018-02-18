//
// Created by lorenzo on 2/17/18.
//

#ifndef CPPLNS_OBJECT_H
#define CPPLNS_OBJECT_H

#include <memory>
#include <cstring>
#include <map>
#include <sstream>
#include <cmath>
#include <set>
#include "utils.h"

#define WRONG_OP(OP) throw lns::INVALID_OP(#OP,this->type,&o.type);
#define WRONG_OP_UN(OP) throw lns::INVALID_OP(#OP,this->type,nullptr);

namespace lns {

    typedef enum objtype {
        NUMBER_T, STRING_T, BOOL_T, NULL_T, ARRAY_T, CALLABLE_T, NATIVE_CALLABLE_T, CONTEXT_T, EXCEPTION_T,
        EXCEPTION_DEFINITION_T, CLASS_DEFINITION_T, INSTANCE_T, UNSPECIFIED
    } object_type;

    const char *type_to_string(object_type t);

    const char *INVALID_OP(const char *OP, const lns::object_type t1, const lns::object_type *t2);

    class object {
    private:
        object();

    public:
        objtype type;

        explicit object(objtype t);

        virtual std::string str() const = 0;

        virtual bool operator==(const object &o) const { WRONG_OP(==) };

        virtual bool operator&&(const object &o) const { WRONG_OP(and) };

        virtual bool operator||(const object &o) const { WRONG_OP(or) };

        virtual std::shared_ptr<object> operator!() const { WRONG_OP_UN(not) };

        virtual bool operator>(const object &o) const { WRONG_OP(>) };

        virtual bool operator>=(const object &o) const { WRONG_OP(>=) };

        virtual bool operator<=(const object &o) const { WRONG_OP(<=) };

        virtual bool operator<(const object &o) const { WRONG_OP(<) };

        virtual void operator+=(const object &o) { WRONG_OP(+=) };

        virtual void operator-=(const object &o) { WRONG_OP(-=) };

        virtual void operator*=(const object &o) { WRONG_OP(*=) };

        virtual void operator/=(const object &o) { WRONG_OP(/=) };

        virtual std::shared_ptr<object> operator+(const object &o) const { WRONG_OP(+) };

        virtual std::shared_ptr<object> operator-(const object &o) const { WRONG_OP(-) };

        virtual std::shared_ptr<object> operator*(const object &o) const { WRONG_OP(*) };

        virtual std::shared_ptr<object> operator/(const object &o) const { WRONG_OP(/) };

        virtual std::shared_ptr<object> operator^(const object &o) const { WRONG_OP(^) };

        virtual std::shared_ptr<object> operator-() const { WRONG_OP_UN(-) };

        virtual std::shared_ptr<object> operator++() { WRONG_OP_UN(++) };

        virtual std::shared_ptr<object> operator--() { WRONG_OP_UN(--) };

        virtual std::shared_ptr<object> operator++(int) { WRONG_OP_UN(++) };

        virtual std::shared_ptr<object> operator--(int) { WRONG_OP_UN(--) };

        virtual std::shared_ptr<object> clone() const = 0;
    };

    bool check_type(object_type type, const lns::object &o1, const lns::object &o);

    class string_o : public object {
    public:
        std::string value;

        std::shared_ptr<object> clone() const override;

        explicit string_o(std::string value);

        string_o();

        std::string str() const override;

        bool operator==(const object &o) const override;

        void operator+=(const object &o) override;

        bool operator>(const object &o) const override;

        bool operator>=(const object &o) const override;

        bool operator<=(const object &o) const override;

        bool operator<(const object &o) const override;

        std::shared_ptr<object> operator+(const object &o) const override;

        virtual std::shared_ptr<object> operator/(const object &o) const;
    };

    class number_o : public object {
    public:
        long double value;

        explicit number_o(double value);

        std::string str() const override;

        bool operator==(const object &o) const override;

        bool operator>(const object &o) const override;

        bool operator>=(const object &o) const override;

        bool operator<=(const object &o) const override;

        bool operator<(const object &o) const override;

        void operator+=(const object &o) override;

        void operator-=(const object &o) override;

        void operator*=(const object &o) override;

        void operator/=(const object &o) override;

        std::shared_ptr<object> operator+(const object &o) const override;

        std::shared_ptr<object> operator-(const object &o) const override;

        std::shared_ptr<object> operator*(const object &o) const override;

        std::shared_ptr<object> operator/(const object &o) const override;

        std::shared_ptr<object> operator^(const object &o) const override;

        std::shared_ptr<object> operator-() const override;

        std::shared_ptr<object> operator++() override;

        std::shared_ptr<object> operator--() override;

        std::shared_ptr<object> operator++(int) override;

        std::shared_ptr<object> operator--(int) override;

        std::shared_ptr<object> clone() const;
    };

    class bool_o : public object {
    public:
        bool value;

        explicit bool_o(bool value);

        std::string str() const override {
            return value ? "true" : "false";
        }

        bool operator==(const object &o) const override;

        bool operator&&(const object &o) const override;

        bool operator||(const object &o) const override;

        std::shared_ptr<object> operator!() const override;

        std::shared_ptr<object> operator+(const object &o) const override;

        std::shared_ptr<object> clone() const override;
    };

    class null_o : public object {
    public:
        explicit null_o();

        std::string str() const override;

        bool operator==(const object &o) const override;

        std::shared_ptr<object> clone() const override;
    };

    class array_o : public object {
    private:
    public:
        explicit array_o();

        std::map<double, std::shared_ptr<object>> values;

        bool operator==(const object &o) const override;

        std::string str() const override;

        const bool contains_key(double t);

        std::shared_ptr<object> clone() const;

        std::shared_ptr<object> operator+(const object &o) const override;
    };


    class exception_definition : public object {
    public:
        const std::string &name;
        const std::set<std::string> &fields;
        const char *def_file;
        int def_line;

        exception_definition(const char *def_file, int def_line, const std::string &name,
                             const std::set<std::string> &fields);

        std::string str() const override;

        bool operator==(const object &o) const override;

        std::shared_ptr<object> clone() const override;
    };
}

#endif //CPPLNS_OBJECT_H
