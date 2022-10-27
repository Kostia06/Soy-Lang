#include <string> 
#include <iostream>
#include <fstream>
#include <vector>
#include "../include/lexer.hpp"
#include "../include/statement.hpp"
using namespace std;

// 0 = none, 1 = if, 2 = else
int state = 0;
string statement(vector<Token> tokens, string scope){
    string exe = tokens[tokens.size()-1].value;
    int count = 1;
    vector<Token> new_tokens;
    if(tokens[0].value == "else" && tokens[1].value == "if" && tokens[0].type=="KEYWORD" && tokens[1].type=="KEYWORD"){
        count ++;
    }
    //if current statement is "else if" and the statement before that is true then end execution
    if(count == 2 && state == 0){return "!~!NONE!~!";}
    //remove the ifs and elses
    for(int i =count;i<tokens.size()-1;i++){new_tokens.push_back(tokens[i]);}
    if(count != tokens.size()-1){
        bool value = get_value_statement(new_tokens,scope);
        if(value){state = 1;}
        else{state = 2;}
    }
    else if(state>0){state =1;}
    if(state==1){
        state = 0;
        string result = read_and_execute(exe,scope);
        return result;
    }
    return "!~!NONE!~!";
}
bool get_equality(Token value_1, Token value_2, string op){
    // cout << value_1.value << " " << value_1.type << "\t" << value_2.value << " " << value_2.type << "\t" <<op<<endl;
    if(value_1.type != value_2.type){return false;}
    if(value_1.type =="STRING"){
        if(op == "=="){return value_1.value == value_2.value;}
        if(op == "!="){return value_1.value != value_2.value;}
    }else if(value_1.type == "NUMBER"){
        if(op == "=="){return stoi(value_1.value) == stoi(value_2.value);}
        if(op == "!="){return stoi(value_1.value) != stoi(value_2.value);}
        if(op == ">="){return stoi(value_1.value) >= stoi(value_2.value);}
        if(op == "<="){return stoi(value_1.value) <= stoi(value_2.value);}
        if(op == "<"){return stoi(value_1.value) < stoi(value_2.value);}
        if(op == ">"){return stoi(value_1.value) > stoi(value_2.value);}
    }
    return false;
}
bool get_value_statement(vector<Token> tokens,string scope){
    string equalities[] = {"==","!=","<=",">=",">","<"};
    vector<string> equality_tokens;
    vector<string> simplify_tokens;
    vector<Token> new_tokens;
    vector<Token> hold_tokens;
    if(tokens.size() < 3){display_error(tokens,scope,"Statement must have 3 or more tokens in condition");}
    for(int i=0;i<tokens.size();i++){
         if(i == tokens.size()-1){
            hold_tokens.push_back(tokens[i]);
            string result = get_value(hold_tokens,scope,-1);
            new_tokens.push_back(token_init(result,get_type(result)));
        }
        else if(tokens[i].type == "STATEMENT"){
            string result = get_value(hold_tokens,scope,-1);
            new_tokens.push_back(token_init(result,get_type(result)));
            new_tokens.push_back(tokens[i]);
            hold_tokens.clear();
        }else{hold_tokens.push_back(tokens[i]);}
    }
    for(int i=0; i<new_tokens.size(); i++){
        Token current_token = new_tokens[i];
        if(current_token.type == "STATEMENT" &&  find_item_in_array(equalities,current_token.value)){
            bool equality = get_equality(new_tokens[i-1],new_tokens[i+1],current_token.value);
            if(equality){
                equality_tokens.push_back("true");
            }else{
                equality_tokens.push_back("false");
            }

        }else if(current_token.type == "STATEMENT"){
            equality_tokens.push_back(current_token.value);
        }
    }
    int count = 0;
    for(int _=0;_<equality_tokens.size();_++){
        string current_token = equality_tokens[count];
        if(count >= equality_tokens.size()){break;}
        if(current_token == "!"){
            if(equality_tokens[count+1] =="true"){simplify_tokens.push_back("false");}
            else{simplify_tokens.push_back("true");}
            count++;
        }else{simplify_tokens.push_back(current_token);}
        count++;
    }
    count = 0;
    for(int _=0;_<simplify_tokens.size();_++){
        if(simplify_tokens[count] == "||"){
            if(simplify_tokens[count-1]=="true"){
                simplify_tokens.erase(simplify_tokens.begin()+1);
                simplify_tokens.erase(simplify_tokens.begin()+1);
                simplify_tokens[0] = "true";
                count = 0;
            }
            else if(simplify_tokens[count+1]=="true"){
                simplify_tokens.erase(simplify_tokens.begin()+1);
                simplify_tokens.erase(simplify_tokens.begin()+1);
                simplify_tokens[0] = "true";
                count = 0;
            }else{
                simplify_tokens.erase(simplify_tokens.begin()+1);
                simplify_tokens.erase(simplify_tokens.begin()+1);
                simplify_tokens[0] = "false";
                count = 0;
            }
        }
        if(simplify_tokens[count] == "&&"){
            if(simplify_tokens[count-1]=="false"){
                simplify_tokens.erase(simplify_tokens.begin()+1);
                simplify_tokens.erase(simplify_tokens.begin()+1);
                simplify_tokens[0] = "false";
                count = 0;
            }
            else if(simplify_tokens[count+1]=="false"){
                simplify_tokens.erase(simplify_tokens.begin()+1);
                simplify_tokens.erase(simplify_tokens.begin()+1);
                simplify_tokens[0] = "false";
                count = 0;
            }else{
                simplify_tokens.erase(simplify_tokens.begin()+1);
                simplify_tokens.erase(simplify_tokens.begin()+1);
                simplify_tokens[0] = "true";
                count = 0;
            }
        }
        count++;
    }
    if(simplify_tokens[0]=="true"){return true;}
    return false;
}

