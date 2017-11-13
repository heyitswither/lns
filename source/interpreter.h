//
// Created by lorenzo on 10/09/17.
//

#ifndef CPPLNS_INTERPRETER_H
#define CPPLNS_INTERPRETER_H

#include "defs.h"
#include "expr.h"
#include "stmt.h"
#include <vector>
#include "math.h"
#include <ctime>
#include "environments.h"
#include "exceptions.h"
#include <string>
#include <chrono>
#include <thread>
using namespace lns;
using namespace std;
using namespace std::chrono;
namespace lns{
    class callable;
}
namespace natives{
    vector<lns::callable*>* builtin_natives();
}
namespace lns {
    class interpreter;
    runtime_environment * retr_globals(interpreter * i);
    class callable : public object{
    public:
        callable() : object(objtype::CALLABLE_T) {}
        virtual const int arity() = 0;
        virtual const string& name() = 0;
        virtual object* call(lns::interpreter* env, vector<object*> args) = 0;
        bool operator==(const object &o) const override {
            return false;
        }
        string str() override{
            string s = *new string();
            s+="<function '" + name() + "'>";
            return s;
        }
    };
    class function : public callable{
        function_stmt* declaration;
    public:
        const string& name();
        const int arity();
        object* call(interpreter* i, vector<object*> args);
        explicit function(function_stmt *f) : declaration(f), callable(){}

    };
    class interpreter : public expr_visitor, stmt_visitor {
    public:
        virtual void execute(stmt *s) {
            if(s == nullptr) return;
            s->accept(this);
        }
    protected:
        vector<stack_call*> stack_trace = *new vector<stack_call*>();
        object *evaluate(expr *e) {
            if(e == nullptr)
                return lns::GET_DEFAULT_NULL();
            object* o = e->accept(this);
            return o == nullptr ? lns::GET_DEFAULT_NULL() : o;
        }
    private:
        stmt_visitor *dsv;
        expr_visitor *dev;


        void check_bool_operands(const token &op, object *left, object *right) {
            if (left->type == BOOL_T && right->type == BOOL_T) return;
            throw runtime_exception(op, *new string("operands must be booleans"));
        }

        void check_number_operands(const token &op, object *left, object *right) {
            if (left->type == NUMBER_T && right->type == NUMBER_T) return;
            throw runtime_exception(op, *new string("operands must be numbers"));
        }

        void check_number_operand(const token &token, object *o) {
            if (o->type == NUMBER_T)return;
            throw runtime_exception(token, *new string("operand must be a number"));
        }


    public:
        static bool is_bool_true_eq(object *o) {
            if (o->str() == "true") return true;
            if (o->type == NUMBER_T)
                return dynamic_cast<number_o *>(o)->value == 1;
            return false;
        }
        runtime_environment *globals = new runtime_environment();
        runtime_environment *environment = globals;
        void execute_block(vector<stmt*> stmts, runtime_environment* env){
            if(stmts.empty()) return;
            runtime_environment* previous = this->environment;
            this->environment = env;
            int i;
            try{
                for(i = 0; i < stmts.size(); ++i){
                    execute(stmts[i]);
                }
                delete(env);
                this->environment = previous;
            }catch(return_signal s){
                delete(env);
                this->environment = previous;
                throw s;
            }
            //delete env;
        }

        object *visit_context_expr(context_expr *c) override {
            object* o = environment->get(c->context_name);
            if(o->type != CONTEXT_T) {
                string s = *new string();
                s+="object '" + c->context_name.lexeme + "' is not a context.";
                throw runtime_exception(c->context_name,s);
            }
            return dynamic_cast<context*>(o)->get(c->context_identifier);
        }

        object *visit_context_assign_expr(context_assign_expr *c) override {
            object* o = environment->get(c->context_name);
            if(o->type != CONTEXT_T) {
                string s = *new string();
                s+="object '" + c->context_name.lexeme + "' is not a context.";
                throw runtime_exception(c->context_name,s);
            }
            object* value = evaluate(const_cast<expr *>(c->value));
            dynamic_cast<context*>(o)->assign(c->context_identifier,value);
            return value;
        }


        object *visit_increment_expr(increment_expr *i) override {
            return environment->increment(i->name);;
        }

