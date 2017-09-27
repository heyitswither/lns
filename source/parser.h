//
// Created by lorenzo on 04/09/17.
//

#ifndef CPPLNS_PARSER_H
#define CPPLNS_PARSER_H

#include <string>
#include <vector>
#include "defs.h"
#include "expr.h"
#include "stmt.h"
#include "options.h"
#include "scanner.h"
#include <initializer_list>

#define PARSE_ERROR (-1)
using namespace std;
using namespace lns;
namespace lns {
    class parser {
    private:
        int start;
        int current;
        bool use_allowed = true;
        vector<token> &tokens;
        vector<stmt *> &statements;

        bool match(initializer_list<token_type> types) {
            for (token_type t : types) {
                if (check(t)) {
                    advance();
                    return true;
                }
            }
            return false;
        }

        token &advance() {
            if (!is_at_end()) current++;
            return previous();
        }

        token &previous() {
            return tokens.at(current - 1);
        }

        bool check(token_type tokenType) {
            if (is_at_end()) return false;
            return peek().type == tokenType;
        }

        bool is_at_end() {
            return peek().type == EOF_;
        }

        token &peek() {
            return tokens[current];
        }

        void synchronize() {
            advance();
            while (!is_at_end()) {
                if (previous().type == SEMICOLON) return;
                switch (peek().type) {
                    case CLASS:
                    case FUNCTION:
                    case VAR:
                    case FOR:
                    case IF:
                    case WHILE:
                    case RETURN:
                    case CONTEXT:
                        return;
                    default:
                        break;
                }
                advance();
            }
        }

        token &consume(token_type type, const char message[]) {
            if (check({type})) return advance();
            throw error(peek(), message);
        }

        void use_not_allowed() {
            token &name = previous();
            throw error(name, "'use' statements are not allowed at this location.");
        }

        stmt *use() {
            bool isnatives = false;
            if (!use_allowed)use_not_allowed();
            if (match({NATIVES})) isnatives = true;
            token &t = consume(STRING, "expected string inside use statement");
            string &s = dynamic_cast<string_o *>(&t.literal)->value;
            consume(SEMICOLON, "expected semicolon after use statement");
            if (isnatives) return new uses_native_stmt(t, s);
            ifstream file(s);
            string source;
            stringstream ss;
            if (!file.is_open()) {
                error(previous(), "use: file was not found or is unaccessible");
                return nullptr;
            }
            ss << file.rdbuf();
            source = ss.str();
            scanner scanner = *new lns::scanner(s.c_str(), source);
            vector<token> tkns = scanner.scan_tokens(true);
            parser p(tkns);
            vector<stmt *> &toadd = p.parse();
            int i = 0;
            for (; i < toadd.size(); i++) {
                statements.push_back(toadd[i]);
            }
            return nullptr;
        }

        vector<stmt *> &context_block() {
            int line = previous().line;
            stmt* s;
            vector<stmt *> &stmts = *new vector<stmt *>();
            while(!check(RIGHT_BRACE) && !is_at_end()){
                s = declaration();
                if(s->type == CONTEXT_STMT_T || s->type == VAR_STMT_T || s->type == FUNCTION_STMT_T){
                    stmts.push_back(s);
                    continue;
                }
                throw error(previous(),"contexts can only contain declarations");
            }
            string str = *new string("expected '}' to close block (opening brace at line ");
            str += to_string(line);
            str += ")";
            consume(RIGHT_BRACE, str.c_str());
            return stmts;
        }

        vector<stmt *> &block() {
            int line = previous().line;
            vector<stmt *> &stmts = *new vector<stmt *>();
            while (!check(RIGHT_BRACE) && !is_at_end()) {
                stmts.push_back(declaration());
            }
            string str = *new string("expected '}' to close block (opening brace at line ");
            str += to_string(line);
            str += ")";
            consume(RIGHT_BRACE, str.c_str());
            return stmts;
        }

        stmt *context_declaration(bool global, bool final) {
            token& name = consume(IDENTIFIER,"expected context name");
            token& open = consume(LEFT_BRACE,"expected opening brace");
            vector<stmt*>& stmts = context_block();
            return new context_stmt(name,stmts,global,final);
        }

        stmt *declaration() {
            try {
                if (match({USE})) {
                    return use();
                }
                use_allowed = false;
                bool is_global = false, is_final = false;
                while (match({GLOBAL, FINAL})) {
                    is_global = previous().type == GLOBAL;
                    is_final = previous().type == FINAL;
                }
                if(match({CONTEXT})) return context_declaration(is_global,is_final);
                if (match({VAR})) return var_declaration(is_global, is_final);
                if (match({FUNCTION})) {
                    if (is_final) {
                        if (!permissive)
                            throw error(peek(),
                                        "functions are unchangeable by definition: keyword 'final' is redundant (--permissive).");
                    }
                    return function(is_global);
                }
                return statement();
            } catch (int i) {
                synchronize();
                throw i;
            }
        }

