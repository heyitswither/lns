#ifndef LNS_STMT_H
#define LNS_STMT_H
#include "defs.h"
#include <list>
#include <string>
#include <iostream>
#include "expr.h"
#include <memory>
using namespace std;
using namespace lns;


namespace lns{
enum stmt_type{
BLOCK_STMT_T, EXPRESSION_STMT_T, FUNCTION_STMT_T, CONTEXT_STMT_T, IF_STMT_T, RETURN_STMT_T, BREAK_STMT_T, CONTINUE_STMT_T, VAR_STMT_T, S_WHILE_STMT_T, S_FOR_STMT_T, S_FOR_EACH_STMT_T, EXCEPTION_DECL_STMT_T, RAISE_STMT_T, USES_NATIVE_STMT_T, HANDLE_STMT_T, BEGIN_HANDLE_STMT_T, CONSTRUCTOR_STMT_T, CLASS_DECL_STMT_T, NULL_STMT_T, 
};
class stmt_visitor;
class stmt{
public:

stmt(int line, const char* file, stmt_type type) : type(type), line(line), file(file){}
virtual void accept(stmt_visitor* v) = 0;
stmt_type type;
 int line; const char* file;
};
class block_stmt;
class expression_stmt;
class function_stmt;
class context_stmt;
class if_stmt;
class return_stmt;
class break_stmt;
class continue_stmt;
class var_stmt;
class s_while_stmt;
class s_for_stmt;
class s_for_each_stmt;
class exception_decl_stmt;
class raise_stmt;
class uses_native_stmt;
class handle_stmt;
class begin_handle_stmt;
class constructor_stmt;
class class_decl_stmt;
class null_stmt;
class stmt_visitor{
public:
virtual void visit_block_stmt(block_stmt *b) = 0;
virtual void visit_expression_stmt(expression_stmt *e) = 0;
virtual void visit_function_stmt(function_stmt *f) = 0;
virtual void visit_context_stmt(context_stmt *c) = 0;
virtual void visit_if_stmt(if_stmt *i) = 0;
virtual void visit_return_stmt(return_stmt *r) = 0;
virtual void visit_break_stmt(break_stmt *b) = 0;
virtual void visit_continue_stmt(continue_stmt *c) = 0;
virtual void visit_var_stmt(var_stmt *v) = 0;
virtual void visit_s_while_stmt(s_while_stmt *s) = 0;
virtual void visit_s_for_stmt(s_for_stmt *s) = 0;
virtual void visit_s_for_each_stmt(s_for_each_stmt *s) = 0;
virtual void visit_exception_decl_stmt(exception_decl_stmt *e) = 0;
virtual void visit_raise_stmt(raise_stmt *r) = 0;
virtual void visit_uses_native_stmt(uses_native_stmt *u) = 0;
virtual void visit_handle_stmt(handle_stmt *h) = 0;
virtual void visit_begin_handle_stmt(begin_handle_stmt *b) = 0;
virtual void visit_constructor_stmt(constructor_stmt *c) = 0;
virtual void visit_class_decl_stmt(class_decl_stmt *c) = 0;
virtual void visit_null_stmt(null_stmt *n) = 0;
};

class block_stmt : public stmt {
public:
    const vector<shared_ptr<stmt>> statements;

    explicit block_stmt(const char *file, const int line, const vector<shared_ptr<stmt>> statements) : stmt(line, file,
                                                                                                            BLOCK_STMT_T),
                                                                                                       statements(
                                                                                                               statements) {}
void accept(stmt_visitor *v) override{
v->visit_block_stmt(this);
}
};



class expression_stmt : public stmt {
public:
    const shared_ptr<expr> exprs;

