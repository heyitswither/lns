import java.io.IOException;
import java.io.PrintWriter;
import java.util.Arrays;
import java.util.List;

public class generate {
    static String visitorName;
    public static void main(String[] args) throws IOException {
        if (args.length != 1) {
            System.err.println("Usage: generate <output directory>");
            System.exit(1);
        }
        String outputDir = args[0];
        defineAst(outputDir, "expr", Arrays.asList(
                "increment : token& name, expr* value",
                "decrement : token& name, expr* value",
                "assign    : token& name, expr* value",
                "binary    : expr* left, token& op, expr* right",
                "call	   : expr* callee, token& paren, vector<expr*>& args",
                "grouping  : expr* expression",
                "literal   : object* value",
                "unary     : token& op, expr* right",
                "variable  : token& name",
                "map_field : token& name, expr* key",
                "assign_map_field : token& name, expr* key, expr* value",
                "null      : token& where"
        ), Arrays.asList(), "object*");
        defineAst(outputDir, "stmt", Arrays.asList(
                "block      : vector<stmt*>& statements",
                "expression : expr& exprs",
                "function   : token& name, vector<token>& parameters, vector<stmt*>& body, bool isglobal",
                "if         : expr& condition, stmt* thenBranch, stmt* elseBranch",
                "return     : token& keyword, expr& value",
                "break      : token& keyword",
                "continue   : token& keyword",
                "var        : token& name, expr& initializer, bool isglobal, bool isfinal",
                "s_while      : expr& condition, stmt* body",
                "uses_native : token& token, string& path",
                "null       : token& where"
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
        for (String include : addIncludes) {
            writer.println("#include " + include);
        }
        visitorName = baseName + "_visitor";
        writer.println("using namespace std;\nusing namespace lns;");
        writer.println("\n");
        writer.println("namespace lns{");
        writer.println("class " + visitorName + ";");
        writer.println("class " + baseName + "{");
        writer.println("public:\n");
        writer.println("virtual " + vReturnType + " accept(" + visitorName + "* v) = 0;");
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
        writer.println(className + "_" + baseName + "(" + types + ") : " + genFieldsAssign(fieldArray) + " {}");
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