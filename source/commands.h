//
// Created by lorenzo on 11/8/17.
//

#ifndef CPPLNS_COMMAND_HANDLER_H
#define CPPLNS_COMMAND_HANDLER_H

#include <string>
#include "defs.h"
#include <sstream>
#include <iostream>
#include "exceptions.h"

namespace lns {
    class command_interpreter;

    enum action {
        CONTINUE_A, STEP_IN, STEP_OUT, STEP_OVER, BREAK_A
    };

    class breakpoint {
    public:
        breakpoint(const char *file, int line);

        const char *filename;
        int line;
    };

    class command_handler {
        friend class command_interpreter;

    protected:
        virtual void load(std::string &filename) = 0;

        virtual void run() = 0;

        virtual void set_break_point(breakpoint *p) = 0;

        virtual void remove_break_point(int id) = 0;

        virtual void list_break_points() = 0;

        virtual void watch(std::string &expr) = 0;

        virtual void watch_all() = 0;

        virtual void add_watch(std::string &expression) = 0;

        virtual void remove_watch(int id) = 0;

        virtual void load_break_points(std::string &basic_string) = 0;

        virtual void action(lns::action action) = 0;

        virtual void exit() = 0;
    };

    class command_interpreter {
    private:
        command_handler *handler;
        std::stringstream &ss;

        std::string &advance();

        inline bool has_next();

        std::string &advance_or_throw();

        std::string &consume();

        void load();

        void run();

        void breakpoint();

        std::string &line();

        void watch();

        void exit();

        std::string syntax(const std::string &command);

    public:
        command_interpreter() = delete;

        explicit command_interpreter(command_handler *handler);

        void help();

        void step();

        void continue_();

        void interpret(std::string &input);
    };
}

#endif //CPPLNS_COMMAND_HANDLER_H
