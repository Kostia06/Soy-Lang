#include <string> 
#include <iostream>
#include <fstream>
#include <vector>
#include "../include/variable.hpp"
#include "../include/function.hpp"
using namespace std;
void variable_init(vector<Token> tokens,string var_scope,string scope,string directory){
    vector<Token> name_tokens;
    string type = "";
    for(int i = 0; i < tokens.size(); i++){
        Token current_token = tokens[i];
        if(type == ""&&(current_token.type == "EQUAL")){
            type = current_token.value;
            break;
        }
        name_tokens.push_back(current_token);
    }
    string name = name_tokens[0].value;
    Variable var;var.scope = var_scope;
    if(tokens.size()-name_tokens.size() == 1){display_error(tokens,scope,"Unfinished declaration");}
    string value = get_value(tokens,scope,name_tokens.size());
    var.value = value;
    if(name_tokens.size() == 1){
        var.name = name,var.type = get_type(value);
        for(int i = 0; i < variable_list.size(); i++){
            Variable current_variable = variable_list[i];
            if(current_variable.name == name){
                var.scope = variable_list[i].scope;
                if(variable_list[i].bind > -1){bind_execute(bind_list[variable_list[i].bind],scope,directory);}
                var.bind = variable_list[i].bind;
                variable_list[i] = var;
                return;
            }
        }
        var.bind = -1;variable_list.push_back(var);
        return;
    }
    display_error(tokens,scope,"Too many name tokens");

}

