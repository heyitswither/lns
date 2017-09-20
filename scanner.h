//
// Created by lorenzo on 31/08/17.
//

#ifndef CPPLNS_SCANNER_H
#define CPPLNS_SCANNER_H
#include <string>
#include <vector>
#include <map>
#include "defs.h"
using namespace std;
namespace lns{
    token_type get_token_type(string& key) {
        if(key == "and") return AND;
        if(key == "class") return CLASS;
        if(key == "else") return ELSE;
        if(key == "false") return FALSE;
        if(key == "for") return FOR;
        if(key == "function") return FUNCTION;
        if(key == "if") return IF;
        if(key == "null") return NUL;
        if(key == "or") return OR;
        if(key == "nor") return NOR;
        if(key == "xor") return XOR;
        if(key == "nand") return NAND;
        if(key == "xnor") return XNOR;
        if(key == "return") return RETURN;
        if(key == "super") return SUPER;
        if(key == "this") return THIS;
        if(key == "true") return TRUE;
        if(key == "var") return VAR;
        if(key == "while") return WHILE;
        if(key == "natives") return NATIVES;
        if(key == "use") return USE;
        if(key == "global") return GLOBAL;
        if(key == "final") return FINAL;
        if(key == "break") return BREAK;
        if(key == "continue") return CONTINUE;
        return UNRECOGNIZED;
    }
    class scanner{
        std::string source;
        const char* filename;
        vector<token> tokens;
        int start;
        int current;
        int line;
    public:
        scanner(const char* filename,std::string& source) : source(source), start(0), current(0), line(1){
            this->filename = filename;
        }
        explicit scanner() = delete;
        void add_eof() {
            object& o = *new null_o();
            token eof = *new token(EOF_,"",o,filename,line+1);
            tokens.push_back(eof);
        }

        vector<token> scan_tokens(bool addEOF){
            //cout << source << endl;
            while(!is_at_end()){
                start = current;
                scan_token();
            }

            if(addEOF) add_eof();
            return tokens;
        }
    private:
        bool is_at_end(){
            return current >= source.size();
        }

        char peek_next() {
            if(current + 1 < source.length()) return source.at(current + 1);
            return '\0';
        }

        void number() {
            while(isdigit(peek())) advance();
            if(peek() == '.' && isdigit(peek_next())){
                advance();
                while(isdigit(peek())) advance();
            }
            token_type type = NUMBER;
            string lexeme = source.substr(start, (current) - (start));
            double value;
            stringstream ss(lexeme);
            ss >> value;
            number_o& n = *new number_o(value);
            add_token(type,n);
        }
        void identifier() {
            while (isalnum(peek()) || peek() == '_' != 0) advance();
            string text = source.substr(start,(current) - (start));
            token_type type = get_token_type(text);
            if(type == UNRECOGNIZED) type = IDENTIFIER;
            add_token(type);
        }

        void line_comment() {
            while (peek() != '\n' && !is_at_end()) advance();
        }

        void block_comment() {
            advance();
            while(!is_at_end()){
                if(advance() == '*') if(match('/')) return;
            }
            errors::parse_error(filename,line,"unterminated block comment");
        }

        void scan_token(){
            char c = advance();
            switch(c){
                case '(': add_token(LEFT_PAREN); break;
                case ')': add_token(RIGHT_PAREN); break;
                case '{': add_token(LEFT_BRACE); break;
                case '}': add_token(RIGHT_BRACE); break;
                case '[': add_token(LEFT_SQR); break;
                case ']': add_token(RIGHT_SQR); break;
                case ',': add_token(COMMA); break;
                case '.': add_token(DOT); break;
                case '-': add_token(match('-') ? MINUS_MINUS : MINUS); break;
                case '^': add_token(HAT); break;
                case '+': add_token(match('+') ? PLUS_PLUS : PLUS); break;
                case ';': add_token(SEMICOLON); break;
                case '*': add_token(STAR); break;
                case '!': add_token(match('=') ? BANG_EQUAL : BANG); break;
                case '=': add_token(match('=') ? EQUAL_EQUAL : EQUAL); break;
                case '<': add_token(match('=') ? LESS_EQUAL : LESS); break;
                case '>': add_token(match('=') ? GREATER_EQUAL : GREATER); break;
                case ' ':
                case '\r':
                case '\t':
                    break;
                case '\n':
                    line++;
                    break;
                case '/':
                    if (match('/')) {
                        line_comment();
                    }else if(match('*')){
                        block_comment();
                    } else {
                        add_token(SLASH);
                    }
                    break;
                case '#':
                    line_comment();
                    break;
                case '"': string_(); break;
                default:
                    if(isdigit(c) != 0){
                        number();
                    }else if(isalpha(c) != 0){
                        identifier();
                    }else{
                        errors::parse_error(filename,line,(string("unexpected character '") + string(1,c) + string("'")).c_str());
                    }
            }
        }
        char advance(){
            current++;
            return source.at(current -1);
        }
        void add_token(token_type type,object& f){
            std::string text = source.substr(start, current - start);
            token t = *new token(type,text.c_str(),f,filename,line);
            tokens.push_back(t);
        }
        void add_token(token_type type){
            object& obj = *new null_o();
            add_token(type,obj);
        }
        bool match(char expected){
            if(is_at_end()) return false;
            if(source.at(current) != expected) return false;
            current++;
            return true;
        }
        char peek(){
            if(is_at_end()) return '\0';
            return source.at(current);
        }
        void string_(){
            while(true){
                if(peek() == '"'){
                    if(!prmatch('\\')) break;
                }
                if(is_at_end()) break;
                if(peek() == '\n') line++;
                advance();
            }
            if(is_at_end()){
                errors::parse_error(filename,line,"unterminated string");
                return;
            }
            advance();
            string_o& value = *new string_o(source.substr(start + 1, (current  - 1) - (start + 1)));
            add_token(STRING,value);
        }
        bool prmatch(char c){
            if(current == 0) return false;
            if(source.length() == 1) return false;
            return source.at(current-1) == c;
        }
    };

}
#endif //CPPLNS_SCANNER_H
