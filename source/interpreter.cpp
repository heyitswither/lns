//
// Created by lorenzo on 11/30/17.
//
#include "interpreter.h"
#include "errors.h"
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

bool interpreter::is_bool_true_eq(object *o) {
    if(!(o->type == BOOL_T)) return false;
    return DCAST(bool_o *,o)->value;
}

void interpreter::execute_block(vector<shared_ptr<stmt>> stmts, runtime_environment *env) {
    if (stmts.empty()) return;
    runtime_environment *previous = this->environment;
    this->environment = env;
    int i;
    try {
        for (i = 0; i < stmts.size(); ++i) {
            execute(stmts[i].get());
        }
        this->environment = previous;
    } catch (return_signal &s) {
        this->environment = previous;
        throw s;
    }
}

object *interpreter::visit_member_expr(member_expr *c) {
    object *o = evaluate(c->container_name.get());
    if (o->type == CONTEXT_T) {
        return DCAST(context*, o)->get(c->member_identifier, c->file);
    }else if(o->type == EXCEPTION_T){
        auto DCAST_ASN(cast,incode_exception*,o);
        object* o2 = cast->get(const_cast<string&>(c->member_identifier->lexeme));
        if(o2 == nullptr) throw runtime_exception(c->member_identifier,EXCEPTION_NO_MEMBER(cast->message,c->member_identifier->lexeme));
        return o2;
    }
    throw runtime_exception(c->member_identifier, OBJECT_NOT_CONTAINER);
}

object *interpreter::visit_member_assign_expr(member_assign_expr *c) {
    object *o = evaluate(c->container_name.get());
    if (o->type != CONTEXT_T) {
        throw runtime_exception(c->member_identifier, OBJECT_NOT_CONTEXT);
    }
    object *value = evaluate(c->value.get());
    dynamic_cast<context *>(o)->assign(c->member_identifier, c->op,
                                       clone_or_keep(value, c->value->type, c->member_identifier), c->file);
    return value;
}

object *interpreter::visit_assign_expr(assign_expr *a) {
    object *value = evaluate(a->value.get());
    environment->assign(a->name, a->op, clone_or_keep(value, a->value->type, a->name), a->file);
    return value;
}

object *interpreter::visit_binary_expr(binary_expr *b) {
    object *left;
    left = evaluate(b->left.get());
    if (left == nullptr)
        left = lns::GET_DEFAULT_NULL();
    object *right = evaluate(b->right.get());
    if (right == nullptr) {
        right = lns::GET_DEFAULT_NULL();
    }
    token_type type = b->op->type;
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
            case SLASH:
                return (*left / *right);
            case STAR:
                return (*left * *right);
            case HAT:
                return (*left ^ *right);
            default:
                break;
        }
    } catch (const char* p) {
        throw runtime_exception(b->op, p);
    }
    // unreachable
    return nullptr;
}

object *interpreter::visit_call_expr(call_expr *c) {
    object *callee = evaluate(c->callee.get());
    vector<object *> args;
    callable *function = dynamic_cast<callable *>(callee);
    if (function == nullptr) throw runtime_exception(c->paren, INVALID_CALL_TARGET);
    int i = 0;
    for (; i < c->args.size(); i++) {
        args.push_back(evaluate(c->args[i].get()));
    }
    int total = function->arity().parameters.size();
    int optional = function->arity().optional();
    int required = total - optional;
    if (args.size() < required || args.size() > total) {
        throw runtime_exception(c->paren, total == required ? WRONG_ARG_NR(required,args.size()) : WRONG_ARG_NR_BETWEEN(required, total,args.size()));
    }
    stack_trace.push_back(
            new stack_call(c->paren->filename, c->paren->line, function->name(), globals->is_native(function)));
    if(stack_trace.size() >= RECURSION_LIMIT){
        throw runtime_exception(c->file,c->line,RECURSION_LIMIT_EXCEEDED);
    }
    object * p;
    try{
        p = function->call(args);
        stack_trace.pop_back();
    }catch(incode_exception& ex){
        ++ex.calls_bypassed;
        throw ex;
    }
    return p;
}

object *interpreter::visit_grouping_expr(grouping_expr *g) {
    return evaluate(g->expression.get());
}

object *interpreter::visit_literal_expr(literal_expr *l) {
    return const_cast<object *>(l->value);
}

