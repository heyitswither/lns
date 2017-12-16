//
// Created by lorenzo on 11/30/17.
//
#include "interpreter.h"
#include "errors.h"
#include "options.h"
#include <dlfcn.h>

using namespace lns;

void lns::interpreter::execute(stmt *s) {
    if (s == nullptr) return;
    s->accept(this);
}

object *interpreter::evaluate(expr *e) {
    if (e == nullptr)
        return lns::GET_DEFAULT_NULL();
    object *o = e->accept(this);
    return o == nullptr ? lns::GET_DEFAULT_NULL() : o;
}

void interpreter::check_bool_operands(const token &op, object *left, object *right) {
    if (left->type == BOOL_T && right->type == BOOL_T) return;
    throw runtime_exception(op, *new string("operands must be booleans"));
}

void interpreter::check_number_operands(const token &op, object *left, object *right) {
    if (left->type == NUMBER_T && right->type == NUMBER_T) return;
    throw runtime_exception(op, *new string("operands must be numbers"));
}

void interpreter::check_number_operand(const token &token, object *o) {
    if (o->type == NUMBER_T)return;
    throw runtime_exception(token, *new string("operand must be a number"));
}

bool interpreter::is_bool_true_eq(object *o) {
    if (o->str() == "true") return true;
    if (o->type == NUMBER_T)
        return dynamic_cast<number_o *>(o)->value == 1;
    return false;
}

void interpreter::execute_block(vector<stmt *> stmts, runtime_environment *env) {
    if (stmts.empty()) return;
    runtime_environment *previous = this->environment;
    this->environment = env;
    int i;
    try {
        for (i = 0; i < stmts.size(); ++i) {
            execute(stmts[i]);
        }
        delete (env);
        this->environment = previous;
    } catch (return_signal &s) {
        delete (env);
        this->environment = previous;
        throw s;
    }
    //delete env;
}

object *interpreter::visit_context_expr(context_expr *c) {
    object *o = evaluate(const_cast<expr *>(c->context_name));
    if (o->type != CONTEXT_T) {
        throw runtime_exception(c->context_identifier, *new string("object is not a context"));
    }
    return dynamic_cast<context *>(o)->get(c->context_identifier);
}

object *interpreter::visit_context_assign_expr(context_assign_expr *c) {
    object *o = evaluate(const_cast<expr *>(c->context_name));
    if (o->type != CONTEXT_T) {
        throw runtime_exception(c->context_identifier, *new string("object is not a context"));
    }
    object *value = evaluate(const_cast<expr *>(c->value));
    dynamic_cast<context *>(o)->assign(c->context_identifier, c->op, clone_or_keep(value,c->value->type,c->context_identifier));
    return value;
}

object *interpreter::visit_increment_expr(increment_expr *i) {
    return environment->increment(i->name);;
}

object *interpreter::visit_decrement_expr(decrement_expr *d) {
    return environment->decrement(d->name);
}

object *interpreter::visit_assign_expr(assign_expr *a) {
    object *value = evaluate(const_cast<expr *>(a->value));
    environment->assign(a->name, a->op, clone_or_keep(value,a->value->type,a->name));
    return value;
}

object *interpreter::visit_binary_expr(binary_expr *b) {
    object *left = evaluate(const_cast<expr *>(b->left));
    if (left == nullptr)
        left = lns::GET_DEFAULT_NULL();
    object *right = evaluate(const_cast<expr *>(b->right));
    if (right == nullptr) {
        right = lns::GET_DEFAULT_NULL();
    }
    token_type type = b->op.type;
    try {
        switch (type) {
            case AND:
                return new bool_o(*left && *right);
            case NAND:
                return new bool_o(!(*left && *right));
            case NOR:
                return new bool_o(!(*left || *right));
            case OR:
                return new bool_o(*left || *right);
            case XOR:
                return new bool_o((*left && *!(*right)) || (*!(*left) && *right));
            case EQUAL_EQUAL:
                return new bool_o(((*left) == (*right)));
            case BANG_EQUAL:
                return new bool_o(!((*left) == (*right)));
            case GREATER:
                return new bool_o(*left > *right);
            case GREATER_EQUAL:
                return new bool_o(*left >= *right);
            case LESS:
                return new bool_o(*left < *right);
            case LESS_EQUAL:
                return new bool_o(*left <= *right);
            case MINUS:
                return (*left - *right);
            case PLUS:
                return (*left + *right);
        }
        if (type == SLASH) {
            return (*left / *right);
        }
        if (type == STAR) {
            return (*left * *right);
        }
        if (type == HAT) {
            return (*left ^ *right);
        }
    } catch (const char* p) {
        throw runtime_exception(b->op, STR(p));
    }
    // unreachable
    return nullptr;
}

