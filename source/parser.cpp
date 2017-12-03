//
// Created by lorenzo on 11/30/17.
//

#include <cstring>
#include "parser.h"
#include "errors.h"

using namespace lns;

bool parser::match(initializer_list<token_type> types) {
    for (token_type t : types) {
        if (check(t)) {
            advance();
            return true;
        }
    }
    return false;
}

token &parser::advance() {
    if (!is_at_end()) current++;
    return previous();
}

token &parser::previous() {
    return *tokens.at(current - 1);
}

bool parser::check(token_type tokenType) {
    if (is_at_end()) return false;
    return peek().type == tokenType;
}

token &parser::peek() {
    return *tokens[current];
}

void parser::synchronize() {
    advance();
    while (!is_at_end()) {
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

token &parser::consume(token_type type, const char *message) {
    if (check({type})) return advance();
    throw error(peek(), message);
}

void parser::use_not_allowed() {
    token &name = previous();
    throw error(name, "'use' statements are not allowed at this location.");
}

stmt *parser::use() {
    bool isnatives = false;
    if (!use_allowed)use_not_allowed();
    if (match({NATIVES})) isnatives = true;
    token &t = consume(STRING, "expected filename in use statement");
    string &s = dynamic_cast<string_o *>(&t.literal)->value;
    if (isnatives) {
        consume(BIND,"expected 'bind' after filename");
        return new uses_native_stmt(t.filename, t.line, t, s, consume(IDENTIFIER,"expected identifier after 'bind'"));
    }
    ifstream file(best_file_path(s.c_str()));
    string source;
    stringstream ss;
    if (!file.is_open()) {
        throw error(previous(), "use: file was not found or is unaccessible");
        return nullptr;
    }
    ss << file.rdbuf();
    source = ss.str();
    scanner scanner = *new lns::scanner(s.c_str(), source);
    vector<token *> &tkns = scanner.scan_tokens(true);
    parser p(tkns);
    vector<stmt *> &toadd = p.parse();
    int i = 0;
    for (; i < toadd.size(); i++) {
        statements.push_back(toadd[i]);
    }
    return nullptr;
}

vector<stmt *> &parser::context_block() {
    int line = previous().line;
    stmt *s;
    vector<stmt *> &stmts = *new vector<stmt *>();
    while (!check(END) && !is_at_end()) {
        s = declaration();
        if (s->type == CONTEXT_STMT_T || s->type == VAR_STMT_T || s->type == FUNCTION_STMT_T) {
            stmts.push_back(s);
            continue;
        }
        throw error(previous(), "contexts can only contain declarations");
    }
    consume(END, EXPTOCLOSE(context block, line).c_str());
    return stmts;
}

vector<stmt *> &parser::stmts_until(initializer_list<token_type> list) {
    vector<stmt *> &stmts = *new vector<stmt *>();
    bool should_break = false;
    while (!is_at_end()) {
        for (token_type t : list)
            if (check(t)) {
                should_break = true;
                break;
            }
        if (should_break) break;
        stmts.push_back(declaration());
    }
    if (!match(list)) throw -1;
    return stmts;
}

vector<stmt *> &parser::block() {
    int line = previous().line;
    vector<stmt *> &stmts = *new vector<stmt *>();
    while (!check(END) && !is_at_end()) {
        stmts.push_back(declaration());
    }
    consume(END, EXPTOCLOSE(block, line).c_str());
    return stmts;
}

stmt *parser::context_declaration(bool global, bool final) {
    token &name = consume(IDENTIFIER, "expected context name");
    vector<stmt *> &stmts = context_block();
    return new context_stmt(name.filename, name.line, name, stmts, global, final);
}

bool parser::check_file(const char *str) {
    for (const char *file : parsed_files)
        if (std::strcmp(str, file) == 0) return true;
    return false;
}

bool parser::dpcheck() {
    token &s = consume(STRING, "expected string after 'dkcheck' keyword");
    return check_file(s.literal.str().c_str());
}

stmt *parser::declaration() {
    try {
        if (match({USE})) return use();
        if (match({DPCHECK})) {
            if (dpcheck()) throw SIG_EXIT_PARSER;
        }
        use_allowed = false;
        bool is_global = false, is_final = false;
        while (match({GLOBAL, FINAL})) {
            is_global = previous().type == GLOBAL;
            is_final = previous().type == FINAL;
        }
        if (match({CONTEXT})) return context_declaration(is_global, is_final);
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

int parser::error(token &token, const char *message) {
    errors::parse_error(token.filename, token.line, message);
    return PARSE_ERROR_CODE;
}

var_stmt *parser::var_declaration(bool is_global, bool is_final) {
    token &name = consume(IDENTIFIER, "expected identifier in variable declaration.");
    expr *initializer = new null_expr(name.filename, name.line, name);
    if (match({EQUAL})) {
        delete initializer;
        initializer = expression();
    }
    //consume(SEMICOLON, "expected ';' after variable declaration");
    return new var_stmt(name.filename, name.line, name, *initializer, is_global, is_final);
}

stmt *parser::statement() {
    if (match({IF})) return if_statement();
    if (match({RETURN})) return return_statement();
    if (match({BREAK})) return break_statement();
    if (match({CONTINUE})) return continue_statement();
    if (match({WHILE})) return while_statement();
    if (match({FOR}))
        return for_statement();
    if (match({BEGIN})) return new block_stmt(previous().filename, previous().line, block());
    return expression_statement();
}

stmt *parser::return_statement() {
    token &keyword = previous();
    expr *value = new null_expr(keyword.filename, keyword.line, previous());
    if (!is_at_end())
        if (peek().line == keyword.line)
            value = expression();
    //consume(SEMICOLON, "expected ';' after return value.");
    return new return_stmt(keyword.filename, keyword.line, keyword, *value);
}

stmt *parser::break_statement() {
    token &t = previous();
    //consume(SEMICOLON, "expected ';' after break statement.");
    return new break_stmt(t.filename, t.line, t);
}

stmt *parser::continue_statement() {
    token &t = previous();
    //consume(SEMICOLON, "expected ';' after continue statement.");
    return new continue_stmt(t.filename, t.line, t);
}

stmt *parser::if_statement() {
    token keyword = previous();
    expr *condition = expression();
    consume(THEN, "expected 'then' after if condition");
    stmt *then_branch;
    try {
        then_branch = new block_stmt(condition->file, condition->line, stmts_until({ELSE, END}));
    } catch (int) {
        throw error(previous(), EXPTOCLOSE(if statement, keyword.line).c_str());
    }
    stmt *else_branch = new null_stmt(keyword.filename, keyword.line, keyword);
    token &closing = previous();
    if (closing.type == ELSE)
        if (match({IF})) else_branch = if_statement();
        else
            try { else_branch = new block_stmt(previous().filename, previous().line, stmts_until({END})); } catch (
                    int) { throw error(previous(), EXPTOCLOSE(else statement, closing.line).c_str()); }
    return new if_stmt(keyword.filename, keyword.line, *condition, then_branch, else_branch);
}

stmt *parser::while_statement() {
    expr *condition = expression();
    token &d = consume(DO, "expected 'do' after loop condition.");
    stmt *body;
    try {
        body = new block_stmt(d.filename, d.line, stmts_until({END}));
    } catch (int) {
        throw error(previous(), EXPTOCLOSE(if statement, d.line).c_str());
    }
    return new s_while_stmt(condition->file, condition->line, *condition, body);
}

stmt *parser::for_statement() {
    stmt *initializer = nullptr;
    if (!match({COMMA}))
        if (match({VAR}))
            initializer = var_declaration(false, false);
        else
            initializer = expression_statement();
    consume(COMMA, "expected ',' after for loop initializer");
    expr *condition = nullptr;
    if (!check({COMMA})) {
        condition = expression();
    }
    consume(COMMA, "expected ',' after loop condition");
    expr *increment = nullptr;
    if (!check(DO)) {
        increment = expression();
    }
    token p = consume(DO, "expected 'do' after for loop increment");
    stmt *body;
    try {
        body = new block_stmt(p.filename, p.line, stmts_until({END}));
    } catch (int) {
        throw error(previous(), EXPTOCLOSE(for
                                                   statement, p.line).c_str());
    }
    s_for_stmt *s = new s_for_stmt(condition == nullptr ? p.filename : condition->file,
                                   condition == nullptr ? p.line : condition->line, initializer, condition, increment,
                                   body);
    return s;
}

stmt *parser::expression_statement() {
    expr *expr = expression();
    //consume(SEMICOLON, "expected ';' after expression");
    return new expression_stmt(expr->file, expr->line, *expr);
}

stmt *parser::function(bool isglobal) {
    token &name = consume(IDENTIFIER, "expected function name");
    consume(LEFT_PAREN, "expected '(' after function name.");
    vector<token> &parameters = *new vector<token>();
    if (!check(RIGHT_PAREN)) {
        do {
            token &t = consume(IDENTIFIER, "expected parameter name");
            parameters.push_back(t);
        } while (match({COMMA}));
    }
    consume(RIGHT_PAREN, "expected ')' after parameter list");
    //consume(LEFT_BRACE, "expected '{' for function body");
    try {
        vector<stmt *> &body = stmts_until({END});
        return new function_stmt(name.filename, name.line, name, parameters, body, isglobal);
    } catch (int) {
        int l = name.line;
        throw error(previous(), EXPTOCLOSE(function, l).c_str());
    }
}

expr *parser::assignment() {
    variable_expr *var;
    sub_script_expr *map;
    context_expr *context;
    expr *expr = logical(), *value, *key;
    if (match({EQUAL, PLUS_EQUALS, MINUS_EQUALS, STAR_EQUALS, SLASH_EQUALS})) {
        token &op = previous();
        value = assignment();
        if ((var = dynamic_cast<variable_expr *>(expr)) != nullptr) {
            token &name = const_cast<token &>(var->name);
            return new assign_expr(var->file, var->line, name, op.type, value);
        }
        if ((map = dynamic_cast<sub_script_expr *>(expr)) != nullptr) {
            return new assign_map_field_expr(map->file, map->line, const_cast<token &>(map->where), map->name, op.type,
                                             map->key, value);
        }
        if ((context = dynamic_cast<context_expr *>(expr)) != nullptr) {
            return new context_assign_expr(context->file, context->line,
                                           context->context_name, op.type,
                                           const_cast<token &>(context->context_identifier), value);
        }
        throw error(op, "invalid assignment target");
    }
    if (match({PLUS_PLUS})) {
        token &pp = previous();
        if ((var = dynamic_cast<variable_expr *>(expr)) != nullptr) {
            const token &name = var->name;
            return new increment_expr(name.filename, name.line, const_cast<token &>(name), expr);
        }
        error(pp, "invalid increment target");
    }
    if (match({MINUS_MINUS})) {
        token &mm = previous();
        if ((var = dynamic_cast<variable_expr *>(expr)) != nullptr) {
            const token &name = var->name;
            return new decrement_expr(name.filename, name.line, const_cast<token &>(name), expr);
        }
        error(mm, "Invalid decrement target.");
    }
    return expr;
}

expr *parser::comparison() {
    expr *expr = addition(), *right;
    while (match({GREATER, GREATER_EQUAL, LESS, LESS_EQUAL})) {
        token &op = previous();
        right = addition();
        expr = new binary_expr(op.filename, op.line, expr, op, right);
    }
    return expr;
}

expr *parser::addition() {
    expr *expr = multiplication(), *right;
    while (match({MINUS, PLUS})) {
        token &op = previous();
        right = multiplication();
        expr = new binary_expr(op.filename, op.line, expr, op, right);
    }
    return expr;
}

expr *parser::multiplication() {
    expr *expr = power(), *right;
    while (match({SLASH, STAR})) {
        token &op = previous();
        right = power();
        expr = new binary_expr(op.filename, op.line, expr, op, right);
    }
    return expr;
}

expr *parser::power() {
    expr *expr = unary(), *right;
    while (match({HAT})) {
        token &op = previous();
        right = unary();
        expr = new binary_expr(op.filename, op.line, expr, op, right);
    }
    return expr;
}

expr *parser::unary() {
    if (match({NOT, MINUS})) {
        token &op = previous();
        expr *right = unary();
        return new unary_expr(op.filename, op.line, op, right);
    }
    return call();
}

expr *parser::call() {
    vector<expr *> &args = *new vector<expr *>();
    expr *expr = special_assignment(), *keyexpr;
    token &identifier = previous();
    /*if(match({DOT})){
        token &context_identifier = consume(IDENTIFIER,"expected identifier for context call");
        return new context_expr(identifier.filename,identifier.line,identifier,context_identifier);
    }
    if (match({LEFT_SQR})) {
        keyexpr = expression();
        consume(RIGHT_SQR, "expected ']' after key expression");
        return new map_field_expr(identifier.filename,identifier.line,identifier, keyexpr);
    }*/
    while (true)
        if (match({LEFT_PAREN})) {
            if (!check(RIGHT_PAREN)) {
                do {
                    args.push_back(expression());
                } while (match({COMMA}));
            }
            token &paren = consume(RIGHT_PAREN, "expected ')' after arguments");
            expr = new call_expr(paren.filename, paren.line, expr, paren, args);
        } else break;
    return expr;
}

expr *parser::finish_call(expr *callee) {
    vector<expr *> args;
    if (!check(RIGHT_PAREN)) {
        do {
            args.push_back(expression());
        } while (match({COMMA}));
    }
    token paren = consume(RIGHT_PAREN, "expected ')' after arguments");
    call_expr *expr = new call_expr(paren.filename, paren.line, callee, paren, args);
    return expr;
}

expr *parser::special_assignment() {
    expr *expr = primary(), *key;
    while (match({DOT, LEFT_SQR})) {
        token &op = previous();
        if (op.type == DOT) {
            token &fname = consume(IDENTIFIER, "expected field name.");
            expr = new context_expr(expr->file, expr->line, expr, fname);
        } else {
            key = expression();
            consume(RIGHT_SQR, "expected closing ']' after key expression.");
            expr = new sub_script_expr(expr->file, expr->line, op, expr, key);
        }
    }
    return expr;
}

expr *parser::primary() {
    if (match({FALSE})) return new literal_expr(previous().filename, previous().line, new bool_o(false));
    if (match({TRUE})) return new literal_expr(previous().filename, previous().line, new bool_o(true));
    if (match({NUL})) return new literal_expr(previous().filename, previous().line, new null_o());
    if (match({NUMBER, STRING})) return new literal_expr(previous().filename, previous().line, &previous().literal);
    if (match({IDENTIFIER})) {
        token &identifier = previous();
        return new variable_expr(identifier.filename, identifier.line, identifier);
    }
    if (match({LEFT_PAREN})) {
        expr *expr = expression();
        consume(RIGHT_PAREN, "missing closing ')'");
        return new grouping_expr(expr->file, expr->line, expr);
    }
    throw error(peek(), "expected expression");
}

parser::parser(vector<token *> &tokens) : tokens(tokens), start(0), current(0), use_allowed(true),
                                          statements(*new vector<stmt *>()) {}

vector<stmt *> &parser::parse() {
    stmt *s;
    while (!is_at_end()) {
        try {
            s = declaration();
            if (s == nullptr) continue;
            statements.push_back(s);
        } catch (int i) {
            if (i == SIG_EXIT_PARSER) return statements;
            continue;
        }
    }
    parsed_files.push_back(tokens.back()->filename);
    return statements;
}

expr *parser::expression() { return assignment(); }

expr *parser::logical() {
    expr *expr = comparison(), *right;
    while (match({BANG_EQUAL, EQUAL_EQUAL, AND, OR, NOR, XOR, NAND})) {
        token &op = previous();
        right = comparison();
        expr = new binary_expr(op.filename, op.line, expr, op, right);
    }
    return expr;
}

bool parser::is_at_end() {
    return peek().type == EOF_;
}

void parser::reset(vector<token *> tokens) {
    this->tokens.clear();
    for (token *t : tokens) {
        this->tokens.push_back(t);
    }
    this->current = 0;
    this->start = 0;
}