object *interpreter::visit_unary_expr(unary_expr *u) {//
    object *right = evaluate(u->right.get());
    try {
        switch (u->op->type) {
            case PLUS_PLUS:
                return u->location == PREFIX ? ++(*right) : (*right)++;
            case MINUS_MINUS:
                return u->location == PREFIX ? --(*right) : (*right)--;
            case NOT:
                return new bool_o(is_bool_true_eq(!*right));
            case MINUS:
                return (-*right);
            default:
                break;
        }
    } catch (const char *p) {
        throw runtime_exception(u->op, p);
    }
    //unreachable
    return nullptr;
}

object *interpreter::visit_variable_expr(variable_expr *v) {
    return environment->get(v->name, v->file);
}

object *interpreter::visit_sub_script_expr(sub_script_expr *m) {
    object *key = evaluate(m->key.get());
    object *eval = evaluate(m->name.get());
    if (eval->type == ARRAY_T) {
        return get_map_field(const_cast<token *>(m->where), eval, key);
    } else if (eval->type == STRING_T) {
        string evstring = eval->str();
        if (key->type != NUMBER_T)
            throw runtime_exception(m->where, KEY_MUST_BE_NUMBER);
        int i = static_cast<int>(dynamic_cast<number_o *>(key)->value);
        if (evstring.size() > i) return new string_o(*new string() + evstring.at(i));
        else return new null_o();
    } else {
        throw runtime_exception(m->where, INVALID_OPERAND_FOR_SUBSCRIPT);
    }
}

object *lns::interpreter::get_map_field(token *where, object *obj, object *key) {
    auto DCAST_ASN(map,array_o*,obj);
    if(key->type != NUMBER_T) throw runtime_exception(where, KEY_MUST_BE_NUMBER);
    auto DCAST_ASN(nr,number_o*,key);
    if (!map->contains_key(nr->value)) return new null_o();
    return map->values[nr->value];
}

object *interpreter::assign_map_field(const token *where, object *obj, const token_type op, number_o *key, object *v) {
    array_o *map;
    if ((map = dynamic_cast<array_o *>(obj)) == nullptr) {
        throw runtime_exception(where, OBJECT_NOT_AN_ARRAY);
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
            default:
                break;
        }
        return map->values[key->value];
    } catch (string &s) {
        throw runtime_exception(where, s.c_str());
    }
}

object *interpreter::visit_sub_script_assign_expr(sub_script_assign_expr *a) {
    object *key = evaluate(a->key.get());
    object *value = evaluate(a->value.get());
    number_o* nr;
    if(DCAST_ASNCHK(nr,number_o*,key))
        return assign_map_field(a->where, evaluate(a->name.get()), a->op, nr, value);
    else
        throw runtime_exception(a->where,KEY_MUST_BE_NUMBER);
}

object *interpreter::visit_null_expr(null_expr *n) {
    return lns::GET_DEFAULT_NULL();
}


void interpreter::visit_block_stmt(block_stmt *b) {
    shared_ptr<runtime_environment> env = make_shared<runtime_environment>(this->environment);
    execute_block(b->statements, env.get());
}

void interpreter::visit_s_for_each_stmt(s_for_each_stmt *s) {
    object *container = evaluate(s->container.get());
    if(container->type == ARRAY_T){
        auto DCAST_ASN(array,array_o*,container);
        environment->define(s->identifier,new null_o(),false,V_UNSPECIFIED,s->file);
        for(const auto &a : array->values){
            environment->assign(s->identifier, token_type::EQUAL, a.second, s->file);
            execute(s->body.get());
        }
        environment->clear_var(s->identifier);
    }else{
        throw runtime_exception(s->identifier,OBJECT_NOT_AN_ARRAY);
    }
}


void interpreter::visit_s_for_stmt(s_for_stmt *s) {
    execute(s->init.get());
    while (is_bool_true_eq(evaluate(s->condition.get()))) {
        execute(s->body.get());
        evaluate(s->increment.get());
    }
}

void interpreter::visit_expression_stmt(expression_stmt *e) {
    object *o = evaluate(e->exprs.get());
    if (!lns::prompt) return;
    expr_type type = e->exprs->type;
    switch (type) {
        case ASSIGN_EXPR_T:
        case CALL_EXPR_T:
        case NULL_EXPR_T:
        case SUB_SCRIPT_ASSIGN_EXPR_T:
        case MEMBER_ASSIGN_EXPR_T:
            return;
        case UNARY_EXPR_T:
            token_type op = DCAST(const unary_expr*, e->exprs.get())->op->type;
            if(op == PLUS_PLUS || op == MINUS_MINUS) return;

    }
    cout << o->str() << endl;
}