object *interpreter::visit_call_expr(call_expr *c) {
    object *callee = evaluate(const_cast<expr *>(c->callee));
    vector<object *> &args = *new vector<object *>();
    callable *function = dynamic_cast<callable *>(callee);
    if (function == nullptr) throw runtime_exception(c->paren, *new string("target not a callable"));
    int i = 0;
    for (; i < c->args.size(); i++) {
        args.push_back(evaluate(c->args[i]));
        //if(stack_trace.size() < 0 || stack_trace.size() > 2) throw 'a';
    }
    if (args.size() != function->arity()) {
        string s = *new string();
        s += "expected " + to_string(function->arity()) + " argument" + (function->arity() == 1 ? "" : "s") +
             " but got " + to_string(args.size());
        throw runtime_exception(c->paren, *new string(s.c_str()));
    }
    stack_trace.push_back(
            new stack_call(c->paren.filename, c->paren.line, function->name(), globals->is_native(function)));
    object *p = function->call(args);
    stack_trace.pop_back();
    return p;
}

object *interpreter::visit_grouping_expr(grouping_expr *g) {
    return evaluate(const_cast<expr *>(g->expression));
}

object *interpreter::visit_literal_expr(literal_expr *l) {
    return const_cast<object *>(l->value);
}

object *interpreter::visit_unary_expr(unary_expr *u) {//
    object *right = evaluate(const_cast<expr *>(u->right));
    try {
        switch (u->op.type) {
            case NOT:
                return new bool_o(is_bool_true_eq(!*right));
            case MINUS:
                check_number_operand(u->op, right);
                return (-*right);
        }
    } catch (const char *p) {
        throw runtime_exception(u->op, STR(p));
    }
    //unreachable
    return nullptr;
}

object *interpreter::visit_variable_expr(variable_expr *v) {
    return environment->get(v->name);
}

object *interpreter::visit_sub_script_expr(sub_script_expr *m) {
    object *key = evaluate(const_cast<expr *>(m->key));
    object *eval = evaluate(const_cast<expr *>(m->name));
    if (eval->type == MAP_T) {
        return get_map_field(const_cast<token &>(m->where), eval, key);
    } else if (eval->type == STRING_T) {
        string evstring = eval->str();
        if (key->type != NUMBER_T)
            throw runtime_exception(m->where, *new string(
                    "key for operator [] (string) must be an number"));
        int i = static_cast<int>(dynamic_cast<number_o *>(key)->value);
        if (evstring.size() > i) return new string_o(*new string() + evstring.at(i));
        else return new null_o();
    } else {
        throw runtime_exception(m->where,
                                *new string("invalid operand for operator []: operand must be either string or map"));
    }
}

object *lns::interpreter::get_map_field(token &where, object *obj, object *key) {
    map_o *map = dynamic_cast<map_o *>(obj);
    if (!map->contains_key(key->str())) return new null_o();
    return map->values[key->str()];
}

object *
interpreter::assign_map_field(const token &where, object *obj, const token_type op, string_o *key, object *v) {
    map_o *map;
    if ((map = dynamic_cast<map_o *>(obj)) == nullptr) {
        throw runtime_exception(where, *new string("object is not a map"));
    }
    try {
        object* value = clone_or_keep(v, static_cast<const expr_type>(op), where);
        switch (op) {
            case EQUAL:
                map->values[key->value] = value;
                break;
            case PLUS_EQUALS:
                *(map->values[key->value]) += *value;
                break;
            case MINUS_EQUALS:
                *(map->values[key->value]) -= *value;
                break;
            case STAR_EQUALS:
                *(map->values[key->value]) *= *value;
                break;
            case SLASH_EQUALS:
                *(map->values[key->value]) /= *value;
                break;
        }
        return map->values[key->value];
    } catch (string &s) {
        throw runtime_exception(where, s);
    }
}

