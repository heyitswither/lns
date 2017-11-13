//
// Created by lorenzo on 11/11/17.
//

#ifndef CPPLNS_DEBUGGER_DEFS_H
#define CPPLNS_DEBUGGER_DEFS_H
#include "interpreter.h"
#include "scanner.h"
#include "parser.h"
namespace lns{
    class expr;
    enum action{
        CONTINUE, STEP_IN, STEP_OUT, STEP_OVER
    };
}

#endif //CPPLNS_DEBUGGER_DEFS_H
