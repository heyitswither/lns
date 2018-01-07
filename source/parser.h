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

#define CHECK_ACCESS_SPEC_NOT_ALLOWED(condition,spec,for_) if(condition && !lns::permissive) throw error(previous(),ACCESS_SPEC_NOT_ALLOWED(spec,for_));

#define SIG_EXIT_PARSER (4096)

using namespace std;
using namespace lns;
namespace lns {
    static auto parsed_files = *new vector<const char *>();
    class parser {
    private:
        int start;
        int current;
        vector<token *> tokens;

        bool match(initializer_list<token_type> types);

        token *advance();

        token *previous();

        bool check(token_type tokenType);

        token *peek();

        void synchronize();

        token *consume(token_type type, const char message[]);

        void use_not_allowed();

        shared_ptr<stmt> use();

        vector<shared_ptr<stmt> > context_block();

        vector<shared_ptr<stmt> > stmts_until(initializer_list<token_type> list);

        shared_ptr<stmt> context_declaration(visibility vis, bool final);

        bool check_file(const char *str);

        bool dpcheck();

        shared_ptr<stmt> declaration();

        int error(token *token, const char *message);

        shared_ptr<var_stmt> var_declaration(visibility vis, bool is_final);

        shared_ptr<stmt> statement();

        shared_ptr<stmt> return_statement();

        shared_ptr<stmt> break_statement();

        shared_ptr<stmt> continue_statement();

        shared_ptr<stmt> if_statement();

        shared_ptr<stmt> while_statement();

        shared_ptr<stmt> for_statement();

        shared_ptr<stmt> expression_statement();

        shared_ptr<function_stmt> function(visibility vis);

        shared_ptr<expr> assignment(bool nested);

        shared_ptr<expr> array();

        shared_ptr<expr> comparison(bool nested);

        shared_ptr<expr> addition(bool nested);

        shared_ptr<expr> multiplication(bool nested);

        shared_ptr<expr> power(bool nested);

        shared_ptr<expr> unary(bool nested);

        shared_ptr<expr> call(bool nested, bool strict);

        shared_ptr<expr> special_assignment(bool nested);

        shared_ptr<expr> primary(bool nested);

    public:
        explicit parser(vector<token *> tokens);

        parser();

        vector<shared_ptr<stmt> > parse(bool ld_std);

        shared_ptr<expr> expression(bool nested);

        shared_ptr<expr> logical(bool nested);

        bool is_at_end();

        void reset(vector<token *> tokens);

        void ld_stmts(string, token *);

        shared_ptr<stmt> foreach_statement();

        shared_ptr<stmt> exception_(visibility vis);

        shared_ptr<stmt> raise();

        shared_ptr<stmt> begin_handle_statement();

        pair<visibility, bool> get_access_specifiers();

        shared_ptr<stmt> class_(lns::visibility vis);

        shared_ptr<constructor_stmt> constructor(visibility visibility);

        parameter_declaration parameters();

        shared_ptr<stmt> for_statement_init(token *where);

        shared_ptr<expr> for_statement_condition(token *where);

        shared_ptr<expr> for_statement_increment(token *where);

        vector<shared_ptr<stmt> > statements;
        bool use_allowed = true;

        shared_ptr<expr> new_();
    };


}
#endif //CPPLNS_PARSER_H
