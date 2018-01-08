//
// Created by lorenzo on 05/09/17.
//

#ifndef CPPLNS_OPTIONS_H
#define CPPLNS_OPTIONS_H

#include <vector>
#include <string>
namespace lns{
    extern bool ignore_unresolved;
    extern bool silent_full;
    extern bool silent_execution;
    extern bool permissive;
    extern bool time_count;
    extern bool parse_only;
    extern bool prompt;
    extern bool debugger_option;
    extern bool no_format;
    extern bool update_option;
    extern bool no_std_option;
    extern bool exec_from_command_line;
    extern std::string command_line_source;
    extern std::vector<std::string> command_line_use;
}
#endif //CPPLNS_OPTIONS_H
