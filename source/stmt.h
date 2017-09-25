#ifndef LNS_STMT_H
#define LNS_STMT_H
#include "defs.h"
#include <list>
#include <string>
#include "expr.h"
#include <memory>
using namespace std;
using namespace lns;


namespace lns{
class stmt_visitor;
class stmt{
public:

virtual void accept(stmt_visitor* v) = 0;
};
class block_stmt;
class expression_stmt;
class function_stmt;
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
explicit block_stmt(vector<stmt*>& statements) : statements(statements) {}
void accept(stmt_visitor *v) override{
v->visit_block_stmt(this);
}
const vector<stmt*>& statements;
};



class expression_stmt : public stmt {
public:
explicit expression_stmt(expr& exprs) : exprs(exprs) {}
void accept(stmt_visitor *v) override{
v->visit_expression_stmt(this);
}
const expr& exprs;
};



class function_stmt : public stmt {
public:
function_stmt(token& name, vector<token>& parameters, vector<stmt*>& body, bool isglobal) : name(name), parameters(parameters), body(body), isglobal(isglobal) {}
void accept(stmt_visitor *v) override{
v->visit_function_stmt(this);
}
const token& name;
const vector<token>& parameters;
const vector<stmt*>& body;
const bool isglobal;
};



class if_stmt : public stmt {
public:
if_stmt(expr& condition, stmt* thenBranch, stmt* elseBranch) : condition(condition), thenBranch(thenBranch), elseBranch(elseBranch) {}
void accept(stmt_visitor *v) override{
v->visit_if_stmt(this);
}
const expr& condition;
const stmt* thenBranch;
const stmt* elseBranch;
};



class return_stmt : public stmt {
public:
return_stmt(token& keyword, expr& value) : keyword(keyword), value(value) {}
void accept(stmt_visitor *v) override{
v->visit_return_stmt(this);
}
const token& keyword;
const expr& value;
};



class break_stmt : public stmt {
public:
explicit break_stmt(token& keyword) : keyword(keyword) {}
void accept(stmt_visitor *v) override{
v->visit_break_stmt(this);
}
const token& keyword;
};



class continue_stmt : public stmt {
public:
explicit continue_stmt(token& keyword) : keyword(keyword) {}
void accept(stmt_visitor *v) override{
v->visit_continue_stmt(this);
}
const token& keyword;
};



class var_stmt : public stmt {
public:
var_stmt(token& name, expr& initializer, bool isglobal, bool isfinal) : name(name), initializer(initializer), isglobal(isglobal), isfinal(isfinal) {}
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
s_while_stmt(expr& condition, stmt* body) : condition(condition), body(body) {}
void accept(stmt_visitor *v) override{
v->visit_s_while_stmt(this);
}
const expr& condition;
const stmt* body;
};



class s_for_stmt : public stmt {
public:
s_for_stmt(stmt* init, expr* condition, expr* increment, stmt* body) : init(init), condition(condition), increment(increment), body(body) {}
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
uses_native_stmt(token& token, string& path) : token(token), path(path) {}
void accept(stmt_visitor *v) override{
v->visit_uses_native_stmt(this);
}
const token& token;
const string& path;
};



class null_stmt : public stmt {
public:
explicit null_stmt(token& where) : where(where) {}
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
