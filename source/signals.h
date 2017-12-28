//
// Created by lorenzo on 12/27/17.
//

#ifndef CPPLNS_SIGNALS_H
#define CPPLNS_SIGNALS_H

#include <csignal>
#include "iostream"

#define SIG_DESC(a) "Received signal: " a ". Terminating.\n"
#define SIG_CASE(nr,name) case name: printf(SIG_DESC(#nr " (" #name ")")); exit(128 + nr);


void __signal_handler(int signal);

#endif //CPPLNS_SIGNALS_H
