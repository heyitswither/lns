//
// Created by lorenzo on 11/30/17.
//

#include "parser.h"
#include "defs.h"
#include <utility>
#include "exceptions.h"
#include "options.h"
#include "error_message.h"

using namespace lns;
using namespace std;

lns::object::object() : type(object_type::UNSPECIFIED){}

lns::object::object(lns::objtype t) : type(t) {}


lns::string_o::string_o(string value) : object(objtype::STRING_T), value(std::move(value)) {}

string lns::string_o::str() const {
    return value;
}

bool lns::string_o::operator==(const lns::object &o) const {
    if (o.type != STRING_T) return false;
    return (value == dynamic_cast<const string_o *>(&o)->value);
}

lns::object *lns::string_o::operator+=(const lns::object &o) {
    this->value += o.str();
    return this;
}

bool lns::string_o::operator>(const lns::object &o) const {
    if (!check_type(STRING_T, *this, o)) WRONG_OP(>);
    return this->value > dynamic_cast<const string_o &>(o).value;
}

bool lns::string_o::operator>=(const lns::object &o) const {
    if (!check_type(STRING_T, *this, o)) WRONG_OP(>=);
    return this->value >= dynamic_cast<const string_o &>(o).value;
}

bool lns::string_o::operator<=(const lns::object &o) const {
    if (!check_type(STRING_T, *this, o)) WRONG_OP(<=);
    return this->value <= dynamic_cast<const string_o &>(o).value;
}

bool lns::string_o::operator<(const lns::object &o) const {
    if (!check_type(STRING_T, *this, o)) WRONG_OP(<);
    return this->value < dynamic_cast<const string_o &>(o).value;
}

lns::object *lns::string_o::operator+(const lns::object &o) const {
    return new string_o(this->value + o.str());
}

lns::object *lns::string_o::operator/(const lns::object &o) const {
    if(o.type != STRING_T)
        return GET_DEFAULT_NULL();
    auto *map = new array_o();
    unsigned long i = 0;
    unsigned long current;
    string partial = this->value;
    while((current = partial.find(o.str())) != string::npos){
        map->values[i++] = new string_o(partial.substr(0, current));
        partial = partial.substr(current + 1,partial.size()-1);
    }
    map->values[i++] = new string_o(move(partial));
    return map;
}

object *string_o::clone() const {
    return new string_o(*new string(this->value));
}

string_o::string_o() : object(STRING_T), value(*new string()){

}

object *number_o::clone() const {
    return new number_o(this->value);
}
object *null_o::clone() const {
    return new null_o();
}
object *bool_o::clone() const{
    return new bool_o(this->value);
}
object *array_o::clone() const {
    array_o* map = new array_o();
    map->values = this->values;
    return map;
}

object *callable::clone() const {
    return nullptr;
}
object *context::clone() const {
    return nullptr;
}

lns::number_o::number_o(double value) : object(objtype::NUMBER_T), value(value) {}

string lns::number_o::str() const {
    stringstream ss;
    ss << value;
    return *new string(ss.str());
}

bool lns::number_o::operator==(const lns::object &o) const {
    if (o.type != NUMBER_T) return false;
    return value == dynamic_cast<number_o *>(const_cast<object *>(&o))->value;
}

bool lns::number_o::operator>(const lns::object &o) const {
    if (!check_type(NUMBER_T, *this, o)) WRONG_OP(>);
    return this->value > dynamic_cast<const number_o &>(o).value;
}

bool lns::number_o::operator>=(const lns::object &o) const {
    if (!check_type(NUMBER_T, *this, o)) WRONG_OP(>=);
    return this->value >= dynamic_cast<const number_o &>(o).value;
}

bool lns::number_o::operator<=(const lns::object &o) const {
    if (!check_type(NUMBER_T, *this, o)) WRONG_OP(<=);
    return this->value >= dynamic_cast<const number_o &>(o).value;
}

