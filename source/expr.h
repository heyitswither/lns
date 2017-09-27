#ifndef LNS_EXPR_H
#define LNS_EXPR_H
#include "defs.h"
#include <list>
#include <string>
#include <iostream>
using namespace std;
using namespace lns;


namespace lns{
enum expr_type{
INCREMENT_EXPR_T, DECREMENT_EXPR_T, ASSIGN_EXPR_T, BINARY_EXPR_T, CALL_EXPR_T, GROUPING_EXPR_T, LITERAL_EXPR_T, UNARY_EXPR_T, VARIABLE_EXPR_T, MAP_FIELD_EXPR_T, ASSIGN_MAP_FIELD_EXPR_T, CONTEXT_EXPR_T, CONTEXT_ASSIGN_EXPR_T, NULL_EXPR_T, 
};
class expr_visitor;
class expr{
public:

expr(expr_type type) : type(type) {}
virtual object* accept(expr_visitor* v) = 0;
expr_type type;
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
class map_field_expr;
class assign_map_field_expr;
class context_expr;
class context_assign_expr;
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
virtual object* visit_map_field_expr(map_field_expr *m) = 0;
virtual object* visit_assign_map_field_expr(assign_map_field_expr *a) = 0;
virtual object* visit_context_expr(context_expr *c) = 0;
virtual object* visit_context_assign_expr(context_assign_expr *c) = 0;
virtual object* visit_null_expr(null_expr *n) = 0;
};

class increment_expr : public expr {
public:
increment_expr(token& name, expr* value) : name(name), value(value), expr(INCREMENT_EXPR_T) {}
object* accept(expr_visitor *v) override{
return v->visit_increment_expr(this);
}
const token& name;
const expr* value;
};



class decrement_expr : public expr {
public:
decrement_expr(token& name, expr* value) : name(name), value(value), expr(DECREMENT_EXPR_T) {}
object* accept(expr_visitor *v) override{
return v->visit_decrement_expr(this);
}
const token& name;
const expr* value;
};



class assign_expr : public expr {
public:
assign_expr(token& name, expr* value) : name(name), value(value), expr(ASSIGN_EXPR_T) {}
object* accept(expr_visitor *v) override{
return v->visit_assign_expr(this);
}
const token& name;
const expr* value;
};



class binary_expr : public expr {
public:
binary_expr(expr* left, token& op, expr* right) : left(left), op(op), right(right), expr(BINARY_EXPR_T) {}
object* accept(expr_visitor *v) override{
return v->visit_binary_expr(this);
}
const expr* left;
const token& op;
const expr* right;
};



class call_expr : public expr {
public:
call_expr(expr* callee, token& paren, vector<expr*>& args) : callee(callee), paren(paren), args(args), expr(CALL_EXPR_T) {}
object* accept(expr_visitor *v) override{
return v->visit_call_expr(this);
}
const expr* callee;
const token& paren;
const vector<expr*>& args;
};



class grouping_expr : public expr {
public:
explicit grouping_expr(expr* expression) : expression(expression), expr(GROUPING_EXPR_T) {}
object* accept(expr_visitor *v) override{
return v->visit_grouping_expr(this);
}
const expr* expression;
};



class literal_expr : public expr {
public:
explicit literal_expr(object* value) : value(value), expr(LITERAL_EXPR_T) {}
object* accept(expr_visitor *v) override{
return v->visit_literal_expr(this);
}
const object* value;
};



class unary_expr : public expr {
public:
unary_expr(token& op, expr* right) : op(op), right(right), expr(UNARY_EXPR_T) {}
object* accept(expr_visitor *v) override{
return v->visit_unary_expr(this);
}
const token& op;
const expr* right;
};



class variable_expr : public expr {
public:
explicit variable_expr(token& name) : name(name), expr(VARIABLE_EXPR_T) {}
object* accept(expr_visitor *v) override{
return v->visit_variable_expr(this);
}
const token& name;
};



class map_field_expr : public expr {
public:
map_field_expr(token& name, expr* key) : name(name), key(key), expr(MAP_FIELD_EXPR_T) {}
object* accept(expr_visitor *v) override{
return v->visit_map_field_expr(this);
}
const token& name;
const expr* key;
};



class assign_map_field_expr : public expr {
public:
assign_map_field_expr(token& name, expr* key, expr* value) : name(name), key(key), value(value), expr(ASSIGN_MAP_FIELD_EXPR_T) {}
object* accept(expr_visitor *v) override{
return v->visit_assign_map_field_expr(this);
}
const token& name;
const expr* key;
const expr* value;
};



class context_expr : public expr {
public:
context_expr(token& context_name, token& context_identifier) : context_name(context_name), context_identifier(context_identifier), expr(CONTEXT_EXPR_T) {}
object* accept(expr_visitor *v) override{
return v->visit_context_expr(this);
}
const token& context_name;
const token& context_identifier;
};



class context_assign_expr : public expr {
public:
context_assign_expr(token& context_name, token& context_identifier, expr* value) : context_name(context_name), context_identifier(context_identifier), value(value), expr(CONTEXT_ASSIGN_EXPR_T) {}
object* accept(expr_visitor *v) override{
return v->visit_context_assign_expr(this);
}
const token& context_name;
const token& context_identifier;
const expr* value;
};



class null_expr : public expr {
public:
explicit null_expr(token& where) : where(where), expr(NULL_EXPR_T) {}
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
virtual object* visit_map_field_expr(map_field_expr *m) override {
std::cout << "map_field_expr" << std::endl;
}
virtual object* visit_assign_map_field_expr(assign_map_field_expr *a) override {
std::cout << "assign_map_field_expr" << std::endl;
}
virtual object* visit_context_expr(context_expr *c) override {
std::cout << "context_expr" << std::endl;
}
virtual object* visit_context_assign_expr(context_assign_expr *c) override {
std::cout << "context_assign_expr" << std::endl;
}
virtual object* visit_null_expr(null_expr *n) override {
std::cout << "null_expr" << std::endl;
}
};
}
#endif
