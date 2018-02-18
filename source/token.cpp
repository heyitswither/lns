//
// Created by lorenzo on 2/17/18.
//

#include "token.h"

using namespace lns;
using namespace std;

token::token(token_type type, string lexeme, shared_ptr<object> literal, const char *filename, int line) : type(type),
                                                                                                           lexeme(std::move(
                                                                                                                   lexeme)),
                                                                                                           literal(literal),
                                                                                                           filename(
                                                                                                                   filename),
                                                                                                           line(line) {}

string token::format() const {
    stringstream ss;
    ss << "type: " << KEYWORDS_S_VALUES[type] << ", lexeme: " << lexeme << ", literal: |";
    string str = literal->str();
    ss << str;
    ss << "|, file: \"" << filename << "\", line:" << line;
    return *new string(ss.str());
}
