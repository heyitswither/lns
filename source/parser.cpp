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

token *parser::advance() {
    if (!is_at_end()) current++;
    return previous();
}

token *parser::previous() {
    return tokens.at(current - 1);
}

bool parser::check(token_type tokenType) {
    if (is_at_end()) return false;
    return peek()->type == tokenType;
}

token *parser::peek() {
    return tokens[current];
}

void parser::synchronize() {
    advance();
    while (!is_at_end()) {
        switch (peek()->type) {
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

token *parser::consume(token_type type, const char *message) {
    if (check({type})) return advance();
    throw error(peek(), message);
}

void parser::use_not_allowed() {
    throw error(previous(), USE_NOT_ALLOWED);
}

stmt *parser::use() {
    bool isnatives = false;
    if (!use_allowed)use_not_allowed();
    if (match({NATIVES})) isnatives = true;
    token *t = consume(STRING, EXPECTED("filename"));
    string &s = dynamic_cast<string_o *>(t->literal)->value;
    if (isnatives) {
        consume(BIND, EXPECTED_AFTER("'bind'","filename"));
        return new uses_native_stmt(t->filename, t->line, t, s, consume(IDENTIFIER, EXPECTED_AFTER("identifier","'bind'")));
    }
    ld_stmts(s);
    return nullptr;
}

vector<stmt *> &parser::context_block() {
    int line = previous()->line;
    stmt *s;
    vector<stmt *> &stmts = *new vector<stmt *>();
    while (!check(END) && !is_at_end()) {
        s = declaration();
        if (s->type == CONTEXT_STMT_T || s->type == VAR_STMT_T || s->type == FUNCTION_STMT_T ||
            s->type == EXCEPTION_DECL_STMT_T) {
            stmts.push_back(s);
            continue;
        }
        throw error(previous(), CAN_ONLY_CONTAIN("contexts","declarations"));
    }
    consume(END, EXPTOCLOSE("context block", line));
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
    int line = previous()->line;
    vector<stmt *> &stmts = *new vector<stmt *>();
    while (!check(END) && !is_at_end()) {
        stmts.push_back(declaration());
    }
    consume(END, EXPTOCLOSE("block", line));
    return stmts;
}

stmt *parser::context_declaration(visibility vis, bool final) {
    token *name = consume(IDENTIFIER, EXPECTED("context name"));
    vector<stmt *> &stmts = context_block();
    return new context_stmt(name->filename, name->line, name, stmts, vis, final);
}

bool parser::check_file(const char *str) {
    for (const char *file : parsed_files)
        if (std::strcmp(str, file) == 0) return true;
    return false;
}

bool parser::dpcheck() {
    if (check(STRING)) {
        token *s = advance();
        return check_file(s->literal->str().c_str());
    } else if (check(THIS)) {
        token *s = advance();
        return check_file(s->filename);
    } else throw error(previous(), EXPECTED_AFTER("string or 'this'","dpcheck"));
}

stmt *parser::declaration() {
    try {
        if (match({USE})) return use();
        if (match({DPCHECK})) {
            if (dpcheck()) throw SIG_EXIT_PARSER;
            return nullptr;
        }
        use_allowed = false;
        pair<visibility,bool> specs = get_access_specifiers();
        bool is_final = specs.second;
        visibility  vis = specs.first;
        if (match({EXCEPTION})) {
            CHECK_ACCESS_SPEC_NOT_ALLOWED(is_final,"final","exception declarations")
            return exception_(vis);
        }
        if (match({CONTEXT})) return context_declaration(vis, is_final);
        if (match({VAR})) return var_declaration(vis, is_final);
        if (match({FUNCTION})) {
            CHECK_ACCESS_SPEC_NOT_ALLOWED(is_final,"final","functions")
            return function(vis);
        }
        if(match({CLASS})){
            CHECK_ACCESS_SPEC_NOT_ALLOWED(is_final,"final","functions")
            return class_(vis);
        }
        if(match({CONSTRUCTOR})) throw error(previous(),CONSTRUCTOR_OUTSIDE_OF_CLASS);
        if(specs.first != V_UNSPECIFIED || specs.second) throw error(previous(),INVALID_STATEMENT);
        return statement();
    } catch (int i) {
        synchronize();
        throw i;
    }
}

stmt *parser::class_(visibility vis) {
    token* keyword = previous();
    token* name = consume(IDENTIFIER,EXPECTED("class name"));
    vector<var_stmt*>& variables = *new vector<var_stmt*>();
    vector<constructor_stmt*>& constructors = *new vector<constructor_stmt*>();
    vector<function_stmt*>& methods = *new vector<function_stmt*>();
    while(!(is_at_end() || peek()->type == END)){
        auto specs = get_access_specifiers();
        if(match({VAR})) variables.push_back(var_declaration(specs.first,specs.second));
        else if(match({FUNCTION})) methods.push_back(function(specs.first));
        else if(match({CONSTRUCTOR})) constructors.push_back(constructor(specs.first));
        else throw error(peek(),CAN_ONLY_CONTAIN("classes","variables, methods and constructors"));
    }
    consume(END,EXPTOCLOSE("class declaration",keyword->line));
    return new class_decl_stmt(keyword->filename,keyword->line,name,methods,constructors,variables,vis);
}

int parser::error(token *token, const char *message) {
    errors::parse_error(token->filename, token->line, message);
    return PARSE_ERROR;
}

var_stmt *parser::var_declaration(visibility  vis, bool is_final) {
    token *name = consume(IDENTIFIER, EXPECTED_IN("identifier","variable declaration"));
    expr *initializer = new null_expr(name->filename, name->line, name);
    if (match({EQUAL})) {
        delete initializer;
        initializer = expression(true);
    }
    //consume(SEMICOLON, "expected ';' after variable declaration");
    return new var_stmt(name->filename, name->line, name, *initializer, vis, is_final);
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
    if (match({HANDLE})) throw error(previous(), WITHOUT("'handle'","'begin' statement"));
    if (match({ELSE})) throw error(previous(), WITHOUT("'else'","'if' statement"));
    if (match({BEGIN})) return begin_handle_statement();
    return expression_statement();
}

stmt *parser::raise() {
    token *raise = previous();
    expr *name = expression(true);
    if (match({WITH})) {
        map<string, expr *> &v = *new map<string, expr *>();
        do {
            token *vname = consume(IDENTIFIER, EXPECTED("exception field identifier"));
            consume(EQUAL, EXPECTED_AFTER("'='","exception field identifier"));
            expr *val = expression(true);
            v[vname->lexeme] = val;
        } while (match({COMMA}));
        return new raise_stmt(name->file, name->line, raise, name, v);
    }
    return new raise_stmt(name->file, name->line, raise, name, *new map<string, expr *>());
}


stmt *parser::return_statement() {
    token *keyword = previous();
    expr *value = new null_expr(keyword->filename, keyword->line, previous());
    if (!is_at_end())
        if (peek()->line == keyword->line)
            value = expression(true);
    return new return_stmt(keyword->filename, keyword->line, keyword, *value);
}

stmt *parser::break_statement() {
    token *t = previous();
    return new break_stmt(t->filename, t->line, t);
}

stmt *parser::continue_statement() {
    token *t = previous();
    return new continue_stmt(t->filename, t->line, t);
}

stmt *parser::if_statement() {
    token *keyword = previous();
    expr *condition = expression(true);
    consume(THEN, EXPECTED_AFTER("'then'","if condition"));
    stmt *then_branch;
    try {
        then_branch = new block_stmt(condition->file, condition->line, stmts_until({ELSE, END}));
    } catch (int) {
        throw error(previous(), EXPTOCLOSE("if statement", keyword->line));
    }
    stmt *else_branch = new null_stmt(keyword->filename, keyword->line, keyword);
    token *closing = previous();
    if (closing->type == ELSE)
        if (match({IF})) else_branch = if_statement();
        else
            try { else_branch = new block_stmt(previous()->filename, previous()->line, stmts_until({END})); } catch (
                    int) { throw error(previous(), EXPTOCLOSE("else statement", closing->line)); }
    return new if_stmt(keyword->filename, keyword->line, *condition, then_branch, else_branch);
}

stmt *parser::while_statement() {
    expr *condition = expression(true);
    token *d = consume(DO, EXPECTED_AFTER("'do'","loop condition"));
    stmt *body;
    try {
        body = new block_stmt(d->filename, d->line, stmts_until({END}));
    } catch (int) {
        throw error(previous(), EXPTOCLOSE("if statement", d->line));
    }
    return new s_while_stmt(condition->file, condition->line, *condition, body);
}


stmt *parser::foreach_statement() {
    token *init = consume(IDENTIFIER, EXPECTED_AFTER("identifier","'foreach'"));
    consume(IN, EXPECTED_AFTER("'in'","identifier"));
    expr *container = expression(true);
    token *p = consume(DO, EXPECTED("'do'"));
    try {
        return new s_for_each_stmt(p->filename, p->line, init, container,
                                   new block_stmt(p->filename, p->line, stmts_until({END})));
    } catch (int) {
        throw error(previous(), EXPTOCLOSE("foreach statement", p->line));
    }
}

stmt *parser::for_statement() {
    stmt *initializer = nullptr;
    if (!match({COMMA}))
        if (match({VAR}))
            initializer = var_declaration(V_UNSPECIFIED, false);
        else
            initializer = expression_statement();
    consume(COMMA, EXPECTED_AFTER("','","for loop initializer"));
    expr *condition = nullptr;
    if (!check({COMMA})) {
        condition = expression(true);
    }
    consume(COMMA, EXPECTED_AFTER("','","loop condition"));
    expr *increment = nullptr;
    if (!check(DO)) {
        increment = expression(true);
    }
    token *p = consume(DO, EXPECTED_AFTER("'do'","for loop increment"));
    stmt *body;
    try {
        body = new block_stmt(p->filename, p->line, stmts_until({END}));
    } catch (int) {
        throw error(previous(), EXPTOCLOSE("for statement", p->line));
    }
    s_for_stmt *s = new s_for_stmt(condition == nullptr ? p->filename : condition->file,
                                   condition == nullptr ? p->line : condition->line, initializer, condition, increment,
                                   body);
    return s;
}

stmt *parser::expression_statement() {
    expr *expr = expression(false);
    return new expression_stmt(expr->file, expr->line, *expr);
}

function_stmt * parser::function(visibility vis) {
    token *name = consume(IDENTIFIER, EXPECTED("function name"));
    consume(LEFT_PAREN, EXPECTED_AFTER("'('","function name"));
    vector<token *> &parameters = *new vector<token *>();
    if (!check(RIGHT_PAREN)) {
        do {
            token *t = consume(IDENTIFIER, EXPECTED("parameter name"));
            parameters.push_back(t);
        } while (match({COMMA}));
    }
    consume(RIGHT_PAREN, EXPECTED_AFTER("')'","parameter list"));
    try {
        vector<stmt *> &body = stmts_until({END});
        return new function_stmt(name->filename, name->line, name, parameters, body, vis);
    } catch (int) {
        int l = name->line;
        throw error(previous(), EXPTOCLOSE("function", l));
    }
}

expr *parser::array() {
    token *opening = previous();
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
        else if (peek()->type == RIGHT_BRACE)
            break;
        else
            throw error(peek(), EXPECTED_AFTER("',' or '}'","array expression"));
    }
    consume(RIGHT_BRACE, EXPTOCLOSE("array literal", opening->line));
    return new array_expr(opening->filename, opening->line, opening, pairs);
}

expr *parser::assignment(bool nested) {
    variable_expr *var;
    sub_script_expr *map;
    member_expr *context;
    expr *expr = logical(nested), *value, *key;
    if (match({EQUAL, PLUS_EQUALS, MINUS_EQUALS, STAR_EQUALS, SLASH_EQUALS})) {
        token *op = previous();
        value = assignment(true);
        if ((var = dynamic_cast<variable_expr *>(expr)) != nullptr) {
            token *name = const_cast<token *>(var->name);
            return new assign_expr(var->file, var->line, name, op->type, value);
        }
        if ((map = dynamic_cast<sub_script_expr *>(expr)) != nullptr) {
            return new sub_script_assign_expr(map->file, map->line, const_cast<token *>(map->where), map->name, op->type,
                                              map->key, value);
        }
        if ((context = dynamic_cast<member_expr *>(expr)) != nullptr) {
            return new member_assign_expr(context->file, context->line,
                                          context->container_name, op->type,
                                          const_cast<token *>(context->member_identifier), value);
        }
        throw error(op, INVALID_ASSIGNMENT_TARGET);
    }
    if(match({PLUS_PLUS,MINUS_MINUS}))
        return new unary_expr(expr->file,expr->line,previous(),operator_location ::POSTFIX,expr);
    return expr;
}

expr *parser::comparison(bool nested) {
    expr *expr = addition(nested), *right;
    while (match({BANG_EQUAL, EQUAL_EQUAL, GREATER, GREATER_EQUAL, LESS, LESS_EQUAL})) {
        token *op = previous();
        right = addition(true);
        expr = new binary_expr(op->filename, op->line, expr, op, right);
    }
    return expr;
}

expr *parser::addition(bool nested) {
    expr *expr = multiplication(nested), *right;
    while (match({MINUS, PLUS})) {
        token *op = previous();
        right = multiplication(true);
        expr = new binary_expr(op->filename, op->line, expr, op, right);
    }
    return expr;
}

expr *parser::multiplication(bool nested) {
    expr *expr = power(nested), *right;
    while (match({SLASH, STAR})) {
        token *op = previous();
        right = power(true);
        expr = new binary_expr(op->filename, op->line, expr, op, right);
    }
    return expr;
}

expr *parser::power(bool nested) {
    expr *expr = unary(nested), *right;
    while (match({HAT})) {
        token *op = previous();
        right = unary(true);
        expr = new binary_expr(op->filename, op->line, expr, op, right);
    }
    return expr;
}

expr *parser::unary(bool nested) {
    if (match({PLUS_PLUS,MINUS_MINUS,NOT, MINUS})) {
        token *op = previous();
        expr *right = unary(true);
        return new unary_expr(op->filename, op->line, op,operator_location ::PREFIX, right);
    }
    return call(nested);
}

expr *parser::call(bool nested) {
    vector<expr *> &args = *new vector<expr *>();
    expr *expr = special_assignment(nested), *keyexpr;
    token *identifier = previous();
    while (true)
        if (match({LEFT_PAREN})) {
            if (!check(RIGHT_PAREN)) {
                do {
                    args.push_back(expression(true));
                } while (match({COMMA}));
            }
            token *paren = consume(RIGHT_PAREN, EXPECTED_AFTER("')'","arguments"));
            expr = new call_expr(paren->filename, paren->line, expr, paren, args);
        } else break;
    return expr;
}

expr *parser::special_assignment(bool nested) {
    expr *expr = primary(nested), *key;
    while (match({DOT, LEFT_SQR})) {
        token *op = previous();
        if (op->type == DOT) {
            token *fname = consume(IDENTIFIER, EXPECTED("field name"));
            expr = new member_expr(expr->file, expr->line, expr, fname);
        } else {
            key = expression(true);
            consume(RIGHT_SQR, EXPECTED_AFTER("closing ']'","key expression"));
            expr = new sub_script_expr(expr->file, expr->line, op, expr, key);
        }
    }
    return expr;
}

expr *parser::primary(bool nested) {
    if (match({FALSE})) return new literal_expr(previous()->filename, previous()->line, new bool_o(false));
    if (match({TRUE})) return new literal_expr(previous()->filename, previous()->line, new bool_o(true));
    if (match({NUL})) return new literal_expr(previous()->filename, previous()->line, new null_o());
    if (match({NUMBER, STRING})) return new literal_expr(previous()->filename, previous()->line, previous()->literal);
    if (match({IDENTIFIER})) {
        token *identifier = previous();
        return new variable_expr(identifier->filename, identifier->line, identifier);
    }
    if (match({LEFT_PAREN})) {
        expr *expr = expression(true);
        consume(RIGHT_PAREN, MISSING_CLOSING("')'"));
        return new grouping_expr(expr->file, expr->line, expr);
    }
    throw error(peek(), nested ? EXPECTED("expression") : INVALID_STATEMENT);
}

parser::parser(vector<token *> &tokens) : tokens(tokens), start(0), current(0), use_allowed(true),
                                          statements(*new vector<stmt *>()) {}

vector<stmt *> &parser::parse(bool ld_std) {
    stmt *s;
    if (ld_std) ld_stmts(STR("std"));
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
        token *op = previous();
        right = comparison(true);
        expr = new binary_expr(op->filename, op->line, expr, op, right);
    }
    return expr;
}

bool parser::is_at_end() {
    return peek()->type == EOF_;
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
        throw error(previous(), USE_FILE_NOT_FOUND);
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

stmt *parser::exception_(visibility is_global) {
    token *name = consume(IDENTIFIER, EXPECTED("exception name"));
    if (match({WITH})) {
        set<string> &tokens = *new set<string>();
        do {
            token *t = consume(IDENTIFIER, EXPECTED("identifier list"));
            tokens.insert(t->lexeme);
        } while (match({COMMA}));

        return new exception_decl_stmt(name->filename, name->line, name, tokens, is_global);
    }
    return new exception_decl_stmt(name->filename, name->line, name, *new set<string>(), is_global);
}

stmt *parser::begin_handle_statement() {
    token *keyword = previous();
    vector<stmt *> &stmts = stmts_until({HANDLE});
    vector<handle_stmt *> handles = *new vector<handle_stmt *>();
    int last_line = 0;
    do {
        if (peek()->type == END) break;
        expr *e_iden = expression(true);
        token *name = new token(UNRECOGNIZED, string(), new null_o(), e_iden->file, e_iden->line);
        if (match({BIND}))
            name = consume(IDENTIFIER, EXPECTED_AFTER("variable name","'bind'"));
        vector<stmt *> &hstmts = *new vector<stmt *>();
        while (!(check(HANDLE) || check(END))) {
            hstmts.push_back(declaration());
        }
        handles.push_back(new handle_stmt(name->filename, name->line, e_iden, name, hstmts));
    } while (match({HANDLE}) || check(END));
    consume(END, EXPTOCLOSE("handle", last_line));
    return new begin_handle_stmt(keyword->filename, keyword->line, stmts, handles);
}

pair<visibility, bool> parser::get_access_specifiers() {
    visibility vis = V_UNSPECIFIED;
    bool is_final = false;
    while (match({GLOBAL, LOCAL ,FINAL})) {
        if(previous()->type == FINAL)
            if(is_final && !permissive)
                throw error(previous(),DUPLICATE_ACCESS_SPEC("final"));
            else
                is_final = true;
        else if(previous()->type == GLOBAL){
            if(!permissive)
                if(vis == V_LOCAL)
                    throw error(previous(), CONFILICTING_SPECS("global","local"));
                else if(vis == V_GLOBAL)
                    throw error(previous(),DUPLICATE_ACCESS_SPEC("global"));
            vis = V_GLOBAL;
        } else{
            if(!permissive)
                if(vis == V_GLOBAL)
                    throw error(previous(), CONFILICTING_SPECS("local","global"));
                else if(vis == V_LOCAL)
                    throw error(previous(),DUPLICATE_ACCESS_SPEC("local"));
            vis = V_LOCAL;
        }
    }
    return pair<lns::visibility,bool>(vis,is_final);
}

constructor_stmt *parser::constructor(visibility visibility) {
    token *keyword = previous();
    consume(LEFT_PAREN, EXPECTED_AFTER("'('","'constructor'"));
    vector<token *> &parameters = *new vector<token *>();
    if (!check(RIGHT_PAREN)) {
        do {
            token *t = consume(IDENTIFIER, EXPECTED("parameter name"));
            parameters.push_back(t);
        } while (match({COMMA}));
    }
    consume(RIGHT_PAREN, EXPECTED_AFTER("')'","parameter list"));
    try {
        vector<stmt *> &body = stmts_until({END});
        return new constructor_stmt(keyword->filename, keyword->line, keyword, parameters, body, visibility);
    } catch (int) {
        int l = keyword->line;
        throw error(previous(), EXPTOCLOSE("constructor", l));
    }
}
