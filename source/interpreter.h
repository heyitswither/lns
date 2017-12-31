//
// Created by lorenzo on 10/09/17.
//

#ifndef CPPLNS_INTERPRETER_H
#define CPPLNS_INTERPRETER_H

#include "defs.h"
#include "expr.h"
#include "stmt.h"
#include <vector>
#include <cmath>
#include <ctime>
#include "exceptions.h"
#include <string>
#include <chrono>
#include <thread>


#define RECURSION_LIMIT 1000

namespace lns {
    class interpreter;

    class interpreter : public expr_visitor, public stmt_visitor {
    public:
        virtual void execute(stmt *s);

        vector<stack_call *>& stack_trace;
    public:
        void visit_s_for_each_stmt(s_for_each_stmt *s) override;

        object *visit_array_expr(array_expr *a) override;

    protected:
        object *evaluate(expr *e);

    private:
        stmt_visitor *dsv;
        expr_visitor *dev;


    public:
        static bool is_bool_true_eq(object *o);

        runtime_environment *globals = new runtime_environment();
        runtime_environment *environment = globals;

        void execute_block(vector<shared_ptr<stmt>> stmts, runtime_environment *env);

        object *visit_member_expr(member_expr *c) override;

        object *visit_member_assign_expr(member_assign_expr *c) override;

        void visit_handle_stmt(handle_stmt *h) override;

        void visit_begin_handle_stmt(begin_handle_stmt *b) override;

        void visit_raise_stmt(raise_stmt *r) override;

        object *visit_assign_expr(assign_expr *a) override; //

        object *visit_binary_expr(binary_expr *b) override; //

        object *visit_call_expr(call_expr *c) override; //

        object *visit_grouping_expr(grouping_expr *g) override; //

        object *visit_literal_expr(literal_expr *l) override;//

        object *visit_unary_expr(unary_expr *u) override;

        object *visit_variable_expr(variable_expr *v) override; //

        object *get_map_field(token *where, object *obj, object *key);

        object *visit_sub_script_expr(sub_script_expr *m) override; //

        object *assign_map_field(const token *where, object *obj, const token_type op, number_o* key, object *value);

        object *visit_sub_script_assign_expr(sub_script_assign_expr *a) override;//

        object *visit_null_expr(null_expr *n) override; //

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

    public:

        interpreter();

        void interpret(vector<shared_ptr<stmt>> &statements);

        object *clone_or_keep(object *obj, const expr_type type, const token *where);

        void visit_exception_decl_stmt(exception_decl_stmt *e) override;

    };

    class function : public callable{
        const lns::function_stmt* declaration;
        interpreter* i;
    public:
        const std::string & name()const;
        const parameter_declaration& arity() const;
        object* call(std::vector<object *> &args);
        explicit function(interpreter* i, const lns::function_stmt *f);

        string str() const override;
    };

}
#endif //CPPLNS_INTERPRETER_H
