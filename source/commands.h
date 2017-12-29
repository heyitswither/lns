/** lns, 2017 */


#ifndef CPPLNS_COMMAND_HANDLER_H
#define CPPLNS_COMMAND_HANDLER_H

#include <string>
#include "defs.h"
#include <sstream>
#include <iostream>
#include "exceptions.h"

namespace lns {

    /*Required for class command_handler*/
    class command_interpreter;

    /*Enumeration containing the possible actions given to the debugger*/
    enum action {
        CONTINUE_A, STEP_IN, STEP_OUT, STEP_OVER, BREAK_A
    };

    /*Class representing a breakpoint, whose only two properties are the filename and line*/
    class breakpoint {
    public:
        breakpoint(const char *file, int line);

        const char *filename;
        int line;
    };

    class command_handler {

        /*Grant access to methods in this class to comand_interpreter*/
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

    /*Debugger console handler*/
    class command_interpreter {
    private:
        /*Pointer to the handler that will execute the commands*/
        command_handler *handler;

        /*used to get one token at a time from user input*/
        std::stringstream &ss;

        /*returns the next token*/
        std::string &advance();

        /*calls advance() and checks if the value is not empty*/
        inline bool has_next();

        /*calls advance(), throws an integer if the end of input has beed reached*/
        std::string &advance_or_throw();

        /* expects the next token to be the last one. Calls advance_or_throw(), then throws an integer
         * if has_next() returns true */
        std::string &consume();

        /*parses command load*/
        void load();

        /*parses command run*/
        void run();

        /*parses command breakpoint*/
        void breakpoint();

        /*parses command watch*/
        void watch();

        /*parses command exit*/
        void exit();

        /*returns the remainder of the current input*/
        std::string &line();

        /*returns the syntad usage for the given argument*/
        std::string syntax(const std::string &command);

    public:
        /*the pointer to the handler is necessary*/
        command_interpreter() = delete;

        /*only constructor*/
        explicit command_interpreter(command_handler *handler);

        /*prints help*/
        void help();

        /*parses command step*/
        void step();

        /*parses command continue*/
        void continue_();

        /*interprets the input: consumes the first token and calls the right method to parse the rest of the line*/
        void interpret(std::string &input);
    };
}

#endif //CPPLNS_COMMAND_HANDLER_H
