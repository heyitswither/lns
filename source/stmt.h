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
explicit block_stmt(const char* file,const int line, const vector<stmt*>& statements) : statements(statements), stmt(line, file, BLOCK_STMT_T) {}
void accept(stmt_visitor *v) override{
v->visit_block_stmt(this);
}
const vector<stmt*>& statements;
};



class expression_stmt : public stmt {
public:
explicit expression_stmt(const char* file,const int line, const expr& exprs) : exprs(exprs), stmt(line, file, EXPRESSION_STMT_T) {}
void accept(stmt_visitor *v) override{
v->visit_expression_stmt(this);
}
const expr& exprs;
};



class function_stmt : public stmt {
public:
function_stmt(const char* file,const int line, const token* name, const vector<token*>& parameters, const vector<stmt*>& body, const visibility visibility) : name(name), parameters(parameters), body(body), visibility(visibility), stmt(line, file, FUNCTION_STMT_T) {}
void accept(stmt_visitor *v) override{
v->visit_function_stmt(this);
}
const token* name;
const vector<token*>& parameters;
const vector<stmt*>& body;
const visibility visibility;
};



class context_stmt : public stmt {
public:
context_stmt(const char* file,const int line, const token* name, const vector<stmt*> body, const visibility visibility, const bool isfinal) : name(name), body(body), visibility(visibility), isfinal(isfinal), stmt(line, file, CONTEXT_STMT_T) {}
void accept(stmt_visitor *v) override{
v->visit_context_stmt(this);
}
const token* name;
const vector<stmt*> body;
const visibility visibility;
const bool isfinal;
};



class if_stmt : public stmt {
public:
if_stmt(const char* file,const int line, const expr& condition, const stmt* thenBranch, const stmt* elseBranch) : condition(condition), thenBranch(thenBranch), elseBranch(elseBranch), stmt(line, file, IF_STMT_T) {}
void accept(stmt_visitor *v) override{
v->visit_if_stmt(this);
}
const expr& condition;
const stmt* thenBranch;
const stmt* elseBranch;
};



class return_stmt : public stmt {
public:
return_stmt(const char* file,const int line, const token* keyword, const expr& value) : keyword(keyword), value(value), stmt(line, file, RETURN_STMT_T) {}
void accept(stmt_visitor *v) override{
v->visit_return_stmt(this);
}
const token* keyword;
const expr& value;
};



class break_stmt : public stmt {
public:
explicit break_stmt(const char* file,const int line, const token* keyword) : keyword(keyword), stmt(line, file, BREAK_STMT_T) {}
void accept(stmt_visitor *v) override{
v->visit_break_stmt(this);
}
const token* keyword;
};



class continue_stmt : public stmt {
public:
explicit continue_stmt(const char* file,const int line, const token* keyword) : keyword(keyword), stmt(line, file, CONTINUE_STMT_T) {}
void accept(stmt_visitor *v) override{
v->visit_continue_stmt(this);
}
const token* keyword;
};



class var_stmt : public stmt {
public:
var_stmt(const char* file,const int line, const token* name, const expr& initializer, const visibility visibility, const bool isfinal) : name(name), initializer(initializer), visibility(visibility), isfinal(isfinal), stmt(line, file, VAR_STMT_T) {}
void accept(stmt_visitor *v) override{
v->visit_var_stmt(this);
}
const token* name;
const expr& initializer;
const visibility visibility;
const bool isfinal;
};



class s_while_stmt : public stmt {
public:
s_while_stmt(const char* file,const int line, const expr& condition, const stmt* body) : condition(condition), body(body), stmt(line, file, S_WHILE_STMT_T) {}
void accept(stmt_visitor *v) override{
v->visit_s_while_stmt(this);
}
const expr& condition;
const stmt* body;
};



class s_for_stmt : public stmt {
public:
s_for_stmt(const char* file,const int line, const stmt* init, const expr* condition, const expr* increment, const stmt* body) : init(init), condition(condition), increment(increment), body(body), stmt(line, file, S_FOR_STMT_T) {}
void accept(stmt_visitor *v) override{
v->visit_s_for_stmt(this);
}
const stmt* init;
const expr* condition;
const expr* increment;
const stmt* body;
};



class s_for_each_stmt : public stmt {
public:
s_for_each_stmt(const char* file,const int line, const token* identifier, const expr* container, const stmt* body) : identifier(identifier), container(container), body(body), stmt(line, file, S_FOR_EACH_STMT_T) {}
void accept(stmt_visitor *v) override{
v->visit_s_for_each_stmt(this);
}
const token* identifier;
const expr* container;
const stmt* body;
};



class exception_decl_stmt : public stmt {
public:
exception_decl_stmt(const char* file,const int line, const token* name, const set<string>& identifiers, const visibility visibility) : name(name), identifiers(identifiers), visibility(visibility), stmt(line, file, EXCEPTION_DECL_STMT_T) {}
void accept(stmt_visitor *v) override{
v->visit_exception_decl_stmt(this);
}
const token* name;
const set<string>& identifiers;
const visibility visibility;
};



class raise_stmt : public stmt {
public:
raise_stmt(const char* file,const int line, const token* where, const expr* name, const map<string,expr*>& fields) : where(where), name(name), fields(fields), stmt(line, file, RAISE_STMT_T) {}
void accept(stmt_visitor *v) override{
v->visit_raise_stmt(this);
}
const token* where;
const expr* name;
const map<string,expr*>& fields;
};



