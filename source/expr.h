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
ASSIGN_EXPR_T, BINARY_EXPR_T, CALL_EXPR_T, GROUPING_EXPR_T, LITERAL_EXPR_T, UNARY_EXPR_T, VARIABLE_EXPR_T, SUB_SCRIPT_EXPR_T, SUB_SCRIPT_ASSIGN_EXPR_T, MEMBER_EXPR_T, MEMBER_ASSIGN_EXPR_T, ARRAY_EXPR_T, NULL_EXPR_T, 
};
class expr_visitor;
class expr{
public:

expr(int line, const char* file, expr_type type) : type(type), line(line), file(file){}
virtual object* accept(expr_visitor* v) = 0;
expr_type type;
 int line; const char* file;
};
class assign_expr;
class binary_expr;
class call_expr;
class grouping_expr;
class literal_expr;
class unary_expr;
class variable_expr;
class sub_script_expr;
class sub_script_assign_expr;
class member_expr;
class member_assign_expr;
class array_expr;
class null_expr;
class expr_visitor{
public:
virtual object* visit_assign_expr(assign_expr *a) = 0;
virtual object* visit_binary_expr(binary_expr *b) = 0;
virtual object* visit_call_expr(call_expr *c) = 0;
virtual object* visit_grouping_expr(grouping_expr *g) = 0;
virtual object* visit_literal_expr(literal_expr *l) = 0;
virtual object* visit_unary_expr(unary_expr *u) = 0;
virtual object* visit_variable_expr(variable_expr *v) = 0;
virtual object* visit_sub_script_expr(sub_script_expr *s) = 0;
virtual object* visit_sub_script_assign_expr(sub_script_assign_expr *s) = 0;
virtual object* visit_member_expr(member_expr *m) = 0;
virtual object* visit_member_assign_expr(member_assign_expr *m) = 0;
virtual object* visit_array_expr(array_expr *a) = 0;
virtual object* visit_null_expr(null_expr *n) = 0;
};

class assign_expr : public expr {
public:
const token* name;
const token_type op;
const expr* value;
assign_expr(const char* file,const int line, const token* name, const token_type op, const expr* value) : expr(line, file, ASSIGN_EXPR_T), name(name), op(op), value(value) {}
object* accept(expr_visitor *v) override{
return v->visit_assign_expr(this);
}
};



class binary_expr : public expr {
public:
const expr* left;
const token* op;
const expr* right;
binary_expr(const char* file,const int line, const expr* left, const token* op, const expr* right) : expr(line, file, BINARY_EXPR_T), left(left), op(op), right(right) {}
object* accept(expr_visitor *v) override{
return v->visit_binary_expr(this);
}
};



class call_expr : public expr {
public:
const expr* callee;
const token* paren;
const vector<expr*>& args;
call_expr(const char* file,const int line, const expr* callee, const token* paren, const vector<expr*>& args) : expr(line, file, CALL_EXPR_T), callee(callee), paren(paren), args(args) {}
object* accept(expr_visitor *v) override{
return v->visit_call_expr(this);
}
};



class grouping_expr : public expr {
public:
const expr* expression;
explicit grouping_expr(const char* file,const int line, const expr* expression) : expr(line, file, GROUPING_EXPR_T), expression(expression) {}
object* accept(expr_visitor *v) override{
return v->visit_grouping_expr(this);
}
};



class literal_expr : public expr {
public:
const object* value;
explicit literal_expr(const char* file,const int line, const object* value) : expr(line, file, LITERAL_EXPR_T), value(value) {}
object* accept(expr_visitor *v) override{
return v->visit_literal_expr(this);
}
};



class unary_expr : public expr {
public:
const token* op;
const operator_location location;
const expr* right;
unary_expr(const char* file,const int line, const token* op, const operator_location location, const expr* right) : expr(line, file, UNARY_EXPR_T), op(op), location(location), right(right) {}
object* accept(expr_visitor *v) override{
return v->visit_unary_expr(this);
}
};



class variable_expr : public expr {
public:
const token* name;
explicit variable_expr(const char* file,const int line, const token* name) : expr(line, file, VARIABLE_EXPR_T), name(name) {}
object* accept(expr_visitor *v) override{
return v->visit_variable_expr(this);
}
};