void interpreter::visit_function_stmt(function_stmt *f) {
    callable *func = new function(this, f);
    environment->define(const_cast<token *>(f->name), func, true, f->vis,f->file);
}

void interpreter::visit_if_stmt(if_stmt *i) {
    object *eval = evaluate(i->condition.get());
    //boolean definitions for the language are in function is_bool_true_eq()
    if (is_bool_true_eq(eval)) {
        execute(i->thenBranch.get());
        return;
    }
    if (i->elseBranch != nullptr && i->elseBranch->type != NULL_STMT_T) {
        execute(i->elseBranch.get());
    }
}

void interpreter::visit_return_stmt(return_stmt *r) {
    object *value = lns::GET_DEFAULT_NULL();
    if (r->value->type != NULL_EXPR_T) value = evaluate(r->value.get());
    throw return_signal(value, r->keyword);
}

void interpreter::visit_break_stmt(break_stmt *b) {
    throw break_signal(b->keyword);
}

void interpreter::visit_continue_stmt(continue_stmt *c) {
    throw continue_signal(c->keyword);
}

void interpreter::visit_var_stmt(var_stmt *v) {
    if (v->initializer->type != NULL_EXPR_T) {
        object *val = evaluate(v->initializer.get());
        environment->define(const_cast<token *>(v->name), clone_or_keep(val, v->initializer->type, v->name), v->isfinal,
                            v->vis, v->file);
    }else{
        environment->define(const_cast<token *>(v->name), new null_o, v->isfinal, v->vis,v->file);
    }
}

void interpreter::visit_s_while_stmt(s_while_stmt *s) {
    object *o;
    while (is_bool_true_eq(o = evaluate(s->condition.get()))) {
        try {
            execute(s->body.get());
        } catch (break_signal b) {
            break;
        } catch (continue_signal s) {
            continue;
        }
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
        globals->define(u->bind,obj,true,u->vis,u->file);
        function_container* ptr;
        if(obj->type == NATIVE_CALLABLE_T)
            globals->add_native(DCAST(callable*,obj));
        else if(DCAST_ASNCHK(ptr,function_container*,obj))
            globals->add_natives(ptr->declare_natives());
    } catch (char *what) {
        throw runtime_exception(u->where,COULD_NOT_LOAD_DL(what));
    }

}

void interpreter::visit_null_stmt(null_stmt *n) {}

void interpreter::visit_context_stmt(context_stmt *c) {
    runtime_environment *previous = environment;
    context *ctx = new context(previous);
    environment = ctx;
    for (shared_ptr<stmt> s : c->body) {
        execute(s.get());
    }
    environment = previous;
    environment->define(c->name, ctx, c->isfinal,c->vis, c->file);
}

interpreter::interpreter() : stack_trace(*new vector<stack_call *>()){
    dsv = new default_stmt_visitor();
    dev = new default_expr_visitor();
}

void interpreter::interpret(vector<shared_ptr<stmt>> &statements) {
    try {
        int i;
        shared_ptr<stmt> s;
        for (i = 0; i < statements.size(); i++) {
            s = statements[i];
            if (s == nullptr) continue;
            execute(s.get());
        }
    } catch (runtime_exception& e) {
        errors::runtime_error(e, stack_trace);
    } catch (return_signal &s) {
        runtime_exception e(s.keyword, CAN_ONLY_BE_USED_IN("return statements","functions"));
        errors::runtime_error(e, stack_trace);
    } catch (continue_signal &s) {
        runtime_exception e(s.keyword, CAN_ONLY_BE_USED_IN("continue statements","loops"));
        errors::runtime_error(e, stack_trace);
    } catch (break_signal &s) {
        runtime_exception e(s.keyword, CAN_ONLY_BE_USED_IN("break statements","loops"));
        errors::runtime_error(e, stack_trace);
    }
    if(errors::had_runtime_error && !lns::prompt) std::exit(errors::exit_status);
}