        object *visit_decrement_expr(decrement_expr *d) override {
            return environment->decrement(d->name);
        }

        object *visit_assign_expr(assign_expr *a) override {
            object *value = evaluate(const_cast<expr *>(a->value));
            environment->assign(a->name, value);
            return value;
        } //

        object *visit_binary_expr(binary_expr *b) override {
            object *left = evaluate(const_cast<expr *>(b->left));
            if(left == nullptr)
                left = lns::GET_DEFAULT_NULL();
            object *right = evaluate(const_cast<expr *>(b->right));
            if(right == nullptr){
                right = lns::GET_DEFAULT_NULL();
            }
            token_type type = b->op.type;
            switch (type) {
                case AND:
                    check_bool_operands(b->op, left, right);
                    return (new bool_o(dynamic_cast<bool_o *>(left)->value && (dynamic_cast<bool_o *>(right)->value)));
                case NAND:
                    check_bool_operands(b->op, left, right);
                    return (new bool_o(
                            !(dynamic_cast<bool_o *>(left)->value && (dynamic_cast<bool_o *>(right)->value))));
                case NOR:
                    check_bool_operands(b->op, left, right);
                    return (new bool_o(
                            !(dynamic_cast<bool_o *>(left)->value || (dynamic_cast<bool_o *>(right)->value))));
                case OR:
                    check_bool_operands(b->op, left, right);
                    return (new bool_o(
                            (dynamic_cast<bool_o *>(left)->value || (dynamic_cast<bool_o *>(right)->value))));
                case XOR:
                    check_bool_operands(b->op, left, right);
                    return (new bool_o(
                            (dynamic_cast<bool_o *>(left)->value && !(dynamic_cast<bool_o *>(right)->value)) ||
                            !(dynamic_cast<bool_o *>(left)->value && (dynamic_cast<bool_o *>(right)->value))));
                case XNOR:
                    check_bool_operands(b->op, left, right);
                    return (new bool_o(
                            (dynamic_cast<bool_o *>(left)->value == (dynamic_cast<bool_o *>(right)->value))));
                case EQUAL_EQUAL:
                    return new bool_o(((*left) == (*right)));
                case BANG_EQUAL:
                    return new bool_o(!((*left) == (*right)));
                case GREATER:
                    if (left->type == NUMBER_T && right->type == NUMBER_T) {
                        return new bool_o(
                                dynamic_cast<number_o *>(left)->value > dynamic_cast<number_o *>(right)->value);
                    }
                    if (left->type == NUMBER && right->type == NUMBER) {
                        return new bool_o(
                                dynamic_cast<string_o *>(left)->value > dynamic_cast<string_o *>(right)->value);
                    }
                    throw runtime_exception(b->op, *new string("operands bust be either two numbers or two strings"));
                case GREATER_EQUAL:
                    if (left->type == NUMBER_T && right->type == NUMBER_T) {
                        return new bool_o(
                                dynamic_cast<number_o *>(left)->value >= dynamic_cast<number_o *>(right)->value);
                    }
                    if (left->type == STRING_T && right->type == STRING_T) {
                        return new bool_o(
                                dynamic_cast<string_o *>(left)->value >= dynamic_cast<string_o *>(right)->value);
                    }
                    throw runtime_exception(b->op, *new string("operands bust be either two numbers or two strings"));
                case LESS:
                    if (left->type == NUMBER_T && right->type == NUMBER_T) {
                        return new bool_o(
                                dynamic_cast<number_o *>(left)->value < dynamic_cast<number_o *>(right)->value);
                    }
                    if (left->type == STRING_T && right->type == STRING_T) {
                        return new bool_o(
                                dynamic_cast<string_o *>(left)->value < dynamic_cast<string_o *>(right)->value);
                    }
                    throw runtime_exception(b->op, *new string("operands bust be either two numbers or two strings"));
                case LESS_EQUAL:
                    if (left->type == NUMBER_T && right->type == NUMBER_T) {
                        return new bool_o(
                                dynamic_cast<number_o *>(left)->value <= dynamic_cast<number_o *>(right)->value);
                    }
                    if (left->type == STRING_T && right->type == STRING_T) {
                        return new bool_o(
                                dynamic_cast<string_o *>(left)->value <= dynamic_cast<string_o *>(right)->value);
                    }
                    throw runtime_exception(b->op, *new string("operands bust be either two numbers or two strings"));
                case MINUS:
                    check_number_operands(b->op, left, right);
                    return new number_o(dynamic_cast<number_o *>(left)->value - dynamic_cast<number_o *>(right)->value);
                case PLUS:
                    if (left->type == NUMBER_T && right->type == NUMBER_T) {
                        return new number_o(
                                dynamic_cast<number_o *>(left)->value + dynamic_cast<number_o *>(right)->value);
                    }
                    string s = *new string();
                    s += left->str() + right->str();
                    return new string_o(s);
            }
            if(type == SLASH){
                check_number_operands(b->op, left, right);
                if (dynamic_cast<number_o *>(right)->value == 0) throw runtime_exception(b->op, *new string("dividing by zero"));
                return new number_o(dynamic_cast<number_o *>(left)->value / dynamic_cast<number_o *>(right)->value);
            }
            if(type == STAR){
                check_number_operands(b->op, left, right);
                return new number_o(dynamic_cast<number_o *>(left)->value * dynamic_cast<number_o *>(right)->value);
            }
            if(type == HAT){
                check_number_operands(b->op, left, right);
                if (dynamic_cast<number_o *>(right)->value == 0) return new number_o(1);
                return new number_o(
                        pow(dynamic_cast<number_o *>(left)->value, dynamic_cast<number_o *>(right)->value));
            }
            // unreachable
            return nullptr;
        } //