class uses_native_stmt : public stmt {
public:
uses_native_stmt(const char* file,const int line, const token* where, const string& lib_name, const token* bind) : where(where), lib_name(lib_name), bind(bind), stmt(line, file, USES_NATIVE_STMT_T) {}
void accept(stmt_visitor *v) override{
v->visit_uses_native_stmt(this);
}
const token* where;
const string& lib_name;
const token* bind;
};



class handle_stmt : public stmt {
public:
handle_stmt(const char* file,const int line, const expr* exception_name, const token* bind, const vector<stmt*>& block) : exception_name(exception_name), bind(bind), block(block), stmt(line, file, HANDLE_STMT_T) {}
void accept(stmt_visitor *v) override{
v->visit_handle_stmt(this);
}
const expr* exception_name;
const token* bind;
const vector<stmt*>& block;
};



class begin_handle_stmt : public stmt {
public:
begin_handle_stmt(const char* file,const int line, const vector<stmt*>& body, const vector<handle_stmt*> handles) : body(body), handles(handles), stmt(line, file, BEGIN_HANDLE_STMT_T) {}
void accept(stmt_visitor *v) override{
v->visit_begin_handle_stmt(this);
}
const vector<stmt*>& body;
const vector<handle_stmt*> handles;
};



class constructor_stmt : public stmt {
public:
constructor_stmt(const char* file,const int line, const token* keyword, const vector<token*>& parameters, const vector<stmt*>& body, const visibility visibility) : keyword(keyword), parameters(parameters), body(body), visibility(visibility), stmt(line, file, CONSTRUCTOR_STMT_T) {}
void accept(stmt_visitor *v) override{
v->visit_constructor_stmt(this);
}
const token* keyword;
const vector<token*>& parameters;
const vector<stmt*>& body;
const visibility visibility;
};



class class_decl_stmt : public stmt {
public:
class_decl_stmt(const char* file,const int line, const token* name, const vector<function_stmt*>& methods, const vector<constructor_stmt*>& constructors, const vector<var_stmt*> variables, const visibility visibility) : name(name), methods(methods), constructors(constructors), variables(variables), visibility(visibility), stmt(line, file, CLASS_DECL_STMT_T) {}
void accept(stmt_visitor *v) override{
v->visit_class_decl_stmt(this);
}
const token* name;
const vector<function_stmt*>& methods;
const vector<constructor_stmt*>& constructors;
const vector<var_stmt*> variables;
const visibility visibility;
};



class null_stmt : public stmt {
public:
explicit null_stmt(const char* file,const int line, const token* where) : where(where), stmt(line, file, NULL_STMT_T) {}
void accept(stmt_visitor *v) override{
v->visit_null_stmt(this);
}
const token* where;
};


class default_stmt_visitor : public stmt_visitor {
public:
virtual void visit_block_stmt(block_stmt *b) override {
std::cout << "block_stmt" << std::endl;
}
virtual void visit_expression_stmt(expression_stmt *e) override {
std::cout << "expression_stmt" << std::endl;
}
virtual void visit_function_stmt(function_stmt *f) override {
std::cout << "function_stmt" << std::endl;
}
virtual void visit_context_stmt(context_stmt *c) override {
std::cout << "context_stmt" << std::endl;
}
virtual void visit_if_stmt(if_stmt *i) override {
std::cout << "if_stmt" << std::endl;
}
virtual void visit_return_stmt(return_stmt *r) override {
std::cout << "return_stmt" << std::endl;
}
virtual void visit_break_stmt(break_stmt *b) override {
std::cout << "break_stmt" << std::endl;
}
virtual void visit_continue_stmt(continue_stmt *c) override {
std::cout << "continue_stmt" << std::endl;
}
virtual void visit_var_stmt(var_stmt *v) override {
std::cout << "var_stmt" << std::endl;
}
virtual void visit_s_while_stmt(s_while_stmt *s) override {
std::cout << "s_while_stmt" << std::endl;
}
virtual void visit_s_for_stmt(s_for_stmt *s) override {
std::cout << "s_for_stmt" << std::endl;
}
virtual void visit_s_for_each_stmt(s_for_each_stmt *s) override {
std::cout << "s_for_each_stmt" << std::endl;
}
virtual void visit_exception_decl_stmt(exception_decl_stmt *e) override {
std::cout << "exception_decl_stmt" << std::endl;
}
virtual void visit_raise_stmt(raise_stmt *r) override {
std::cout << "raise_stmt" << std::endl;
}
virtual void visit_uses_native_stmt(uses_native_stmt *u) override {
std::cout << "uses_native_stmt" << std::endl;
}
virtual void visit_handle_stmt(handle_stmt *h) override {
std::cout << "handle_stmt" << std::endl;
}
virtual void visit_begin_handle_stmt(begin_handle_stmt *b) override {
std::cout << "begin_handle_stmt" << std::endl;
}
virtual void visit_constructor_stmt(constructor_stmt *c) override {
std::cout << "constructor_stmt" << std::endl;
}
virtual void visit_class_decl_stmt(class_decl_stmt *c) override {
std::cout << "class_decl_stmt" << std::endl;
}
virtual void visit_null_stmt(null_stmt *n) override {
std::cout << "null_stmt" << std::endl;
}
};
}
#endif
