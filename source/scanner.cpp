//
// Created by lorenzo on 11/30/17.
//

#include "scanner.h"
#include "errors.h"

using namespace lns;
token_type lns::get_token_type(string &key) {
    if (key == "and") return AND;
    if (key == "class") return CLASS;
    if (key == "else") return ELSE;
    if (key == "false") return FALSE;
    if (key == "for") return FOR;
    if (key == "foreach") return FOREACH;
    if (key == "function") return FUNCTION;
    if (key == "if") return IF;
    if (key == "null") return NUL;
    if (key == "or") return OR;
    if (key == "nor") return NOR;
    if (key == "xor") return XOR;
    if (key == "nand") return NAND;
    if (key == "not") return NOT;
    if (key == "return") return RETURN;
    if (key == "super") return SUPER;
    if (key == "this") return THIS;
    if (key == "true") return TRUE;
    if (key == "var") return VAR;
    if (key == "while") return WHILE;
    if (key == "natives") return NATIVES;
    if (key == "use") return USE;
    if (key == "global") return GLOBAL;
    if (key == "local") return LOCAL;
    if (key == "final") return FINAL;
    if (key == "break") return BREAK;
    if (key == "continue") return CONTINUE;
    if (key == "context") return CONTEXT;
    if (key == "end") return END;
    if (key == "begin") return BEGIN;
    if (key == "then") return THEN;
    if (key == "do") return DO;
    if (key == "bind") return BIND;
    if (key == "dpcheck") return DPCHECK;
    if (key == "in") return IN;
    if (key == "exception") return EXCEPTION;
    if (key == "with") return WITH;
    if (key == "raise") return RAISE;
    if (key == "handle") return HANDLE;
    if (key == "constructor") return CONSTRUCTOR;
    return UNRECOGNIZED;
}

scanner::scanner(const char *filename, std::string &source) : source(source), start(0), current(0), line(1) {
    this->filename = filename;
    errors::had_parse_error = false;
}

void scanner::add_eof() {
    token *eof = new token(EOF_, "", nullptr, filename, line + 1);
    tokens.push_back(eof);
}

void scanner::reset(const char *filename, std::string &source) {
    this->tokens.clear();
    this->filename = filename;
    this->source = source;
    start = 0;
    current = 0;
    line = 1;
}

vector<token *> &scanner::scan_tokens(bool addEOF) {
    while (!is_at_end()) {
        start = current;
        scan_token();
    }

    if (addEOF) add_eof();
    return tokens;
}

bool scanner::is_at_end() {
    return current >= source.size();
}

char scanner::peek_next() {
    if (current + 1 < source.length()) return source.at(current + 1);
    return '\0';
}

void scanner::number() {
    while (isdigit(peek())) advance();
    if (peek() == '.' && isdigit(peek_next())) {
        advance();
        while (isdigit(peek())) advance();
    }
    token_type type = NUMBER;
    string lexeme = source.substr(start, (current) - (start));
    double value;
    stringstream ss(lexeme);
    ss >> value;
    number_o *n = new number_o(value);
    add_token(type, n);
}

void scanner::identifier() {
    while (isalnum(peek()) || peek() == '_') advance();
    string text = source.substr(start, (current) - (start));
    token_type type = get_token_type(text);
    if (type == UNRECOGNIZED) type = IDENTIFIER;
    add_token(type);
}

void scanner::line_comment() {
    while (peek() != '\n' && !is_at_end()) advance();
}

void scanner::block_comment() {
    advance();
    while (!is_at_end()) {
        if (advance() == '*') if (match('/')) return;
    }
    errors::parse_error(filename, line, UNTERMINATED_BLOCK_COMMENT);
}

void scanner::scan_token() {
    char c = advance();
    switch (c) {
        case '(':
            add_token(LEFT_PAREN);
            break;
        case ')':
            add_token(RIGHT_PAREN);
            break;
            //case '{': add_token(LEFT_BRACE); break;
            //case '}': add_token(RIGHT_BRACE); break;
        case '[':
            add_token(LEFT_SQR);
            break;
        case ']':
            add_token(RIGHT_SQR);
            break;
        case ',':
            add_token(COMMA);
            break;
        case '.':
            add_token(DOT);
            break;
        case '-':
            add_token(match('-') ? MINUS_MINUS : (match('=') ? MINUS_EQUALS : MINUS));
            break;
        case '^':
            add_token(HAT);
            break;
        case '+':
            add_token(match('+') ? PLUS_PLUS : (match('=') ? PLUS_EQUALS : PLUS));
            break;
        case '*':
            add_token(match('=') ? STAR_EQUALS : STAR);
            break;
        case '!':
            add_token(match('=') ? BANG_EQUAL : BANG);
            break;
        case '{':
            add_token(LEFT_BRACE);
            break;
        case '}':
            add_token(RIGHT_BRACE);
            break;
        case ':':
            add_token(COLON);
            break;
        case '=':
            add_token(match('=') ? EQUAL_EQUAL : EQUAL);
            break;
        case '<':
            add_token(match('=') ? LESS_EQUAL : LESS);
            break;
        case '>':
            add_token(match('=') ? GREATER_EQUAL : GREATER);
            break;
        case '?':
            add_token(QUESTION_MARK);
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
            } else if (match('*')) {
                block_comment();
            } else if (match('=')) {
                add_token(SLASH_EQUALS);
            } else {
                add_token(SLASH);
            }
            break;
        case '#':
            line_comment();
            break;
        case '"':
            string_();
            break;
        default:
            if (isdigit(c) != 0) {
                number();
            } else if (isalpha(c) != 0 || c == '_') {
                identifier();
            } else {
                errors::parse_error(filename, line,
                                    (UNEXPECTED_CHARACTER(CHARSTR(c))));
            }
    }
}

void scanner::add_token(token_type type, object *f) {
    std::string text = source.substr(start, current - start);
    token *t = new token(type, text, f, filename, line);
    tokens.push_back(t);
}

void scanner::add_token(token_type type) {
    add_token(type, nullptr);
}

bool scanner::match(char expected) {
    if (is_at_end()) return false;
    if (source.at(current) != expected) return false;
    current++;
    return true;
}

char scanner::peek() {
    if (is_at_end()) return '\0';
    return source.at(current);
}

void scanner::string_() {
    string& s = *new string();
    while (true) {
        if (is_at_end()) break;
        if (peek() == '\"') break;
        if (peek() == '\n') line++;
        if (peek() == '\\') {
            advance();
            char c;
            switch (c = peek()) {
                case 'n':
                    s += '\n';
                    break;
                case 'r':
                    s += '\r';
                case 't':
                    s += '\t';
                    break;
                case 'b':
                    s += '\b';
                    break;
                case 'f':
                    s += '\f';
                    break;
                case 'v':
                    s += '\v';
                    break;
                case '\\':
                    s += '\\';
                    break;
                case '\'':
                    s += '\'';
                    break;
                case '\"':
                    s += '\"';
                    break;
                default:
                    errors::parse_error(filename, line, INVALID_ESCAPE_CHARACTER(CHARSTR(c)));
                    break;
            }
            advance();
        } else {
            s += advance();
        }
    }
    if (is_at_end()) {
        errors::parse_error(filename, line, UNTERMINATED("string"));
        return;
    }
    advance();
    add_token(STRING, new string_o(s));
}


bool scanner::prmatch(char c) {
    if (current == 0) return false;
    if (source.length() == 1) return false;
    return source.at(current - 1) == c;
}
