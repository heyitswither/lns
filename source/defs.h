//
// Created by lorenzo on 31/08/17.
//

#ifndef CPPLNS_DEFS_H
#define CPPLNS_DEFS_H
#include <sstream>
#include <string>
#include "errors.h"
#include <map>
#define CODE_LEAK_ERROR (-250)
//#include "interpreter.h"

using namespace std;
enum token_type {
    // Single-character tokens.
            LEFT_PAREN, RIGHT_PAREN, LEFT_BRACE, RIGHT_BRACE, LEFT_SQR, RIGHT_SQR,
    COMMA, DOT, MINUS, PLUS, PLUS_PLUS, MINUS_MINUS, SEMICOLON,SLASH, STAR, HAT,
    // One or two character tokens.
            BANG, BANG_EQUAL,
    EQUAL, EQUAL_EQUAL,
    GREATER, GREATER_EQUAL,
    LESS, LESS_EQUAL,

    // Literals.
            IDENTIFIER, STRING, NUMBER,

    // Keywords.
            NATIVES,AND, CLASS, ELSE, FALSE, FUNCTION, FOR, IF, NUL, OR, XOR, NOR, XNOR, NAND, RETURN, SUPER, THIS, TRUE, VAR, WHILE, GLOBAL, FINAL, USE,
    BREAK, CONTINUE,
    UNRECOGNIZED,
    EOF_
};
enum objtype{
    NUMBER_T, STRING_T, BOOL_T, NULL_T, MAP_T, CALLABLE_T
};
namespace lns{
    const string KEYWORDS_S_VALUES[] = {// Single-character tokens.
            "LEFT_PAREN", "RIGHT_PAREN", "LEFT_BRACE", "RIGHT_BRACE", "LEFT_SQR", "RIGHT_SQR",
            "COMMA", "DOT", "MINUS", "PLUS", "PLUS_PLUS", "MINUS_MINUS", "SEMICOLON","SLASH", "STAR", "HAT",
            // One or two character tokens.
            "BANG", "BANG_EQUAL",
            "EQUAL", "EQUAL_EQUAL",
            "GREATER", "GREATER_EQUAL",
            "LESS", "LESS_EQUAL",

            // Literals.
            "IDENTIFIER", "STRING", "NUMBER",

            // Keywords.
            "NATIVES","AND", "CLASS", "ELSE", "FALSE", "FUNCTION", "FOR", "IF", "NUL", "OR", "XOR", "NOR", "XNOR"
            , "NAND", "RETURN", "SUPER", "THIS", "TRUE", "VAR", "WHILE"
            , "GLOBAL", "FINAL", "USE",
            "BREAK", "CONTINUE",
            "UNRECOGNIZED",
            "EOF"};
    class object{
    private:
        object(){}
    public:
        objtype type;
        object(objtype t) : type(t){}
        virtual string str() = 0;
        //    return *new string("object::plain");
        //};
        virtual bool operator == (const object& o) const = 0;
    };
    class string_o : public object{
    public:
        string value;
        string_o(string value) : object(objtype::STRING_T), value(value){}
        virtual string str() {
            return value;
        }
        virtual bool operator == (const object& o) const {
            if(o.type != STRING_T) return false;
            return (value == dynamic_cast<string_o*>(const_cast<object*>(&o))->value);
        }
    };
    class number_o : public object{
    public:
        double value;
        number_o(double value) : object(objtype::NUMBER_T),value(value){}
        virtual string str(){
            stringstream ss;
            ss << value;
            return *new string(ss.str());
        }
        bool operator==(const object &o) const override {
            if(o.type != NUMBER_T) return false;
            return value == dynamic_cast<number_o*>(const_cast<object*>(&o))->value;
        }
    };
    class bool_o : public object{
    public:
        bool value;
        bool_o(bool value) : object(objtype::BOOL_T), value(value){}
        virtual string str(){
            return value ? "true" : "false";
        }
        bool operator==(const object &o) const override {
            if(o.type != BOOL_T) return false;
            return value == dynamic_cast<bool_o*>(const_cast<object*>(&o))->value;
        }
    };
    class null_o : public object{
    public:
        null_o() : object(NULL_T){}
        string str() {
            return "null";
        }
        bool operator==(const object &o) const override {
            if(o.type == NULL_T) return true;
            return false;
        }
    };
    struct o_o_map_cprt{
        bool operator()(const object* a, const object* b) const {
            return ((*a).operator==(*b));
        }
    };
    class map_o : public object{
    private:
    public:
        map_o() : object(MAP_T){}
        map<string,object*> values;

        bool operator==(const object &o) const override;

        string str() override{
            return "<map>"; //TODO: implement JSON map conversion
        }
        const bool contains_key(string& s){
            return values.find(s) != values.end();
        }
    };

    bool map_o::operator==(const object &o) const {
        if(o.type != MAP_T) return false;
        return values == dynamic_cast<map_o*>(const_cast<object*>(&o))->values;
    }

    class token{
    public:
        token(token_type type,string lexeme, object& literal,const char* filename, int line) : type(type),
                                                                                                    lexeme(lexeme),
                                                                                                    literal(literal),
                                                                                                    filename(filename),
                                                                                                    line(line) {}

        token_type  type;
        string lexeme;
        object& literal;
        const char* filename;
        int line;

        string format()const {
            stringstream ss;
            ss << "type: " << KEYWORDS_S_VALUES[type] << ", lexeme: " << lexeme << ", literal: |";
            string str = literal.str();
            ss << str;
            ss << "|, file: \"" << filename << "\", line:" << line;
            return *new string(ss.str());
        }
        //void operator delete(void*){}
    };
    class stack_call{
    public:
        const char* filename;
        const int line;
        const string& method;
        stack_call(const char* filename,const int line,const string& method) : filename(filename), line(line), method(method){}
    };
    class variable{
    public:
        const bool isfinal;
        object* value;
        explicit variable() : value(new null_o()) , isfinal(false){}
        variable(const bool isfinal, object *value) : isfinal(isfinal), value(value) {}
        const operator= (variable& v) {
            this->value = v.value;
        }
    };
    class return_signal : public exception{
    public:
        object* value;
        const token& keyword;
        return_signal(object* value, const token& keyword) : value(value), keyword(keyword){}
        const char *what() const throw() {
            return exception::what();
        }
    };
    class break_signal : public exception{
    public:
        const token& keyword;

        break_signal(const token &keyword) : keyword(keyword) {}
        const char *what() const throw() {
            return exception::what();
        }
    };
    class continue_signal : public exception{
    public:
        const token& keyword;

        const char *what() const throw() {
            return exception::what();
        }

        continue_signal(const token &keyword) : keyword(keyword) {}
    };
    static object* GET_DEFAULT_NULL(){
        return new null_o();
    }
}

#endif //CPPLNS_DEFS_H