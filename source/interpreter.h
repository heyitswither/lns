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

namespace natives {
    vector<lns::callable *> *builtin_natives();
}
namespace lns {
    class interpreter;
    runtime_environment *retr_globals(interpreter *i);

    class interpreter : public expr_visitor, stmt_visitor {
    public:
        virtual void execute(stmt *s);

        vector<stack_call *>& stack_trace;
    protected:
        object *evaluate(expr *e);

    private:
        stmt_visitor *dsv;
        expr_visitor *dev;


        void check_bool_operands(const token &op, object *left, object *right);

        void check_number_operands(const token &op, object *left, object *right);

        void check_number_operand(const token &token, object *o);


    public:
        static bool is_bool_true_eq(object *o);

        runtime_environment *globals = new runtime_environment();
        runtime_environment *environment = globals;

        void execute_block(vector<stmt *> stmts, runtime_environment *env);

        object *visit_context_expr(context_expr *c) override;

        object *visit_context_assign_expr(context_assign_expr *c) override;


        object *visit_increment_expr(increment_expr *i) override;

        object *visit_decrement_expr(decrement_expr *d) override;

        object *visit_assign_expr(assign_expr *a) override; //

        object *visit_binary_expr(binary_expr *b) override; //

        object *visit_call_expr(call_expr *c) override; //

        object *visit_grouping_expr(grouping_expr *g) override; //

        object *visit_literal_expr(literal_expr *l) override;//

        object *visit_unary_expr(unary_expr *u) override;

        object *visit_variable_expr(variable_expr *v) override; //

        object *get_map_field(token &where, object *obj, object *key);

        object *visit_sub_script_expr(sub_script_expr *m) override; //

        object *assign_map_field(const token &where, object *obj, const token_type op, string_o *key, object *value);

        object *visit_assign_map_field_expr(assign_map_field_expr *a) override;//

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

        void visit_uses_native_stmt(uses_native_stmt *u) override; //TODO: implement natives

        void visit_null_stmt(null_stmt *n) override; //
        void visit_context_stmt(context_stmt *c) override;//
        void register_natives();

    public:

        interpreter();

        void interpret(vector<stmt *> &statements);

    };

    class function : public callable{
        const lns::function_stmt* declaration;
        interpreter* i;
    public:
        const std::string & name()const;
        const int arity() const;
        object* call(std::vector<object *> &args);
        explicit function(interpreter* i, const lns::function_stmt *f);

        string str() const override;
    };

}
namespace natives {
    static auto load_time = std::chrono::high_resolution_clock::now();

    class exit_c : public callable {
    public:
        const int arity() const override {
            return 1;
        }

        const string &name() const override {
            return *new string("exit");
        }

        object *call( vector<object *> &args) override {
            double i = 0;
            if (args[0]->type == NUMBER_T) i = dynamic_cast<number_o *>(args[0])->value;
            exit(static_cast<int>(i));
        }
    };//
    class print_c : public callable {
    public:
        const int arity() const override {
            return 1;
        }

        const string &name() const override {
            return *new string("print");
        }

        object *call( vector<object *> &args) override {
            cout << args[0]->str();
            flush(cout);
            return lns::GET_DEFAULT_NULL();
        }
    };//
    class println_c : public callable {
    public:
        const int arity() const override {
            return 1;
        }

        const string &name() const override {
            return *new string("println");
        }

        object *call( vector<object *> &args) override {
            cout << args[0]->str() << endl;
            return lns::GET_DEFAULT_NULL();
        }
    };//
    class millis_c : public callable {
    public:
        const int arity() const override {
            return 0;
        }

        const string &name() const override {
            return *new string("millis");
        }

        object *call( vector<object *> &args) override {
            number_o *o = new number_o(
                    (double) chrono::duration_cast<chrono::microseconds>(chrono::high_resolution_clock::now() - load_time).count() / 1000);
            return o;
        }
    };//
    class type_c : public callable {
        const int arity() const override {
            return 1;
        }

        const string &name() const override {
            return *new string("type");
        }

