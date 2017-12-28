//
// Created by lorenzo on 11/30/17.
//
#include "debug.h"
#include "errors.h"

#define ARROW "---> "
using namespace std;
using namespace lns;

breakpoint::breakpoint(const char *file, int line) : filename(file), line(line) {}

lns::watch::watch(string &text, expr *expr) : text(text), e(expr) {}

lns::debugger::debugger(lns::debug *debug) : expr_scanner(*new scanner("", *new string())),
                                             expr_parser(*new parser(*new vector<token *>())),
                                             debug_env(debug),
                                             current_action(CONTINUE_A),
                                             depth(0){}

bool lns::debugger::set_break_point(breakpoint *p, bool silent) {
    int i;
    for (i = 0; i < MAX_BREAKPOINTS; ++i) {
        if (breakpoints[i] == nullptr) {
            breakpoints[i] = p;
            break;
        }
    }
    if (silent) return i != MAX_BREAKPOINTS;
    if (i == MAX_BREAKPOINTS)
        cout << "Reached the maximum number of breakpoints." << endl;
    else
        cout << "Breakpoint #" << i << " set." << endl;
    return i != MAX_BREAKPOINTS;
}

void lns::debugger::watch(string &str) {
    expr *e;
    try {
        e = parse_expression(str);
        object *eval = this->evaluate(e);
        cout << str << " --> " << (eval->type == STRING_T ? ("\"" + eval->str() + "\"") : eval->str()) << endl;
    } catch (int) {
        cout << "The expression contains syntax errors." << endl;
        return;
    } catch (runtime_exception &ex) {
        cout << "Couldn't evaluate the expression in the current context: " << ex.what() << "." << endl;
    }

}

void lns::debugger::load_break_points(string &filename) {
    ifstream file(filename);
    string tmp;
    string *f;
    unsigned long pos;
    int line = 1;
    int l;
    int count = 0;
    if (file.is_open()) {
        while (std::getline(file, tmp)) {
            pos = tmp.find(BP_DELIMITER);
            if (pos == string::npos) {
                cout << "Warning: couldn't parse a breakpoint in file '" << filename << "', line " << line
                     << ": invalid format." << endl;
            } else {
                f = new string();
                stringstream(tmp.substr(0, pos)) >> *f;
                tmp = tmp.substr(pos + BP_DELIMITER.length());
                stringstream ss(tmp);
                ss >> l;
                if (ss.fail()) {
                    cout << "Warning: couldn't parse a breakpoint in file '" << filename << "', line " << line
                         << ": invalid line number." << endl;
                } else {
                    if (set_break_point(new breakpoint(f->c_str(), l), true)) {
                        ++count;
                    } else {
                        cout << "Reached the maximum number of breakpoints. ";
                        break;
                    }
                }
            }
            ++line;
        }
        file.close();
        cout << count << " breakpoints were added." << endl;
    } else {
        cout << "Couldn't load breakpoints from file " << filename << ": the file does not exist or is unaccessible."
             << endl;
    }
}

void lns::debugger::remove_watch(int id) {
    if (id >= MAX_WATCHES) {
        cout << "ID #" << id << " is out of bounds." << endl;
        return;
    }
    if (watches[id] == nullptr) {
        cout << "Couldn't find a watch with ID #" << id << "." << endl;
        return;
    }
    delete watches[id];
    watches[id] = nullptr;
    cout << "Watch #" << id << " removed." << endl;
}

void lns::debugger::add_watch(string &expression) {
    try {
        int i;
        for (i = 0; i < MAX_WATCHES; ++i) {
            if (watches[i] == nullptr) break;
        }
        watches[i] = new lns::watch(expression, parse_expression(expression));
        if (i == MAX_WATCHES)
            cout << "Reached the maximum number of watches." << endl;
        else
            cout << "Watch #" << i << " set." << endl;
    } catch (int i) {
        cout << "The expression contains syntax errors." << endl;
        return;
    }
}

void lns::debugger::list_break_points() {
    int i = 0, d;
    if (breakpoints[i] == nullptr) {
        cout << "No breakpoints were set." << endl;
        return;
    }
    cout << "Listing breakpoints:\n" << endl;
    for (; i < MAX_BREAKPOINTS; ++i) {
        if (breakpoints[i] == nullptr) continue;
        if (i < 10) d = 2;
        else if (i < 100) d = 1;
        else d = 0;
        cout << "Breakpoint " << (d == 2 ? "  " : (d == 1 ? " " : "")) << "#" << i << ":    "
             << breakpoints[i]->filename << ":" << breakpoints[i]->line << endl;

    }
}