        object *visit_call_expr(call_expr *c) override {
            object * callee = evaluate(const_cast<expr *>(c->callee));
            vector<object*> args;
            callable * function = dynamic_cast<callable*>(callee);
            if(function == nullptr) throw runtime_exception(c->paren,*new string("target not a callable"));
            int i = 0;
            for(;i<c->args.size();i++){
                args.push_back(evaluate(c->args[i]));
            }
            if(args.size() != function->arity()){
                string s = *new string();
                s+="expected " + to_string(function->arity()) + " but got " + to_string(args.size());
                throw runtime_exception(c->paren,*new string(s.c_str()));
            }
            if(!globals->is_native(function->name())){
                stack_call* call = new stack_call(c->paren.filename, c->paren.line, function->name());
                stack_trace.push_back(call);
            }
            return function->call(this,args);
        } //

        object *visit_grouping_expr(grouping_expr *g) override {
            return evaluate(const_cast<expr *>(g->expression));
        } //

        object *visit_literal_expr(literal_expr *l) override {
            return const_cast<object *>(l->value);
        }//

        object *visit_unary_expr(unary_expr *u) override {//
            object *right = evaluate(const_cast<expr *>(u->right));
            switch (u->op.type) {
                case BANG:
                    return new bool_o(!is_bool_true_eq(right));
                case MINUS:
                    check_number_operand(u->op, right);
                    return new number_o(-(dynamic_cast<number_o *>(right)->value));
            }
            //unreachable
            return nullptr;
        }

        object *visit_variable_expr(variable_expr *v) override {
            return environment->get(v->name);
        } //

        object *visit_map_field_expr(map_field_expr *m) override {
            object *o = evaluate(const_cast<expr *>(m->key));
            string str = o->str();
            return environment->get_map_field(m->name, str);
        } //

        object *visit_assign_map_field_expr(assign_map_field_expr *a) override {
            object *key = evaluate(const_cast<expr *>(a->key));
            object *value = evaluate(const_cast<expr *>(a->value));
            string_o *k = dynamic_cast<string_o *>(key);
            if (k == nullptr) {
                k = new string_o(key->str());
            }
            return environment->assign_map_field(a->name, k, value);
        }//

        object *visit_null_expr(null_expr *n) override {
            return lns::GET_DEFAULT_NULL();
        } //

