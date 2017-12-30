#include <iostream>
#include "errors.h"
#include <fstream>
#include "scanner.h"
#include "expr.h"
#include "parser.h"
#include "interpreter.h"
#include "debug.h"
#include <iomanip>
#include "signals.h"

#define INTERPRETER_VERSION "0.2"
#define PROGRAM_START_SIGN "LNS v" INTERPRETER_VERSION ". All rights reserved.\n"

void register_signal_handlers();

using namespace std;
using namespace errors;
using namespace lns;
using namespace std::chrono;
namespace lns{

    static char * file = nullptr;
    static auto start_time = high_resolution_clock::now(),parsing_time = high_resolution_clock::now(),execution_time = high_resolution_clock::now();
    void option(string& o){
        if(o == "p" || o == "-skip-preprocessing"){
            direct_execution = true;
        }else if(o == "s" || o == "-silent-execution"){
            silent_execution = true;
        }else if(o == "S" || o == "-silent-full"){
            silent_full = true;
        }else if(o == "-permissive"){
            permissive = true;
        }else if(o == "t" || o == "-time-info"){
            time_count = true;
        }else if(o == "o" || o == "-parse-only"){
            parse_only = true;
        }else if(o == "d" || o == "-debugger"){
            debugger_option = true;
        }else if(o == "f" || o == "-no-format"){
            no_format = true;
        }else{
            throw unknown_option_exception(o);
        }
    }
    void inspect_args(int fc, const char **args){
        int i;
        string s;
        for(i = 1;i <= fc; i++){
            s = args[i];
            if(args[i][0] == '-'){
                s = string(args[i]).substr(1);
                option(s);
            }else{
                if(file == nullptr){
                    file = (char*)malloc((strlen(args[i]) + 1) * sizeof(char));
                    strcpy(file,args[i]);
                }else{
                    throw wrong_usage_exception();
                }
            }
        }
    }
    void run(const char* filename,string& source, interpreter * i, bool load_std){
        scanner scn (filename,source);
        vector<token*> tokens = scn.scan_tokens(true);
        vector<stmt*> stmts;
        if(had_parse_error) throw parse_exception();
        start_time = high_resolution_clock::now();
        parser parser(tokens);
        stmts = parser.parse(load_std);
        parsing_time = high_resolution_clock::now();
        if(parse_only){
            if(time_count) cout << "\nParsing time: " << std::setprecision(5) << duration_cast<microseconds>(parsing_time - start_time).count()/1000 << "ms.\n";
            if(lns::prompt) return; else std::exit(errors::exit_status);
        }
        //cout << "Total statements: " << to_string(stmts.size()) << endl;
        if(had_parse_error) throw parse_exception();
        if(i == nullptr) i = new interpreter();
        i->interpret(stmts);
        execution_time = high_resolution_clock::now();
        if(time_count){
            cout << endl;
            cout << "\nParse time:   " << std::setprecision(5) << (double)duration_cast<microseconds>(parsing_time - start_time).count()/1000 << "ms.\n";
            cout << "Execution time: " << std::setprecision(5) << (double)duration_cast<microseconds>(execution_time - parsing_time).count()/1000 << "ms.\n";
            cout << "Total time:     " << std::setprecision(5) << (double)duration_cast<microseconds>(execution_time - start_time).count()/1000 << "ms.\n";
        }
    }

    void reinit_errors() {
        had_parse_error = false;
        had_runtime_error = false;
    }
    void run_prompt(){
        lns::prompt = true;
        bool load_std = true;
        string source;
        const char filename[] = "stdin";
        interpreter *i = new interpreter();
        cout << PROGRAM_START_SIGN << endl;
        while(true){
            cin.clear();
            cout << "> ";
            getline(cin,source);
            try {
                run(filename,source,i,load_std);
                load_std = false;
            }catch(std::exception e){

            }
            reinit_errors();
        }
    }

    void run_file(const char* filename){
        ifstream file (filename);
        string source;
        stringstream ss;
        if(file.is_open()){
            stringstream ss;
            ss << file.rdbuf();
            source = ss.str();
            run(filename,source, nullptr,true);
        }else{
            throw file_not_found_exception(filename);
        }
    }
}


void register_signal_handlers() {
    signal(SIGINT,__signal_handler);
    signal(SIGSEGV,__signal_handler);
    signal(SIGABRT,__signal_handler);
}

int main(const int argc, const char* argv[]) {
    register_signal_handlers();
    try{
        lns::inspect_args(argc - 1, argv);
        if(debugger_option){
            debug* d = new debug(file);
            d->start();
        }else if(lns::file == nullptr){
            lns::run_prompt();
        }else{
            lns::run_file(lns::file);
        }
    }catch(lns::parse_exception& e){
        fatal_error(e, PARSE_ERROR);
    }catch(std::exception& e){
        fatal_error(e, FATAL_ERROR);
    }
    return EXIT_SUCCESS;
}

