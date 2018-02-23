//
// Created by lorenzo on 10/09/17.
//

#ifndef CPPLNS_INTERPRETER_H
#define CPPLNS_INTERPRETER_H

#include <memory>
#include "utils.h"
#include "expr.h"
#include "stmt.h"
#include <vector>
#include <cmath>
#include <ctime>
#include "exceptions.h"
#include <string>
#include <chrono>
#include <thread>
#include "primitives.h"
#include "environments.h"
#include "callable.h"
#include "errors.h"
#include <dlfcn.h>

#define THIS_DEFINITION string("#this#")
#define RECURSION_LIMIT 1000

namespace lns {
    class interpreter;

    class interpreter : public expr_visitor, public stmt_visitor {
    public:
        virtual void execute(stmt *s);

        vector<stack_call *> stack_trace;
    public:
        void visit_s_for_each_stmt(s_for_each_stmt *s) override;

        shared_ptr<object> visit_array_expr(array_expr *a) override;

    protected:
        std::shared_ptr<object> evaluate(expr *e);

    public:
        static bool is_bool_true_eq(object *o);

        std::shared_ptr<runtime_environment> globals = make_shared<runtime_environment>();
        std::shared_ptr<runtime_environment> environment = globals;

        void execute_block(vector<shared_ptr<stmt>> stmts, std::shared_ptr<runtime_environment> env);

        shared_ptr<object> visit_member_expr(member_expr *c) override;

        shared_ptr<object> visit_member_assign_expr(member_assign_expr *c) override;

        void visit_handle_stmt(handle_stmt *h) override;

        void visit_begin_handle_stmt(begin_handle_stmt *b) override;

        void visit_raise_stmt(raise_stmt *r) override;

        shared_ptr<object> visit_assign_expr(assign_expr *a) override; //

        shared_ptr<object> visit_binary_expr(binary_expr *b) override; //

        shared_ptr<object> visit_call_expr(call_expr *c) override; //

        shared_ptr<object> visit_grouping_expr(grouping_expr *g) override; //

        shared_ptr<object> visit_literal_expr(literal_expr *l) override;//

        shared_ptr<object> visit_unary_expr(unary_expr *u) override;

        shared_ptr<object> visit_variable_expr(variable_expr *v) override; //

        std::shared_ptr<object> get_map_field(token *where, std::shared_ptr<object> obj, std::shared_ptr<object> key);

        shared_ptr<object> visit_sub_script_expr(sub_script_expr *m) override; //

        std::shared_ptr<object>
        assign_map_field(const token *where, std::shared_ptr<object> obj, const token_type op, const number_o *key,
                         std::shared_ptr<object> value);

        shared_ptr<object> visit_sub_script_assign_expr(sub_script_assign_expr *a) override;//

        shared_ptr<object> visit_null_expr(null_expr *n) override; //

        void visit_block_stmt(block_stmt *b) override; //

        void visit_s_for_stmt(s_for_stmt *s) override;

    public:
        void visit_expression_stmt(expression_stmt *e) override; //

        void visit_function_stmt(function_stmt *f) override; //

        void visit_if_stmt(if_stmt *i) override; //

        void visit_return_stmt(return_stmt *r) override; //

        void visit_break_stmt(break_stmt *b) override; //

        void visit_continue_stmt(continue_stmt *c) override; //

        void visit_var_stmt(var_stmt *v) override;//

        void visit_s_while_stmt(s_while_stmt *s) override; //

        void visit_uses_native_stmt(uses_native_stmt *u) override;

        void visit_null_stmt(null_stmt *n) override; //

        void visit_context_stmt(context_stmt *c) override;//
        void visit_constructor_stmt(constructor_stmt *c) override;

        void visit_class_decl_stmt(class_decl_stmt *c) override;

        shared_ptr<object> visit_new_expr(new_expr *n) override;

        shared_ptr<object> visit_this_expr(this_expr *t) override;

    public:

        interpreter();

        void interpret(vector<shared_ptr<stmt>> &statements);

        std::shared_ptr<object> clone_or_keep(std::shared_ptr<object> obj, const expr_type type, const token *where);

        void visit_exception_decl_stmt(exception_decl_stmt *e) override;

        void interpret_stmts0(vector<shared_ptr<stmt>> &vector);

        virtual void terminate0();

        bool check_params(const token *where, const parameter_declaration &declaration, unsigned long size);
    };

    class function : public callable{
    protected:
        const lns::function_stmt* declaration;
        interpreter* i;
    public:
        const std::string name() const;
        const parameter_declaration& arity() const;

        shared_ptr<object> call(std::vector<shared_ptr<object>> &args);
        explicit function(interpreter* i, const lns::function_stmt *f);

        string str() const override;
    };

    class constructor;

    class instance_o;

    class method;

    class class_definition : public function_container {
    public:
        const char *def_file;
        interpreter *i;
        const string name;
        vector<shared_ptr<lns::function_stmt>> methods;
        vector<shared_ptr<lns::constructor>> constructors;
        vector<shared_ptr<lns::var_stmt>> variables;

        class_definition(const string name,
                         const vector<shared_ptr<var_stmt>> &variables,
                         const vector<shared_ptr<function_stmt>> &methods, const char *def_file, interpreter *);

        string str() const override;

        bool operator==(const object &o) const override;

        shared_ptr<object> clone() const override;

        set<callable *> declare_natives() const override;

        shared_ptr<method> getMethod(const token *name, const char *accessing_file, instance_o *instance);
    };

    class constructor : public callable {
    public:
        const lns::constructor_stmt *declaration;
        interpreter *i;
        const std::shared_ptr<lns::class_definition> container;

        constructor(interpreter *i, const shared_ptr<class_definition> container, const constructor_stmt *decl);

        const parameter_declaration &arity() const override;

        const string name() const override;

        shared_ptr<object> call(std::vector<std::shared_ptr<object>> &args) override;

        bool operator==(const object &o) const override;

        string str() const override;

        shared_ptr<object> clone() const override;
    };

    class instance_o : public object, public runtime_environment {
    public:
        instance_o(shared_ptr<class_definition> class_, std::shared_ptr<runtime_environment> globals,
                   map<string, variable> vars);

        shared_ptr<class_definition> class_;

        void define(const token *name, std::shared_ptr<object> o, bool is_final, visibility visibility,
                    const char *def_file) override;

        explicit instance_o(shared_ptr<class_definition> class_, std::shared_ptr<runtime_environment> globals);

        string str() const override;

        shared_ptr<object> clone() const override;

        bool operator==(const object &o) const override;

        shared_ptr<object> operator+(const object &o) const override;

        shared_ptr<object> get(const token *name, const char *accessing_file) override;

        void assign_field(const token *name, const token_type op, shared_ptr<object> obj, const char *assigning_file);

    };

    class method : public function {
    public:
        instance_o *instance;

        method(interpreter *i, const function_stmt *f, instance_o *instance);

        shared_ptr<object> call(std::vector<shared_ptr<object>> &args) override;

        string str() const override;
    };
}
#endif //CPPLNS_INTERPRETER_H