//loop
string loop(vector<Token> tokens,string scope){
    string exe = tokens[tokens.size()-1].value;
    tokens.pop_back();
    vector<Token> new_tokens;
    string current_scope = scope + "!~!do";
    Token temporary_token;
    bool temporary_var = false;
    string type = "FOR";
    for(int i=1;i<tokens.size();i++){
        if(tokens[i].type == "KEYWORD" && tokens[i].value == "as"){temporary_var = true;}
        else if(temporary_var){temporary_token = tokens[i];break;}
        else{new_tokens.push_back(tokens[i]);}
        if(tokens[i].type == "STATEMENT"){type = "WHILE";}
    }
    if(type == "FOR"){
        string value = new_tokens[0].value;
        string value_type = get_type(value);
        Variable var;
        var.name = temporary_token.value;
        var.scope = current_scope;
        int index = variable_list.size();
        if(temporary_token.value != "_" && temporary_var){
            variable_list.push_back(var);
        }
        if(value_type == "NUMBER"){
            for(int i=0;i<stoi(value);i++){
                if(temporary_token.value != "_" && temporary_var){
                    variable_list[index].value = to_string(i);
                    variable_list[index].type = "NUMBER";
                }
                string result = read_and_execute(exe,current_scope);
                if(result != "!~!NONE!~!"){
                    if(result == "!~!STOP!~!"){break;}
                    else if(result == "!~!SKIP!~!"){continue;}
                    return result;
                }
            }
        }  
        else if(value_type == "STRING"){
            for(int i=0;i<value.size();i++){
                if(temporary_token.value != "_" && temporary_var){
                    variable_list[index].value = split(value,"")[i];
                    variable_list[index].type = "STRING";
                }
                string result = read_and_execute(exe,current_scope);
                if(result != "!~!NONE!~!"){
                    if(result == "!~!STOP!~!"){break;}
                    else if(result == "!~!SKIP!~!"){continue;}
                    return result;
                }
            }
        }
        else if(value_type == "ARRAY"){
            vector<vector<Token> > array = split_tokens(value,scope);
            for(int i=0;i<array.size();i++){
                if(temporary_token.value != "_" && temporary_var){
                    string answer = get_value(array[i],scope,-1);
                    variable_list[index].value =answer;
                    variable_list[index].type = get_type(answer);
                }
                string result = read_and_execute(exe,current_scope);
                if(result != "!~!NONE!~!"){
                    if(result == "!~!STOP!~!"){break;}
                    else if(result == "!~!SKIP!~!"){continue;}
                    return result;
                }
            }
        }
    
    }
    else if(type == "WHILE"){
        while(get_value_statement(new_tokens,scope)){
            string result = read_and_execute(exe,current_scope);
            if(result != "!~!NONE!~!"){
                if(result == "!~!STOP!~!"){break;}
                else if(result == "!~!SKIP!~!"){continue;}
                return result;
            }
        }
    }
    //remove the temporary variable and every other variable that was declared in the loop
    int count = 0;
    while(count != variable_list.size()){
        if(variable_list[count].scope == current_scope){
            variable_list.erase(variable_list.begin()+count);
            count--;
        }
        count++;
    }
    return "!~!NONE!~!";
}
