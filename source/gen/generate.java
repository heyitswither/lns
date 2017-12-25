package gen;

import java.io.IOException;
import java.io.PrintWriter;
import java.util.Arrays;
import java.util.List;

public class generate {
    static String visitorName;
    static String enumName;
    public static void main(String[] args) throws IOException {
        if (args.length != 1) {
            System.err.println("Usage: generate <output directory>");
            System.exit(1);
        }
        String outputDir = args[0];
        defineAst(outputDir, "expr", Arrays.asList(
                "assign    : token* name, token_type op, expr* value",
                "binary    : expr* left, token* op, expr* right",
                "call	   : expr* callee, token* paren, vector<expr*>& args",
                "grouping  : expr* expression",
                "literal   : object* value",
                "unary     : token* op, operator_location location, expr* right",
                "variable  : token* name",
                "sub_script : token* where, expr* name, expr* key",
                "sub_script_assign : token* where, expr* name, token_type op, expr* key, expr* value",
                "member : expr* container_name, token* member_identifier",
                "member_assign : expr* container_name, token_type op, token* member_identifier, expr* value",
                "array     : token* open_brace, vector<pair<expr*,expr*>>& pairs",
                "null      : token* where"
        ), Arrays.asList("<utility>"), "object*");
        defineAst(outputDir, "stmt", Arrays.asList(
                "block      : vector<stmt*>& statements",
                "expression : expr& exprs",
                "function   : token* name, vector<token*>& parameters, vector<stmt*>& body, visibility visibility",
                "context    : token* name, vector<stmt*> body, visibility visibility, bool isfinal",
                "if         : expr& condition, stmt* thenBranch, stmt* elseBranch",
                "return     : token* keyword, expr& value",
                "break      : token* keyword",
                "continue   : token* keyword",
                "var        : token* name, expr& initializer, visibility visibility, bool isfinal",
                "s_while    : expr& condition, stmt* body",
                "s_for      : stmt* init, expr* condition, expr* increment, stmt* body",
                "s_for_each : token* identifier, expr* container, stmt* body",
                "exception_decl : token* name, set<string>& identifiers, visibility visibility",
                "raise      : token* where, expr* name, map<string,expr*>& fields",
                "uses_native : token* where, string& lib_name, token* bind",
                "handle     :  expr* exception_name, token* bind, vector<stmt*>& block",
                "begin_handle : vector<stmt*>& body, vector<handle_stmt*> handles",
                "null       : token* where"
        ), Arrays.asList("\"expr.h\"","<memory>"), "void");
    }

    public static void defineAst(String outputDir, String baseName, List<String> types, List<String> addIncludes, String vReturnType) throws IOException {
        String path = outputDir + baseName + ".h";
        PrintWriter writer = new PrintWriter(path, "UTF-8");
        writer.println("#ifndef LNS_" + baseName.toUpperCase() + "_H");
        writer.println("#define LNS_" + baseName.toUpperCase() + "_H");
        writer.println("#include \"defs.h\"");
        writer.println("#include <list>");
        writer.println("#include <string>");
        writer.println("#include <iostream>");
        for (String include : addIncludes) {
            writer.println("#include " + include);
        }
        visitorName = baseName + "_visitor";
        enumName = baseName + "_type";
        writer.println("using namespace std;\nusing namespace lns;");
        writer.println("\n");
        writer.println("namespace lns{");
        writer.println("enum " + enumName + "{");
        for(String type : types){
            String className = type.split(":")[0].trim();
            writer.print(className.toUpperCase() + "_" + baseName.toUpperCase() + "_T, ");
        }
        writer.println("\n};");
        writer.println("class " + visitorName + ";");
        writer.println("class " + baseName + "{");
        writer.println("public:\n");
        writer.println(baseName + "(int line, const char* file, " + enumName + " type) : type(type), line(line), file(file){}");
        writer.println("virtual " + vReturnType + " accept(" + visitorName + "* v) = 0;");
        writer.println(enumName + " type;\n int line; const char* file;");
        writer.println("};"); //class
        declareAll(writer,baseName,types);
        defineVisitor(writer,baseName,vReturnType,types);
        for (String type : types) {
            String className = type.split(":")[0].trim();
            String fields = type.split(":")[1].trim();
            defineType(writer,baseName,className,fields,vReturnType);
        }
        defineDefaultVisitor(writer,baseName,vReturnType,types);
        writer.println("}"); // namespace
        writer.println("#endif");
        writer.close();
    }

    private static void defineVisitor(PrintWriter writer,String baseName, String vReturnType, List<String> types) {
        writer.println("class " + visitorName + "{");
        writer.println("public:");
        for(String type : types){
            String className = type.split(":")[0].trim() + "_" + baseName;
            writer.println("virtual " + vReturnType + " visit_" + className + "(" + className + " *" + className.charAt(0) + ") = 0;");
        }
        writer.println("};");    }

    private static void defineDefaultVisitor(PrintWriter writer,String baseName, String vReturnType, List<String> types) {
        writer.println("class default_" + baseName + "_visitor : public " + visitorName + " {");
                writer.println("public:");
                for(String type : types){
                    String className = type.split(":")[0].trim() + "_" + baseName;
                    writer.println("virtual " + vReturnType + " visit_" + className + "(" + className + " *" + className.charAt(0) + ") override {");
                    writer.println("std::cout << \"" + className + "\" << std::endl;");
                    writer.println("}");
                }
        writer.println("};");
        }
    public static void declareAll(PrintWriter writer,String baseName, List<String> types) {
        for (String type : types) {
            String className = type.split(":")[0].trim();
            writer.println("class " + className + "_" + baseName + ";");
        }
    }

    public static void defineType(PrintWriter writer, String baseName, String className, String types, String vReturnType) {
        writer.println("\nclass " + className + "_" + baseName + " : public " + baseName + " {");
        writer.println("public:");
        String[] fieldArray = types.split(", ");
        writer.print(fieldArray.length == 1 ? "explicit " : "");
        writer.println(className + "_" + baseName + "(const char* file,int line, " + types + ") : " + genFieldsAssign(fieldArray) + ", " + baseName + "(line, file, " + className.toUpperCase() + "_" + baseName.toUpperCase() + "_T" + ") {}");
        writer.println(vReturnType + " accept(" + visitorName + " *v) override{");
        writer.println((vReturnType == "void" ? "" : "return ") +  "v->visit_" + className + "_" + baseName + "(this);");
        writer.println("}");
        for (String s : fieldArray) {
            writer.println("const " + s + ";");
        }
        writer.println("};");
        writer.println("\n");
    }

    public static String genFieldsAssign(String[] fields) {
        StringBuilder sb = new StringBuilder();
        int i = 1;
        for (String field : fields) {
            String name = field.split(" ")[1];
            sb.append(name + "(" + name + ")");
            if (!(i == fields.length)) sb.append(", ");
            i++;
        }
        return sb.toString();
    }

}