        void visit_block_stmt(block_stmt *b) override {
            runtime_environment* env = new runtime_environment(environment);
            execute_block(b->statements,env);
        } //
        void visit_s_for_stmt(s_for_stmt *s) override {
            execute(const_cast<stmt *>(s->init));
            while(is_bool_true_eq(evaluate(const_cast<expr *>(s->condition)))){
                execute(const_cast<stmt *>(s->body));
                evaluate(const_cast<expr *>(s->increment));
            }
        }
    public:
        void visit_expression_stmt(expression_stmt *e) override {
            object *o = evaluate((expr *) &(e->exprs));
            if(!lns::prompt) return;
            expr_type type = e->exprs.type;
            switch(type){
                case ASSIGN_EXPR_T:
                case CALL_EXPR_T:
                case NULL_EXPR_T:
                case ASSIGN_MAP_FIELD_EXPR_T:
                case INCREMENT_EXPR_T:
                case DECREMENT_EXPR_T:
                    return;
            }
            cout << o->str() << endl;
        } //

        void visit_function_stmt(function_stmt *f) override {
            callable* func = new function(f);
            environment->define(const_cast<token &>(f->name), func, true, f->isglobal);
        } //

        void visit_if_stmt(if_stmt *i) override {
            object* eval = evaluate(const_cast<expr *>(&i->condition));
            //boolean definitions for the language are in function is_bool_true_eq()
            if(is_bool_true_eq(eval)){
                execute(const_cast<stmt *>(i->thenBranch));
                return;
            }
            if (i->elseBranch != nullptr && i->elseBranch->type != NULL_STMT_T){
                execute(const_cast<stmt *>(i->elseBranch));
            }
        } //

        void visit_return_stmt(return_stmt *r) override {
            object* value = lns::GET_DEFAULT_NULL();
            null_expr* null =  dynamic_cast<null_expr*>(const_cast<expr*>(&r->value));
            if(null == nullptr) value = evaluate(const_cast<expr *>(&r->value));
            if(stack_trace.size() > 0) stack_trace.pop_back();
            throw return_signal(value,r->keyword);
        } //

        void visit_break_stmt(break_stmt *b) override {
            throw break_signal(b->keyword);
        } //

        void visit_continue_stmt(continue_stmt *c) override {
            throw continue_signal(c->keyword);
        } //

        void visit_var_stmt(var_stmt *v) override {
            object* val = lns::GET_DEFAULT_NULL();
            if(v->initializer.type != NULL_STMT_T){
                delete val;
                val = evaluate(const_cast<expr *>(&v->initializer));
            }
            environment->define(const_cast<token &>(v->name), val, v->isfinal, v->isglobal);
        }//

        void visit_s_while_stmt(s_while_stmt *s) override {
            object* o;
            while(is_bool_true_eq(o = evaluate(const_cast<expr *>(&s->condition)))){
                try{
                    execute(const_cast<stmt *>(s->body));
                }catch(break_signal b){
                    break;
                }catch(continue_signal s){
                    continue;
                }
                delete o;
            }
        } //

        void visit_uses_native_stmt(uses_native_stmt *u) override {

        } //TODO: implement natives

        void visit_null_stmt(null_stmt *n) override {

        } //
        void visit_context_stmt(context_stmt *c) override {
            runtime_environment* previous = environment;
            context* ctx = new context(previous);
            environment = ctx;
            for(stmt* s : c->body){
                execute(s);
            }
            environment = previous;
            environment->define(c->name,ctx,c->is_global,c->isfinal);
        }//
        void register_natives() {
            vector<callable*>* natives = natives::builtin_natives();
            callable* current;
            token* t;
            string name;
            int i;
            for(i = 0;i < natives->size();i++){
                current = natives->operator[](i);
                name = const_cast<string&>(current->name());
                t = new token(token_type::IDENTIFIER,name,*new string_o(name),"builtin_natives",-1);
                globals->define(*t,current,true,true);
                globals->add_native(current->name());
            }
        }
    public:

        interpreter() {
            stmt_visitor *sv = new default_stmt_visitor();
            expr_visitor *ev = new default_expr_visitor();
            dsv = static_cast<stmt_visitor *>(sv);
            dev = static_cast<expr_visitor *>(ev);
            register_natives();
        }

