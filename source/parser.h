//
// Created by lorenzo on 04/09/17.
//

#ifndef CPPLNS_PARSER_H
#define CPPLNS_PARSER_H
#include <string>
#include <vector>
#include "defs.h"
#include "expr.h"
#include "stmt.h"
#include "options.h"
#include "scanner.h"
#include <initializer_list>
#include <fstream>
#include <climits>

#define EXPTOCLOSE(what, line) (string("expected 'end' to close " #what " (opened at line ") + std::to_string(line) + string(")"))
#define SIG_EXIT_PARSER (4096)

using namespace std;
using namespace lns;
namespace lns {
    static auto parsed_files = *new vector<const char *>();
    class parser {
    private:
        int start;
        int current;
        bool use_allowed = true;
        vector<token *> &tokens;
        vector<stmt *> &statements;

        bool match(initializer_list<token_type> types);

        token &advance();

        token &previous();

        bool check(token_type tokenType);

        token &peek();

        void synchronize();

        token &consume(token_type type, const char message[]);

        void use_not_allowed();

        stmt *use();

        vector<stmt *> &context_block();

        vector<stmt *> &stmts_until(initializer_list<token_type> list);

        vector<stmt *> &block();

        stmt *context_declaration(bool global, bool final);

        bool check_file(const char *str);

        bool dpcheck();

        stmt *declaration();

        int error(token &token, const char *message);

        var_stmt *var_declaration(bool is_global, bool is_final);

        stmt *statement();

        stmt *return_statement();

        stmt *break_statement();

        stmt *continue_statement();

        stmt *if_statement();

        stmt *while_statement();

        stmt *for_statement();

        stmt *expression_statement();

        stmt *function(bool isglobal);

        expr *assignment(bool nested);

        expr *array();

        expr *comparison(bool nested);

        expr *addition(bool nested);

        expr *multiplication(bool nested);

        expr *power(bool nested);

        expr *unary(bool nested);

        expr *call(bool nested);

        expr *finish_call(expr *callee);

        expr *special_assignment(bool nested);

        expr *primary(bool nested);

    public:
        explicit parser(vector<token *> &tokens);

        parser() = delete;

        vector<stmt *> &parse(bool ld_std);

        expr *expression(bool nested);

        expr *logical(bool nested);

        bool is_at_end();

        void reset(vector<token *> tokens);

        void ld_stmts(string &s);

        stmt *foreach_statement();
    };


}
#endif //CPPLNS_PARSER_H
