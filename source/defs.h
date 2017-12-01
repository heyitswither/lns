//
// Created by lorenzo on 31/08/17.
//

#ifndef CPPLNS_DEFS_H
#define CPPLNS_DEFS_H

#include <sstream>
#include <string>
//#include "stmt.h"
//#include "errors.h"
//#include "environments.h"
#include <string>
#include <map>
#include <cmath>
#include <utility>
#include <vector>
#include <set>

#define WRONG_OP(OP) throw "invalid operand(s) for operator " #OP;
#define NULL_OP(OP) throw "null operand(s) given for operator " #OP;
#define CODE_LEAK_ERROR (-250)

namespace lns {
    enum token_type {
        // Single-character tokens.
                LEFT_PAREN,
        RIGHT_PAREN,
        LEFT_SQR,
        RIGHT_SQR,
        COMMA,
        DOT,
        MINUS,
        MINUS_EQUALS,
        PLUS,
        PLUS_EQUALS,
        PLUS_PLUS,
        MINUS_MINUS,
        SEMICOLON,
        SLASH,
        SLASH_EQUALS,
        STAR,
        STAR_EQUALS,
        HAT,
        // One or two character tokens.
                BANG,
        BANG_EQUAL,
        EQUAL,
        EQUAL_EQUAL,
        GREATER,
        GREATER_EQUAL,
        LESS,
        LESS_EQUAL,

        // Literals.
                IDENTIFIER,
        STRING,
        NUMBER,

        // Keywords.
                NATIVES,
        AND,
        CLASS,
        ELSE,
        FALSE,
        FUNCTION,
        FOR,
        IF,
        NUL,
        OR,
        XOR,
        NOR,
        NAND,
        NOT,
        RETURN,
        SUPER,
        THIS,
        TRUE,
        VAR,
        WHILE,
        GLOBAL,
        FINAL,
        USE,
        BREAK,
        CONTINUE,
        CONTEXT,
        BEGIN,
        THEN,
        DO,
        END,
        DPCHECK,
        UNRECOGNIZED,
        EOF_
    };
    enum objtype {
        NUMBER_T, STRING_T, BOOL_T, NULL_T, MAP_T, CALLABLE_T, CONTEXT_T
    };
    typedef objtype object_type;

    const std::string KEYWORDS_S_VALUES[] = {"LEFT_PAREN", "RIGHT_PAREN", "LEFT_SQR", "RIGHT_SQR", "COMMA", "DOT", "MINUS",
                                        "MINUS_EQUALS", "PLUS", "PLUS_EQUALS", "PLUS_PLUS", "MINUS_MINUS", "SEMICOLON",
                                        "SLASH", "SLASH_EQUALS", "STAR", "STAR_EQUALS", "HAT", "BANG", "BANG_EQUAL",
                                        "EQUAL", "EQUAL_EQUAL", "GREATER", "GREATER_EQUAL", "LESS", "LESS_EQUAL",
                                        "IDENTIFIER", "STRING", "NUMBER", "NATIVES", "AND", "CLASS", "ELSE", "FALSE",
                                        "FUNCTION", "FOR", "IF", "NUL", "OR", "XOR", "NOR", "NAND", "NOT", "RETURN",
                                        "SUPER", "THIS", "TRUE", "VAR", "WHILE", "GLOBAL", "FINAL", "USE", "BREAK",
                                        "CONTINUE", "CONTEXT", "BEGIN", "THEN", "DO", "DPCHECK", "END", "UNRECOGNIZED",
                                        "EOF_"};

    class object {
    private:
        object();

    public:
        objtype type;

        explicit object(objtype t);

        virtual std::string str() const = 0;

        //    return *new string("object::plain");
        //};
        virtual bool operator==(const object &o) const = 0;

        virtual bool operator&&(const object &o) const = 0;

        virtual bool operator||(const object &o) const = 0;

        virtual object *operator!() const = 0;

        virtual bool operator>(const object &o2) const = 0;

        virtual bool operator>=(const object &o2) const = 0;

        virtual bool operator<=(const object &o2) const = 0;

        virtual bool operator<(const object &o2) const = 0;

