#ifndef LANG_UTIL
#define LANG_UTIL
#include <string> 
#include <iostream>
#include <fstream>
#include <vector>
using namespace std;



struct File{
    string name;
    string type;
    string file;
};
struct Token{
    string value;
    string type;
};
struct Component{
    vector<Token> tokens;
    string type;
};
struct Variable{
    string name;
    string value;
    string type;
    string scope;
    int bind;
};
struct Function{
    string name;
    string file;
    string arguments;
    string scope;
};
struct Bind{
    Variable variable;
    vector<Token> function;
};
struct Object{
    string name;
    string arguments;
    string scope;
};

extern vector<Variable> variable_list;
extern vector<Function> function_list;
extern vector<Bind> bind_list;
extern vector<Object> object_list;

vector<string> split (string s, string delimiter);
string join(vector<string> list, string target);
string replace_chars(string text, string target, string to);

bool in_function_list(string name,string scope);
bool in_object_list(string name,string scope);

bool check_scope(string from, string to);
bool find_item_in_array(string list[], string item);

void output(vector<Token> tokens, string scope);
string input(Token token, string scope);

void display_error(vector<Token> tokens, string scope,string text);
vector<string> get_scope(string scope);

string indexing(Token token, Token index_token, string scope);
vector<int> get_index(vector<Token> tokens,string scope);

vector<vector<Token> > split_tokens(string value, string scope);
string get_token_value(Token token);
string get_value(vector<Token> tokens,string scope,int index,bool stringify=false);
string get_type(string value);
#endif