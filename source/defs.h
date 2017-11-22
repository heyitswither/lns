//
// Created by lorenzo on 31/08/17.
//

#ifndef CPPLNS_DEFS_H
#define CPPLNS_DEFS_H

#include <sstream>
#include <string>
#include "errors.h"
#include <map>
#include <cmath>

#define WRONG_OP(OP) throw "invalid operand(s) for operator " #OP;
#define NULL_OP(OP) throw "null operand(s) given for operator " #OP;
#define CODE_LEAK_ERROR (-250)
//#include "interpreter.h"

using namespace std;
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
    UNRECOGNIZED,
    EOF_
};
enum objtype {
    NUMBER_T, STRING_T, BOOL_T, NULL_T, MAP_T, CALLABLE_T, CONTEXT_T
};
typedef objtype object_type;
namespace lns {
    const string KEYWORDS_S_VALUES[] = {// Single-character tokens.
            // Single-character tokens.
            "LEFT_PAREN",
            "RIGHT_PAREN",
            "LEFT_SQR",
            "RIGHT_SQR",
            "COMMA",
            "DOT",
            "MINUS",
            "MINUS_EQUALS",
            "PLUS",
            "PLUS_EQUALS",
            "PLUS_PLUS",
            "MINUS_MINUS",
            "SEMICOLON",
            "SLASH",
            "SLASH_EQUALS",
            "STAR",
            "STAR_EQUALS",
            "HAT",
            // One or two character tokens.
            "BANG",
            "BANG_EQUAL",
            "EQUAL",
            "EQUAL_EQUAL",
            "GREATER",
            "GREATER_EQUAL",
            "LESS",
            "LESS_EQUAL",

            // Literals.
            "IDENTIFIER",
            "STRING",
            "NUMBER",

            // Keywords.
            "NATIVES",
            "AND",
            "CLASS",
            "ELSE",
            "FALSE",
            "FUNCTION",
            "FOR",
            "IF",
            "NUL",
            "OR",
            "XOR",
            "NOR",
            "NAND",
            "RETURN",
            "SUPER",
            "THIS",
            "TRUE",
            "VAR",
            "WHILE",
            "GLOBAL",
            "FINAL",
            "USE",
            "BREAK",
            "CONTINUE",
            "CONTEXT",
            "BEGIN",
            "THEN",
            "DO",
            "END",
            "UNRECOGNIZED",
            "EOF_"};

    class object {
    private:
        object() {}

    public:
        objtype type;

        object(objtype t) : type(t) {}

        virtual string str() const = 0;

        //    return *new string("object::plain");
        //};
        virtual bool operator==(const object &o) const = 0;

        virtual bool operator&&(const object &o) const = 0;

        virtual bool operator||(const object &o) const = 0;

        virtual object* operator!() const = 0;

        virtual bool operator>(const object &o2) const = 0;

        virtual bool operator>=(const object &o2) const = 0;

        virtual bool operator<=(const object &o2) const= 0;

        virtual bool operator<(const object &o2) const= 0;

        virtual object *operator+=(const object &o) = 0;

        virtual object *operator-=(const object &o) = 0;

        virtual object *operator*=(const object &o) = 0;

        virtual object *operator/=(const object &o) = 0;

        virtual object *operator+(const object &o2) const= 0;

        virtual object *operator-(const object &o2) const= 0;

        virtual object *operator*(const object &o2) const= 0;

        virtual object *operator/(const object &o2) const= 0;

        virtual object *operator^(const object &o2) const= 0;

        virtual object *operator-() const= 0;

        virtual object *operator++() = 0;

        virtual object *operator--() = 0;


    };
    bool check_type(object_type type, const object& o1, const object& o2);
    class string_o : public object {
    public:
        string value;

        string_o(string value) : object(objtype::STRING_T), value(value) {}

        virtual string str() const {
             return value;
        }

        bool operator==(const object &o) const override {
            if (o.type != STRING_T) return false;
            return (value == dynamic_cast<const string_o *>(&o)->value);
        }

        bool operator||(const object &o) const override {
            WRONG_OP(||)
        }

        object *operator!() const override {
            WRONG_OP(!)
        }

        bool operator&&(const object &o) const override {
            WRONG_OP(&&);
        }

        object *operator+=(const object &o) override {
            this->value += o.str();
            return this;
        }

        object *operator-=(const object &o) override {
            WRONG_OP(-);
        }

        object *operator*=(const object &o) override {
            WRONG_OP(*);
        }

        object *operator/=(const object &o) override {
            WRONG_OP(/);
        }

        bool operator>(const object &o2) const override {
            if(!check_type(STRING_T,*this,o2)) WRONG_OP(>);
            return this->value > dynamic_cast<const string_o&>(o2).value;
        }

        bool operator>=(const object &o2) const override {
            if(!check_type(STRING_T,*this,o2)) WRONG_OP(>=);
            return this->value >= dynamic_cast<const string_o&>(o2).value;
        }

