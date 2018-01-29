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

shared_ptr<object> interpreter::evaluate(expr *e) {
    if (e == nullptr)
        return make_shared<null_o>();
    auto o = e->accept(this);
    return o == nullptr ? make_shared<null_o>() : o;
}

bool interpreter::is_bool_true_eq(object *o) {
    if(!(o->type == BOOL_T)) return false;
    return DCAST(bool_o *,o)->value;
}

void interpreter::execute_block(vector<shared_ptr<stmt>> stmts, std::shared_ptr<runtime_environment> env) {
    if (stmts.empty()) return;
    std::shared_ptr<runtime_environment> previous = this->environment;
    this->environment = env;
    int i;
    try {
        for (i = 0; i < stmts.size(); ++i) {
            execute(&*stmts[i]);
        }
        this->environment = previous;
    } catch (return_signal &s) {
        this->environment = previous;
        throw s;
    }
}

shared_ptr<object> interpreter::visit_member_expr(member_expr *c) {
    auto o = evaluate(c->container_name.get());
    if (o->type == CONTEXT_T) {
        return DCAST(context*, o.get())->get(c->member_identifier, c->file);
    }else if(o->type == EXCEPTION_T){
        auto DCAST_ASN(cast, incode_exception*, o.get());
        std::shared_ptr<object> o2 = cast->get(const_cast<string &>(c->member_identifier->lexeme));
        if(o2 == nullptr) throw runtime_exception(c->member_identifier,EXCEPTION_NO_MEMBER(cast->message,c->member_identifier->lexeme));
        return o2;
    } else if (o->type == INSTANCE_T) {
        return DCAST(instance_o*, o.get())->get(c->member_identifier, c->file);
    }
    throw runtime_exception(c->member_identifier, OBJECT_NOT_CONTAINER);
}

shared_ptr<object> interpreter::visit_member_assign_expr(member_assign_expr *c) {
    auto o = evaluate(c->container_name.get());
    auto value = evaluate(c->value.get());
    if (o->type == CONTEXT_T) {
        DCAST(context*, o.get())->assign(c->member_identifier, c->op,
                                         clone_or_keep(value, c->value->type, c->member_identifier), c->file);

    } else if (o->type == INSTANCE_T) {
        DCAST(instance_o*, o.get())->assign_field(c->member_identifier, c->op,
                                                  clone_or_keep(value, c->value->type, c->member_identifier), c->file);
    } else throw runtime_exception(c->member_identifier, OBJECT_NOT_AN_ASSIGNABLE_CONTAINER);
    return value;
}

shared_ptr<object> interpreter::visit_assign_expr(assign_expr *a) {
    auto value = evaluate(a->value.get());
    environment->assign(a->name, a->op, clone_or_keep(value, a->value->type, a->name), a->file);
    return value;
}