        int error(token &token, const char *message) {
            errors::parse_error(token.filename, token.line, message);
            return PARSE_ERROR;
        }

        var_stmt *var_declaration(bool is_global, bool is_final) {
            token &name = consume(IDENTIFIER, "expected identifier in variable declaration.");
            expr *initializer = new null_expr(name);
            if (match({EQUAL})) {
                delete initializer;
                initializer = expression();
            }
            consume(SEMICOLON, "expected ';' after variable declaration");
            return new var_stmt(name, *initializer, is_global, is_final);
        }

        stmt *statement() {
            if (match({IF})) return if_statement();
            if (match({RETURN})) return return_statement();
            if (match({BREAK})) return break_statement();
            if (match({CONTINUE})) return continue_statement();
            if (match({WHILE})) return while_statement();
            if (match({FOR}))
                return for_statement();
            if (match({LEFT_BRACE})) return new block_stmt(block());
            return expression_statement();
        }

        stmt *return_statement() {
            token &keyword = previous();
            expr *value = new null_expr(previous());
            if (!check(SEMICOLON)) {
                value = expression();
            }
            consume(SEMICOLON, "expected ';' after return value.");
            return new return_stmt(keyword, *value);
        }

        stmt *break_statement() {
            token &t = previous();
            consume(SEMICOLON, "expected ';' after break statement.");
            return new break_stmt(t);
        }

        stmt *continue_statement() {
            token &t = previous();
            consume(SEMICOLON, "expected ';' after continue statement.");
            return new continue_stmt(t);
        }

        stmt *if_statement() {
            consume(LEFT_PAREN, "expected '(' after 'if'.");
            expr *condition = expression();
            token rparen = consume(RIGHT_PAREN, "expected ')' after condition.");
            stmt *then_branch = statement();
            stmt *else_branch = new null_stmt(rparen);
            if (match({ELSE}))
                else_branch = statement();
            return new if_stmt(*condition, then_branch, else_branch);
        }

        stmt *while_statement() {
            consume(LEFT_PAREN, "expected '(' after 'while'");
            expr *condition = expression();
            consume(RIGHT_PAREN, "expected ')' after condition.");
            stmt *body = statement();

            return new s_while_stmt(*condition, body);
        }

        stmt *for_statement() {
            consume(LEFT_PAREN, "expected '(' after 'for'");
            stmt *initializer = nullptr;
            if (!match({SEMICOLON}))
                if (match({VAR}))
                    initializer = var_declaration(false, false);
                else
                    initializer = expression_statement();

            expr *condition = nullptr;
            if (!check({SEMICOLON})) {
                condition = expression();
            }
            consume(SEMICOLON, "expected ';' after loop condition");
            expr *increment = nullptr;
            if (!check(RIGHT_PAREN)) {
                increment = expression();
            }
            consume(RIGHT_PAREN, "expected ')' after for increment");
            stmt *body = statement();
            s_for_stmt *s = new s_for_stmt(initializer,condition,increment,body);
            return s;
        }

        stmt *expression_statement() {
            expr *expr = expression();
            consume(SEMICOLON, "expected ';' after expression");
            return new expression_stmt(*expr);
        }

        stmt *function(bool isglobal) {
            token &name = consume(IDENTIFIER, "expected function name");
            consume(LEFT_PAREN, "expected '(' after function name.");
            vector<token> &parameters = *new vector<token>();
            if (!check(RIGHT_PAREN)) {
                token &t = consume(IDENTIFIER, "expected parameter name");
                do {
                    parameters.push_back(t);
                } while (match({COMMA}));
            }
            consume(RIGHT_PAREN, "expected ')' after parameter list");
            consume(LEFT_BRACE, "expected '{' for function body");
            vector<stmt *> &body = block();
            return new function_stmt(name, parameters, body, isglobal);
        }

