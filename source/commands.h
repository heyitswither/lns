//
// Created by lorenzo on 11/8/17.
//

#ifndef CPPLNS_COMMAND_HANDLER_H
#define CPPLNS_COMMAND_HANDLER_H

#include <string>
#include <sstream>
#include <iostream>
#include "exceptions.h"
#include "debugger_defs.h"
#define MAX_BREAKPOINTS 128
#define MAX_WATCHES 32
using namespace std;
namespace lns{
    class breakpoint{
    public:
        breakpoint(const char* file, int line) : filename(file), line(line){}
        const char* filename;
        int line;
    };
    class command_handler {
        friend class command_interpreter;
    protected:
        virtual void load(string& filename) = 0;
        virtual void run() = 0;
        virtual void set_break_point(breakpoint* p) = 0;
        virtual void remove_break_point(int id) = 0;
        virtual void list_break_points() = 0;
        virtual void watch(string& expr) = 0;
        virtual void watch_all() = 0;
        virtual void add_watch(string& expression) = 0;
        virtual void remove_watch(int id) = 0;
        virtual void load_break_points(string& basic_string) = 0;
        virtual void action(lns::action action) = 0;
        virtual void exit() = 0;
    };

    class command_interpreter{
    private:
        command_handler* handler;
        stringstream& ss;
        string& advance(){
            string& s = *new string();
            if(ss){ ss >> s; return s;}
            return *new string("");

        }
        inline bool has_next() { return !advance().empty();}
        string& advance_or_throw(){
            string& b = advance();
            if(b.empty()) throw 1;
            return b;
        }
        string &consume(){
            string& b = advance_or_throw();
            if(has_next()) throw 1;
            return b;
        }
        void load() {
            try{
                string& file = consume();
                handler->load(file);
            }catch(int){
                throw "load";
            }
        }

        void run() {
            try{
                if(!has_next())
                    handler->run();
                else
                    throw 1;
            }catch(int){
                throw "run";
            }
        }

        void breakpoint() {
            try{
                string& sub = advance_or_throw();
                if(sub == "list" && !has_next())
                    handler->list_break_points();
                else if(sub == "set"){
                    string& file = advance_or_throw();
                    stringstream nrs(consume());
                    int n;
                    nrs >> n;
                    if(nrs.fail()) throw 1;
                    handler->set_break_point(new lns::breakpoint(file.c_str(),n));
                } else if(sub == "remove"){
                    stringstream nrs(consume());
                    int n;
                    nrs >> n;
                    if(nrs.fail()) throw 1;
                    handler->remove_break_point(n);
                } else if(sub == "load"){
                    handler->load_break_points(consume());
                } else throw 1;
            }catch(int){
                throw "breakpoint";
            }
        }

        string &line() {
            string& s = *new string();
            try{
                while(true) {
                    s += advance_or_throw();
                    s += " ";
                }
            }catch(int){}
            return s;
        }

        void watch() {
            try{
                string& sub = advance_or_throw();
                if(sub == "add"){
                    string& expression = line();
                    expression = expression.substr(0,expression.length()-1);
                    handler->add_watch(expression);
                } else if(sub == "remove"){
                    stringstream nrs(consume());
                    int n;
                    nrs >> n;
                    if(nrs.fail()) throw 1;
                    handler->remove_watch(n);
                }else if(sub == "all"){
                    handler->watch_all();
                } else {
                    string& expr = *new string();
                    expr = sub + " " + line();
                    handler->watch(expr);
                };
            }catch(int){
                throw "watch";
            }
        }

        void exit() {
            try{
                if(!has_next()) handler->exit();
                else throw 1;
            }catch(int){
                throw "exit";
            }
        }

        string syntax(const string &command) {
            if(command == "load")
                return "    - load|l <file>";
            if(command == "run")
                return "    - run|r";
            if(command == "breakpoint")
                return "    - breakpoint list | set <filename> <line> | remove <id> | load <file>";
            if(command == "watch")
                return "    - watch add <expression> | remove <id> | list";
            if(command == "exit")
                return "    - exit | e | quit | q";
            if(command == "step")
                return "    - step in|out|over";
            if(command == "continue"){
                return "    - continue";
            }
            return "";
        }

    public:
        command_interpreter() = delete;
        explicit command_interpreter(command_handler* handler) : handler(handler), ss(*new stringstream()){};

        void help() {
            cout    << "Available commands:\n\n"
                    << "load: \n"
                    << syntax("load") << endl << endl
                    << "run: \n"
                    << syntax("run") << endl << endl
                    << "breakpoint: \n"
                    << syntax("breakpoint") << endl << endl
                    << "watch: \n"
                    << syntax("watch") << endl << endl
                    << "continue: \n"
                    << syntax("continue") << endl << endl
                    << "step: \n"
                    << syntax("step") << endl << endl
                    << "exit: \n"
                    << syntax("exit") << endl << endl;
        }

        void step() {
            try{
                string& sub = consume();
                if(sub == "in") handler->action(action::STEP_IN);
                else if(sub == "out") handler->action(action::STEP_OUT);
                else if(sub == "over") handler->action(action::STEP_OVER);
                else throw 1;
            }catch(int){
                throw "step";
            }
        }

        void continue_() {
            try{
            if(!has_next()) handler->action(action::CONTINUE);
            else throw 1;
            }catch(int){
                throw "continue";
            }
        }

        void interpret(string& input){
            try{
                ss = stringstream(input);
                string& command = advance();
                if(command.empty()) throw command.c_str();
                else if(command == "load" || command == "l") load();
                else if(command == "run" || command == "r") run();
                else if(command == "breakpoint" || command == "b") breakpoint();
                else if(command == "watch" || command == "w") watch();
                else if(command == "exit" || command == "quit" || command == "q" || command == "e") exit();
                else if(command == "step" || command == "s") step();
                else if(command == "continue" || command == "c") continue_();
                else if(command == "help" || command == "h") help();
                else throw "";
            }catch(const char* e){
                if(string(e).empty())
                    cout << "Invalid command. Type 'help' for a list of commands." << endl;
                else
                    cout << "Wrong usage for command '" << e << "'. Usage:\n" << syntax(string(e)) << endl;
            }
        }

    };

}

#endif //CPPLNS_COMMAND_HANDLER_H
