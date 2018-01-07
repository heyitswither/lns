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

shared_ptr<stmt> parser::use() {
    bool isnatives = false;
    if (!use_allowed)use_not_allowed();
    if (match({NATIVES})) isnatives = true;
    token *t = consume(STRING, EXPECTED("filename"));
    string s = t->literal->str();
    if (isnatives) {
        consume(BIND, EXPECTED_AFTER("'bind'","filename"));
        auto specs = get_access_specifiers();
        CHECK_ACCESS_SPEC_NOT_ALLOWED(specs.second,"final","native bound")
        return make_shared<uses_native_stmt>(t->filename, t->line, t, s,
                                             consume(IDENTIFIER, EXPECTED_AFTER("identifier", "'bind'")), specs.first);
    }
    ld_stmts(s, t);
    return nullptr;
}

vector<shared_ptr<stmt>> parser::context_block() {
    int line = previous()->line;
    vector<shared_ptr<stmt>> stmts;
    while (!check(END) && !is_at_end()) {
        shared_ptr<stmt> s = declaration();
        if (s->type == CONTEXT_STMT_T || s->type == VAR_STMT_T || s->type == FUNCTION_STMT_T ||
            s->type == EXCEPTION_DECL_STMT_T || s->type == CLASS_DECL_STMT_T) {
            stmts.push_back(s);
            continue;
        }
        throw error(previous(), CAN_ONLY_CONTAIN("contexts","declarations"));
    }
    consume(END, EXPTOCLOSE("context block", line));
    return stmts;
}