object *interpreter::clone_or_keep(object *obj, const expr_type type, const token *where) {
    switch(type){
        case BINARY_EXPR_T:
        case CALL_EXPR_T:
        case UNARY_EXPR_T:
        case NULL_EXPR_T:
        case ASSIGN_EXPR_T:
        case SUB_SCRIPT_ASSIGN_EXPR_T:
        case MEMBER_ASSIGN_EXPR_T:
            return obj;
        default: break;
    }
    object* ret = obj->clone();
    if(ret == nullptr) throw runtime_exception(where,ILLEGAL_ASSIGNMENT);
    return ret;
}

object *interpreter::visit_array_expr(array_expr *a) {
    array_o *array = new array_o;
    number_o* key;
    object* obj;
    for(auto& pair : a->pairs){
        if (DCAST_ASNCHK(key, number_o*, evaluate(pair.first.get()))) {
            obj = evaluate(pair.second.get());
            array->values[key->value] = obj;
        }else{
            throw runtime_exception(a->open_brace,KEY_MUST_BE_NUMBER);
        }
    }
    return array;
}

void interpreter::visit_exception_decl_stmt(exception_decl_stmt *e) {
    try{
        environment->define(e->name,new exception_definition(e->file,e->line,e->name->lexeme,e->identifiers),true,e->vis,e->file);
    }catch(exception_definition& ptr){
        throw runtime_exception(e->name,EXCEPTION_PREVIOUSLY_DEFIDED(ptr.def_file,ptr.def_line));
    }
}

void interpreter::visit_raise_stmt(raise_stmt *r) {
    exception_definition* exc;
    if ((exc = dynamic_cast<exception_definition *>(evaluate(r->name.get()))) == nullptr) {
        throw runtime_exception(r->where,RAISE_NOT_EXCEPTION);
    }
    map<string,object*> fields = *new map<string,object*>();
    for(auto& pair : r->fields)
        if(exc->fields.find(pair.first) == exc->fields.end())
            throw runtime_exception(r->where,EXCEPTION_NO_MEMBER(exc->name,pair.first));
        else
            fields.insert(*new std::pair<string, object *>(pair.first, evaluate(pair.second.get())));

    for(auto& key : exc->fields)
        if(fields.find(key) == fields.end()) fields[key] = new null_o;

    throw *new incode_exception(r->where,exc->name,fields);
}

void interpreter::visit_handle_stmt(handle_stmt *h) {} //this should remain empty

void interpreter::visit_begin_handle_stmt(begin_handle_stmt *b) {
    try{
        execute_block(b->body, make_shared<runtime_environment>(environment).get());
    }catch(incode_exception& e){
        bool found = false;
        exception_definition *def;
        for(auto& handle : b->handles){
            if ((DCAST_ASN(def, exception_definition*, evaluate(const_cast<expr *>(handle->exception_name.get())))) ==
                nullptr) {
                throw runtime_exception(handle->bind,
                                        HANDLE_NOT_EXCEPTION);
            }
            if(def->name == e.message) {
                found = true;
                for(int i = 0; i < e.calls_bypassed; i++)
                    stack_trace.pop_back();
                runtime_environment *env = make_shared<runtime_environment>(environment).get();
                if(handle->bind->type != UNRECOGNIZED) env->define(handle->bind,&e,true,V_UNSPECIFIED,b->file);
                execute_block(handle->block,env);
                break;
            }
        }
        if(!found) throw e;
    }
}

void interpreter::visit_constructor_stmt(constructor_stmt *c) {

}

void interpreter::visit_class_decl_stmt(class_decl_stmt *c) {

}

const parameter_declaration& lns::function::arity() const {
    return declaration->parameters;
}

object *lns::function::call(vector<object *> &args) {
    shared_ptr<runtime_environment> env = make_shared<runtime_environment>(i->environment);
    int j;
    object* o;
    for (j = 0; j < declaration->parameters.parameters.size(); j++) {
        o = j < args.size() ? args.at(j) : static_cast<object *>(new lns::null_o);
        env->define(declaration->parameters.parameters.at(j).name, o, false, V_UNSPECIFIED,declaration->file);
    }
    try {
        i->execute_block(declaration->body, env.get());
    } catch (return_signal &r) {
        return r.value;
    }
    return lns::GET_DEFAULT_NULL();
}

const string &lns::function::name() const {
    return declaration->name->lexeme;
}

string lns::function::str() const {
    return callable::str();
}

lns::function::function(interpreter *i, const lns::function_stmt *f) : i(i), declaration(f), callable(false) {}

