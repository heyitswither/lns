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
BLOCK_STMT_T, EXPRESSION_STMT_T, FUNCTION_STMT_T, CONTEXT_STMT_T, IF_STMT_T, RETURN_STMT_T, BREAK_STMT_T, CONTINUE_STMT_T, VAR_STMT_T, S_WHILE_STMT_T, S_FOR_STMT_T, USES_NATIVE_STMT_T, NULL_STMT_T, 
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
class uses_native_stmt;
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
virtual void visit_uses_native_stmt(uses_native_stmt *u) = 0;
virtual void visit_null_stmt(null_stmt *n) = 0;
};

class block_stmt : public stmt {
public:
explicit block_stmt(const char* file,int line, vector<stmt*>& statements) : statements(statements), stmt(line, file, BLOCK_STMT_T) {}
void accept(stmt_visitor *v) override{
v->visit_block_stmt(this);
}
const vector<stmt*>& statements;
};



class expression_stmt : public stmt {
public:
explicit expression_stmt(const char* file,int line, expr& exprs) : exprs(exprs), stmt(line, file, EXPRESSION_STMT_T) {}
void accept(stmt_visitor *v) override{
v->visit_expression_stmt(this);
}
const expr& exprs;
};



class function_stmt : public stmt {
public:
function_stmt(const char* file,int line, token& name, vector<token>& parameters, vector<stmt*>& body, bool isglobal) : name(name), parameters(parameters), body(body), isglobal(isglobal), stmt(line, file, FUNCTION_STMT_T) {}
void accept(stmt_visitor *v) override{
v->visit_function_stmt(this);
}
const token& name;
const vector<token>& parameters;
const vector<stmt*>& body;
const bool isglobal;
};



class context_stmt : public stmt {
public:
context_stmt(const char* file,int line, token& name, vector<stmt*> body, bool is_global, bool isfinal) : name(name), body(body), is_global(is_global), isfinal(isfinal), stmt(line, file, CONTEXT_STMT_T) {}
void accept(stmt_visitor *v) override{
v->visit_context_stmt(this);
}
const token& name;
const vector<stmt*> body;
const bool is_global;
const bool isfinal;
};



class if_stmt : public stmt {
public:
if_stmt(const char* file,int line, expr& condition, stmt* thenBranch, stmt* elseBranch) : condition(condition), thenBranch(thenBranch), elseBranch(elseBranch), stmt(line, file, IF_STMT_T) {}
void accept(stmt_visitor *v) override{
v->visit_if_stmt(this);
}
const expr& condition;
const stmt* thenBranch;
const stmt* elseBranch;
};



class return_stmt : public stmt {
public:
return_stmt(const char* file,int line, token& keyword, expr& value) : keyword(keyword), value(value), stmt(line, file, RETURN_STMT_T) {}
void accept(stmt_visitor *v) override{
v->visit_return_stmt(this);
}
const token& keyword;
const expr& value;
};



class break_stmt : public stmt {
public:
explicit break_stmt(const char* file,int line, token& keyword) : keyword(keyword), stmt(line, file, BREAK_STMT_T) {}
void accept(stmt_visitor *v) override{
v->visit_break_stmt(this);
}
const token& keyword;
};



class continue_stmt : public stmt {
public:
explicit continue_stmt(const char* file,int line, token& keyword) : keyword(keyword), stmt(line, file, CONTINUE_STMT_T) {}
void accept(stmt_visitor *v) override{
v->visit_continue_stmt(this);
}
const token& keyword;
};



class var_stmt : public stmt {
public:
var_stmt(const char* file,int line, token& name, expr& initializer, bool isglobal, bool isfinal) : name(name), initializer(initializer), isglobal(isglobal), isfinal(isfinal), stmt(line, file, VAR_STMT_T) {}
void accept(stmt_visitor *v) override{
v->visit_var_stmt(this);
}
const token& name;
const expr& initializer;
const bool isglobal;
const bool isfinal;
};



class s_while_stmt : public stmt {
public:
s_while_stmt(const char* file,int line, expr& condition, stmt* body) : condition(condition), body(body), stmt(line, file, S_WHILE_STMT_T) {}
void accept(stmt_visitor *v) override{
v->visit_s_while_stmt(this);
}
const expr& condition;
const stmt* body;
};



class s_for_stmt : public stmt {
public:
s_for_stmt(const char* file,int line, stmt* init, expr* condition, expr* increment, stmt* body) : init(init), condition(condition), increment(increment), body(body), stmt(line, file, S_FOR_STMT_T) {}
void accept(stmt_visitor *v) override{
v->visit_s_for_stmt(this);
}
const stmt* init;
const expr* condition;
const expr* increment;
const stmt* body;
};



class uses_native_stmt : public stmt {
public:
uses_native_stmt(const char* file,int line, token& where, string& lib_name, token& bind) : where(where), lib_name(lib_name), bind(bind), stmt(line, file, USES_NATIVE_STMT_T) {}
void accept(stmt_visitor *v) override{
v->visit_uses_native_stmt(this);
}
const token& where;
const string& lib_name;
const token& bind;
};



class null_stmt : public stmt {
public:
explicit null_stmt(const char* file,int line, token& where) : where(where), stmt(line, file, NULL_STMT_T) {}
void accept(stmt_visitor *v) override{
v->visit_null_stmt(this);
}
const token& where;
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
virtual void visit_uses_native_stmt(uses_native_stmt *u) override {
std::cout << "uses_native_stmt" << std::endl;
}
virtual void visit_null_stmt(null_stmt *n) override {
std::cout << "null_stmt" << std::endl;
}
};
}
#endif