object *interpreter::visit_assign_map_field_expr(assign_map_field_expr *a) {
    object *key = evaluate(const_cast<expr *>(a->key));
    object *value = evaluate(const_cast<expr *>(a->value));
    string_o *k = dynamic_cast<string_o *>(key);
    if (k == nullptr) {
        k = new string_o(key->str());
    }
    return assign_map_field(a->where, evaluate(const_cast<expr *>(a->name)), a->op, k, value);
}

object *interpreter::visit_null_expr(null_expr *n) {
    return lns::GET_DEFAULT_NULL();
}

void interpreter::visit_block_stmt(block_stmt *b) {
    runtime_environment *env = new runtime_environment(environment);
    execute_block(b->statements, env);
}

void interpreter::visit_s_for_stmt(s_for_stmt *s) {
    execute(const_cast<stmt *>(s->init));
    while (is_bool_true_eq(evaluate(const_cast<expr *>(s->condition)))) {
        execute(const_cast<stmt *>(s->body));
        evaluate(const_cast<expr *>(s->increment));
    }
}

void interpreter::visit_expression_stmt(expression_stmt *e) {
    object *o = evaluate((expr *) &(e->exprs));
    if (!lns::prompt) return;
    expr_type type = e->exprs.type;
    switch (type) {
        case ASSIGN_EXPR_T:
        case CALL_EXPR_T:
        case NULL_EXPR_T:
        case ASSIGN_MAP_FIELD_EXPR_T:
        case INCREMENT_EXPR_T:
        case DECREMENT_EXPR_T:
        case CONTEXT_ASSIGN_EXPR_T:
            return;
    }
    cout << o->str() << endl;
}

void interpreter::visit_function_stmt(function_stmt *f) {
    callable *func = new function(this, f);
    environment->define(const_cast<token &>(f->name), func, true, f->isglobal);
}

void interpreter::visit_if_stmt(if_stmt *i) {
    object *eval = evaluate(const_cast<expr *>(&i->condition));
    //boolean definitions for the language are in function is_bool_true_eq()
    if (is_bool_true_eq(eval)) {
        execute(const_cast<stmt *>(i->thenBranch));
        return;
    }
    if (i->elseBranch != nullptr && i->elseBranch->type != NULL_STMT_T) {
        execute(const_cast<stmt *>(i->elseBranch));
    }
}

void interpreter::visit_return_stmt(return_stmt *r) {
    object *value = lns::GET_DEFAULT_NULL();
    null_expr *null = dynamic_cast<null_expr *>(const_cast<expr *>(&r->value));
    if (null == nullptr) value = evaluate(const_cast<expr *>(&r->value));
    throw return_signal(value, r->keyword);
}

void interpreter::visit_break_stmt(break_stmt *b) {
    throw break_signal(b->keyword);
}

void interpreter::visit_continue_stmt(continue_stmt *c) {
    throw continue_signal(c->keyword);
}

void interpreter::visit_var_stmt(var_stmt *v) {
    if (v->initializer.type != NULL_STMT_T) {
        object* val = evaluate(const_cast<expr *>(&v->initializer));
        environment->define(const_cast<token &>(v->name), clone_or_keep(val,v->initializer.type,v->name), v->isfinal, v->isglobal);
    }else{
        environment->define(const_cast<token &>(v->name), new null_o, v->isfinal, v->isglobal);
    }
}

void interpreter::visit_s_while_stmt(s_while_stmt *s) {
    object *o;
    while (is_bool_true_eq(o = evaluate(const_cast<expr *>(&s->condition)))) {
        try {
            execute(const_cast<stmt *>(s->body));
        } catch (break_signal b) {
            break;
        } catch (continue_signal s) {
            continue;
        }
        delete o;
    }
}