        expr *assignment() {
            variable_expr *var;
            map_field_expr *map;
            context_expr* context;
            expr *expr = logical(), *value, *key;
            if (match({EQUAL})) {
                token &equals = previous();
                value = assignment();
                if (!((var = dynamic_cast<variable_expr *>(expr)) == nullptr)) {
                    token &name = const_cast<token &>(var->name);
                    return new assign_expr(name, value);
                }
                if (!((map = dynamic_cast<map_field_expr *>(expr)) == nullptr)) {
                    token &name = const_cast<token &>(map->name);
                    return new assign_map_field_expr(name, map->key, value);
                }
                if (!((context = dynamic_cast<context_expr *>(expr)) == nullptr)) {
                    token &name = const_cast<token &>(context->context_name);
                    return new context_assign_expr(name, const_cast<token &>(context->context_identifier), value);
                }
                throw error(equals, "invalid assignment target.");
            }
            if(match({PLUS_PLUS})){
                token& pp = previous();
                if((var = dynamic_cast<variable_expr*>(expr)) != nullptr){
                    const token& name = var->name;
                    return new increment_expr(const_cast<token &>(name), expr);
                }
                error(pp,"Invalid increment target.");
            }
            if(match({MINUS_MINUS})){
                token& pp = previous();
                if((var = dynamic_cast<variable_expr*>(expr)) != nullptr){
                    const token& name = var->name;
                    return new decrement_expr(const_cast<token &>(name), expr);
                }
                error(pp,"Invalid decrement target.");
            }
            return expr;
        }

        expr *expression() { return assignment(); }

        expr *logical() {
            expr *expr = comparison(), *right;
            while (match({BANG_EQUAL, EQUAL_EQUAL, AND, OR, NOR, XOR, XNOR, NAND})) {
                token &op = previous();
                right = comparison();
                expr = new binary_expr(expr, op, right);
            }
            return expr;
        }

        expr *comparison() {
            expr *expr = addition(), *right;
            while (match({GREATER, GREATER_EQUAL, LESS, LESS_EQUAL})) {
                token &op = previous();
                right = addition();
                expr = new binary_expr(expr, op, right);
            }
            return expr;
        }

        expr *addition() {
            expr *expr = multiplication(), *right;
            while (match({MINUS, PLUS})) {
                token &op = previous();
                right = multiplication();
                expr = new binary_expr(expr, op, right);
            }
            return expr;
        }

        expr *multiplication() {
            expr *expr = power(), *right;
            while (match({SLASH, STAR})) {
                token &op = previous();
                right = power();
                expr = new binary_expr(expr, op, right);
            }
            return expr;
        }

        expr *power() {
            expr *expr = unary(), *right;
            while (match({HAT})) {
                token &op = previous();
                right = unary();
                expr = new binary_expr(expr, op, right);
            }
            return expr;
        }

        expr *unary() {
            if (match({BANG, MINUS})) {
                token &op = previous();
                expr *right = unary();
                return new unary_expr(op, right);
            }
            return call();
        }

        expr *call() {
            vector<expr *> &args = *new vector<expr *>();
            expr *expr = primary(), *keyexpr;
            token &identifier = previous();
            if(match({DOT})){
                token &context_identifier = consume(IDENTIFIER,"expected identifier for context call");
                return new context_expr(identifier,context_identifier);
            }
            if (match({LEFT_SQR})) {
                keyexpr = expression();
                consume(RIGHT_SQR, "expected ']' after key expression");
                return new map_field_expr(identifier, keyexpr);
            }
            while (true)
                if (match({LEFT_PAREN})) {
                    if (!check(RIGHT_PAREN)) {
                        do {
                            args.push_back(expression());
                        } while (match({COMMA}));
                    }
                    token &paren = consume(RIGHT_PAREN, "expected ')' after arguments");
                    expr = new call_expr(expr, paren, args);
                } else break;
            return expr;
        }

        expr *finish_call(expr *callee) {
            vector<expr *> args;
            if (!check(RIGHT_PAREN)) {
                do {
                    args.push_back(expression());
                } while (match({COMMA}));
            }
            token paren = consume(RIGHT_PAREN, "expected ')' after arguments");
            call_expr *expr = new call_expr(callee, paren, args);
            return expr;
        }

        expr *primary() {
            if (match({FALSE})) return new literal_expr(new bool_o(false));
            if (match({TRUE})) return new literal_expr(new bool_o(true));
            if (match({NUL})) return new literal_expr(new null_o());
            if (match({NUMBER, STRING})) return new literal_expr(&previous().literal);
            if (match({IDENTIFIER})) {
                token &identifier = previous();
                return new variable_expr(identifier);
            }
            if (match({LEFT_PAREN})) {
                expr *expr = expression();
                consume(RIGHT_PAREN, "missing closing ')'");
                return new grouping_expr(expr);
            }
            throw error(peek(), "expected expression");
        }

    public:
        explicit parser(vector<token> &tokens) : tokens(tokens), start(0), current(0), use_allowed(true),
                                                 statements(*new vector<stmt *>()) {}

        parser() = delete;

        vector<stmt *> &parse() {
            stmt *s;
            while (!is_at_end()) {
                try {
                    s = declaration();
                    if (s == nullptr) continue;
                    statements.push_back(s);
                } catch (int) {
                    continue;
                }
            }
            return statements;
        }

    };
}
#endif //CPPLNS_PARSER_H