bool lns::number_o::operator<(const lns::object &o) const {
    if (!check_type(NUMBER_T, *this, o)) WRONG_OP(<);
    return this->value < dynamic_cast<const number_o &>(o).value;
}

lns::object *lns::number_o::operator+=(const lns::object &o) {
    if (!check_type(NUMBER_T, *this, o))WRONG_OP(+=);
    this->value += dynamic_cast<const number_o &>(o).value;
    return this;
}

lns::object *lns::number_o::operator-=(const lns::object &o) {
    if (!check_type(NUMBER_T, *this, o)) WRONG_OP(-=);
    this->value -= dynamic_cast<const number_o &>(o).value;
    return this;
}

lns::object *lns::number_o::operator*=(const lns::object &o) {
    if (!check_type(NUMBER_T, *this, o)) WRONG_OP(*=);
    this->value *= dynamic_cast<const number_o &>(o).value;
    return this;
}

lns::object *lns::number_o::operator/=(const lns::object &o) {
    if (!check_type(NUMBER_T, *this, o)) WRONG_OP(/=);
    this->value /= dynamic_cast<const number_o &>(o).value;
    return this;
}

lns::object *lns::number_o::operator+(const lns::object &o) const {
    if (!check_type(NUMBER_T, *this, o))  if(!(o.type == STRING_T)) WRONG_OP(+);
    if(o.type == STRING_T)
        return new string_o(this->str() + o.str());
    return new number_o(this->value + dynamic_cast<const number_o &>(o).value);
}

lns::object *lns::number_o::operator-(const lns::object &o) const {
    if (!check_type(NUMBER_T, *this, o)) WRONG_OP(-);
    return new number_o(this->value - dynamic_cast<const number_o &>(o).value);
}

lns::object *lns::number_o::operator*(const lns::object &o) const {
    if (!check_type(NUMBER_T, *this, o)) WRONG_OP(+);
    return new number_o(this->value * dynamic_cast<const number_o &>(o).value);
}

lns::object *lns::number_o::operator/(const lns::object &o) const {
    if (!check_type(NUMBER_T, *this, o)) WRONG_OP(+);
    return new number_o(this->value / dynamic_cast<const number_o &>(o).value);
}

lns::object *lns::number_o::operator^(const lns::object &o) const {
    if (!check_type(NUMBER_T, *this, o)) WRONG_OP(+);
    return new number_o(pow(this->value, dynamic_cast<const number_o &>(o).value));
}

lns::object *lns::number_o::operator-() const {
    return new number_o(this->value);
}

lns::object *lns::number_o::operator++() {
    return new number_o(++this->value);
}

lns::object *lns::number_o::operator--() {
    return new number_o(--this->value);
}

lns::object *lns::number_o::operator++(int) {
    return new number_o(this->value++);
}

lns::object *lns::number_o::operator--(int) {
    return new number_o(this->value++);
}

lns::bool_o::bool_o(bool value) : object(objtype::BOOL_T), value(value) {}

bool lns::bool_o::operator==(const lns::object &o) const {
    if (o.type != BOOL_T) return false;
    return value == dynamic_cast<const bool_o &>(o).value;
}

bool lns::bool_o::operator&&(const lns::object &o) const {
    if (!check_type(BOOL_T, *this, o)) WRONG_OP(and)
    return this->value && dynamic_cast<const bool_o &>(o).value;
}

bool lns::bool_o::operator||(const lns::object &o) const {
    if (!check_type(BOOL_T, *this, o)) WRONG_OP(or)
    return this->value || dynamic_cast<const bool_o &>(o).value;
}

lns::object *lns::bool_o::operator!() const {
    return new bool_o(!this->value);
}


lns::object *lns::bool_o::operator+(const lns::object &o) const {
    if(o.type == STRING_T) return new string_o(this->str() + o.str());
    WRONG_OP(+)
}


lns::null_o::null_o() : object(NULL_T) {}

string lns::null_o::str() const {
    return "null";
}