shared_ptr<object> interpreter::visit_binary_expr(binary_expr *b) {
    auto left = evaluate(b->left.get());
    if (left == nullptr)
        left = lns::GET_DEFAULT_NULL();
    auto right = evaluate(b->right.get());
    if (right == nullptr) {
        right = lns::GET_DEFAULT_NULL();
    }
    token_type type = b->op->type;
    try {
        switch (type) {
            case AND:
                return make_shared<bool_o>(*left && *right);
            case NAND:
                return make_shared<bool_o>(!(*left && *right));
            case NOR:
                return make_shared<bool_o>(!(*left || *right));
            case OR:
                return make_shared<bool_o>(*left || *right);
            case XOR:
                return make_shared<bool_o>((*left && *!(*right)) || (*!(*left) && *right));
            case EQUAL_EQUAL:
                return make_shared<bool_o>(((*left) == (*right)));
            case BANG_EQUAL:
                return make_shared<bool_o>(!((*left) == (*right)));
            case GREATER:
                return make_shared<bool_o>(*left > *right);
            case GREATER_EQUAL:
                return make_shared<bool_o>(*left >= *right);
            case LESS:
                return make_shared<bool_o>(*left < *right);
            case LESS_EQUAL:
                return make_shared<bool_o>(*left <= *right);
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

shared_ptr<object> interpreter::visit_call_expr(call_expr *c) {
    auto callee = evaluate(c->callee.get());
    vector<shared_ptr<object>> args;
    callable *function = dynamic_cast<callable *>(callee.get());
    if (function == nullptr) throw runtime_exception(c->paren, INVALID_CALL_TARGET);
    check_params(c->paren, function->arity(), c->args.size());
    int i = 0;
    for (; i < c->args.size(); i++) {
        args.push_back(evaluate(c->args[i].get()));
    }
    stack_trace.push_back(
            new stack_call(c->paren->filename, c->paren->line, function->name(), globals->is_native(function)));
    if(stack_trace.size() >= RECURSION_LIMIT){
        throw runtime_exception(c->file,c->line,RECURSION_LIMIT_EXCEEDED);
    }
    try{
        std::shared_ptr<object> p = function->call(args);
        stack_trace.pop_back();
        return p;
    }catch(incode_exception& ex){
        ++ex.calls_bypassed;
        throw ex;
    }
}

shared_ptr<object> interpreter::visit_grouping_expr(grouping_expr *g) {
    return evaluate(g->expression.get());
}

shared_ptr<object> interpreter::visit_literal_expr(literal_expr *l) {
    return l->value;
}

shared_ptr<object> interpreter::visit_unary_expr(unary_expr *u) {//
    auto right = evaluate(u->right.get());
    try {
        switch (u->op->type) {
            case PLUS_PLUS:
                return u->location == PREFIX ? ++(*right) : (*right)++;
            case MINUS_MINUS:
                return u->location == PREFIX ? --(*right) : (*right)--;
            case NOT:
                return make_shared<bool_o>(!is_bool_true_eq(right.get()));
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

shared_ptr<object> interpreter::visit_variable_expr(variable_expr *v) {
    return environment->get(v->name, v->file);
}

shared_ptr<object> interpreter::visit_sub_script_expr(sub_script_expr *m) {
    auto key = evaluate(m->key.get());
    auto eval = evaluate(m->name.get());
    if (eval->type == ARRAY_T) {
        return get_map_field(const_cast<token *>(m->where), eval, key);
    } else if (eval->type == STRING_T) {
        string evstring = eval->str();
        if (key->type != NUMBER_T)
            throw runtime_exception(m->where, KEY_MUST_BE_NUMBER);
        int i = static_cast<int>(dynamic_cast<number_o *>(key.get())->value);
        if (evstring.size() > i) return make_shared<string_o>(string(1, evstring.at(i)));
        else return make_shared<null_o>();
    } else {
        throw runtime_exception(m->where, INVALID_OPERAND_FOR_SUBSCRIPT);
    }
}

std::shared_ptr<object>
lns::interpreter::get_map_field(token *where, std::shared_ptr<object> obj, std::shared_ptr<object> key) {
    auto DCAST_ASN(map, array_o*, obj.get());
    if(key->type != NUMBER_T) throw runtime_exception(where, KEY_MUST_BE_NUMBER);
    auto DCAST_ASN(nr, number_o*, key.get());
    if (!map->contains_key(nr->value)) return make_shared<null_o>();
    return map->values[nr->value];
}

std::shared_ptr<object>
interpreter::assign_map_field(const token *where, std::shared_ptr<object> obj, const token_type op, const number_o *key,
                              std::shared_ptr<object> v) {
    array_o *map;
    if ((map = dynamic_cast<array_o *>(obj.get())) == nullptr) {
        throw runtime_exception(where, OBJECT_NOT_AN_ARRAY);
    }
    try {
        std::shared_ptr<object> value = clone_or_keep(v, static_cast<const expr_type>(op), where);
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

shared_ptr<object> interpreter::visit_sub_script_assign_expr(sub_script_assign_expr *a) {
    std::shared_ptr<object> key = evaluate(a->key.get());
    std::shared_ptr<object> value = evaluate(a->value.get());
    number_o* nr;
    if (DCAST_ASNCHK(nr, number_o*, key.get()))
        return assign_map_field(a->where, evaluate(a->name.get()), a->op, nr, value);
    else
        throw runtime_exception(a->where,KEY_MUST_BE_NUMBER);
}

shared_ptr<object> interpreter::visit_null_expr(null_expr *n) {
    return make_shared<null_o>();
}


void interpreter::visit_block_stmt(block_stmt *b) {
    shared_ptr<runtime_environment> env = make_shared<runtime_environment>(this->environment);
    execute_block(b->statements, env);
}

void interpreter::visit_s_for_each_stmt(s_for_each_stmt *s) {
    std::shared_ptr<object> container = evaluate(s->container.get());
    if(container->type == ARRAY_T){
        auto DCAST_ASN(array, array_o*, container.get());
        environment->define(s->identifier, make_shared<null_o>(), false, V_UNSPECIFIED, s->file);
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
    while (is_bool_true_eq(evaluate(s->condition.get()).get())) {
        execute(s->body.get());
        evaluate(s->increment.get());
    }
}

void interpreter::visit_expression_stmt(expression_stmt *e) {
    std::shared_ptr<object> o = evaluate(e->exprs.get());
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
    shared_ptr<object> func = make_shared<function>(this, f);
    environment->define(const_cast<token *>(f->name), func, true, f->vis,f->file);
}

void interpreter::visit_if_stmt(if_stmt *i) {
    std::shared_ptr<object> eval = evaluate(i->condition.get());
    //boolean definitions for the language are in function is_bool_true_eq()
    if (is_bool_true_eq(eval.get())) {
        execute(i->thenBranch.get());
        return;
    }
    if (i->elseBranch != nullptr && i->elseBranch->type != NULL_STMT_T) {
        execute(i->elseBranch.get());
    }
}

void interpreter::visit_return_stmt(return_stmt *r) {
    throw return_signal(evaluate(r->value.get()), r->keyword);
}

void interpreter::visit_break_stmt(break_stmt *b) {
    throw break_signal(b->keyword);
}

void interpreter::visit_continue_stmt(continue_stmt *c) {
    throw continue_signal(c->keyword);
}

void interpreter::visit_var_stmt(var_stmt *v) {
    if (v->initializer->type != NULL_EXPR_T) {
        std::shared_ptr<object> val = evaluate(v->initializer.get());
        environment->define(const_cast<token *>(v->name), clone_or_keep(val, v->initializer->type, v->name), v->isfinal,
                            v->vis, v->file);
    }else{
        environment->define(const_cast<token *>(v->name), make_shared<null_o>(), v->isfinal, v->vis, v->file);
    }
}

void interpreter::visit_s_while_stmt(s_while_stmt *s) {
    object *o;
    while (is_bool_true_eq(o = evaluate(s->condition.get()).get())) {
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
        typedef shared_ptr<object> supplier();
        supplier* suppl = (supplier*)dlsym(handler,"supplier");
        if(!suppl) throw dlerror();
        shared_ptr<object> obj = suppl();
        globals->define(u->bind,obj,true,u->vis,u->file);
        function_container* ptr;
        if(obj->type == NATIVE_CALLABLE_T)
            globals->add_native(DCAST(callable*, obj.get()));
        else if (DCAST_ASNCHK(ptr, function_container*, obj.get()))
            globals->add_natives(ptr->declare_natives());
    } catch (char *what) {
        throw runtime_exception(u->where,COULD_NOT_LOAD_DL(what));
    }

}

void interpreter::visit_null_stmt(null_stmt *n) {}

void interpreter::visit_context_stmt(context_stmt *c) {
    std::shared_ptr<runtime_environment> previous = environment;
    shared_ptr<context> ctx = make_shared<context>(previous);
    environment = ctx;
    for (shared_ptr<stmt> s : c->body) {
        execute(s.get());
    }
    environment = previous;
    environment->define(c->name, ctx, c->isfinal,c->vis, c->file);
}

interpreter::interpreter() : stack_trace(vector<stack_call *>()) {}

void interpreter::interpret(vector<shared_ptr<stmt>> &statements) {
    interpret_stmts0(statements);
    this->terminate0();
}

void interpreter::interpret_stmts0(vector<shared_ptr<stmt>> &statements) {
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
}

void interpreter::terminate0() {
    if(errors::had_runtime_error && !lns::prompt) std::exit(errors::exit_status);
}

shared_ptr<object> interpreter::clone_or_keep(std::shared_ptr<object> obj, const expr_type type, const token *where) {
    switch(type){
        case BINARY_EXPR_T:
        case CALL_EXPR_T:
        case UNARY_EXPR_T:
        case NULL_EXPR_T:
        case ASSIGN_EXPR_T:
        case SUB_SCRIPT_ASSIGN_EXPR_T:
        case MEMBER_ASSIGN_EXPR_T:
        case THIS_EXPR_T:
        case NEW_EXPR_T:
            return obj;
        case VARIABLE_EXPR_T:
            if (obj->type == INSTANCE_T) return obj;
            break;
        default: break;
    }
    std::shared_ptr<object> ret = obj->clone();
    if(ret == nullptr) throw runtime_exception(where,ILLEGAL_ASSIGNMENT);
    return ret;
}

shared_ptr<object> interpreter::visit_array_expr(array_expr *a) {
    std::shared_ptr<array_o> array = make_shared<array_o>();
    number_o* key;
    std::shared_ptr<object> obj;
    for(auto& pair : a->pairs){
        if (DCAST_ASNCHK(key, number_o*, evaluate(pair.first.get()).get())) {
            obj = evaluate(pair.second.get());
            array->values.insert_or_assign(key->value, obj);
        }else{
            throw runtime_exception(a->open_brace,KEY_MUST_BE_NUMBER);
        }
    }
    return array;
}

void interpreter::visit_exception_decl_stmt(exception_decl_stmt *e) {
    try{
        environment->define(e->name,
                            make_shared<exception_definition>(e->file, e->line, e->name->lexeme, e->identifiers), true,
                            e->vis, e->file);
    }catch(exception_definition& ptr){
        throw runtime_exception(e->name,EXCEPTION_PREVIOUSLY_DEFIDED(ptr.def_file,ptr.def_line));
    }
}

void interpreter::visit_raise_stmt(raise_stmt *r) {
    exception_definition* exc;
    if ((exc = dynamic_cast<exception_definition *>(evaluate(r->name.get()).get())) == nullptr) {
        throw runtime_exception(r->where,RAISE_NOT_EXCEPTION);
    }
    map<string, std::shared_ptr<object> > fields;
    for(auto& pair : r->fields)
        if(exc->fields.find(pair.first) == exc->fields.end())
            throw runtime_exception(r->where,EXCEPTION_NO_MEMBER(exc->name,pair.first));
        else
            fields.insert(std::pair(pair.first, evaluate(pair.second.get())));

    for(auto& key : exc->fields)
        if (fields.find(key) == fields.end()) fields[key] = make_shared<null_o>();

    throw incode_exception(r->where, exc->name, fields);
}

void interpreter::visit_handle_stmt(handle_stmt *h) {} //this should remain empty

void interpreter::visit_begin_handle_stmt(begin_handle_stmt *b) {
    try{
        execute_block(b->body, make_shared<runtime_environment>(environment));
    }catch(incode_exception& e){
        bool found = false;
        exception_definition *def;
        for(auto& handle : b->handles){
            if ((DCAST_ASN(def, exception_definition*,
                           evaluate(const_cast<expr *>(handle->exception_name.get())).get())) ==
                nullptr) {
                throw runtime_exception(handle->bind,
                                        HANDLE_NOT_EXCEPTION);
            }
            if(def->name == e.message) {
                found = true;
                for(int i = 0; i < e.calls_bypassed; i++)
                    stack_trace.pop_back();
                auto env = make_shared<runtime_environment>(environment);
                if (handle->bind->type != UNRECOGNIZED)
                    env->define(handle->bind, e.clone(), true, V_UNSPECIFIED, b->file);
                execute_block(handle->block, env);
                break;
            }
        }
        if(!found) throw e;
    }
}

void interpreter::visit_constructor_stmt(constructor_stmt *c) {}

void interpreter::visit_class_decl_stmt(class_decl_stmt *c) {
    auto class_ = make_shared<class_definition>(c->name->lexeme, c->variables, c->methods, c->file);
    vector<shared_ptr<constructor>> constructors;
    for (auto &item : c->constructors)
        constructors.emplace_back(make_shared<constructor>(this, class_, item.get()));
    class_->constructors = constructors;
    environment->define(c->name, class_, true, c->vis, c->file);
}

shared_ptr<object> interpreter::visit_new_expr(new_expr *n) {
    auto DCAST_ASN(class_, class_definition *, evaluate(n->class_.get()).get());
    if (class_ == nullptr) throw runtime_exception(n->keyword, OBJECT_NOT_A_CLASS);
    constructor *cstrt = nullptr;
    for (auto &item : class_->constructors)
        if (check_params(nullptr, item->arity(), n->args.size())) {
            cstrt = item.get();
            break;
        }
    if (!cstrt) throw runtime_exception(n->keyword, CONSTRUCTOR_NOT_FOUND(n->args.size()));
    vector<shared_ptr<object>> args;
    int i = 0;
    for (; i < n->args.size(); i++) {
        args.push_back(evaluate(n->args[i].get()));
    }
    stack_trace.push_back(
            new stack_call(n->keyword->filename, n->keyword->line, cstrt->name(), globals->is_native(cstrt)));
    try {
        std::shared_ptr<object> p = cstrt->call(args);
        stack_trace.pop_back();
        return p;
    } catch (incode_exception &ex) {
        ++ex.calls_bypassed;
        throw ex;
    }
}

bool interpreter::check_params(const token *where, const parameter_declaration &declaration, unsigned long size) {
    auto total = declaration.parameters.size();
    auto optional = declaration.optional();
    auto required = total - optional;
    if (size < required || size > total)
        if (where)
            throw runtime_exception(where,
                                    total == required ? WRONG_ARG_NR(required, size) : WRONG_ARG_NR_BETWEEN(required,
                                                                                                            total,
                                                                                                            size));
        else
            return false;
    return true;
}

shared_ptr<object> interpreter::visit_this_expr(this_expr *t) {
    try {
        token tkn = token(token_type::UNRECOGNIZED, THIS_DEFINITION, make_shared<null_o>(), t->file, t->line);
        return environment->get(&tkn, t->file);
    } catch (runtime_exception &r) {
        throw runtime_exception(t->keyword, THIS_OUTSIDE_OF_CLASS);
    }
}

const parameter_declaration& lns::function::arity() const {
    return declaration->parameters;
}

shared_ptr<object> lns::function::call(vector<shared_ptr<object>> &args) {
    shared_ptr<runtime_environment> env = make_shared<runtime_environment>(i->globals);
    int j;
    shared_ptr<object> o;
    for (j = 0; j < declaration->parameters.parameters.size(); j++) {
        o = j < args.size() ? args.at(j) : declaration->parameters.parameters[j].default_value->accept(i);
        env->define(declaration->parameters.parameters.at(j).name, o, false, V_UNSPECIFIED,declaration->file);
    }
    try {
        i->execute_block(declaration->body, env);
    } catch (return_signal &r) {
        return r.value;
    }
    return lns::GET_DEFAULT_NULL();
}

const string lns::function::name() const {
    return declaration->name->lexeme;
}

string lns::function::str() const {
    stringstream s;
    s << "<" << (this->type == NATIVE_CALLABLE_T ? "native_" : "") << "function@" << static_cast<const void *>(this)
      << ", name: \"" << name() << "\">";
    return s.str();
}

lns::function::function(interpreter *i, const lns::function_stmt *f) : i(i), declaration(f), callable(false) {}


class_definition::class_definition(const string name,
                                   const vector<shared_ptr<var_stmt>> &variables,
                                   const vector<shared_ptr<function_stmt>> &methods, const char *def_file) : name(name),
                                                                                                             function_container(
                                                                                                                     CLASS_DEFINITION_T),
                                                                                                             variables(
                                                                                                                     variables),
                                                                                                             methods(methods),
                                                                                                             def_file(
                                                                                                                     def_file) {}

string class_definition::str() const {
    stringstream s;
    s << "<class@" << static_cast<const void *>(this) << ", name: \"" << name << "\">";
    return s.str();
}

bool class_definition::operator==(const object &o) const {
    return this == &o;
}

shared_ptr<object> class_definition::clone() const {
    return nullptr;
}

set<callable *> class_definition::declare_natives() const {
    return std::set<callable *>();
}

constructor::constructor(interpreter *i, const shared_ptr<class_definition> container,
                         const lns::constructor_stmt *decl)
        : callable(false), container(container), i(i), declaration(decl) {}

const parameter_declaration &constructor::arity() const {
    return declaration->parameters;
}

const string constructor::name() const {
    return container->name + ".constructor";
}

shared_ptr<object> constructor::call(std::vector<std::shared_ptr<object>> &args) {
    shared_ptr<instance_o> instance = make_shared<instance_o>(container, i->globals);
    std::shared_ptr<runtime_environment> prev = i->environment;
    i->environment = instance;

    for (auto &item : container->variables)
        item->accept(i);
    for (auto &item : container->methods)
        instance->define(item->name, make_shared<method>(i, item.get(), instance), true, item->vis, item->file);

    i->environment = prev;

    auto env = make_shared<runtime_environment>(i->environment);
    env->define(THIS_DEFINITION, instance, true, V_UNSPECIFIED, container->def_file);
    int j;
    for (j = 0; j < declaration->parameters.parameters.size(); j++) {
        auto o1 = j < args.size() ? args.at(j) : declaration->parameters.parameters[j].default_value->accept(i);
        env->define(declaration->parameters.parameters.at(j).name, o1, false, V_UNSPECIFIED, declaration->file);
    }
    try {
        i->execute_block(declaration->body, env);
    } catch (return_signal &r) {
        if (r.value->type != NULL_T && !permissive)
            throw runtime_exception(r.keyword, string(RETURN_STMT_VALUE_IN_CONSTRUCTOR));
    }
    return instance;
}

bool constructor::operator==(const object &o) const {
    return this == &o;
}

string constructor::str() const {
    stringstream s;
    s << "<constructor@" << static_cast<const void *>(this) << ", params: " << declaration->parameters.parameters.size()
      << ">";
    return s.str();
}

shared_ptr<object> constructor::clone() const {
    return callable::clone();
}

instance_o::instance_o(shared_ptr<class_definition> class_, std::shared_ptr<runtime_environment> globals) : object(
        INSTANCE_T),
                                                                                                            runtime_environment(
                                                                                                    globals),
                                                                                                            class_(class_) {}

instance_o::instance_o(shared_ptr<class_definition> class_, std::shared_ptr<runtime_environment> globals,
                       map<string, variable> vars)
        : object(INSTANCE_T),
          runtime_environment(
                                                                                                          globals),
          class_(class_) {
    values = vars;
}

string instance_o::str() const {
    stringstream s;
    s << "<" << class_->name << " instance@" << static_cast<const void *>(this) << ">";
    return s.str();
}

shared_ptr<object> instance_o::clone() const {
    return make_shared<instance_o>(this->class_, this->enclosing, this->values);
}

bool instance_o::operator==(const object &o) const {
    return false;
}

shared_ptr<object> instance_o::operator+(const object &o) const {
    return make_shared<string_o>(this->str() + o.str());
}

void
instance_o::assign_field(const token *name, const token_type op, shared_ptr<object> obj, const char *assigning_file) {
    if (contains_key(name->lexeme)) {
        if (values[name->lexeme].is_final) {
            throw runtime_exception(name, VARIABLE_FINAL(name->lexeme));
        }
        //values[name.lexeme]->isfinal = false;
        try {
            variable v = values[name->lexeme];
            if (v.visibility_ == V_LOCAL) {
                if (strcmp(v.def_file, assigning_file) != 0 && strcmp(DEBUGGER_WATCHES_FILENAME, assigning_file) != 0) {
                    throw runtime_exception(name, VARIABLE_NOT_VISIBLE(name->lexeme));
                }
            }
            switch (op) {
                case EQUAL:
                    values[name->lexeme].value = obj;
                    break;
                case PLUS_EQUALS:
                    *v.value += *obj;
                    break;
                case MINUS_EQUALS:
                    *v.value -= *obj;
                    break;
                case STAR_EQUALS:
                    *v.value *= *obj;
                    break;
                case SLASH_EQUALS:
                    *v.value /= *obj;
                    break;
            }
        } catch (const char *s) {
            throw runtime_exception(name, s);
        }
        return;
    }
    if (permissive) { return define(name, obj, false, V_UNSPECIFIED, assigning_file); }
    throw runtime_exception(name, VARIABLE_UNDEFINED(name->lexeme));
}

void instance_o::define(const token *name, std::shared_ptr<object> o, bool is_final, visibility vis,
                        const char *def_file) {
    if (contains_key(name->lexeme)) {
        throw runtime_exception(name, VARIABLE_ALREADY_DEFINED(name->lexeme));
    }
    runtime_environment::define(name->lexeme, o, is_final, vis, def_file);
}

method::method(interpreter *i, const function_stmt *f, shared_ptr<instance_o> instance) : function(i, f),
                                                                                          instance(instance) {}

shared_ptr<object> method::call(std::vector<shared_ptr<object>> &args) {
    shared_ptr<runtime_environment> enclosing = make_shared<runtime_environment>(i->globals);
    shared_ptr<runtime_environment> env = make_shared<runtime_environment>(instance);
    env->define(THIS_DEFINITION, instance, true, V_UNSPECIFIED, declaration->file);
    int j;
    shared_ptr<object> o;
    for (j = 0; j < declaration->parameters.parameters.size(); j++) {
        o = j < args.size() ? args.at(j) : declaration->parameters.parameters[j].default_value->accept(i);
        env->define(declaration->parameters.parameters.at(j).name, o, false, V_UNSPECIFIED, declaration->file);
    }
    try {
        i->execute_block(declaration->body, env);
    } catch (return_signal &r) {
        return r.value;
    }
    return lns::GET_DEFAULT_NULL();
}

string method::str() const {
    stringstream s;
    s << "<" << (this->type == NATIVE_CALLABLE_T ? "native_" : "") << "method@" << static_cast<const void *>(this)
      << ", class: \"" << instance->class_ << "\", name: \"" << name() << "\">";
    return s.str();
}
