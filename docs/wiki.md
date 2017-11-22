# 1 Welcome to *lns*! #

*lns* is a very basic scripting language implemented in C++.
Its concise syntax and its built-in native functions make it a perfect introductory level programming language.  
Anyone can contribute to make *lns* better.  
In this page you will find documentation over the command, the language and the implementation.

# 2 Installation #

To install, execute the script `source/install.sh`. The program will be installed in the `/usr/bin/` folder(it will require you to input the password for administrator privileges).

# 3 Command overview #

The command syntax is simple:  
    
    lns [options] [file]


*Note:* if no source file is provided, the program will start in interactive console mode.  
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
* `-d` or `--debugger`: starts the program in debugger mode. See the [Debugger](#5-debugger) section for info.  
# 4 Language #
### 4.1 Syntax Basics ###
In source file mode, the interpreter, provided that parsing succeeded, will start executing the statements.

In lns, line in any high-level programming language, a statement is the smallest stand-alone that produces an action.
Statements can be of many kinds from function and variable declarations to control flow statements, but we'll get later to that.
Unlike other languages like C or Java, *lns* doesn't require an semicolon after each statement.  

#### 4.1.1 Variables and types ####
Variables can be declared with the following syntax:

    var name
    var name = expression

The first one declares a variable without assigning anything to it, while the second one evaluates the given expression and assigns that value the the variable.
Variable access can be modified through access modifier, **global** and **final**.  

Variables declared as ***global*** are defined in the outer-most scope, allowing you to access them outside of the current scope.  

The keyword ***final*** makes the variable unchangeable: if you perform an assign operation on a final variable, this will cause a runtime error.  

**Note:** variables that are not declared as ***global*** are defined in the inner-most scope.

Here's a simple example that shows usage of the access modifiers:

    global var a = 2
    global final var b = 5
    
    a = 4 //assignment ok
    b = 5 //error: variable b is declared as final!
##### 4.1.2 Variables and Types #####
lns is not a strongly-typed language, which means that when you declare a variable you don't have to specify a type for that variable: the interpreter will handle types for you.
This also implies that a variable can be reassigned with a different type.
The built-in types are **number**, **string**, **boolean**, **map** and **null**.
Functions and contexts can be considered types as well, but they work a little differently (more on that later).  

Maps are a very useful feature. They can store an infinite number of elements. They are basically a sey of keys, where each one points to a specific object. To make a map, call the built-in function `map()`. You can then use the **[]** operator to access and assign in the map:  
    
    var m = map()
    m[1] = "one"
    m[2] = "two"
    m[2.15] = "two point one five"
    
    m[1] ---> returns "one"

    m[0] ---> key not present! returns null
     
**Note**: anything can be a key for a map (even *null* or another map), but the keys are all converted to strings: for example *1* and *"1"*, *false* and *"false"*, *null* and *"null"* point to the same field.

Here are some examples:
    
    var n = 1                  //integer number
    final var pi = 3.14159     //decimal number
    
    var str = "hello, world!"       //string of characters
    
    var hungry = true        //boolean value 'true'
    var angry = false        //boolean value 'false'
    
    var english_to_spanish = map()       //map initialization
    english_to_spanish["hi"] = "hola"    //string as key
    english_to_spanish[1] = "uno"        //integer as key
    english_to_spanish[false] = "falso"  //boolean as key

#### 4.1.2 Operators ####
##### 4.1.2.1 Numbers #####
Numeric operators respect standard precedence rules, but unary operators have precedence over the binary ones.  
The numeric operators are:
* Standard binary arithmetic operators **_+_**,**_-_**,**_\*_**,**_/_**
* Binary operator **^**, *at the power of*
* Binary operator _**mod**_ or _**%**_, *modulo*
* Unary operator ***-*** (goes before the operand)
* Binary boolean operators **_==_** (equals to), (not equals to), **_>_** (greater than), _**>=**_ (greater than or equal to), _**<**_ (less than), **_<=_** (less than or equal to)
* Unary increment and decrement operators ***++*** and ***--*** (go after the operand)    
    
##### 4.1.2.2 Strings #####
The string operators are:
* _**+**_, concatenator (works with any other type, as long as the first operand is a string)
* _**/**_, split operator, returns a map of strings (adjacent in the map, starting at key *0*) containing all the substrings
* Binary boolean operators **_==_** (equals to), (not equals to), **_>_** (greater than), _**>=**_ (greater than or equal to), _**<**_ (less than), **_<=_** (less than or equal to)

##### 4.1.2.3 Booleans #####


The operators between booleans are:
* Binary logic operators _**and**_, **_nand_**, **_or_**, **_nor_**, _**xnor**_
* Unary operator _**not**_ (goes before the operand)

##### 4.1.2.4 Maps #####
The only operator for maps is the _**[]**_ operator, already explained the the [Types](#411-variables-and-types) section.  

#### 4.1.3 Comments ####

_lns_ supports both C-like and Bash-like comments:
    
    //this is a line comment
    ...
    /* this is a block comment
     * it can extend on multiple lines
     */
    ...
    # this is also a line comment

**Tip**: on Unix-like systems you can add `#/usr/bin/lns` at the beginning of your script file, so the shell interpreter knows that the script can be executed with the _lns_ interpreter.
### 4.2 Control flow ###
#### 4.2.1 If statements ####
The syntax for `if` statements is the following:
    
    if condition then
       ...
    end
For `else if` and `else` clauses:

    if condition_1 then
       ...
    else if condition_2 then
       ...
    else if condition_3 then
       ...
    else
       ...
    end
 
#### 4.2.2 While statements ####
`while` statements execute the same code as long as a condition is true.  
The syntax for `while` statements is:
   
    while condition do
       //loop body here
    end
    
#### 4.2.3 For statements ####
`For` statements, just like `while` statements, iterate as long as a condition is true. For statements have the possibility to have an iterating variable and an operation performed on such variable at the end of each iteration.  
The syntax is:

    for initializer, condition, increment do
        //loop body here
    end    
    
The `initializer` can be a variable declaration or an assignment and will be executed only once, before the first iteration, whereas the `increment` can be any expression, which will be evaluated every iteration.

`initializer`,`condition` and `increment` can all be left empty (although the commas are necessary), generating a loop without exit condition
#### 4.2.4 Loop control operations ####
In `while` and `for` loops, you can use `continue` and `break` statements to alter the loop status.  
`break` statements force-exit the loop.  
`continue` statements cause the loop to skip the rest of the current iteration and go to the next one.  
Example:
   
    for var i = 0, i < 15, i++ do
        if i == 7 then
            continue
        else if i == 12 then
            break
        else
           print(i + " ")
        end
    end
The output is going to be `0 1 2 3 4 5 6 8 9 10 11`.
#### 4.2.5 Functions ####
Functions are declared like the following:

    function name(arg1, arg2 /*arguments here, separated by comma*/)
        //function body here
    end
    
Anything can be argument of a function, even maps, contexts and other functions. 

The keyword `return` is used to exit from a function. If the keyword is followed by an expression on the same line, the function will return the value, otherwise `null`.
If no `return` statement is encountered before the closing `end`, the function will return `null`.
#### 4.3 Contexts ####
Context are a collection of static variables and functions. Their main purpose is to resolve ambiguity between two functions or variables. If you are familiar with C++, they are pretty much namespaces.  
They can only contain variable, functions and contexes declarations.
The operator _**.**_ is used to access fields in a context.
Example: 
    
    context math
        final var pi  = 3.1415926535
        final var e   = 2.7182818284
        final var phi = 1.6180339887
        
        function half(nr)
            return nr/2
        end
    end
    
    //access
    
    println(math.pi)
    println(math.half(math.pi))
#### 4.4 File references ####
 You don't need to keep all your source in one file: the `use` statement allows to load other source files at compile time.
 
     use "file.lns"
     
 You can have multiple `use` statements, but they can only be at the beginning of the file.  
 **Note:** Make sure you don't load the same source twice: you code won't work or you will get duplicate definition errors! 
# 5 Debugger #

The debugger mode allows you to set breakpoints in the execution. When a break point gets hit, the program enters break mode and the debugger console starts up: it allows you to check the program status, such as watching variables and checking the call stack.
To start the debugger, use the `-d` command line option.   
**Note**: the debugger ignores every other command line option except `-p` (permissive execution).  
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
# 6 Implementation #
### 6.1 Structure ###
### 6.2 Files ###
# 7 Known Bugs #
_lns_ is still in development. As of today, I cannot assure full stability of the program. If you encounter an error, please report it at [lorynot@gmail.com](mailto:lorynot@gmail.com).  
Here's an up-to-date list of known bugs:
* 1
* 2
# 8 Credits #
The *lns* interpreter started off as a personal project inspired to [Bob Nystrom's "Crafting Interpreters"](http://www.craftinginterpreters.com/ "Crafting Interpreters website") online book, which shows a very basic way to implement an interpreter in Java.  
At this time the two projects differ in most of their parts (including the syntax definitions, the features of the language and the implementation language itself), but you can see some common aspects in their structure.  
The two projects don't have any file in common.    
You can visit Bob Nystrom's GitHub repository [here](https://github.com/munificent/craftinginterpreters "Bob Nystrom's repository").  