bool lns::null_o::operator==(const lns::object &o) const {
    return o.type == NULL_T;
}


bool array_o::operator==(const object &o) const {
    if (o.type != ARRAY_T) return false;
    return values == dynamic_cast<array_o *>(const_cast<object *>(&o))->values;
}


object *array_o::operator+(const object &o) const {
    if(o.type == STRING_T) return new string_o(this->str() + o.str());
    WRONG_OP(+)
}


array_o::array_o() : object(ARRAY_T) {}

string array_o::str() const {
    stringstream ss;
    ss << "{";
    int i = 0;
    for (auto p : values) {
        if (++i != 1) ss << ", ";
        ss << p.first << " : ";
        if (p.second->type == STRING_T) ss << "\"";
        ss << p.second->str();
        if (p.second->type == STRING_T) ss << "\"";
    }
    ss << "}";
    return ss.str();
}

const bool array_o::contains_key(double s) {
    return values.find(s) != values.end();
}

token::token(token_type type, string lexeme, object *literal, const char *filename, int line) : type(type),
                                                                                                lexeme(std::move(
                                                                                                        lexeme)),
                                                                                                literal(literal),
                                                                                                filename(filename),
                                                                                                line(line) {}

string token::format() const {
    stringstream ss;
    ss << "type: " << KEYWORDS_S_VALUES[type] << ", lexeme: " << lexeme << ", literal: |";
    string str = literal->str();
    ss << str;
    ss << "|, file: \"" << filename << "\", line:" << line;
    return *new string(ss.str());
}

stack_call::stack_call(const char *filename, const int line, const string &method, const bool native) : filename(filename), line(line),
                                                                                     method(method) , native(native){}

return_signal::return_signal(object *value, const token *keyword) : value(value), keyword(keyword) {}

const char *lns::return_signal::what() const throw(){
    return exception::what();
}

const char *lns::break_signal::what() const throw(){
    return exception::what();
}

break_signal::break_signal(const token *keyword) : keyword(keyword) {}

continue_signal::continue_signal(const token *keyword) : keyword(keyword) {}

const char *continue_signal::what() const throw(){
    return exception::what();
}

string context::str() const {
    stringstream s;
    s << "<context@" << static_cast<const void *>(this) << ">";
    return s.str();
}

bool context::operator==(const object &o) const {
    return false;
}

set<callable *> & context::declare_natives() const{
    std::set<callable*>& natives = *new std::set<callable*>();
    function_container* f;
    callable* clb;
    for(const auto &elem : this->values){
        if(DCAST_ASNCHK(f,function_container*,elem.second.value)){
            std::set<callable*>& temp = f->declare_natives();
            natives.insert(temp.begin(),temp.end());
        }
        if(elem.second.value->type == NATIVE_CALLABLE_T)
            natives.insert(DCAST(callable*,elem.second.value));
    }
    return natives;
}

bool lns::runtime_environment::contains_key(const std::string name) {
    return values.find(name) != values.end();
}

lns::runtime_environment::runtime_environment() : enclosing(nullptr), values(*new std::map<std::string, variable>()) {}

lns::runtime_environment::runtime_environment(runtime_environment *enc) : enclosing(enc),
                                                                          values(*new std::map<std::string, variable>) {}

object *runtime_environment::get(const token *name, const char *accessing_file) {
    if (contains_key(name->lexeme)) {
        variable s = values[name->lexeme];
        if(s.visibility_ == V_LOCAL){
            if(strcmp(s.def_file,accessing_file) != 0){
                throw runtime_exception(name, VARIABLE_NOT_VISIBLE(name->lexeme));
            }
        }
        return s.value;
    }
    if (enclosing != nullptr) return enclosing->get(name, accessing_file);
    if (permissive) { return lns::GET_DEFAULT_NULL(); }
    throw runtime_exception(name, VARIABLE_UNDEFINED(name->lexeme));
}


