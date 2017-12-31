
#ifndef CPPLNS_DEBUG_H
#define CPPLNS_DEBUG_H

#include <string>
#include <sstream>
#include <iostream>
#include "commands.h"
#include "interpreter.h"
#include "scanner.h"
#include "parser.h"
#include "revision.h"

#define DEBUGGER_STARTING "\n>>>>>>>>lns debugger rev" << std::to_string(REVISION_NUMBER) << "<<<<<<<<\n      All rights reserved.      \n\n\n"
#define BP_DELIMITER string(":") /*The delimiter to be used in breakpoint files*/
#define MAX_BREAKPOINTS 128
#define MAX_WATCHES 32

namespace lns {

    class watch {
    public:
        /*The input text to be shown next to the evaluation*/
        string &text;
        /*The parsed expression*/
        lns::expr *e;

        watch() = delete;

        watch(string &text, expr *expr);

    };

    /*Required for class debug*/
    class debugger;

    /*The debug environment, instanced directly in file lns.cpp*/
    class debug : command_handler {
    private:
        /*The statements to be executed*/
        vector<shared_ptr<stmt>> stmts;
        /*The scanner*/
        scanner code_scanner;
        /*The parser*/
        parser code_parser;
        /*The command interpreter*/
        command_interpreter command;
        /*The code interpreter*/
        debugger *interpreter;
        /*Stores the name of the file given by the user*/
        char *file;

        /*Calls the corresponding method in the debugger*/
        void action(lns::action action) override;

        /*Calls the corresponding method in the debugger*/
        void watch_all() override;

        /*Calls the method loadcode()*/
        void load(string &filename) override;

        /*Calls the corresponding method in the debugger*/
        void watch(string &expr) override;

        /*Begins the execution*/
        void run() override;

        /*Calls the corresponding method in the debugger*/
        void set_break_point(breakpoint *p) override;

        /*Calls the corresponding method in the debugger*/
        void remove_break_point(int id) override;

        /*Calls the corresponding method in the debugger*/
        void list_break_points() override;

        /*Calls the corresponding method in the debugger*/
        void add_watch(string &expression) override;

        /*Calls the corresponding method in the debugger*/
        void remove_watch(int id) override;

        /*Calls the corresponding method in the debugger*/
        void load_break_points(string &filename) override;

        /*Exits the program*/
        void exit() override;

        /*Opens the specified file and returns its content as a string*/
        string &open_file(const char *filename);

        /*Scans and parses the code, stores the statements in 'stmts' and the source code in 'source'*/
        void loadcode(const char *filename);

    public:
        /*Status indicators*/
        bool broken;
        bool started;
        bool ready;

        explicit debug(char *source);

        /* Called by the debugger, enters break mode, prints information about the current breakpoint and
         * calls broken_loop*/
        void break_(bool stepping, breakpoint *bp, int id);

        /*While 'broken' is true, waits for user input and calls command_interpreter.interpret()*/
        void broken_loop();

        /*Prints the first line to stdout, calls broken_loop()*/
        void start();

    };


    class debugger : public interpreter{
    private:
        breakpoint *breakpoints[MAX_BREAKPOINTS];
        lns::watch *watches[MAX_WATCHES];
        scanner expr_scanner;
        parser expr_parser;
        lns::debug *debug_env;
        int bp_id;
    public:

        lns::action current_action;

        int depth;

        explicit debugger(lns::debug *debug);

        /*Checks if the maximum number of breakpoints has been reached, if not adds the new breakpoint*/
        bool set_break_point(breakpoint *p, bool silent);

        /*Removes the breakpoint with the specified index*/
        void remove_break_point(int id);

        /*Lists the current active breakpoints*/
        void list_break_points();

        /*Adds and expression to the watch list*/
        void add_watch(string &expression);

        /*Removes the watch with the specified index*/
        void remove_watch(int id);

        /*Loads breakpoints from a file*/
        void load_break_points(string &filename);

        /*Evaluates an expression and prints its value*/
        void watch(string &str);

        /*Updates the current action and exits the loop in the debug environment*/
        void action(lns::action action);

        /*Executes the statement according to the current action*/
        void execute(stmt *s) override;

        /*Resets all properties of the interpreter*/
        void reset();

        /*Parses an expression*/
        expr *parse_expression(string &str);

        /*Evaluates all the expressions in the watch list and prints their value*/
        void watch_all();

        /*Check if the given statement is targeted by a breakpoint*/
        breakpoint* check_bp(stmt *s);

        /*Returns the length of the stack trace*/
        inline int current_depth();
    };

}


#endif
//CPPLNS_DEBUG_H