        object *call( vector<object *> &args) override {
            object *o = args[0];
            string_o *type = new string_o("");
            switch (o->type) {
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
    class call_c : public callable {
        const int arity() const override {
            return 1;
        }

        const string &name() const override {
            return *new string("call");
        }

        object *call( vector<object *> &args) override {
            string cmd = args[0]->str();
            return new number_o(system(cmd.c_str()));
        }
    };//
    class sleep_c : public callable {
        const int arity() const override {
            return 1;
        }

        const string &name() const override {
            return *new string("sleep");
        }

        object *call( vector<object *> &args) override {
            if (args[0]->type == NUMBER_T) {
                int millis = static_cast<int>(dynamic_cast<number_o *>(args[0])->value);
                std::this_thread::sleep_for(std::chrono::milliseconds(millis));
            }
            return lns::GET_DEFAULT_NULL();
        }
    };//
    class str_c : public callable {
        const int arity() const override {
            return 1;
        }

        const string &name() const override {
            return *new string("str");
        }

        object *call( vector<object *> &args) override {
            return new string_o(args[0]->str());
        }
    };//
    class num_c : public callable {
        const int arity() const override {
            return 1;
        }

        const string &name() const override {
            return *new string("num");
        }

        object *call( vector<object *> &args) override {
            double num = 0;
            object *o = args[0];
            switch (o->type) {
                case NUMBER_T:
                    return o;
                case STRING_T:
                    stringstream ss(o->str());
                    ss >> num;
                    if (ss.fail()) return lns::GET_DEFAULT_NULL();
                    break;
                case BOOL_T:
                    num = dynamic_cast<bool_o *>(o)->value ? 1 : 0;
                    break;
                default:
                    return lns::GET_DEFAULT_NULL();
            }
            return new number_o(num);
        }
    };//
    class int_c : public callable {
        const int arity() const override {
            return 1;
        }

        const string &name() const override {
            return *new string("int");
        }

        object *call( vector<object *> &args) override {
            if (!(args[0]->type == NUMBER_T)) return lns::GET_DEFAULT_NULL();
            return new number_o((int) dynamic_cast<number_o *>(args[0])->value);
        }
    };//
    class read_c : public callable {
        const int arity() const override {
            return 0;
        }

        const string &name() const override {
            return *new string("read");
        }

        object *call( vector<object *> &args) override {
            string d;
            cin >> d;
            return new string_o(d);
        }
    };

    class readln_c : public callable {
        const int arity() const override {
            return 0;
        }

        const string &name() const override {
            return *new string("readln");
        }

        object *call( vector<object *> &args) override {
            string_o *o = new string_o("");
            getline(cin, o->value);
            cin.clear();
            return o;
        }
    };

    class readnr_c : public callable {
        const int arity() const override {
            return 0;
        }

        const string &name() const override {
            return *new string("readnr");
        }

        object *call( vector<object *> &args) override {
            double d = 0;
            cin >> d;
            if (cin.fail()) {
                cin.clear();
                cin.ignore(10000, '\n');
                return lns::GET_DEFAULT_NULL();
            }
            return new number_o(d);
        }
    };

    class readbool_c : public callable {
        const int arity() const override {
            return 0;
        }

        const string &name() const override {
            return *new string("readbool");
        }

        object *call( vector<object *> &args) override {
            string d;
            getline(cin, d);
            return new bool_o(d == "true" || d == "1");
        }
    };

    class map_c : public callable {
        const int arity() const override {
            return 0;
        }

        const string &name() const override {
            return *new string("map");
        }

        object *call( vector<object *> &args) override {
            return new map_o();
        }
    };

    class elems_c : public callable {
        const int arity() const override {
            return 1;
        }

        const string &name() const override {
            return *new string("elems");
        }

        object *call( vector<object *> &args) override {
            if (args[0]->type != MAP_T) return lns::GET_DEFAULT_NULL();
            map_o *o = dynamic_cast<map_o *>(args[0]);
            return new number_o(o->values.size());
        }
    };


}
#endif //CPPLNS_INTERPRETER_H
