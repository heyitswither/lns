//
// Created by lorenzo on 12/27/17.
//

#include "signals.h"

void __signal_handler(int signal) {
    switch(signal){
        SIG_CASE(11,SIGSEGV)
        SIG_CASE(6,SIGABRT)
        SIG_CASE(2,SIGINT)
    }
}
