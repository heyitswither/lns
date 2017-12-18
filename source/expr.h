#ifndef LNS_EXPR_H
#define LNS_EXPR_H
#include "defs.h"
#include <list>
#include <string>
#include <iostream>
#include <utility>
using namespace std;
using namespace lns;


namespace lns{
enum expr_type{
INCREMENT_EXPR_T, DECREMENT_EXPR_T, ASSIGN_EXPR_T, BINARY_EXPR_T, CALL_EXPR_T, GROUPING_EXPR_T, LITERAL_EXPR_T, UNARY_EXPR_T, VARIABLE_EXPR_T, SUB_SCRIPT_EXPR_T, SUB_SCRIPT_ASSIGN_EXPR_T, CONTEXT_EXPR_T, CONTEXT_ASSIGN_EXPR_T, ARRAY_EXPR_T, NULL_EXPR_T, 
};
class expr_visitor;
class expr{
public:

expr(int line, const char* file, expr_type type) : type(type), line(line), file(file){}
virtual object* accept(expr_visitor* v) = 0;
expr_type type;
 int line; const char* file;
};
class increment_expr;
class decrement_expr;
class assign_expr;
class binary_expr;
class call_expr;
class grouping_expr;
class literal_expr;
class unary_expr;
class variable_expr;
class sub_script_expr;
class sub_script_assign_expr;
class context_expr;
class context_assign_expr;
class array_expr;
class null_expr;
class expr_visitor{
public:
virtual object* visit_increment_expr(increment_expr *i) = 0;
virtual object* visit_decrement_expr(decrement_expr *d) = 0;
virtual object* visit_assign_expr(assign_expr *a) = 0;
virtual object* visit_binary_expr(binary_expr *b) = 0;
virtual object* visit_call_expr(call_expr *c) = 0;
virtual object* visit_grouping_expr(grouping_expr *g) = 0;
virtual object* visit_literal_expr(literal_expr *l) = 0;
virtual object* visit_unary_expr(unary_expr *u) = 0;
virtual object* visit_variable_expr(variable_expr *v) = 0;
virtual object* visit_sub_script_expr(sub_script_expr *s) = 0;
virtual object* visit_sub_script_assign_expr(sub_script_assign_expr *s) = 0;
virtual object* visit_context_expr(context_expr *c) = 0;
virtual object* visit_context_assign_expr(context_assign_expr *c) = 0;
virtual object* visit_array_expr(array_expr *a) = 0;
virtual object* visit_null_expr(null_expr *n) = 0;
};

class increment_expr : public expr {
public:
increment_expr(const char* file,int line, token& name, expr* value) : name(name), value(value), expr(line, file, INCREMENT_EXPR_T) {}
object* accept(expr_visitor *v) override{
return v->visit_increment_expr(this);
}
const token& name;
const expr* value;
};



class decrement_expr : public expr {
public:
decrement_expr(const char* file,int line, token& name, expr* value) : name(name), value(value), expr(line, file, DECREMENT_EXPR_T) {}
object* accept(expr_visitor *v) override{
return v->visit_decrement_expr(this);
}
const token& name;
const expr* value;
};



class assign_expr : public expr {
public:
assign_expr(const char* file,int line, token& name, token_type op, expr* value) : name(name), op(op), value(value), expr(line, file, ASSIGN_EXPR_T) {}
object* accept(expr_visitor *v) override{
return v->visit_assign_expr(this);
}
const token& name;
const token_type op;
const expr* value;
};



class binary_expr : public expr {
public:
binary_expr(const char* file,int line, expr* left, token& op, expr* right) : left(left), op(op), right(right), expr(line, file, BINARY_EXPR_T) {}
object* accept(expr_visitor *v) override{
return v->visit_binary_expr(this);
}
const expr* left;
const token& op;
const expr* right;
};



class call_expr : public expr {
public:
call_expr(const char* file,int line, expr* callee, token& paren, vector<expr*>& args) : callee(callee), paren(paren), args(args), expr(line, file, CALL_EXPR_T) {}
object* accept(expr_visitor *v) override{
return v->visit_call_expr(this);
}
const expr* callee;
const token& paren;
const vector<expr*>& args;
};



class grouping_expr : public expr {
public:
explicit grouping_expr(const char* file,int line, expr* expression) : expression(expression), expr(line, file, GROUPING_EXPR_T) {}
object* accept(expr_visitor *v) override{
return v->visit_grouping_expr(this);
}
const expr* expression;
};



class literal_expr : public expr {
public:
explicit literal_expr(const char* file,int line, object* value) : value(value), expr(line, file, LITERAL_EXPR_T) {}
object* accept(expr_visitor *v) override{
return v->visit_literal_expr(this);
}
const object* value;
};



class unary_expr : public expr {
public:
unary_expr(const char* file,int line, token& op, expr* right) : op(op), right(right), expr(line, file, UNARY_EXPR_T) {}
object* accept(expr_visitor *v) override{
return v->visit_unary_expr(this);
}
const token& op;
const expr* right;
};



class variable_expr : public expr {
public:
explicit variable_expr(const char* file,int line, token& name) : name(name), expr(line, file, VARIABLE_EXPR_T) {}
object* accept(expr_visitor *v) override{
return v->visit_variable_expr(this);
}
const token& name;
};



class sub_script_expr : public expr {
public:
sub_script_expr(const char* file,int line, token& where, expr* name, expr* key) : where(where), name(name), key(key), expr(line, file, SUB_SCRIPT_EXPR_T) {}
object* accept(expr_visitor *v) override{
return v->visit_sub_script_expr(this);
}
const token& where;
const expr* name;
const expr* key;
};



class sub_script_assign_expr : public expr {
public:
sub_script_assign_expr(const char* file,int line, token& where, expr* name, token_type op, expr* key, expr* value) : where(where), name(name), op(op), key(key), value(value), expr(line, file, SUB_SCRIPT_ASSIGN_EXPR_T) {}
object* accept(expr_visitor *v) override{
return v->visit_sub_script_assign_expr(this);
}
const token& where;
const expr* name;
const token_type op;
const expr* key;
const expr* value;
};



class context_expr : public expr {
public:
context_expr(const char* file,int line, expr* context_name, token& context_identifier) : context_name(context_name), context_identifier(context_identifier), expr(line, file, CONTEXT_EXPR_T) {}
object* accept(expr_visitor *v) override{
return v->visit_context_expr(this);
}
const expr* context_name;
const token& context_identifier;
};



class context_assign_expr : public expr {
public:
context_assign_expr(const char* file,int line, expr* context_name, token_type op, token& context_identifier, expr* value) : context_name(context_name), op(op), context_identifier(context_identifier), value(value), expr(line, file, CONTEXT_ASSIGN_EXPR_T) {}
object* accept(expr_visitor *v) override{
return v->visit_context_assign_expr(this);
}
const expr* context_name;
const token_type op;
const token& context_identifier;
const expr* value;
};



class array_expr : public expr {
public:
array_expr(const char* file,int line, token& open_brace, vector<pair<expr*,expr*>>& pairs) : open_brace(open_brace), pairs(pairs), expr(line, file, ARRAY_EXPR_T) {}
object* accept(expr_visitor *v) override{
return v->visit_array_expr(this);
}
const token& open_brace;
const vector<pair<expr*,expr*>>& pairs;
};



class null_expr : public expr {
public:
explicit null_expr(const char* file,int line, token& where) : where(where), expr(line, file, NULL_EXPR_T) {}
object* accept(expr_visitor *v) override{
return v->visit_null_expr(this);
}
const token& where;
};


class default_expr_visitor : public expr_visitor {
public:
virtual object* visit_increment_expr(increment_expr *i) override {
std::cout << "increment_expr" << std::endl;
}
virtual object* visit_decrement_expr(decrement_expr *d) override {
std::cout << "decrement_expr" << std::endl;
}
virtual object* visit_assign_expr(assign_expr *a) override {
std::cout << "assign_expr" << std::endl;
}
virtual object* visit_binary_expr(binary_expr *b) override {
std::cout << "binary_expr" << std::endl;
}
virtual object* visit_call_expr(call_expr *c) override {
std::cout << "call_expr" << std::endl;
}
virtual object* visit_grouping_expr(grouping_expr *g) override {
std::cout << "grouping_expr" << std::endl;
}
virtual object* visit_literal_expr(literal_expr *l) override {
std::cout << "literal_expr" << std::endl;
}
virtual object* visit_unary_expr(unary_expr *u) override {
std::cout << "unary_expr" << std::endl;
}
virtual object* visit_variable_expr(variable_expr *v) override {
std::cout << "variable_expr" << std::endl;
}
virtual object* visit_sub_script_expr(sub_script_expr *s) override {
std::cout << "sub_script_expr" << std::endl;
}
virtual object* visit_sub_script_assign_expr(sub_script_assign_expr *s) override {
std::cout << "sub_script_assign_expr" << std::endl;
}
virtual object* visit_context_expr(context_expr *c) override {
std::cout << "context_expr" << std::endl;
}
virtual object* visit_context_assign_expr(context_assign_expr *c) override {
std::cout << "context_assign_expr" << std::endl;
}
virtual object* visit_array_expr(array_expr *a) override {
std::cout << "array_expr" << std::endl;
}
virtual object* visit_null_expr(null_expr *n) override {
std::cout << "null_expr" << std::endl;
}
};
}
#endif