void runtime_environment::define(const token *name, object *o, bool is_final, visibility vis, const char *def_file) {
    if (contains_key(name->lexeme)) {
        throw runtime_exception(name, VARIABLE_ALREADY_DEFINED(name->lexeme));
    }

    if (vis == V_GLOBAL) {
        if (enclosing != nullptr) {
            return enclosing->define(name, o, is_final, vis,def_file);
        }
    }
    define(name->lexeme,o,is_final,vis, def_file);
}


object* lns::GET_DEFAULT_NULL() {
    return new null_o();
}

void runtime_environment::assign(const token *name, token_type op, object *obj, const char *assigning_file) {
    if (contains_key(name->lexeme)) {
        if (values[name->lexeme].is_final) {
            throw runtime_exception(name, VARIABLE_FINAL(name->lexeme));
        }
        //values[name.lexeme]->isfinal = false;
        try {
            variable v = values[name->lexeme];
            if(v.visibility_ == V_LOCAL){
                if(strcmp(v.def_file,assigning_file) != 0){
                    throw runtime_exception(name, VARIABLE_NOT_VISIBLE(name->lexeme));
                }
            }
            switch (op) {
                case EQUAL:
                    values[name->lexeme].value = obj;
                    break;
                case PLUS_EQUALS:
                    *v.value += *obj;
                    break;
                case MINUS_EQUALS:
                    *v.value -= *obj;
                    break;
                case STAR_EQUALS:
                    *v.value *= *obj;
                    break;
                case SLASH_EQUALS:
                    *v.value /= *obj;
                    break;
            }
        } catch (string &s) {
            throw runtime_exception(name, s.c_str());
        }
        return;
    }
    if (enclosing != nullptr) {
        return enclosing->assign(name, op, obj, nullptr);
    }
    if (permissive) { return define(name, obj, false, V_UNSPECIFIED,assigning_file); }
    throw runtime_exception(name, VARIABLE_UNDEFINED(name->lexeme));
}

bool runtime_environment::is_valid_object_type(objtype objtype) {
    switch (objtype) {
        case NUMBER_T:
        case STRING_T:
        case BOOL_T:
        case NULL_T:
        case CALLABLE_T:
        case ARRAY_T:
            return true;
    }
    return false;
}

bool runtime_environment::is_native(callable *ptr) {
    return natives.find(ptr) != natives.end();
}

void runtime_environment::reset() {
    values.clear();
    if (enclosing != nullptr) enclosing->reset();
}

void runtime_environment::define(const std::string &name, object *o, bool is_final,visibility vis, const char *def_file) {
    values.insert(std::pair<string, lns::variable>(name, *new lns::variable(vis,is_final, o, def_file)));
}

void runtime_environment::add_natives(const std::set<callable *> &natives) {
    this->natives.insert(natives.begin(),natives.end());
}

void runtime_environment::add_native(callable *ptr) {
    this->natives.insert(ptr);
}

bool runtime_environment::clear_var(const token *name) {
    return this->values.erase(name->lexeme) != 0;
}

variable::variable() : value(new null_o()), visibility_(V_UNSPECIFIED), is_final(false), def_file("") {}

const variable& variable::operator=(const variable &v) {
    this->value = v.value;
    return *this;
}

variable::variable(lns::visibility vis, bool is_final, object *value, const char *def_file) : visibility_ (vis), value(value), is_final(is_final), def_file(def_file) {}


lns::runtime_exception::runtime_exception(const lns::token *token, const char *m) : native_throw(false),message(m), token(token) {}

const char *lns::runtime_exception::what() const throw(){
    return message;
}

runtime_exception::runtime_exception(const char* filename, int line, const char *message) : native_throw(true), message(message), token(new lns::token(UNRECOGNIZED,STR(""),new null_o(),filename,line)){}
const char *lns::best_file_path(const char *filename) {
    ifstream rel_file(filename);
    auto buf = (char*) malloc(1024 * sizeof(char));
    if(rel_file.is_open()) return filename;
    *buf = '\0';
    strcat(buf,LNS_LIBRARY_LOCATION);
    strcat(buf,filename);
    ifstream path_file(buf);
    if(path_file.is_open()) return buf;
    strcat(buf,".lns");
    ifstream path_file2(buf);
    if(path_file2.is_open()) return buf;
    return filename;
}

