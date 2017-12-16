//
// Created by lorenzo on 11/8/17.
//

#ifndef CPPLNS_DEBUG_H
#define CPPLNS_DEBUG_H
#define BP_DELIMITER string(":")
#define MAX_BREAKPOINTS 128
#define MAX_WATCHES 32

#include <string>
#include <sstream>
#include <iostream>
#include "commands.h"
#include "interpreter.h"
#include "scanner.h"
#include "parser.h"

namespace lns {

    class watch {
    public:
        string &text;
        lns::expr *e;

        watch() = delete;

        watch(string &text, expr *expr);

    };

    class debugger;

    class debug : command_handler {
    private:
        vector<stmt *> stmts;
        default_expr_visitor *visitor;
        scanner code_scanner;
        parser code_parser;
        command_interpreter command;
        debugger *interpreter;
        char *file;

        void action(lns::action action) override;

        void watch_all() override;

        void load(string &filename) override;

        void watch(string &expr) override;

        void build_and_run();

        void run() override;

        void set_break_point(breakpoint *p) override;

        void remove_break_point(int id) override;

        void list_break_points() override;

        void add_watch(string &expression) override;

        void remove_watch(int id) override;

        void load_break_points(string &filename) override;

        void exit() override;

        string &open_file(const char *filename);

        void loadcode(const char *filename);

    public:
        /*INDICATORS*/
        bool broken;
        bool started;
        bool ready;

        explicit debug(char *source);

        void break_(bool stepping, breakpoint *bp, int id);

        void broken_loop();

        void start();

        const char *filename;
        string &source;
    };


    class debugger : public interpreter {
    private:
        breakpoint *breakpoints[MAX_BREAKPOINTS];
        lns::watch *watches[MAX_WATCHES];
        scanner expr_scanner;
        parser expr_parser;
        lns::debug *debug_env;
        int target_depth;
    public:
        explicit debugger(lns::debug *debug);

        bool set_break_point(breakpoint *p, bool silent);

        void remove_break_point(int id);

        void list_break_points();

        void add_watch(string &expression);

        void remove_watch(int id);

        void load_break_points(string &filename);

        void watch(string &str);

        void action(lns::action action);

        void execute(stmt *s) override;

        int current_depth();

        void reset();

        expr *interpret_expression(string &str);

        void watch_all();
    };

}


#endif
//CPPLNS_DEBUG_H