        virtual object *operator+=(const object &o) = 0;

        virtual object *operator-=(const object &o) = 0;

        virtual object *operator*=(const object &o) = 0;

        virtual object *operator/=(const object &o) = 0;

        virtual object *operator+(const object &o2) const = 0;

        virtual object *operator-(const object &o2) const = 0;

        virtual object *operator*(const object &o2) const = 0;

        virtual object *operator/(const object &o2) const = 0;

        virtual object *operator^(const object &o2) const = 0;

        virtual object *operator-() const = 0;

        virtual object *operator++() = 0;

        virtual object *operator--() = 0;


    };

    bool check_type(object_type type, const object &o1, const object &o2);

    class string_o : public object {
    public:
        std::string value;

        explicit string_o(std::string value);

        std::string str() const override;

        bool operator==(const object &o) const override;

        bool operator||(const object &o) const override;

        object *operator!() const override;

        bool operator&&(const object &o) const override;

        object *operator+=(const object &o) override;

        object *operator-=(const object &o) override;

        object *operator*=(const object &o) override;

        object *operator/=(const object &o) override;

        bool operator>(const object &o2) const override;

        bool operator>=(const object &o2) const override;

        bool operator<=(const object &o2) const override;

        bool operator<(const object &o2) const override;

        object *operator+(const object &o2) const override;

        object *operator-(const object &o2) const override;

        object *operator*(const object &o2) const override;

        object *operator/(const object &o2) const override;

        object *operator^(const object &o2) const override;

        object *operator++() override;

        object *operator--() override;

        object *operator-() const override;

    };

    class number_o : public object {
    public:
        double value;

        explicit number_o(double value);

        std::string str() const override;

        bool operator==(const object &o) const override;

        bool operator&&(const object &o) const override;

        bool operator||(const object &o) const override;

        object *operator!() const override;

        bool operator>(const object &o2) const override;

        bool operator>=(const object &o2) const override;

        bool operator<=(const object &o2) const override;

        bool operator<(const object &o2) const override;

        object *operator+=(const object &o) override;

        object *operator-=(const object &o) override;

        object *operator*=(const object &o) override;

        object *operator/=(const object &o) override;

        object *operator+(const object &o2) const override;

        object *operator-(const object &o2) const override;

        object *operator*(const object &o2) const override;

        object *operator/(const object &o2) const override;

        object *operator^(const object &o2) const override;

        object *operator-() const override;

        object *operator++() override;

        object *operator--() override;
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

        object *operator!() const override;

        bool operator>(const object &o2) const override;

        bool operator>=(const object &o2) const override;

        bool operator<=(const object &o2) const override;

        bool operator<(const object &o2) const override;

        object *operator+=(const object &o) override;

        object *operator-=(const object &o) override;

        object *operator*=(const object &o) override;

        object *operator/=(const object &o) override;

        object *operator+(const object &o2) const override;

        object *operator-(const object &o2) const override;

        object *operator*(const object &o2) const override;

        object *operator/(const object &o2) const override;

        object *operator^(const object &o2) const override;

        object *operator-() const override;

        object *operator++() override;

        object *operator--() override;
    };

    class null_o : public object {
    public:
        null_o();

        virtual std::string str() const;

        bool operator==(const object &o) const override;

        bool operator&&(const object &o) const override;

        bool operator||(const object &o) const override;

        object *operator!() const override;

        bool operator>(const object &o2) const override;

        bool operator>=(const object &o2) const override;

        bool operator<=(const object &o2) const override;

        bool operator<(const object &o2) const override;

        object *operator+=(const object &o) override;

        object *operator-=(const object &o) override;

        object *operator*=(const object &o) override;

        object *operator/=(const object &o) override;

        object *operator+(const object &o2) const override;

        object *operator-(const object &o2) const override;

        object *operator*(const object &o2) const override;

        object *operator/(const object &o2) const override;

        object *operator^(const object &o2) const override;

        object *operator-() const override;

        object *operator++() override;

        object *operator--() override;
    };

    class map_o : public object {
    private:
    public:
        map_o();

        std::map<std::string, object *> values;

        bool operator==(const object &o) const override;