void interpreter::visit_uses_native_stmt(uses_native_stmt *u) {
    try {
        void *handler = dlopen(lns::best_file_path(u->lib_name.c_str()), RTLD_LAZY);
        if (!handler) throw dlerror();
        typedef object* supplier();
        supplier* suppl = (supplier*)dlsym(handler,"supplier");
        if(!suppl) throw dlerror();
        object *obj = suppl();
        globals->define(u->bind,obj,true,true);
        function_container* ptr;
        if(obj->type == NATIVE_CALLABLE_T)
            globals->add_native(DCAST(callable*,obj));
        else if(DCAST_ASNCHK(ptr,function_container*,obj))
            globals->add_natives(ptr->declare_natives());

    } catch (char *what) {
        string &s = *new string();
        s += "Couldn't load the dynamic library: ";
        s += what;
        throw runtime_exception(u->where,s);
    }

}

void interpreter::visit_null_stmt(null_stmt *n) {}

void interpreter::visit_context_stmt(context_stmt *c) {
    runtime_environment *previous = environment;
    context *ctx = new context(previous);
    environment = ctx;
    for (stmt *s : c->body) {
        execute(s);
    }
    environment = previous;
    environment->define(c->name, ctx, c->is_global, c->isfinal);
}

interpreter::interpreter() : stack_trace(*new vector<stack_call *>()) {
    dsv = new default_stmt_visitor();
    dev = new default_expr_visitor();
}

void interpreter::interpret(vector<stmt *> &statements) {
    try {
        stmt *s;
        int i;
        for (i = 0; i < statements.size(); i++) {
            s = statements[i];
            if (s == nullptr) continue;
            execute(s);
        }
    } catch (runtime_exception e) {
        errors::runtime_error(e, stack_trace);
    } catch (return_signal &s) {
        runtime_exception e(s.keyword, *new string("return statements can only be used inside of functions"));
        errors::runtime_error(e, stack_trace);
    } catch (continue_signal &s) {
        runtime_exception e(s.keyword, *new string("continue statements can only be used inside of loops"));
        errors::runtime_error(e, stack_trace);
    } catch (break_signal &s) {
        runtime_exception e(s.keyword, *new string("break statements can only be used inside of loops"));
        errors::runtime_error(e, stack_trace);
    }
    if(errors::had_runtime_error && !lns::prompt) std::exit(RUNTIME_ERROR);
}

object *interpreter::clone_or_keep(object *obj, const expr_type type, const token &where) {
    switch(type){
        case INCREMENT_EXPR_T:
        case DECREMENT_EXPR_T:
        case BINARY_EXPR_T:
        case CALL_EXPR_T:
        case LITERAL_EXPR_T:
        case UNARY_EXPR_T:
        case NULL_EXPR_T:
        case ASSIGN_EXPR_T:
        case ASSIGN_MAP_FIELD_EXPR_T:
        case CONTEXT_ASSIGN_EXPR_T:
            return obj;
    }
    object* ret = obj->clone();
    if(ret == nullptr) throw runtime_exception(where,*new string(obj->type == CONTEXT_T ? "illegal assignment: context" : "illegal assignment: callable"));
    return ret;
}


const int lns::function::arity() const {
    return static_cast<int>(declaration->parameters.size());
}

object *lns::function::call(vector<object *> &args) {
    runtime_environment *env = new runtime_environment(retr_globals(i));
    int j;
    for (j = 0; j < declaration->parameters.size(); j++) {
        env->define(const_cast<token &>(declaration->parameters.at(j)), args.at(j), false, false);
    }
    try {
        i->execute_block(declaration->body, env);
    } catch (return_signal &r) {
        return r.value;
    }
    return lns::GET_DEFAULT_NULL();
}

const string &lns::function::name() const {
    return declaration->name.lexeme;
}

string lns::function::str() const {
    return callable::str();
}

lns::function::function(interpreter *i, const lns::function_stmt *f) : i(i), declaration(f), callable(false) {}

runtime_environment *lns::retr_globals(interpreter *i) {
    return i->globals;
}