void debugger::remove_break_point(int id) {
    if (id >= MAX_BREAKPOINTS) {
        cout << "ID #" << id << " is out of bounds." << endl;
        return;
    }
    if (breakpoints[id] == nullptr) {
        cout << "No breakpoint with ID #" << id << " was found." << endl;
        return;
    }
    delete breakpoints[id];
    breakpoints[id] = nullptr;
    cout << "Breakpoint #" << id << " removed" << endl;
}

void debugger::execute(stmt *s) {
    breakpoint *bp;
    if((bp = check_bp(s)) != nullptr){
        debug_env->break_(false,bp,bp_id);
    }
    interpreter::execute(s);
}

void debugger::action(lns::action action) {
    this->current_action = action;
    if (action == action::STEP_OUT)
        depth = current_depth() - 1;
    else if (action == action::STEP_OVER)
        depth = current_depth();
    else if (action == action::STEP_IN) {
        depth = current_depth() + 1;
    }
    debug_env->broken = false;
}


void debugger::reset() {
    if (environment != nullptr)
        environment->reset();
}

expr *debugger::parse_expression(string &expression) {
    expr *e;
    lns::silent_full = true;
    errors::had_parse_error = false;
    expr_scanner.reset("_expr_", expression);
    if (errors::had_parse_error) {
        errors::had_parse_error = false;
        lns::silent_full = false;
        throw 1;
    }
    expr_parser.reset(expr_scanner.scan_tokens(true));
    e = expr_parser.logical(true);
    if (errors::had_parse_error) {
        errors::had_parse_error = false;
        lns::silent_full = false;
        throw 1;
    }
    lns::silent_full = false;
    return e;
}

void debugger::watch_all() {
    int i = 0;
    bool found = false;
    for (; i < MAX_WATCHES; ++i) {
        if (watches[i] == nullptr) continue;
        found = true;
        cout << "Watch #" << i << ": " << watches[i]->text << " --> ";
        try {
            object *eval = evaluate(watches[i]->e);
            cout << (eval->type == STRING_T ? ("\"" + eval->str() + "\"") : eval->str()) << endl;
        } catch (int) {
            cout << "the expression contains syntax errors." << endl;
            return;
        } catch (runtime_exception &ex) {
            cout << "couldn't evaluate the expression in the current context: " << ex.what() << "." << endl;
        }
    }
    if (!found) cout << "No watches registered. Use 'watch add <expression>' to add a watch." << endl;
}

breakpoint *debugger::check_bp(stmt *s) {
    breakpoint *bp = nullptr;
    int i;
    for (i = 0; i < MAX_BREAKPOINTS; ++i) {
        if (breakpoints[i] == nullptr) break;
        if (strcmp(breakpoints[i]->filename, s->file) == 0 && breakpoints[i]->line == s->line) {
            bp = breakpoints[i];
            break;
        }
    }
    bp_id = i;
    return bp;
}

inline int debugger::current_depth() {
    return static_cast<int>(stack_trace.size());
}


void debug::action(lns::action action) {
    if (!started)
        cout << "The debugger hasn't started yet. Use the command 'run' to start the debugger" << endl;
    else
        interpreter->action(action);
}

void debug::watch_all() {
    if (started)
        interpreter->watch_all();
    else
        cout << "The debugger hasn't started yet. Use the command 'run' to start the debugger" << endl;
}

void debug::load(string &filename) {
    try {
        this->file = const_cast<char *>(filename.c_str());
        loadcode(file);
    } catch (exception &e) {
        cerr << "Couldn't load the specified file: " << e.what() << endl << endl;
    }
}

void debug::watch(string &expr) {
    if (started)
        interpreter->watch(expr);
    else
        cout << "The debugger hasn't started yet. Use the command 'run' to start the debugger" << endl;
}

void debug::run() {
    if (!ready) cout << "Load a script first. Use the command 'load' to open a file." << endl;
    else if (started) cout << "The script is already running." << endl;
    else {
        started = true;
        cout << "Running script...\n\n";
        interpreter->interpret(stmts);
        interpreter->reset();
        cout << "\nExecution terminated successfully.\n" << endl;
        broken = true;
        started = false;
    }
}

