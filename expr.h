#ifndef LNS_EXPR_H
#define LNS_EXPR_H
#include "defs.h"
#include <list>
#include <string>
using namespace std;
using namespace lns;


namespace lns{
class expr_visitor;
class expr{
public:

virtual object* accept(expr_visitor* v) = 0;
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
virtual object* visit_null_expr(null_expr *n) = 0;
};

class increment_expr : public expr {
public:
increment_expr(token& name, expr* value) : name(name), value(value) {}
object* accept(expr_visitor *v) override{
return v->visit_increment_expr(this);
}
const token& name;
const expr* value;
};



class decrement_expr : public expr {
public:
decrement_expr(token& name, expr* value) : name(name), value(value) {}
object* accept(expr_visitor *v) override{
return v->visit_decrement_expr(this);
}
const token& name;
const expr* value;
};



class assign_expr : public expr {
public:
assign_expr(token& name, expr* value) : name(name), value(value) {}
object* accept(expr_visitor *v) override{
return v->visit_assign_expr(this);
}
const token& name;
const expr* value;
};



class binary_expr : public expr {
public:
binary_expr(expr* left, token& op, expr* right) : left(left), op(op), right(right) {}
object* accept(expr_visitor *v) override{
return v->visit_binary_expr(this);
}
const expr* left;
const token& op;
const expr* right;
};



class call_expr : public expr {
public:
call_expr(expr* callee, token& paren, vector<expr*>& args) : callee(callee), paren(paren), args(args) {}
object* accept(expr_visitor *v) override{
return v->visit_call_expr(this);
}
const expr* callee;
const token& paren;
const vector<expr*>& args;
};



class grouping_expr : public expr {
public:
explicit grouping_expr(expr* expression) : expression(expression) {}
object* accept(expr_visitor *v) override{
return v->visit_grouping_expr(this);
}
const expr* expression;
};



class literal_expr : public expr {
public:
explicit literal_expr(object* value) : value(value) {}
object* accept(expr_visitor *v) override{
return v->visit_literal_expr(this);
}
const object* value;
};



class unary_expr : public expr {
public:
unary_expr(token& op, expr* right) : op(op), right(right) {}
object* accept(expr_visitor *v) override{
return v->visit_unary_expr(this);
}
const token& op;
const expr* right;
};



class variable_expr : public expr {
public:
explicit variable_expr(token& name) : name(name) {}
object* accept(expr_visitor *v) override{
return v->visit_variable_expr(this);
}
const token& name;
};



class map_field_expr : public expr {
public:
map_field_expr(token& name, expr* key) : name(name), key(key) {}
object* accept(expr_visitor *v) override{
return v->visit_map_field_expr(this);
}
const token& name;
const expr* key;
};



class assign_map_field_expr : public expr {
public:
assign_map_field_expr(token& name, expr* key, expr* value) : name(name), key(key), value(value) {}
object* accept(expr_visitor *v) override{
return v->visit_assign_map_field_expr(this);
}
const token& name;
const expr* key;
const expr* value;
};



class null_expr : public expr {
public:
explicit null_expr(token& where) : where(where) {}
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
virtual object* visit_null_expr(null_expr *n) override {
std::cout << "null_expr" << std::endl;
}
};
}
#endif
