#include <string> 
#include <iostream>
#include <fstream>
#include <vector>
#include <regex>
#include "../include/function.hpp"
#include "../include/lexer.hpp"
#include "../include/parser.hpp"
using namespace std;

int working_functions = 0;

void function_init(vector<Token> tokens, string scope){
    if(tokens.size()<4 || tokens.size()>5){display_error(tokens,scope,"Too many tokens in function declaration");}
    string name = tokens[1].value;
    string arguments = tokens[2].value;
    Function function;function.name =name;
    function.file = tokens[3].value;
    function.arguments = arguments;
    function.scope = scope;
    function_list.push_back(function);
}
string call_function(vector<Token> tokens, string scope){
    working_functions ++;
    Function current_function;
    for(int i=0;i<function_list.size();i++){
        if(function_list[i].name == tokens[0].value){
            if(check_scope(scope, function_list[i].scope)){current_function=function_list[i];break;}
        }
    }
    vector<vector<Token> > array = split_tokens(tokens[1].value,scope);
    vector<string> list = split(current_function.arguments,"");
    string names;
    for(int i=1;i<list.size()-2;i++){names += list[i];}
    vector<string> var_names = split(names,",");
    string current_scope = current_function.scope + "!~!" + current_function.name + to_string(working_functions);
    for(int i=0; i<array.size();i++){
        Variable var;var.name = regex_replace(var_names[i],regex(" "),"");
        var.value = get_value(array[i],scope,-1);
        var.type = get_type(var.value);
        var.scope = current_scope;
        variable_list.push_back(var);
    }
    int count = 0;
    string result = read_and_execute(current_function.file,current_scope);
    while(count != variable_list.size()){
        if(variable_list[count].scope == current_scope){
            variable_list.erase(variable_list.begin()+count);
            count--;
        }
        count++;
    }
    return result;
}


void bind_init(vector<Token> tokens,string scope){
    vector<string> items = split(tokens[0].value,"");
    items.erase(items.begin());
    items.erase(items.begin()+items.size()-1);
    vector<Token> new_tokens = tokenizer(join(items, ""),scope,2)[0].tokens;
    vector<vector<Token> > array;
    vector<Token> current_tokens;
    for(int i=0;i<new_tokens.size();i++){
        Token current_token = new_tokens[i];
        if(current_token.type == "PUNCTUATION" && current_token.value == ","){
            array.push_back(current_tokens);
            current_tokens.clear();
        }else{current_tokens.push_back(current_token);}
        if(i+1 == new_tokens.size()){array.push_back(current_tokens);}
    }
    Bind bind_struct;
    vector<Token> function = array[1];
    Variable var;
    for(int i=0;i<variable_list.size();i++){
        if(variable_list[i].name == array[0][0].value){
            if(check_scope(scope, variable_list[i].scope)){
                var = variable_list[i];
                variable_list[i].bind = bind_list.size();
                break;
            }
        }
    }
    bind_struct.variable = var;
    bind_struct.function = function;
    bind_list.push_back(bind_struct);
}
void bind_execute(Bind bind,string scope, string directory){
    string result = "";
    for(int i=0;i<bind.function.size();i++){
        result += bind.function[i].value;
    }
    read_and_execute('{'+result+'}',directory);
}

void object_init(vector<Token> tokens,string scope){
    Object obj;
    obj.name = tokens[1].value;
    obj.scope = scope;
    obj.arguments = tokens[2].value;
    object_list.push_back(obj);
}
string get_object_value(Token name,Token argument,string scope){
    argument.value.erase(argument.value.begin());
    argument.value.erase(argument.value.begin());

    for(int i=0;i<object_list.size();i++){
        if(object_list[i].name == name.type){
            vector<string> args = split(object_list[i].arguments,"");
            args.erase(args.begin());
            args.erase(args.begin()+args.size()-1);
            args.erase(args.begin()+args.size()-1);
            vector<string> items = split(join(args,""),",");
            for(int j=0;j<items.size();j++){
                if(replace_chars(items[j]," ","") == argument.value){
                    vector<vector<Token> > values = split_tokens("{"+split(name.value,"{")[1]  ,scope); 
                    return get_value(values[j],scope,-1);
                }
            }
        }
    }
    return "";
}
string access_object(Token token, Token target, string scope){
    string name = split(token.value,"{")[0];
    for(int i=0;i<object_list.size();i++){
        if(object_list[i].name == name){
            vector<string> args = split(object_list[i].arguments,"");
            args.erase(args.begin());
            args.erase(args.begin()+args.size()-1);
            args.erase(args.begin()+args.size()-1);
            vector<string> items = split(join(args,""),",");
            for(int j=0;j<items.size();j++){
                if(replace_chars(items[j]," ","") == replace_chars(target.value,":","")){
                    
                    vector<vector<Token> > values = split_tokens("{"+split(token.value,"{")[1],scope); 
                    
                    if(values.size()<=j){
                        vector<Token> error;error.push_back(token);error.push_back(target);
                        display_error(error,scope,"Index out of range");
                    }
                    return get_value(values[j],scope,-1);
                }
            }
        }
    }
    return "";
}
string change_object(Token token,string target,string change_value,string scope){
    string name = split(token.value,"{")[0];
    string final_result = name + "{";
    for(int i=0;i<object_list.size();i++){
        if(object_list[i].name == name){
            vector<string> args = split(object_list[i].arguments,"");
            args.erase(args.begin());
            args.erase(args.begin()+args.size()-1);
            args.erase(args.begin()+args.size()-1);
            vector<string> items = split(join(args,""),",");
            for(int j=0;j<items.size();j++){
                if(replace_chars(items[j]," ","") == replace_chars(target,":","")){
                    vector<vector<Token> > values = split_tokens("{"+split(token.value,"{")[1]  ,scope); 
                    vector<string> result;
                    for(int k=0;k<values.size();k++){
                        string value = get_value(values[k],scope,-1);
                        result.push_back(value);
                    }
                    result[j] = change_value;
                    for(int k=0;k<result.size();k++){
                        final_result += result[k] + ",";
                    }
                    final_result[final_result.size()-1] = '}';
                    return final_result;
                }
            }
        }
    }
    return "";
}