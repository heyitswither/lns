# lns

*lns* is a simple scripting language written in C++.
The repository root is a CLion project folder.  

Visit the Wiki section for information about the language.
## Build ##

To build, execute the script `source/install.sh`. It will install the program in the path (it will require you to input the password for administrator priviledges).

## Command overview ##

The command syntax is concise:  
    
    lns [options] [file]
    
The available options are:
* `-p` or `--skip-preprocessing`: skips preprocessing and executes the code. This can lead to runtime errors.
* `-s` or `--silent-execution`: if a runtime error occurs during execution, silently terminates the program without reporting to stdout.
* `-S` or `--silent-full`: if any error --either parse or runtime-- occurs, silently terminates the program without reporting to stdout.
* `--permissive`: enable auto-correction of several errors that would normally cause the program to terminate, such as:
	* use of an unknown variable: returns `null`;	
	* assignment of an unknown variable: in-place definition of the variable;
  * duplicate declaration of a variable: redefinition of the variable;
  * ignores reduntant or duplicate access specifiers for a declaration;
* `-t` or `--time-info`: at the end of execution, shows information about the time elapsed after parsing and execution.
* `-o` or `--parse-only`: terminates the program after parsing the code.
* `-d` or `--debugger`: starts the program in debugger mode. See next section for info.  

## Debugger ##

The debugger mode allows you to set breakpoints in the execution. When a break point get hit, the program enters break mode and the debugger console allows you to check the program status, such as watching variables and checking the call stack trace.
To start the debugger, use the `-d` command line option. Note that the debugger ignores every other command line option except for `-p` (permissive execution).  
The debugger commands are the following:
* `load`: loads and parses a script file. Syntax:  
    * `load|l [file]`
* `run`: If a file has been loaded, runs the script with the current breakpoint settings. Syntax:  
    * `run|r`.
* `breakpoint`: add, remove or list breakpoints. Syntax:  
    * `breakpoint|b set [file] [line]`  
    * `breakpoint|b remove [id]`  
    * `breakpoint|b list`  
* `watch`: watch an expression (if the execution has already started), add or remove a watch to the watch list or watch the expressions in the watch list. Syntax:  
    * `watch|w [expression]`
    * `watch|w add [expression]`  
    * `watch|w remove [id]`  
    * `watch|w all`
* `step`: steps in/out/over. Syntax:  
    * `step|s in`: steps into the next statement in the code, decending into a method call if necessary
    * `step|s out`: continues execution until the current function ends
    * `step|s over`: steps into the next statement in the current scope
* `continue`: continues execution. Syntax:
    * `continue|c`
* `exit`: exits the program. Syntax:
    * `exit`

## Credits ##

The idea of the *lns* interpreter started off as a personal project inspired to [Bob Nystrom's "Crafting Interpreters"](http://www.craftinginterpreters.com/ "Crafting Interpreters website") online book, which shows a very basic way to implement an interpreter in Java.  
At this time the two project differ in most of their parts (including the syntax definitions, the language structure and the implementation language itself), but you can see some common aspects in their structure.  
The two projects don't have any file in common.    
You can visit Bob Nystrom's GitHub repository [here](https://github.com/munificent/craftinginterpreters "Bob Nystrom's repository").  
  
## Contact ##
If you have any question or advice, please file an issue report in the Issues section or contact me at [lorynot@gmail.com](mailto:lorynot@gmail.com "Send an email").
  
__Lorenzo Notaro, 2017__  
