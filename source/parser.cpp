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
            case BEGIN:
            case EXCEPTION:
            case RAISE:
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
    throw error(name, "'use' statements are not allowed at this location");
}

stmt *parser::use() {
    bool isnatives = false;
    if (!use_allowed)use_not_allowed();
    if (match({NATIVES})) isnatives = true;
    token &t = consume(STRING, "expected filename");
    string &s = dynamic_cast<string_o *>(&t.literal)->value;
    if (isnatives) {
        consume(BIND, "expected 'bind' after filename");
        return new uses_native_stmt(t.filename, t.line, t, s, consume(IDENTIFIER, "expected identifier after 'bind'"));
    }
    ld_stmts(s);
    return nullptr;
}

vector<stmt *> &parser::context_block() {
    int line = previous().line;
    stmt *s;
    vector<stmt *> &stmts = *new vector<stmt *>();
    while (!check(END) && !is_at_end()) {
        s = declaration();
        if (s->type == CONTEXT_STMT_T || s->type == VAR_STMT_T || s->type == FUNCTION_STMT_T || s->type == EXCEPTION_DECL_STMT_T) {
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
    if (check(STRING)) {
        token &s = advance();
        return check_file(s.literal.str().c_str());
    } else if (check(THIS)) {
        token &s = advance();
        return check_file(s.filename);
    } else throw error(previous(), "expected string or 'this' after dpcheck");
}

stmt *parser::declaration() {
    try {
        if (match({USE})) return use();
        if (match({DPCHECK})) {
            if (dpcheck()) throw SIG_EXIT_PARSER;
            return nullptr;
        }
        use_allowed = false;
        bool is_global = false, is_final = false;
        while (match({GLOBAL, FINAL})) {
            is_global = previous().type == GLOBAL;
            is_final = previous().type == FINAL;
        }
        if (match({EXCEPTION})){
            if (is_final) if (!permissive) throw error(peek(),"keyword 'final' is not allowed for " "exception declaration" " (--permissive)");
            return exception_(is_global);
        }
        if (match({CONTEXT})) return context_declaration(is_global, is_final);
        if (match({VAR})) return var_declaration(is_global, is_final);
        if (match({FUNCTION})) {
            if (is_final) if (!permissive) throw error(peek(),"keyword 'final' is not allowed for " "functions" " (--permissive)");
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
    return PARSE_ERROR;
}

var_stmt *parser::var_declaration(bool is_global, bool is_final) {
    token &name = consume(IDENTIFIER, "expected identifier in variable declaration");
    expr *initializer = new null_expr(name.filename, name.line, name);
    if (match({EQUAL})) {
        delete initializer;
        initializer = expression(true);
    }
    //consume(SEMICOLON, "expected ';' after variable declaration");
    return new var_stmt(name.filename, name.line, name, *initializer, is_global, is_final);
}

stmt *parser::statement() {
    if (match({IF})) return if_statement();
    if (match({RETURN})) return return_statement();
    if (match({BREAK})) return break_statement();
    if (match({CONTINUE})) return continue_statement();
    if (match({RAISE})) return raise();
    if (match({WHILE})) return while_statement();
    if (match({FOR})) return for_statement();
    if (match({FOREACH})) return foreach_statement();
    if (match({HANDLE})) throw error(previous(), "'handle' without 'begin' statement");
    if (match({ELSE})) throw error(previous(), "'else' without 'if' statement");
    if (match({BEGIN})) return begin_handle_statement();
    return expression_statement();
}

stmt *parser::raise() {
    token &raise = previous();
    expr* name = expression(true);
    if (match({WITH})) {
        map<string, expr *> &v = *new map<string, expr *>();
        do {
            token &vname = consume(IDENTIFIER, "expected exception field identifier");
            consume(EQUAL, "expected '=' after exception field identifier");
            expr *val = expression(true);
            v[vname.lexeme] = val;
        } while (match({COMMA}));
        return new raise_stmt(name->file, name->line, raise, name, v);
    }
    return new raise_stmt(name->file, name->line, raise, name, *new map<string, expr *>());
}


stmt *parser::return_statement() {
    token &keyword = previous();
    expr *value = new null_expr(keyword.filename, keyword.line, previous());
    if (!is_at_end())
        if (peek().line == keyword.line)
            value = expression(true);
    return new return_stmt(keyword.filename, keyword.line, keyword, *value);
}

stmt *parser::break_statement() {
    token &t = previous();
    return new break_stmt(t.filename, t.line, t);
}

stmt *parser::continue_statement() {
    token &t = previous();
    return new continue_stmt(t.filename, t.line, t);
}

stmt *parser::if_statement() {
    token keyword = previous();
    expr *condition = expression(true);
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
    expr *condition = expression(true);
    token &d = consume(DO, "expected 'do' after loop condition");
    stmt *body;
    try {
        body = new block_stmt(d.filename, d.line, stmts_until({END}));
    } catch (int) {
        throw error(previous(), EXPTOCLOSE(if statement, d.line).c_str());
    }
    return new s_while_stmt(condition->file, condition->line, *condition, body);
}


stmt *parser::foreach_statement() {
    token &init = consume(IDENTIFIER, "expected identifier after 'foreach' keyword");
    consume(IN, "expected 'in' after identifier");
    expr *container = expression(true);
    token &p = consume(DO, "expected 'do'");
    try {
        return new s_for_each_stmt(p.filename, p.line, init, container,
                                   new block_stmt(p.filename, p.line, stmts_until({END})));
    } catch (int) {
        throw error(previous(), EXPTOCLOSE(foreach
                                                   statement, p.line).c_str());
    }
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
        condition = expression(true);
    }
    consume(COMMA, "expected ',' after loop condition");
    expr *increment = nullptr;
    if (!check(DO)) {
        increment = expression(true);
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
    expr *expr = expression(false);
    //consume(SEMICOLON, "expected ';' after expression");
    return new expression_stmt(expr->file, expr->line, *expr);
}

stmt *parser::function(bool isglobal) {
    token &name = consume(IDENTIFIER, "expected function name");
    consume(LEFT_PAREN, "expected '(' after function name");
    vector<token> &parameters = *new vector<token>();
    if (!check(RIGHT_PAREN)) {
        do {
            token &t = consume(IDENTIFIER, "expected parameter name");
            parameters.push_back(t);
        } while (match({COMMA}));
    }
    consume(RIGHT_PAREN, "expected ')' after parameter list");
    try {
        vector<stmt *> &body = stmts_until({END});
        return new function_stmt(name.filename, name.line, name, parameters, body, isglobal);
    } catch (int) {
        int l = name.line;
        throw error(previous(), EXPTOCLOSE(function, l).c_str());
    }
}

expr *parser::array() {
    token &opening = previous();
    vector<pair<expr *, expr *>> &pairs = *new vector<pair<expr *, expr *>>();
    int i = 0;
    expr *e1, *e2;
    while (!is_at_end()) {
        e1 = expression(true);
        if (match({COLON})) {
            e2 = expression(true);
            pairs.push_back(*new pair<expr *, expr *>(e1, e2));
        } else {
            e2 = new literal_expr(e1->file, e1->line, new number_o(i));
            pairs.push_back(*new pair<expr *, expr *>(e2, e1));
        }
        ++i;
        if (is_at_end()) break;
        if (match({COMMA}))
            continue;
        else if (peek().type == RIGHT_BRACE)
            break;
        else
            throw error(peek(), "expected ',' or '}' after array expression");
    }
    consume(RIGHT_BRACE, EXPTOCLOSE(array
                                            literal, opening.line).c_str());
    return new array_expr(opening.filename, opening.line, opening, pairs);
}

expr *parser::assignment(bool nested) {
    variable_expr *var;
    sub_script_expr *map;
    member_expr *context;
    expr *expr = logical(nested), *value, *key;
    if (match({EQUAL, PLUS_EQUALS, MINUS_EQUALS, STAR_EQUALS, SLASH_EQUALS})) {
        token &op = previous();
        value = assignment(true);
        if ((var = dynamic_cast<variable_expr *>(expr)) != nullptr) {
            token &name = const_cast<token &>(var->name);
            return new assign_expr(var->file, var->line, name, op.type, value);
        }
        if ((map = dynamic_cast<sub_script_expr *>(expr)) != nullptr) {
            return new sub_script_assign_expr(map->file, map->line, const_cast<token &>(map->where), map->name, op.type,
                                              map->key, value);
        }
        if ((context = dynamic_cast<member_expr *>(expr)) != nullptr) {
            return new member_assign_expr(context->file, context->line,
                                           context->container_name, op.type,
                                           const_cast<token &>(context->member_identifier), value);
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
        error(mm, "Invalid decrement target");
    }
    return expr;
}

expr *parser::comparison(bool nested) {
    expr *expr = addition(nested), *right;
    while (match({BANG_EQUAL, EQUAL_EQUAL, GREATER, GREATER_EQUAL, LESS, LESS_EQUAL})) {
        token &op = previous();
        right = addition(true);
        expr = new binary_expr(op.filename, op.line, expr, op, right);
    }
    return expr;
}

expr *parser::addition(bool nested) {
    expr *expr = multiplication(nested), *right;
    while (match({MINUS, PLUS})) {
        token &op = previous();
        right = multiplication(true);
        expr = new binary_expr(op.filename, op.line, expr, op, right);
    }
    return expr;
}

expr *parser::multiplication(bool nested) {
    expr *expr = power(nested), *right;
    while (match({SLASH, STAR})) {
        token &op = previous();
        right = power(true);
        expr = new binary_expr(op.filename, op.line, expr, op, right);
    }
    return expr;
}

expr *parser::power(bool nested) {
    expr *expr = unary(nested), *right;
    while (match({HAT})) {
        token &op = previous();
        right = unary(true);
        expr = new binary_expr(op.filename, op.line, expr, op, right);
    }
    return expr;
}

expr *parser::unary(bool nested) {
    if (match({NOT, MINUS})) {
        token &op = previous();
        expr *right = unary(true);
        return new unary_expr(op.filename, op.line, op, right);
    }
    return call(nested);
}

expr *parser::call(bool nested) {
    vector<expr *> &args = *new vector<expr *>();
    expr *expr = special_assignment(nested), *keyexpr;
    token &identifier = previous();
    while (true)
        if (match({LEFT_PAREN})) {
            if (!check(RIGHT_PAREN)) {
                do {
                    args.push_back(expression(true));
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
            args.push_back(expression(true));
        } while (match({COMMA}));
    }
    token paren = consume(RIGHT_PAREN, "expected ')' after arguments");
    call_expr *expr = new call_expr(paren.filename, paren.line, callee, paren, args);
    return expr;
}

expr *parser::special_assignment(bool nested) {
    expr *expr = primary(nested), *key;
    while (match({DOT, LEFT_SQR})) {
        token &op = previous();
        if (op.type == DOT) {
            token &fname = consume(IDENTIFIER, "expected field name");
            expr = new member_expr(expr->file, expr->line, expr, fname);
        } else {
            key = expression(true);
            consume(RIGHT_SQR, "expected closing ']' after key expression");
            expr = new sub_script_expr(expr->file, expr->line, op, expr, key);
        }
    }
    return expr;
}

expr *parser::primary(bool nested) {
    if (match({FALSE})) return new literal_expr(previous().filename, previous().line, new bool_o(false));
    if (match({TRUE})) return new literal_expr(previous().filename, previous().line, new bool_o(true));
    if (match({NUL})) return new literal_expr(previous().filename, previous().line, new null_o());
    if (match({NUMBER, STRING})) return new literal_expr(previous().filename, previous().line, &previous().literal);
    if (match({IDENTIFIER})) {
        token &identifier = previous();
        return new variable_expr(identifier.filename, identifier.line, identifier);
    }
    if (match({LEFT_PAREN})) {
        expr *expr = expression(true);
        consume(RIGHT_PAREN, "missing closing ')'");
        return new grouping_expr(expr->file, expr->line, expr);
    }
    throw error(peek(), nested ? "expected expression" : "invalid statement");
}

parser::parser(vector<token *> &tokens) : tokens(tokens), start(0), current(0), use_allowed(true),
                                          statements(*new vector<stmt *>()) {}

vector<stmt *> &parser::parse(bool ld_std) {
    stmt *s;
    if (ld_std) ld_stmts(*new string("std"));
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

expr *parser::expression(bool nested) {
    if (match({LEFT_BRACE})) return array();
    return assignment(nested);
}

expr *parser::logical(bool nested) {
    expr *expr = comparison(nested), *right;
    while (match({AND, OR, NOR, XOR, NAND})) {
        token &op = previous();
        right = comparison(true);
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

void parser::ld_stmts(string &s) {
    const char *filename = best_file_path(s.c_str());
    ifstream file(filename);
    string source;
    stringstream ss;
    if (!file.is_open()) {
        throw error(previous(), "use: file was not found or is unaccessible");
    }
    ss << file.rdbuf();
    source = ss.str();
    scanner scanner = *new lns::scanner(filename, source);
    vector<token *> &tkns = scanner.scan_tokens(true);
    parser p(tkns);
    vector<stmt *> &toadd = p.parse(false);
    int i = 0;
    for (; i < toadd.size(); i++) {
        statements.push_back(toadd[i]);
    }
}

stmt *parser::exception_(bool is_global) {
    token &name = consume(IDENTIFIER, "expected exception name");
    if (match({WITH})) {
        set<string> &tokens = *new set<string>();
        do {
            token &t = consume(IDENTIFIER, "expected identifier(s)");
            tokens.insert(t.lexeme);
        } while (match({COMMA}));

        return new exception_decl_stmt(name.filename, name.line, name, tokens,is_global);
    }
    return new exception_decl_stmt(name.filename, name.line, name, *new set<string>(), is_global);
}

stmt *parser::begin_handle_statement() {
    token &keyword = previous();
    vector<stmt *> &stmts = stmts_until({HANDLE});
    vector<handle_stmt *> handles = *new vector<handle_stmt *>();
    int last_line = 0;
    do {
        if (peek().type == END) break;
        expr *e_iden = expression(true);
        token *name = new token(UNRECOGNIZED, string(), *new null_o(), e_iden->file, e_iden->line);
        if (match({BIND}))
            name = &consume(IDENTIFIER, "expected variable name after 'bind'");
        vector<stmt *>& hstmts = *new vector<stmt *>();
        while (!(check(HANDLE) || check(END))) {
            hstmts.push_back(declaration());
        }
        handles.push_back(new handle_stmt(name->filename, name->line, e_iden, *name, hstmts));
    } while (match({HANDLE}) || check(END));
    consume(END, EXPTOCLOSE(handle, last_line).c_str());
    return new begin_handle_stmt(keyword.filename, keyword.line, stmts, handles);
}