class sub_script_expr : public expr {
public:
const token* where;
const expr* name;
const expr* key;
sub_script_expr(const char* file,const int line, const token* where, const expr* name, const expr* key) : expr(line, file, SUB_SCRIPT_EXPR_T), where(where), name(name), key(key) {}
object* accept(expr_visitor *v) override{
return v->visit_sub_script_expr(this);
}
};



class sub_script_assign_expr : public expr {
public:
const token* where;
const expr* name;
const token_type op;
const expr* key;
const expr* value;
sub_script_assign_expr(const char* file,const int line, const token* where, const expr* name, const token_type op, const expr* key, const expr* value) : expr(line, file, SUB_SCRIPT_ASSIGN_EXPR_T), where(where), name(name), op(op), key(key), value(value) {}
object* accept(expr_visitor *v) override{
return v->visit_sub_script_assign_expr(this);
}
};



class member_expr : public expr {
public:
const expr* container_name;
const token* member_identifier;
member_expr(const char* file,const int line, const expr* container_name, const token* member_identifier) : expr(line, file, MEMBER_EXPR_T), container_name(container_name), member_identifier(member_identifier) {}
object* accept(expr_visitor *v) override{
return v->visit_member_expr(this);
}
};



class member_assign_expr : public expr {
public:
const expr* container_name;
const token_type op;
const token* member_identifier;
const expr* value;
member_assign_expr(const char* file,const int line, const expr* container_name, const token_type op, const token* member_identifier, const expr* value) : expr(line, file, MEMBER_ASSIGN_EXPR_T), container_name(container_name), op(op), member_identifier(member_identifier), value(value) {}
object* accept(expr_visitor *v) override{
return v->visit_member_assign_expr(this);
}
};



class array_expr : public expr {
public:
const token* open_brace;
const vector<pair<expr*,expr*>>& pairs;
array_expr(const char* file,const int line, const token* open_brace, const vector<pair<expr*,expr*>>& pairs) : expr(line, file, ARRAY_EXPR_T), open_brace(open_brace), pairs(pairs) {}
object* accept(expr_visitor *v) override{
return v->visit_array_expr(this);
}
};



class null_expr : public expr {
public:
const token* where;
explicit null_expr(const char* file,const int line, const token* where) : expr(line, file, NULL_EXPR_T), where(where) {}
object* accept(expr_visitor *v) override{
return v->visit_null_expr(this);
}
};


class default_expr_visitor : public expr_visitor {
public:
object* visit_assign_expr(assign_expr *a) override {
std::cout << "assign_expr" << std::endl;
return nullptr;
}
object* visit_binary_expr(binary_expr *b) override {
std::cout << "binary_expr" << std::endl;
return nullptr;
}
object* visit_call_expr(call_expr *c) override {
std::cout << "call_expr" << std::endl;
return nullptr;
}
object* visit_grouping_expr(grouping_expr *g) override {
std::cout << "grouping_expr" << std::endl;
return nullptr;
}
object* visit_literal_expr(literal_expr *l) override {
std::cout << "literal_expr" << std::endl;
return nullptr;
}
object* visit_unary_expr(unary_expr *u) override {
std::cout << "unary_expr" << std::endl;
return nullptr;
}
object* visit_variable_expr(variable_expr *v) override {
std::cout << "variable_expr" << std::endl;
return nullptr;
}
object* visit_sub_script_expr(sub_script_expr *s) override {
std::cout << "sub_script_expr" << std::endl;
return nullptr;
}
object* visit_sub_script_assign_expr(sub_script_assign_expr *s) override {
std::cout << "sub_script_assign_expr" << std::endl;
return nullptr;
}
object* visit_member_expr(member_expr *m) override {
std::cout << "member_expr" << std::endl;
return nullptr;
}
object* visit_member_assign_expr(member_assign_expr *m) override {
std::cout << "member_assign_expr" << std::endl;
return nullptr;
}
object* visit_array_expr(array_expr *a) override {
std::cout << "array_expr" << std::endl;
return nullptr;
}
object* visit_null_expr(null_expr *n) override {
std::cout << "null_expr" << std::endl;
return nullptr;
}
};
}
#endif
