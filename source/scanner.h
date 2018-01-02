//
// Created by lorenzo on 31/08/17.
//

#ifndef CPPLNS_SCANNER_H
#define CPPLNS_SCANNER_H

#include <string>
#include <vector>
#include <map>
#include "defs.h"

using namespace std;
namespace lns {
    token_type get_token_type(string &key);

    class scanner {
        std::string source;
        const char *filename;
        vector<token *> tokens;
        int start;
        int current;
        int line;
    public:
        scanner(const char *filename, std::string &source);

        explicit scanner() = delete;

        void add_eof();

        void reset(const char *filename, std::string &source);

        vector<token *> &scan_tokens(bool addEOF);

    private:
        bool is_at_end();

        char peek_next();

        void number();

        void identifier();

        void line_comment();

        void block_comment();

        void scan_token();

        char advance() {
            current++;
            return source.at(current - 1);
        }

        void add_token(token_type type, shared_ptr<object> f);

        void add_token(token_type type);

        bool match(char expected);

        char peek();

        void string_();

        bool prmatch(char c);
    };

}
#endif //CPPLNS_SCANNER_H