        void interpret(vector<stmt *> &statements) {
            try {
                stmt* s;
                int i;
                for (i = 0; i < statements.size(); i++) {
                    s = statements[i];
                    if (s == nullptr) continue;
                    execute(s);
                }
            } catch (runtime_exception e) {
                errors::runtime_error(e,stack_trace);
            } catch (return_signal& s) {
                runtime_exception e(s.keyword, *new string("return statements can only be used inside of functions"));
                errors::runtime_error(e,stack_trace);
            } catch(continue_signal& s){
                runtime_exception e(s.keyword, *new string("continue statements can only be used inside of loops"));
                errors::runtime_error(e,stack_trace);
            } catch(break_signal& s){
                runtime_exception e(s.keyword, *new string("break statements can only be used inside of loops"));
                errors::runtime_error(e,stack_trace);
            }
        }

    };
    const int function::arity(){
        return static_cast<int>(declaration->parameters.size());
    }
    object* function::call(interpreter* i, vector<object*> args){
        runtime_environment *env = new runtime_environment(retr_globals(i));
        int j;
        for(j = 0; j < declaration->parameters.size(); j++){
            env->define(const_cast<token &>(declaration->parameters.at(j)), args.at(j), false, false);
        }
        try{
            i->execute_block(declaration->body,env);
        }catch(return_signal& r){
            return r.value;
        }
        return lns::GET_DEFAULT_NULL();
    }
    const string& function::name(){
        return declaration->name.lexeme;
    }
}
namespace natives{
    static auto load_time = high_resolution_clock::now();
    class exit_c : public callable{
    public:
        const int arity() override {
            return 1;
        }

        const string &name() override {
            return *new string("exit");
        }

        object *call(lns::interpreter *env, vector<object *> args) override {
            double i = 0;
            if(args[0]->type == NUMBER_T) i = dynamic_cast<number_o*>(args[0])->value;
            exit(static_cast<int>(i));
        }
    };//
    class print_c : public callable{
    public:
        const int arity() override {
            return 1;
        }

        const string &name() override {
            return *new string("print");
        }

        object *call(lns::interpreter *env, vector<object *> args) override {
            cout << args[0]->str();
            flush(cout);
            return lns::GET_DEFAULT_NULL();
        }
    };//
    class println_c : public callable{
    public:
        const int arity() override {
            return 1;
        }

        const string &name() override {
            return *new string("println");
        }

        object *call(lns::interpreter *env, vector<object *> args) override {
            cout << args[0]->str() << endl;
            return lns::GET_DEFAULT_NULL();
        }
    };//
    class millis_c : public callable{
    public:
        const int arity() override {
            return 0;
        }

        const string &name() override {
            return *new string("millis");
        }

        object *call(lns::interpreter *env, vector<object *> args) override {
            number_o *o = new number_o((double)duration_cast<microseconds>(high_resolution_clock::now() - load_time).count() / 1000);
            return o;
        }
    };//
    class type_c : public callable{
        const int arity() override {
            return 1;
        }

        const string &name() override {
            return *new string("type");
        }

        object *call(lns::interpreter *env, vector<object *> args) override {
            object* o = args[0];
            string_o* type = new string_o("");
            switch(o->type){
                case NUMBER_T:
                    type->value = "number";
                    break;
                case STRING_T:
                    type->value = "string";
                    break;
                case MAP_T:
                    type->value = "map";
                    break;
                case BOOL_T:
                    type->value = "boolean";
                    break;
                case NULL_T:
                    type->value = "null";
                    break;
                case CALLABLE_T:
                    type->value = "callable";
                    break;
            }
            return type;
        }
    };//
    class call_c : public callable{
        const int arity() override {
            return 1;
        }

        const string &name() override {
            return *new string("call");
        }

        object *call(lns::interpreter *env, vector<object *> args) override {
            string cmd = args[0]->str();
            return new number_o(system(cmd.c_str()));
        }
    };//
    class sleep_c : public callable{
        const int arity() override {
            return 1;
        }

        const string &name() override {
            return *new string("sleep");
        }

        object *call(lns::interpreter *env, vector<object *> args) override {
            if(args[0]->type == NUMBER_T){
                int millis = static_cast<int>(dynamic_cast<number_o*>(args[0])->value);
                std::this_thread::sleep_for(std::chrono::milliseconds(millis));
            }
            return lns::GET_DEFAULT_NULL();
        }
    };//
    class str_c : public callable{
        const int arity() override {
            return 1;
        }

