//
// Created by lorenzo on 12/27/17.
//

#ifndef CPPLNS_ERROR_MESSAGE_H
#define CPPLNS_ERROR_MESSAGE_H



/*  SCANNER ERRORS  */
#define UNTERMINATED(what) "unterminated " what
#define INVALID_ESCAPE_CHARACTER(c) lns::concat({"Invalid escape character '", c, "'"})
#define UNEXPECTED_CHARACTER(c) lns::concat({"Unexpected character '", c, "'"})
#define UNTERMINATED_BLOCK_COMMENT "unterminated block comment"

/* PARSER ERRORS */
#define USE_NOT_ALLOWED "'use' statements are not allowed at this location"
#define EXPECTED(what) "expected " what
#define EXPECTED_AFTER(what, after) "expected " what " after " after
#define EXPECTED_IN(what,in) "expected " what " in " in
#define CAN_ONLY_CONTAIN(a,b) a " can only contain " b
#define EXPTOCLOSE(what, line) lns::concat({"expected 'end' to close ", what ," (opened at line ", std::to_string(line), ")"})
#define ACCESS_SPEC_NOT_ALLOWED(key,for_) "Access specifier '" key "' is not allowed for " for_ " (--permissive)"
#define CONSTRUCTOR_OUTSIDE_OF_CLASS "constructor outside of class"
#define INVALID_STATEMENT "invalid statement"
#define INVALID_ASSIGNMENT_TARGET "invalid assignment target"
#define WITHOUT(what,without) what " without " without
#define MISSING_CLOSING(what) "missing closing " what
#define USE_FILE_NOT_FOUND "use: file was not found or is inaccessible"
#define DUPLICATE_ACCESS_SPEC(spec) "duplicate access specifier: '" spec "'"
#define CONFILICTING_SPECS(a,b) "conflicting access specifiers '" a "' and '" b "'"
#define NON_OPTIONAL_NOT_ALLOWED "required parameters must be before the optional ones"

/* INTERPRETER ERRORS */
#define EXCEPTION_NO_MEMBER(name,member) lns::concat({"exception \"",name,"\" has no member named \"", member, "\""})
#define OBJECT_NOT_CONTAINER "object is not a container"
#define OBJECT_NOT_CONTEXT "object is not a context"
#define INVALID_CALL_TARGET "invalid call target"
#define WRONG_ARG_NR(exp,got) lns::concat({"expected ", std::to_string(exp), " argument", exp == 1 ? "" : "s", " but got ", std::to_string(got)})
#define WRONG_ARG_NR_BETWEEN(exp1,exp2,got) lns::concat({"expected between ", std::to_string(exp1), " and ",std::to_string(exp2), " arguments but got ", std::to_string(got)})
#define RECURSION_LIMIT_EXCEEDED "recursion limit exceeded"
#define KEY_MUST_BE_NUMBER "key for operator [] must be a number"
#define INVALID_OPERAND_FOR_SUBSCRIPT "invalid operand for operator []: operand must be either string or map"
#define OBJECT_NOT_AN_ARRAY "object is not an array"
#define COULD_NOT_LOAD_DL(why) lns::concat({"could not load the dynamic library: ", why})
#define CAN_ONLY_BE_USED_IN(what,where) what " can only be used in " where
#define ILLEGAL_ASSIGNMENT "illegal assignment"
#define EXCEPTION_PREVIOUSLY_DEFIDED(file,line) lns::concat({"exception previously defined at ", file, std::to_string(line)})
#define RAISE_NOT_EXCEPTION "only exceptions can be raised"
#define HANDLE_NOT_EXCEPTION "only exceptions can be handled"
#define VARIABLE_NOT_VISIBLE(var) lns::concat({"\"", var, "\" is not visible in the current file"})
#define VARIABLE_UNDEFINED(var) lns::concat({"\"", var, "\" is undefined"})
#define VARIABLE_ALREADY_DEFINED(var) lns::concat({"\"", var, "\" is already defined"})
#define VARIABLE_FINAL(var) lns::concat({"\"", var, "\" is final"})

#endif //CPPLNS_ERROR_MESSAGE_H