vector<shared_ptr<stmt>> parser::stmts_until(initializer_list<token_type> list) {
    vector<shared_ptr<stmt>> stmts;
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

shared_ptr<stmt> parser::context_declaration(visibility vis, bool final) {
    token *name = consume(IDENTIFIER, EXPECTED("context name"));
    vector<shared_ptr<stmt>> stmts = context_block();
    return make_shared<context_stmt>(name->filename, name->line, name, stmts, vis, final);
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

shared_ptr<stmt> parser::declaration() {
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
            CHECK_ACCESS_SPEC_NOT_ALLOWED(is_final, "final", "classes")
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

shared_ptr<stmt> parser::class_(visibility vis) {
    token* keyword = previous();
    token* name = consume(IDENTIFIER,EXPECTED("class name"));
    vector<shared_ptr<var_stmt>> variables;
    vector<shared_ptr<constructor_stmt>> constructors;
    vector<shared_ptr<function_stmt>> methods;
    while(!(is_at_end() || peek()->type == END)){
        auto specs = get_access_specifiers();
        if(match({VAR})) variables.push_back(var_declaration(specs.first,specs.second));
        else if(match({FUNCTION})) methods.push_back(function(specs.first));
        else if(match({CONSTRUCTOR})) constructors.push_back(constructor(specs.first));
        else throw error(peek(),CAN_ONLY_CONTAIN("classes","variables, methods and constructors"));
    }
    consume(END,EXPTOCLOSE("class declaration",keyword->line));
    if (constructors.empty())
        constructors.push_back(make_shared<constructor_stmt>(keyword->filename, keyword->line,
                                                             new token(UNRECOGNIZED, "__implicit_constructor__",
                                                                       make_shared<null_o>(), keyword->filename,
                                                                       keyword->line), parameter_declaration(0),
                                                             vector<shared_ptr<stmt>>(), V_UNSPECIFIED));
    return make_shared<class_decl_stmt>(keyword->filename, keyword->line, name, methods, constructors, variables, vis);
}

int parser::error(token *token, const char *message) {
    errors::parse_error(token->filename, token->line, message);
    return PARSE_ERROR;
}

shared_ptr<var_stmt> parser::var_declaration(visibility vis, bool is_final) {
    token *name = consume(IDENTIFIER, EXPECTED_IN("identifier","variable declaration"));
    shared_ptr<expr> initializer = make_shared<null_expr>(name->filename, name->line, name);
    if (match({EQUAL})) {
        return make_shared<var_stmt>(name->filename, name->line, name, expression(true), vis, is_final);
    }
    return make_shared<var_stmt>(name->filename, name->line, name, initializer, vis, is_final);
}

shared_ptr<stmt> parser::statement() {
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

shared_ptr<stmt> parser::raise() {
    token *raise = previous();
    shared_ptr<expr> name = expression(true);
    if (match({WITH})) {
        map<string, shared_ptr<expr>> v;
        do {
            token *vname = consume(IDENTIFIER, EXPECTED("exception field identifier"));
            consume(EQUAL, EXPECTED_AFTER("'='","exception field identifier"));
            shared_ptr<expr> val = expression(true);
            v[vname->lexeme] = val;
        } while (match({COMMA}));
        return make_shared<raise_stmt>(name->file, name->line, raise, name, v);
    }
    return make_shared<raise_stmt>(name->file, name->line, raise, name, map<string, shared_ptr<expr>>());
}


shared_ptr<stmt> parser::return_statement() {
    token *keyword = previous();
    shared_ptr<expr> value = make_shared<null_expr>(keyword->filename, keyword->line, previous());
    if (!is_at_end())
        if (peek()->line == keyword->line)
            return make_shared<return_stmt>(keyword->filename, keyword->line, keyword, expression(true));
    return make_shared<return_stmt>(keyword->filename, keyword->line, keyword, value);
}

shared_ptr<stmt> parser::break_statement() {
    token *t = previous();
    return make_shared<break_stmt>(t->filename, t->line, t);
}

shared_ptr<stmt> parser::continue_statement() {
    token *t = previous();
    return make_shared<continue_stmt>(t->filename, t->line, t);
}

shared_ptr<stmt> parser::if_statement() {
    token *keyword = previous();
    shared_ptr<expr> condition = expression(true);
    consume(THEN, EXPECTED_AFTER("'then'","if condition"));
    shared_ptr<stmt> then_branch;
    try {
        then_branch = make_shared<block_stmt>(condition->file, condition->line, stmts_until({ELSE, END}));
    } catch (int) {
        throw error(previous(), EXPTOCLOSE("if statement", keyword->line));
    }
    shared_ptr<stmt> else_branch = make_shared<null_stmt>(keyword->filename, keyword->line, keyword);
    token *closing = previous();
    if (closing->type == ELSE)
        if (match({IF})) else_branch = if_statement();
        else
            try {
                return make_shared<if_stmt>(keyword->filename, keyword->line, condition, then_branch,
                                            make_shared<block_stmt>(previous()->filename, previous()->line,
                                                                    stmts_until({END})));
            } catch (int) {
                throw error(previous(), EXPTOCLOSE("else statement", closing->line));
            }
    return make_shared<if_stmt>(keyword->filename, keyword->line, condition, then_branch, else_branch);
}

shared_ptr<stmt> parser::while_statement() {
    shared_ptr<expr> condition = expression(true);
    token *d = consume(DO, EXPECTED_AFTER("'do'","loop condition"));
    try {
        return make_shared<s_while_stmt>(condition->file, condition->line, condition,
                                         make_shared<block_stmt>(d->filename, d->line, stmts_until({END})));
    } catch (int) {
        throw error(previous(), EXPTOCLOSE("if statement", d->line));
    }
}


shared_ptr<stmt> parser::foreach_statement() {
    token *init = consume(IDENTIFIER, EXPECTED_AFTER("identifier","'foreach'"));
    consume(IN, EXPECTED_AFTER("'in'","identifier"));
    shared_ptr<expr> container = expression(true);
    token *p = consume(DO, EXPECTED("'do'"));
    try {
        return make_shared<s_for_each_stmt>(p->filename, p->line, init, container,
                                            make_shared<block_stmt>(p->filename, p->line, stmts_until({END})));
    } catch (int) {
        throw error(previous(), EXPTOCLOSE("foreach statement", p->line));
    }
}

shared_ptr<stmt> parser::for_statement() {
    shared_ptr<stmt> initializer = for_statement_init(previous());
    token *t = consume(COMMA, EXPECTED_AFTER("','", "for loop initializer"));
    shared_ptr<expr> condition = for_statement_condition(t);
    consume(COMMA, EXPECTED_AFTER("','","loop condition"));
    shared_ptr<expr> increment = for_statement_increment(t);
    token *p = consume(DO, EXPECTED_AFTER("'do'","for loop increment"));
    try {
        return make_shared<s_for_stmt>(t->filename, t->line, initializer, condition, increment,
                                       make_shared<block_stmt>(p->filename, p->line, stmts_until({END})));
    } catch (int) {
        throw error(previous(), EXPTOCLOSE("for statement", p->line));
    }
}

shared_ptr<expr> parser::for_statement_increment(lns::token *where) {
    if (!check(DO))
        return expression(true);
    return make_shared<null_expr>(where->filename, where->line, where);
}

shared_ptr<expr> parser::for_statement_condition(lns::token *where) {
    if (!check({COMMA}))
        return expression(true);
    return make_shared<null_expr>(where->filename, where->line, where);
}


shared_ptr<stmt> parser::for_statement_init(lns::token *where) {
    if (!match({COMMA}))
        if (match({VAR}))
            return var_declaration(V_UNSPECIFIED, false);
        else
            return expression_statement();
    return make_shared<null_stmt>(where->filename, where->line, where);
}

shared_ptr<stmt> parser::expression_statement() {
    shared_ptr<expr> expr = expression(false);
    return make_shared<expression_stmt>(expr->file, expr->line, expr);
}

shared_ptr<function_stmt> parser::function(visibility vis) {
    token *name = consume(IDENTIFIER, EXPECTED("function name"));
    consume(LEFT_PAREN, EXPECTED_AFTER("'('","function name"));
    parameter_declaration params = parameters();
    consume(RIGHT_PAREN, EXPECTED_AFTER("')'","parameter list"));
    try {
        vector<shared_ptr<stmt>> body = stmts_until({END});
        return make_shared<function_stmt>(name->filename, name->line, name, params, body, vis);
    } catch (int) {
        throw error(previous(), EXPTOCLOSE("function", name->line));
    }
}

shared_ptr<expr> parser::array() {
    token *opening = previous();
    vector<pair<shared_ptr<expr>, shared_ptr<expr>>> pairs;
    int i = 0;
    while (!is_at_end()) {
        shared_ptr<expr> e1 = expression(true);
        if (match({COLON})) {
            shared_ptr<expr> e2 = expression(true);
            pairs.emplace_back(pair<shared_ptr<expr>, shared_ptr<expr>>(e1, e2));
        } else {
            shared_ptr<expr> e2 = make_shared<literal_expr>(e1->file, e1->line, make_shared<number_o>(i));
            pairs.emplace_back(pair<shared_ptr<expr>, shared_ptr<expr>>(e2, e1));
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
    return make_shared<array_expr>(opening->filename, opening->line, opening, pairs);
}

shared_ptr<expr> parser::assignment(bool nested) {
    shared_ptr<expr> expr_ = logical(nested), value;
    if (match({EQUAL, PLUS_EQUALS, MINUS_EQUALS, STAR_EQUALS, SLASH_EQUALS})) {
        token *op = previous();
        value = assignment(true);
        if (expr_.get()->type == VARIABLE_EXPR_T) {
            auto DCAST_ASN(var, variable_expr*, expr_.get());
            token *name = const_cast<token *>(var->name);
            return make_shared<assign_expr>(var->file, var->line, name, op->type, value);
        }
        if (expr_.get()->type == SUB_SCRIPT_EXPR_T) {
            auto DCAST_ASN(array, sub_script_expr*, expr_.get());
            return make_shared<sub_script_assign_expr>(array->file, array->line, const_cast<token *>(array->where),
                                                       array->name, op->type,
                                                       array->key, value);
        }
        if (expr_.get()->type == MEMBER_EXPR_T) {
            auto DCAST_ASN(container, member_expr*, expr_.get());
            return make_shared<member_assign_expr>(container->file, container->line,
                                                   container->container_name, op->type, container->member_identifier,
                                                   value);
        }
        throw error(op, INVALID_ASSIGNMENT_TARGET);
    }
    if (check(PLUS_PLUS) || check(MINUS_MINUS)) {
        if (peek()->line == expr_->line) {
            advance();
            return make_shared<unary_expr>(expr_->file, expr_->line, previous(), operator_location::POSTFIX, expr_);
        }
    }
    return expr_;
}

shared_ptr<expr> parser::comparison(bool nested) {
    shared_ptr<expr> expr_ = addition(nested), right;
    while (match({BANG_EQUAL, EQUAL_EQUAL, GREATER, GREATER_EQUAL, LESS, LESS_EQUAL})) {
        token *op = previous();
        right = addition(true);
        expr_ = make_shared<binary_expr>(op->filename, op->line, expr_, op, right);
    }
    return expr_;
}

shared_ptr<expr> parser::addition(bool nested) {
    shared_ptr<expr> expr = multiplication(nested), right;
    while (match({MINUS, PLUS})) {
        token *op = previous();
        right = multiplication(true);
        expr = make_shared<binary_expr>(op->filename, op->line, expr, op, right);
    }
    return expr;
}

shared_ptr<expr> parser::multiplication(bool nested) {
    shared_ptr<expr> expr_ = power(nested), right;
    while (match({SLASH, STAR})) {
        token *op = previous();
        right = power(true);
        expr_ = make_shared<binary_expr>(op->filename, op->line, expr_, op, right);
    }
    return expr_;
}

shared_ptr<expr> parser::power(bool nested) {
    shared_ptr<expr> expr = unary(nested), right;
    while (match({HAT})) {
        token *op = previous();
        right = unary(true);
        expr = make_shared<binary_expr>(op->filename, op->line, expr, op, right);
    }
    return expr;
}

shared_ptr<expr> parser::unary(bool nested) {
    if (match({PLUS_PLUS,MINUS_MINUS,NOT, MINUS})) {
        token *op = previous();
        shared_ptr<expr> right = unary(true);
        return make_shared<unary_expr>(op->filename, op->line, op, operator_location::PREFIX, right);
    }
    return call(nested, false);
}

shared_ptr<expr> parser::call(bool nested, bool strict) {
    vector<shared_ptr<expr>> args;
    shared_ptr<expr> expr = special_assignment(nested);
    while (true)
        if (match({LEFT_PAREN})) {
            if (!check(RIGHT_PAREN)) {
                do {
                    args.push_back(expression(true));
                } while (match({COMMA}));
            }
            token *paren = consume(RIGHT_PAREN, EXPECTED_AFTER("')'","arguments"));
            expr = make_shared<call_expr>(paren->filename, paren->line, expr, paren, args);
        } else break;
    if (strict && expr->type != CALL_EXPR_T) throw error(previous(), EXPECTED("call expression"));
    return expr;
}

shared_ptr<expr> parser::special_assignment(bool nested) {
    shared_ptr<expr> expr = primary(nested), key;
    while (match({DOT, LEFT_SQR})) {
        token *op = previous();
        if (op->type == DOT) {
            token *fname = consume(IDENTIFIER, EXPECTED("field name"));
            expr = make_shared<member_expr>(expr->file, expr->line, expr, fname);
        } else {
            key = expression(true);
            consume(RIGHT_SQR, EXPECTED_AFTER("closing ']'","key expression"));
            expr = make_shared<sub_script_expr>(expr->file, expr->line, op, expr, key);
        }
    }
    return expr;
}

shared_ptr<expr> parser::primary(bool nested) {
    if (match({THIS}))
        return make_shared<this_expr>(previous()->filename, previous()->line, previous());
    if (match({FALSE}))
        return make_shared<literal_expr>(previous()->filename, previous()->line, make_shared<bool_o>(false));
    if (match({TRUE}))
        return make_shared<literal_expr>(previous()->filename, previous()->line, make_shared<bool_o>(true));
    if (match({NUL})) return make_shared<literal_expr>(previous()->filename, previous()->line, make_shared<null_o>());
    if (match({NUMBER, STRING}))
        return make_shared<literal_expr>(previous()->filename, previous()->line, previous()->literal);
    if (match({IDENTIFIER})) {
        token *identifier = previous();
        return make_shared<variable_expr>(identifier->filename, identifier->line, identifier);
    }
    if (match({LEFT_PAREN})) {
        shared_ptr<expr> expr = expression(true);
        consume(RIGHT_PAREN, MISSING_CLOSING("')'"));
        return make_shared<grouping_expr>(expr->file, expr->line, expr);
    }
    throw error(peek(), nested ? EXPECTED("expression") : INVALID_STATEMENT);
}

parser::parser(vector<token *> tokens) : tokens(tokens), start(0), current(0), use_allowed(true) {}

vector<shared_ptr<stmt>> parser::parse(bool ld_std) {
    if (ld_std)
        ld_stmts(STR("std"), peek());
    while (!is_at_end()) {
        try {
            statements.push_back(declaration());
        } catch (int i) {
            if (i == SIG_EXIT_PARSER) return statements;
            continue;
        }
    }
    parsed_files.push_back(tokens.back()->filename);
    return statements;
}

shared_ptr<expr> parser::expression(bool nested) {
    if (match({LEFT_BRACE})) return array();
    if (match({NEW})) return new_();
    return assignment(nested);
}


shared_ptr<expr> parser::new_() {
    auto keyword = previous();
    auto c = call(true, true);
    auto DCAST_ASN(class_, call_expr*, c.get());
    return make_shared<new_expr>(keyword->filename, keyword->line, keyword, class_->callee, class_->args);
}

shared_ptr<expr> parser::logical(bool nested) {
    shared_ptr<expr> expr = comparison(nested), right;
    while (match({AND, OR, NOR, XOR, NAND})) {
        token *op = previous();
        right = comparison(true);
        expr = make_shared<binary_expr>(op->filename, op->line, expr, op, right);
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

void parser::ld_stmts(string s, lns::token *where) {
    const char *filename = best_file_path(s.c_str());
    ifstream file(filename);
    string source;
    stringstream ss;
    if (!file.is_open()) {
        if (lns::ignore_unresolved) return;
        else throw error(where, USE_FILE_NOT_FOUND);
    }
    ss << file.rdbuf();
    source = ss.str();
    scanner scanner(filename, source);
    vector<token *> &tkns = scanner.scan_tokens(true);
    parser p(tkns);
    vector<shared_ptr<stmt>> toadd = p.parse(false);
    int i = 0;
    for (; i < toadd.size(); i++) {
        statements.push_back(toadd[i]);
    }
}

shared_ptr<stmt> parser::exception_(visibility is_global) {
    token *name = consume(IDENTIFIER, EXPECTED("exception name"));
    if (match({WITH})) {
        set<string> tokens;
        do {
            token *t = consume(IDENTIFIER, EXPECTED("identifier list"));
            tokens.insert(t->lexeme);
        } while (match({COMMA}));

        return make_shared<exception_decl_stmt>(name->filename, name->line, name, tokens, is_global);
    }
    return make_shared<exception_decl_stmt>(name->filename, name->line, name, set<string>(), is_global);
}

shared_ptr<stmt> parser::begin_handle_statement() {
    token *keyword = previous();
    vector<shared_ptr<stmt>> stmts = stmts_until({HANDLE});
    vector<shared_ptr<handle_stmt>> handles;
    int last_line = 0;
    do {
        if (peek()->type == END) break;
        shared_ptr<expr> e_iden = expression(true);
        token *name = new token(UNRECOGNIZED, string(), make_shared<null_o>(), e_iden->file, e_iden->line);
        if (match({BIND}))
            name = consume(IDENTIFIER, EXPECTED_AFTER("variable name","'bind'"));
        vector<shared_ptr<stmt>> hstmts;
        while (!(check(HANDLE) || check(END))) {
            hstmts.push_back(declaration());
        }
        handles.push_back(make_shared<handle_stmt>(name->filename, name->line, e_iden, name, hstmts));
    } while (match({HANDLE}) || check(END));
    consume(END, EXPTOCLOSE("handle", last_line));
    return make_shared<begin_handle_stmt>(keyword->filename, keyword->line, stmts, handles);
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

shared_ptr<constructor_stmt> parser::constructor(visibility visibility) {
    token *keyword = previous();
    consume(LEFT_PAREN, EXPECTED_AFTER("'('","'constructor'"));
    auto params = parameters();
    consume(RIGHT_PAREN, EXPECTED_AFTER("')'","parameter list"));
    try {
        vector<shared_ptr<stmt>> body = stmts_until({END});
        return make_shared<constructor_stmt>(keyword->filename, keyword->line, keyword, params, body, visibility);
    } catch (int) {
        int l = keyword->line;
        throw error(previous(), EXPTOCLOSE("constructor", l));
    }
}

parameter_declaration parser::parameters() {
    parameter_declaration decl;
    bool found_optional = false;
    if (!check(RIGHT_PAREN)) {
        do {
            token *t = consume(IDENTIFIER, EXPECTED("parameter name"));
            if(match({QUESTION_MARK}))
                found_optional = true;
            else
                if(found_optional) throw error(previous(),NON_OPTIONAL_NOT_ALLOWED);
            decl.parameters.push_back(*new parameter(t->lexeme,found_optional));
        } while (match({COMMA}));
    }
    return decl;
}

parser::parser() : tokens(vector<token *>()) {}