        bool operator<=(const object &o2) const override {
            if(!check_type(STRING_T,*this,o2)) WRONG_OP(<=);
            return this->value <= dynamic_cast<const string_o&>(o2).value;
        }

        bool operator<(const object &o2) const override {
            if(!check_type(STRING_T,*this,o2)) WRONG_OP(<);
            return this->value < dynamic_cast<const string_o&>(o2).value;
        }

        object *operator+(const object &o2) const override {
            return new string_o(this->value + o2.str());
        }

        object *operator-(const object &o2) const override {
            WRONG_OP(-);
        }

        object *operator*(const object &o2) const override {
            WRONG_OP(*);
        }

        object *operator/(const object &o2) const override {
            WRONG_OP(/);
        }

        object *operator^(const object &o2) const override {
            WRONG_OP(^);
        }

        object *operator++() override {
            WRONG_OP(++);
        }

        object *operator--() override {
            WRONG_OP(--);
        }

        object *operator-() const override {
            WRONG_OP(- (unary));
        }

    };

    class number_o : public object {
    public:
        double value;

        number_o(double value) : object(objtype::NUMBER_T), value(value) {}

        virtual string str() const {
            stringstream ss;
            ss << value;
            return *new string(ss.str());
        }

        bool operator==(const object &o) const override {
            if (o.type != NUMBER_T) return false;
            return value == dynamic_cast<number_o *>(const_cast<object *>(&o))->value;
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
            if(!check_type(NUMBER_T,*this,o2)) WRONG_OP(>);
            return this->value > dynamic_cast<const number_o&>(o2).value;
        }

        bool operator>=(const object &o2) const override {
            if(!check_type(NUMBER_T,*this,o2)) WRONG_OP(>=);
            return this->value >= dynamic_cast<const number_o&>(o2).value;
        }

        bool operator<=(const object &o2) const override {
            if(!check_type(NUMBER_T,*this,o2)) WRONG_OP(<=);
            return this->value >= dynamic_cast<const number_o&>(o2).value;
        }

        bool operator<(const object &o2) const override {
            if(!check_type(NUMBER_T,*this,o2)) WRONG_OP(<);
            return this->value < dynamic_cast<const number_o&>(o2).value;
        }

        object *operator+=(const object &o) override {
            if(!check_type(NUMBER_T,*this,o)) WRONG_OP(+=);
            this->value += dynamic_cast<const number_o&>(o).value;
            return this;
        }

        object *operator-=(const object &o) override {
            if(!check_type(NUMBER_T,*this,o)) WRONG_OP(-=);
            this->value -= dynamic_cast<const number_o&>(o).value;
            return this;
        }

        object *operator*=(const object &o) override {
            if(!check_type(NUMBER_T,*this,o)) WRONG_OP(*=);
            this->value *= dynamic_cast<const number_o&>(o).value;
            return this;
        }

        object *operator/=(const object &o) override {
            if(!check_type(NUMBER_T,*this,o)) WRONG_OP(/=);
            this->value /= dynamic_cast<const number_o&>(o).value;
            return this;
        }

        object *operator+(const object &o2) const override {
            if(!check_type(NUMBER_T,*this,o2)) WRONG_OP(+);
            return new number_o(this->value + dynamic_cast<const number_o&>(o2).value);
        }

        object *operator-(const object &o2) const override {
            if(!check_type(NUMBER_T,*this,o2)) WRONG_OP(-);
            return new number_o(this->value - dynamic_cast<const number_o&>(o2).value);
        }

        object *operator*(const object &o2) const override {
            if(!check_type(NUMBER_T,*this,o2)) WRONG_OP(+);
            return new number_o(this->value * dynamic_cast<const number_o&>(o2).value);
        }

        object *operator/(const object &o2) const override {
            if(!check_type(NUMBER_T,*this,o2)) WRONG_OP(+);
            return new number_o(this->value / dynamic_cast<const number_o&>(o2).value);
        }

        object *operator^(const object &o2) const override {
            if(!check_type(NUMBER_T,*this,o2)) WRONG_OP(+);
            return new number_o(pow(this->value,dynamic_cast<const number_o&>(o2).value));
        }

        object *operator-() const override {
            return new number_o(this->value);
        }

        object *operator++() override {
            ++this->value;
            return new number_o(value - 1);
        }

        object *operator--() override {
            --this->value;
            return new number_o(value + 1);
        }
    };

    class bool_o : public object {
    public:
        bool value;

        bool_o(bool value) : object(objtype::BOOL_T), value(value) {}

        virtual string str() const {
            return value ? "true" : "false";
        }

        bool operator==(const object &o) const override {
            if (o.type != BOOL_T) return false;
            return value == dynamic_cast<const bool_o&>(o).value;
        }

        bool operator&&(const object &o) const override {
            if(!check_type(BOOL_T,*this,o)) WRONG_OP(&&)
            return this->value && dynamic_cast<const bool_o&>(o).value;
        }