        std::string str() const override;

        const bool contains_key(std::string s);

        bool operator&&(const object &o) const override;

        bool operator||(const object &o) const override;

        object *operator!() const override;

        bool operator>(const object &o2) const override;

        bool operator>=(const object &o2) const override;

        bool operator<=(const object &o2) const override;

        bool operator<(const object &o2) const override;

        object *operator+=(const object &o) override;

        object *operator-=(const object &o) override;

        object *operator*=(const object &o) override;

        object *operator/=(const object &o) override;

        object *operator+(const object &o2) const override;

        object *operator-(const object &o2) const override;

        object *operator*(const object &o2) const override;

        object *operator/(const object &o2) const override;

        object *operator^(const object &o2) const override;

        object *operator-() const override;

        object *operator++() override;

        object *operator--() override;
    };

    class variable {
    public:
        const bool isfinal;
        object *value;

        explicit variable();

        variable(const bool isfinal, object *value);

        const operator=(variable &v);
    };

    class token {
    public:
        token(token_type type, std::string lexeme, object &literal, const char *filename, int line);

        token_type type;
        std::string lexeme;
        object &literal;
        const char *filename;
        int line;

        std::string format() const;
        //void operator delete(void*){}
    };

    class runtime_environment {
    private:
        std::set <std::string> natives;
        runtime_environment *enclosing;
        std::map<std::string, lns::variable> values;

        bool contains_key(const std::string name);

    public:
        explicit runtime_environment();

        explicit runtime_environment(runtime_environment *enc);

        object *get(const token &name);

        void define(const token &name, object *o, bool is_final, bool is_global);

        void assign(const token &name,token_type op, object *obj);

        bool is_valid_object_type(objtype objtype);

        object *get_map_field(const token &map_name, std::string key);

        object *assign_map_field(const token &name,const token_type op,string_o *key, object *value);

        void add_native(const std::string &s);

        bool is_native(const std::string &basic_string);

        object *increment(const token &name);

        object *decrement(const token &name);

        void reset();
    };

    class context : public lns::runtime_environment, public object {
    public:
        explicit context(runtime_environment *previous) : runtime_environment(previous), object(CONTEXT_T) {}

        std::string str() const override;

        bool operator==(const object &o) const override;

        bool operator&&(const object &o) const override;

        bool operator||(const object &o) const override;

        object *operator!() const override;

        bool operator>(const object &o2) const override;

        bool operator>=(const object &o2) const override;

        bool operator<=(const object &o2) const override;

        bool operator<(const object &o2) const override;

        object *operator+=(const object &o) override;

        object *operator-=(const object &o) override;

        object *operator*=(const object &o) override;

        object *operator/=(const object &o) override;

        object *operator+(const object &o2) const override;

        object *operator-(const object &o2) const override;

        object *operator*(const object &o2) const override;

        object *operator/(const object &o2) const override;

        object *operator^(const object &o2) const override;

        object *operator-() const override;

        object *operator++() override;

        object *operator--() override;
    };



    class stack_call {
    public:
        const char *filename;
        const int line;
        const std::string &method;

        stack_call(const char *filename, const int line, const std::string &method);
    };


    class return_signal : public std::exception {
    public:
        object *value;
        const token &keyword;

        return_signal(object *value, const token &keyword);

        const char *what() const throw() override;
    };

    class break_signal : public std::exception {
    public:
        const token &keyword;

        explicit break_signal(const token &keyword);

        const char *what() const throw() override;
    };

    class continue_signal : public std::exception {
    public:
        const token &keyword;

        const char *what() const throw() override;

        explicit continue_signal(const token &keyword);
    };

    static object *GET_DEFAULT_NULL();

    inline bool check_type(object_type type, const object &o1, const object &o2) {
        return (o1.type == type && o2.type == type);
    }

    object *GET_DEFAULT_NULL() {
        return new null_o();
    }

    class runtime_exception : public std::exception {
    public:
        std::string &message;
        const lns::token &token;

        runtime_exception(const lns::token &token, std::string &m);

        const char *what() const throw() override;
    };
}

#endif //CPPLNS_DEFS_H