        const string &name() override {
            return *new string("str");
        }

        object *call(lns::interpreter *env, vector<object *> args) override {
            return new string_o(args[0]->str());
        }
    };//
    class num_c : public callable{
        const int arity() override {
            return 1;
        }

        const string &name() override {
            return *new string("num");
        }

        object *call(lns::interpreter *env, vector<object *> args) override {
            double num = 0;
            object* o = args[0];
            switch(o->type){
                case NUMBER_T:
                    return o;
                case STRING_T:
                    stringstream ss(o->str());
                    ss >> num;
                    if(ss.fail()) return lns::GET_DEFAULT_NULL();
                    break;
                case BOOL_T:
                    num = dynamic_cast<bool_o*>(o)->value ? 1 : 0;
                    break;
                default:
                    return lns::GET_DEFAULT_NULL();
            }
            return new number_o(num);
        }
    };//
    class int_c : public callable{
        const int arity() override {
            return 1;
        }

        const string &name() override {
            return *new string("int");
        }

        object *call(lns::interpreter *env, vector<object *> args) override {
            if(!(args[0]->type == NUMBER_T)) return lns::GET_DEFAULT_NULL();
            return new number_o((int)dynamic_cast<number_o*>(args[0])->value);
        }
    };//
    class read_c : public callable{
        const int arity() override {
            return 0;
        }

        const string &name() override {
            return *new string("read");
        }

        object *call(lns::interpreter *env, vector<object *> args) override {
            string d;
            cin >> d;
            return new string_o(d);
        }
    };
    class readln_c : public callable{
        const int arity() override {
            return 0;
        }

        const string &name() override {
            return *new string("readln");
        }

        object *call(lns::interpreter *env, vector<object *> args) override {
            string_o* o = new string_o("");
            getline(cin,o->value);
            cin.clear();
            return o;
        }
    };
    class readnr_c : public callable{
        const int arity() override {
            return 0;
        }

        const string &name() override {
            return *new string("readnr");
        }

        object *call(lns::interpreter *env, vector<object *> args) override {
            double d = 0;
            cin >> d;
            if(cin.fail()){
                cin.clear();
                cin.ignore(10000, '\n');
                return lns::GET_DEFAULT_NULL();
            }
            return new number_o(d);
        }
    };
    class readbool_c : public callable{
        const int arity() override {
            return 0;
        }

        const string &name() override {
            return *new string("readbool");
        }

        object *call(lns::interpreter *env, vector<object *> args) override {
            string d;
            getline(cin,d);
            return new bool_o(d == "true" || d == "1");
        }
    };
    class map_c : public callable{
        const int arity() override {
            return 0;
        }

        const string &name() override {
            return *new string("map");
        }

        object *call(lns::interpreter *env, vector<object *> args) override {
            return new map_o();
        }
    };
    class elems_c : public callable{
        const int arity() override {
            return 1;
        }

        const string &name() override {
            return *new string("elems");
        }

        object *call(lns::interpreter *env, vector<object *> args) override {
            if(args[0]->type != MAP_T) return lns::GET_DEFAULT_NULL();
            map_o* o = dynamic_cast<map_o*>(args[0]);
            return new number_o(o->values.size());
        }
    };
    static vector<callable*>* builtin_natives(){
        vector<callable*>* natives = new vector<callable*>();
        natives->push_back(new exit_c());
        natives->push_back(new print_c());
        natives->push_back(new println_c());
        natives->push_back(new millis_c());
        natives->push_back(new type_c());
        natives->push_back(new sleep_c());
        natives->push_back(new str_c());
        natives->push_back(new num_c());
        natives->push_back(new int_c());
        natives->push_back(new read_c());
        natives->push_back(new readln_c());
        natives->push_back(new readnr_c());
        natives->push_back(new readbool_c());
        natives->push_back(new map_c());
        natives->push_back(new call_c());
        natives->push_back(new elems_c());
        return natives;
    }
}
runtime_environment* lns::retr_globals(interpreter *i) {
    return i->globals;
}
#endif //CPPLNS_INTERPRETER_H