callable::callable(bool native) : object(native ? objtype::NATIVE_CALLABLE_T : objtype ::CALLABLE_T) {}

bool callable::operator==(const object &o) const {
    return false;
}

string callable::str() const {
    stringstream s;
    s << "<" << (this->type == NATIVE_CALLABLE_T ? "native_" : "") << "function@" << static_cast<const void *>(this) << ", name: \"" << name() << "\">";
    return s.str();
}


callable::callable() : object(NATIVE_CALLABLE_T) {}

const char* lns::INVALID_OP(const char* OP, const lns::object_type t1, const lns::object_type* t2){
    auto * buf = (char*)malloc(sizeof(char) * 64);
    *buf = '\0';
    strcat(buf,"Operator '");
    strcat(buf,OP);
    strcat(buf,t2 == nullptr ? "' is not applicable to type '" : "' is not applicable to types '");
    strcat(buf,type_to_string(t1));
    if(t2 != nullptr){
        strcat(buf,"' and '");
        strcat(buf,type_to_string(*t2));
    }
    strcat(buf,"'");
    return buf;
}

const char *lns::type_to_string(object_type t) {
    switch(t){
        case NUMBER_T:return "number";
        case STRING_T:return "string";
        case BOOL_T:return "bool";
        case NULL_T:return "null";
        case ARRAY_T:return "array";
        case CALLABLE_T:return "callable";
        case CONTEXT_T:return "context";
        case NATIVE_CALLABLE_T: return "native_callable";
        case EXCEPTION_T: return "exception";
    }
    return "unknown";
}

function_container::function_container(objtype type) : object(type) {}

incode_exception::incode_exception(const lns::token *token, const std::string &name,
                                   const std::map<std::string, lns::object *> &fields) : lns::runtime_exception(token, name.c_str()), lns::object(EXCEPTION_T), fields(fields), calls_bypassed(0) {}

object *incode_exception::get(std::string &key) {
    if(fields.find(key) != fields.end()) return this->fields[key];
    return nullptr;
}

bool incode_exception::operator==(const object &o) const {
    return false;
}

std::string incode_exception::str() const {
    stringstream s;
    s << "<exception@" << static_cast<const void *>(this) << ", name: \"" << message << "\">";
    return s.str();
}

object *incode_exception::clone() const {
    return nullptr;
}

const char *incode_exception::what() const throw() {
    string& s = *new string();
    s += "unhandled exception: \"";
    s += message;
    s += "\"";
    if(this->fields.find("message") != this->fields.end()){
        s += " (message = \"";
        s += fields.at("message")->str();
        s += "\")";
    }
    return s.c_str();
}

lns::exception_definition::exception_definition(const char * file, int line, const std::string &name, const std::set<std::string> &fields) : object(EXCEPTION_DEFINITION_T), name(name), fields(fields), def_file(file), def_line(line){}

string exception_definition::str() const {
    stringstream s;
    s << "<exception_definition@" << static_cast<const void *>(this) << ", name: \"" << name << "\">";
    return s.str();
}

bool exception_definition::operator==(const object &o) const {
    if(o.type != EXCEPTION_DEFINITION_T) return false;
    const exception_definition *e;
    if(DCAST_ASNCHK(e,const exception_definition *,&o)) return this->def_file == e->def_file && this->def_line == e->def_line;
    return false;
}

object *exception_definition::clone() const {
    return nullptr;
}

const char *lns::concat(initializer_list<string> ss) {
    string s;
    for(auto &s1 : ss){
        s.append(s1);
    }
    char *ret = (char *) malloc(sizeof(char) * s.size());
    strcpy(ret, s.c_str());
    return ret;
}