    explicit expression_stmt(const char *file, const int line, const shared_ptr<expr> exprs) : stmt(line, file,
                                                                                                    EXPRESSION_STMT_T),
                                                                                               exprs(exprs) {}
void accept(stmt_visitor *v) override{
v->visit_expression_stmt(this);
}
};



class function_stmt : public stmt {
public:
const token* name;
    const parameter_declaration parameters;
    const vector<shared_ptr<stmt>> body;
const visibility vis;

    function_stmt(const char *file, const int line, const token *name, const parameter_declaration parameters,
                  const vector<shared_ptr<stmt>> body, const visibility vis) : stmt(line, file, FUNCTION_STMT_T),
                                                                               name(name), parameters(parameters),
                                                                               body(body), vis(vis) {}
void accept(stmt_visitor *v) override{
v->visit_function_stmt(this);
}
};



class context_stmt : public stmt {
public:
const token* name;
    const vector<shared_ptr<stmt>> body;
const visibility vis;
const bool isfinal;

    context_stmt(const char *file, const int line, const token *name, const vector<shared_ptr<stmt>> body,
                 const visibility vis, const bool isfinal) : stmt(line, file, CONTEXT_STMT_T), name(name), body(body),
                                                             vis(vis), isfinal(isfinal) {}
void accept(stmt_visitor *v) override{
v->visit_context_stmt(this);
}
};



class if_stmt : public stmt {
public:
    const shared_ptr<expr> condition;
    const shared_ptr<stmt> thenBranch;
    const shared_ptr<stmt> elseBranch;

    if_stmt(const char *file, const int line, const shared_ptr<expr> condition, const shared_ptr<stmt> thenBranch,
            const shared_ptr<stmt> elseBranch) : stmt(line, file, IF_STMT_T), condition(condition),
                                                 thenBranch(thenBranch), elseBranch(elseBranch) {}
void accept(stmt_visitor *v) override{
v->visit_if_stmt(this);
}
};



class return_stmt : public stmt {
public:
const token* keyword;
    const shared_ptr<expr> value;

    return_stmt(const char *file, const int line, const token *keyword, const shared_ptr<expr> value) : stmt(line, file,
                                                                                                             RETURN_STMT_T),
                                                                                                        keyword(keyword),
                                                                                                        value(value) {}
void accept(stmt_visitor *v) override{
v->visit_return_stmt(this);
}
};



class break_stmt : public stmt {
public:
const token* keyword;
explicit break_stmt(const char* file,const int line, const token* keyword) : stmt(line, file, BREAK_STMT_T), keyword(keyword) {}
void accept(stmt_visitor *v) override{
v->visit_break_stmt(this);
}
};



class continue_stmt : public stmt {
public:
const token* keyword;
explicit continue_stmt(const char* file,const int line, const token* keyword) : stmt(line, file, CONTINUE_STMT_T), keyword(keyword) {}
void accept(stmt_visitor *v) override{
v->visit_continue_stmt(this);
}
};



class var_stmt : public stmt {
public:
const token* name;
    const shared_ptr<expr> initializer;
const visibility vis;
const bool isfinal;

    var_stmt(const char *file, const int line, const token *name, const shared_ptr<expr> initializer,
             const visibility vis, const bool isfinal) : stmt(line, file, VAR_STMT_T), name(name),
                                                         initializer(initializer), vis(vis), isfinal(isfinal) {}
void accept(stmt_visitor *v) override{
v->visit_var_stmt(this);
}
};



class s_while_stmt : public stmt {
public:
    const shared_ptr<expr> condition;
    const shared_ptr<stmt> body;

    s_while_stmt(const char *file, const int line, const shared_ptr<expr> condition, const shared_ptr<stmt> body)
            : stmt(line, file, S_WHILE_STMT_T), condition(condition), body(body) {}
void accept(stmt_visitor *v) override{
v->visit_s_while_stmt(this);
}
};



class s_for_stmt : public stmt {
public:
    const shared_ptr<stmt> init;
    const shared_ptr<expr> condition;
    const shared_ptr<expr> increment;
    const shared_ptr<stmt> body;