void debug::set_break_point(breakpoint *p) {
    interpreter->set_break_point(p, false);
}

void debug::remove_break_point(int id) {
    interpreter->remove_break_point(id);
}

void debug::list_break_points() {
    interpreter->list_break_points();
}

void debug::add_watch(string &expression) {
    interpreter->add_watch(expression);
}

void debug::remove_watch(int id) {
    interpreter->remove_watch(id);
}

void debug::load_break_points(string &filename) {
    interpreter->load_break_points(filename);
}

void debug::exit() {
    std::exit(0);
}

string &debug::open_file(const char *filename) {
    ifstream file(filename);
    string &source = *new string();
    stringstream ss;
    if (file.is_open()) {
        ss << file.rdbuf();
        source = ss.str();
        return source;
    } else {
        throw file_not_found_exception(filename);
    }
}

void debug::loadcode(const char *filename) {
    this->filename = filename;
    errors::had_parse_error = false;
    string &source = open_file(filename);
    code_scanner.reset(filename, source);
    vector<token *> &tokens = code_scanner.scan_tokens(true);
    vector<stmt *> stmts;
    if (errors::had_parse_error) throw parse_exception();
    code_parser.reset(tokens);
    vector<stmt *> parsed = code_parser.parse(true);
    if (errors::had_parse_error) throw parse_exception();
    this->stmts.clear();
    for (stmt *s : parsed) {
        this->stmts.push_back(s);
    }
    this->source = source;
    cout << "Script parsed and ready to run." << endl;
    ready = true;
}

lns::debug::debug(char *source) : command(this),
                                  visitor(new default_expr_visitor()),
                                  started(false),
                                  ready(false),
                                  file(source),
                                  stmts(*new vector<stmt *>()),
                                  code_scanner(*new scanner("", *new string())),
                                  code_parser(*new parser(*new vector<token *>())),
                                  source(*new string()) {
    lns::silent_full = false;
    lns::silent_execution = false;
    lns::parse_only = false;
    lns::time_count = false;
}

void debug::break_(bool stepping, breakpoint *bp, int id) {
    interpreter->current_action = action::BREAK_A;
    broken = true;
    ifstream file(bp->filename);
    stringstream ss;
    if (stepping) {
        cout << "Stepped on line " << bp->line << " in file " << bp->filename << ". Entering break mode." << endl;
    } else {
        cout << "Breakpoint #" << id << " (" << bp->filename << ":" << bp->line << ") was hit. Entering break mode."
             << endl;
    }
    if (file.is_open()) {
        string temp;
        string eline;
        string pline;
        string lline;
        int ln = 1;
        while (std::getline(file, temp)) {
            if (ln == bp->line - 1) pline = temp;
            if (ln == bp->line) eline = temp;
            if (ln == bp->line + 1) lline = temp;
            ln++;
        }
        if (eline.empty())
            cout
                    << "Binding error: the sources and the loaded code don't match (has the file been modified?). The debugger will enter break mode anyway."
                    << endl;
        else {
            cout << "In file " << bp->filename << ":\n";
            if (!pline.empty()) cout << "     line " << bp->line - 1 << ":   " << pline << endl;
            cout << ARROW << "line " << bp->line << ":   " << eline << endl;
            if (!lline.empty()) cout << "     line " << bp->line + 1 << ":   " << lline << "\n" << endl;
        }
        file.close();
    } else {
        cout
                << "Binding error: couldn't fetch the source for the breakpoint. The debugger will enter break mode anyway."
                << endl;
    }
    broken_loop();
}

void debug::broken_loop() {
    string input;
    while (broken) {
        cout << "> ";
        getline(cin, input);
        if (input.empty()) continue;
        command.interpret(input);
    }
}

void lns::debug::start() {
    cout << "\n>>>>>>>>lns debugger v1.0<<<<<<<<\n      All rights reserved.      \n\n\n";
    interpreter = new debugger(this);
    if (file != nullptr)
        try {
            loadcode(file);
        } catch (exception &e) {
            cerr << "Couldn't load the specified file: " << e.what() << endl;
        }
    broken = true;
    broken_loop();
}