        bool operator||(const object &o) const override {
            if(!check_type(BOOL_T,*this,o)) WRONG_OP(&&)
            return this->value || dynamic_cast<const bool_o&>(o).value;
        }

        object *operator!() const override {
            return new bool_o(!this->value);
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

    class null_o : public object {
    public:
        null_o() : object(NULL_T) {}

        virtual string str() const {
            return "null";
        }

        bool operator==(const object &o) const override {
            return o.type == NULL_T;
        }

        bool operator&&(const object &o) const override {
            NULL_OP(&&)
        }

        bool operator||(const object &o) const override {
            NULL_OP(||)
        }

        object *operator!() const override {
            NULL_OP(!)
        }

        bool operator>(const object &o2) const override {
            NULL_OP(>)
        }

        bool operator>=(const object &o2) const override {
            NULL_OP(>=)
        }

        bool operator<=(const object &o2) const override {
            NULL_OP(<=)
        }

        bool operator<(const object &o2) const override {
            NULL_OP(<)
        }

        object *operator+=(const object &o) override {
            NULL_OP(+=)
        }

        object *operator-=(const object &o) override {
            NULL_OP(-=)
        }

        object *operator*=(const object &o) override {
            NULL_OP(*=)
        }

        object *operator/=(const object &o) override {
            NULL_OP(/=)
        }

        object *operator+(const object &o2) const override {
            NULL_OP(+)
        }

        object *operator-(const object &o2) const override {
            NULL_OP(-)
        }

        object *operator*(const object &o2) const override {
            NULL_OP(*)
        }

        object *operator/(const object &o2) const override {
            NULL_OP(/)
        }

        object *operator^(const object &o2) const override {
            NULL_OP(^)
        }

        object *operator-() const override {
            NULL_OP(-)
        }

        object *operator++() override {
            NULL_OP(++)
        }

        object *operator--() override {
            NULL_OP(--)
        }
    };

    class map_o : public object {
    private:
    public:
        map_o() : object(MAP_T) {}

        map<string, object *> values;

        bool operator==(const object &o) const override;

        string str() const override {
            stringstream ss;
            ss << "{";
            int i = 0;
            for(auto p : values){
                if(++i != 1) ss << ", ";
                ss << "\"" << p.first << "\" : ";
                if(p.second->type == STRING_T) ss << "\"";
                ss << p.second->str();
                if(p.second->type == STRING_T) ss << "\"";
            }
            ss << "}";
            return ss.str(); //TODO: implement JSON map conversion
        }

        const bool contains_key(string s) {
            return values.find(s) != values.end();
        }

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

    bool map_o::operator==(const object &o) const {
        if (o.type != MAP_T) return false;
        return values == dynamic_cast<map_o *>(const_cast<object *>(&o))->values;
    }

    bool map_o::operator&&(const object &o) const {
        WRONG_OP(&&)
    }

    bool map_o::operator||(const object &o) const {
        WRONG_OP(||)
    }

    object *map_o::operator!() const {
        WRONG_OP(!)
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

    class token {
    public:
        token(token_type type, string lexeme, object &literal, const char *filename, int line) : type(type),
                                                                                                 lexeme(lexeme),
                                                                                                 literal(literal),
                                                                                                 filename(filename),
                                                                                                 line(line) {}

        token_type type;
        string lexeme;
        object &literal;
        const char *filename;
        int line;

        string format() const {
            stringstream ss;
            ss << "type: " << KEYWORDS_S_VALUES[type] << ", lexeme: " << lexeme << ", literal: |";
            string str = literal.str();
            ss << str;
            ss << "|, file: \"" << filename << "\", line:" << line;
            return *new string(ss.str());
        }
        //void operator delete(void*){}
    };

    class stack_call {
    public:
        const char *filename;
        const int line;
        const string &method;

        stack_call(const char *filename, const int line, const string &method) : filename(filename), line(line),
                                                                                 method(method) {}
    };

    class variable {
    public:
        const bool isfinal;
        object *value;

        explicit variable() : value(new null_o()), isfinal(false) {}

        variable(const bool isfinal, object *value) : isfinal(isfinal), value(value) {}

        const operator=(variable &v) {
            this->value = v.value;
        }
    };

    class return_signal : public exception {
    public:
        object *value;
        const token &keyword;

        return_signal(object *value, const token &keyword) : value(value), keyword(keyword) {}

        const char *what() const throw() {
            return exception::what();
        }
    };

    class break_signal : public exception {
    public:
        const token &keyword;

        break_signal(const token &keyword) : keyword(keyword) {}

        const char *what() const throw() {
            return exception::what();
        }
    };

    class continue_signal : public exception {
    public:
        const token &keyword;

        const char *what() const throw() {
            return exception::what();
        }

        explicit continue_signal(const token &keyword) : keyword(keyword) {}
    };

    static object *GET_DEFAULT_NULL() {
        return new null_o();
    }

    inline bool check_type(object_type type, const object& o1, const object& o2){
        return (o1.type == type && o2.type == type);
    }
}

#endif //CPPLNS_DEFS_H