    s_for_stmt(const char *file, const int line, const shared_ptr<stmt> init, const shared_ptr<expr> condition,
               const shared_ptr<expr> increment, const shared_ptr<stmt> body) : stmt(line, file, S_FOR_STMT_T),
                                                                                init(init), condition(condition),
                                                                                increment(increment), body(body) {}
void accept(stmt_visitor *v) override{
v->visit_s_for_stmt(this);
}
};



class s_for_each_stmt : public stmt {
public:
const token* identifier;
    const shared_ptr<expr> container;
    const shared_ptr<stmt> body;

    s_for_each_stmt(const char *file, const int line, const token *identifier, const shared_ptr<expr> container,
                    const shared_ptr<stmt> body) : stmt(line, file, S_FOR_EACH_STMT_T), identifier(identifier),
                                                   container(container), body(body) {}
void accept(stmt_visitor *v) override{
v->visit_s_for_each_stmt(this);
}
};



class exception_decl_stmt : public stmt {
public:
const token* name;
    const set<string> identifiers;
const visibility vis;

    exception_decl_stmt(const char *file, const int line, const token *name, const set<string> identifiers,
                        const visibility vis) : stmt(line, file, EXCEPTION_DECL_STMT_T), name(name),
                                                identifiers(identifiers), vis(vis) {}
void accept(stmt_visitor *v) override{
v->visit_exception_decl_stmt(this);
}
};



class raise_stmt : public stmt {
public:
const token* where;
    const shared_ptr<expr> name;
    const map<string, shared_ptr<expr>> fields;

    raise_stmt(const char *file, const int line, const token *where, const shared_ptr<expr> name,
               const map<string, shared_ptr<expr>> fields) : stmt(line, file, RAISE_STMT_T), where(where), name(name),
                                                             fields(fields) {}
void accept(stmt_visitor *v) override{
v->visit_raise_stmt(this);
}
};



class uses_native_stmt : public stmt {
public:
const token* where;
    const string lib_name;
const token* bind;
const visibility vis;

    uses_native_stmt(const char *file, const int line, const token *where, const string lib_name, const token *bind,
                     const visibility vis) : stmt(line, file, USES_NATIVE_STMT_T), where(where), lib_name(lib_name),
                                             bind(bind), vis(vis) {}
void accept(stmt_visitor *v) override{
v->visit_uses_native_stmt(this);
}
};



class handle_stmt : public stmt {
public:
    const shared_ptr<expr> exception_name;
const token* bind;
    const vector<shared_ptr<stmt>> block;

    handle_stmt(const char *file, const int line, const shared_ptr<expr> exception_name, const token *bind,
                const vector<shared_ptr<stmt>> block) : stmt(line, file, HANDLE_STMT_T), exception_name(exception_name),
                                                        bind(bind), block(block) {}
void accept(stmt_visitor *v) override{
v->visit_handle_stmt(this);
}
};



class begin_handle_stmt : public stmt {
public:
    const vector<shared_ptr<stmt>> body;
    const vector<shared_ptr<handle_stmt>> handles;

    begin_handle_stmt(const char *file, const int line, const vector<shared_ptr<stmt>> body,
                      const vector<shared_ptr<handle_stmt>> handles) : stmt(line, file, BEGIN_HANDLE_STMT_T),
                                                                       body(body), handles(handles) {}
void accept(stmt_visitor *v) override{
v->visit_begin_handle_stmt(this);
}
};



class constructor_stmt : public stmt {
public:
const token* keyword;
    const parameter_declaration parameters;
    const vector<shared_ptr<stmt>> body;
const visibility vis;

