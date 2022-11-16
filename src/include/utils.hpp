#ifndef LANG_UTILS
#define LANG_UTILS

#include <string>
#include <stdio.h>
#include <vector>

//colors
#define ESC "\033["
#define RESET "\033[m"
#define BACKGROUND_COLOR_RED_LIGHT "101"
#define BACKGROUND_COLOR_GREEN_LIGHT "102"
#define BACKGROUND_COLOR_YELLOW_LIGHT "103"
#define BACKGROUND_COLOR_BLUE_LIGHT "104"
#define BACKGROUND_COLOR_PURPLE_LIGHT "105"
#define BACKGROUND_COLOR_CYAN_LIGHT "105"
#define TEXT_COLOR_RED "91"
#define TEXT_COLOR_GREEN "92"
#define TEXT_COLOR_YELLOW "93"
#define TEXT_COLOR_BLUE "94"
#define TEXT_COLOR_PURPLE "95"
#define TEXT_COLOR_CYAN "96"
#define WHITESPACE "\t\f"

enum types{
    NONE,STRING,INTEGER,ARRAY,ACTION,OBJECT,
    KEYWORD,VARIABLE,FUNCTION,STRING_FUNCTION,MODULE,CUSTOM,LINE,
    BIND,ARGUMENT,TARGET,PUBLIC,
    OPERATOR,COMPARATOR,ID,PUNCTUATION,NEW_LINE,SYMBOL,
    FUNCTION_DECLARATION,OBJECT_DECLARATION,VARIABLE_DECLARATION,
    IF,ELSE,LOOP,RETURN,STOP,SKIP,OUT,USE,THREAD
};
typedef struct{
    std::string value;
    enum types type;
} Token;
typedef struct{
    std::string name,value,scope;
    enum types type;
} Variable;
typedef struct{
    std::string name,current_scope,scope,text;
    std::vector<std::string> arguments;
    int line;
} Function;
typedef struct{
    std::string name,scope;
    std::vector<std::string> arguments;
}Object;

namespace tools{
    std::vector<std::string> split(std::string str,std::string delimiter);
    std::vector<std::string> split_with_delimeter(std::string str,std::vector<std::string> delimeters);
    std::string replace(std::string str,std::string old,std::string new_str);
    std::string join(std::vector<std::string>& vec, std::string delimiter);
    int is_number(int c);
    int is_letter(int c);
    int check_scope(std::string& from, std::string& to);
}
namespace stuff{
    void kill(std::vector<Token> tokens, std::string scope, int line);
    void output(std::vector<Token> tokens);
    std::vector<Token> split_tokens(std::vector<Token>& tokens,std::string& scope,int stringify,int line);
    Token get_value(std::vector<Token>& tokens,std::string& scope,int stringify,int& line);
    types get_type(std::string value);
}

extern std::vector<Variable> variables;
extern std::vector<Function> functions;
extern std::vector<Object> objects;
#endif