    constructor_stmt(const char *file, const int line, const token *keyword, const parameter_declaration parameters,
                     const vector<shared_ptr<stmt>> body, const visibility vis) : stmt(line, file, CONSTRUCTOR_STMT_T),
                                                                                  keyword(keyword),
                                                                                  parameters(parameters), body(body),
                                                                                  vis(vis) {}
void accept(stmt_visitor *v) override{
v->visit_constructor_stmt(this);
}
};



class class_decl_stmt : public stmt {
public:
const token* name;
    const vector<shared_ptr<function_stmt>> methods;
    const vector<shared_ptr<constructor_stmt>> constructors;
    const vector<shared_ptr<var_stmt>> variables;
const visibility vis;

    class_decl_stmt(const char *file, const int line, const token *name,
                    const vector<shared_ptr<function_stmt>> methods,
                    const vector<shared_ptr<constructor_stmt>> constructors,
                    const vector<shared_ptr<var_stmt>> variables, const visibility vis) : stmt(line, file,
                                                                                               CLASS_DECL_STMT_T),
                                                                                          name(name), methods(methods),
                                                                                          constructors(constructors),
                                                                                          variables(variables),
                                                                                          vis(vis) {}
void accept(stmt_visitor *v) override{
v->visit_class_decl_stmt(this);
}
};



class null_stmt : public stmt {
public:
const token* where;
explicit null_stmt(const char* file,const int line, const token* where) : stmt(line, file, NULL_STMT_T), where(where) {}
void accept(stmt_visitor *v) override{
v->visit_null_stmt(this);
}
};


class default_stmt_visitor : public stmt_visitor {
public:
void visit_block_stmt(block_stmt *b) override {
std::cout << "block_stmt" << std::endl;
    return;
}
void visit_expression_stmt(expression_stmt *e) override {
std::cout << "expression_stmt" << std::endl;
    return;
}
void visit_function_stmt(function_stmt *f) override {
std::cout << "function_stmt" << std::endl;
    return;
}
void visit_context_stmt(context_stmt *c) override {
std::cout << "context_stmt" << std::endl;
    return;
}
void visit_if_stmt(if_stmt *i) override {
std::cout << "if_stmt" << std::endl;
    return;
}
void visit_return_stmt(return_stmt *r) override {
std::cout << "return_stmt" << std::endl;
    return;
}
void visit_break_stmt(break_stmt *b) override {
std::cout << "break_stmt" << std::endl;
    return;
}
void visit_continue_stmt(continue_stmt *c) override {
std::cout << "continue_stmt" << std::endl;
    return;
}
void visit_var_stmt(var_stmt *v) override {
std::cout << "var_stmt" << std::endl;
    return;
}
void visit_s_while_stmt(s_while_stmt *s) override {
std::cout << "s_while_stmt" << std::endl;
    return;
}
void visit_s_for_stmt(s_for_stmt *s) override {
std::cout << "s_for_stmt" << std::endl;
    return;
}
void visit_s_for_each_stmt(s_for_each_stmt *s) override {
std::cout << "s_for_each_stmt" << std::endl;
    return;
}
void visit_exception_decl_stmt(exception_decl_stmt *e) override {
std::cout << "exception_decl_stmt" << std::endl;
    return;
}
void visit_raise_stmt(raise_stmt *r) override {
std::cout << "raise_stmt" << std::endl;
    return;
}
void visit_uses_native_stmt(uses_native_stmt *u) override {
std::cout << "uses_native_stmt" << std::endl;
    return;
}
void visit_handle_stmt(handle_stmt *h) override {
std::cout << "handle_stmt" << std::endl;
    return;
}
void visit_begin_handle_stmt(begin_handle_stmt *b) override {
std::cout << "begin_handle_stmt" << std::endl;
    return;
}
void visit_constructor_stmt(constructor_stmt *c) override {
std::cout << "constructor_stmt" << std::endl;
    return;
}
void visit_class_decl_stmt(class_decl_stmt *c) override {
std::cout << "class_decl_stmt" << std::endl;
    return;
}
void visit_null_stmt(null_stmt *n) override {
std::cout << "null_stmt" << std::endl;
    return;
}
};
}
#endif
