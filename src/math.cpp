#include <string> 
#include <iostream>
#include <fstream>
#include <vector>
#include "../include/math.hpp"
#include "../include/utils.hpp"
#include "../include/lexer.hpp"
using namespace std;

string numbers ="1234567890.";
string operators = "+-*/%^";
string brackets = "()";
const string signs = "()+-*/%^";

bool is_sign_math(char c){
    for(int i=0;i<signs.size();i++){
        if(c== signs[i]){return true;}
    }
    return false;
}

vector<Token> tokens_for_math(string text,string scope){
    vector<Token> new_tokens;
    int char_index =0;
    int line_size = text.size();
    for(int i=0; i<line_size; i++){
        int max_length = line_size -char_index;
        Token token;
        if(max_length == 0){break;}
        char current_char = text.at(char_index);
        if(current_char == ' '){char_index++; continue;}
        else if(is_number(current_char) || ((new_tokens.size() == 0 || new_tokens[new_tokens.size()-1].type != "NUMBER") && current_char == '-')){
            string result = "";
            for(int _=0;_<max_length;_++){
                result += text.at(char_index);
                char_index++;
                if(char_index == line_size){break;}
                else if(!(numbers.find(text.at(char_index))!= string::npos)){break;}
            }
            token.value = result;
            token.type = "NUMBER";
            new_tokens.push_back(token);
            continue;   
            
        }
        else if(is_sign_math(current_char)){
            token.value = current_char;
            token.type = "";
            new_tokens.push_back(token);
            char_index++;
            continue; 
        }
        else{display_error(new_tokens,scope,"Uknown text in math expression");}        
    }
    return new_tokens;
}
float power(float from , float to){
    int answer = from;
    for(int i = 1; i < to;i++){
        answer *= from;
    }
    return answer;
}

string apply_operations(float a, float b, string op){
    if(op == "+"){return to_string(a+b);}
    if(op == "-"){return to_string(a-b);}
    if(op == "*"){return to_string(a*b);}
    if(op == "/"){return to_string(a/b);}
    if(op == "%"){return to_string((int)a%(int)b);}
    if(op == "^"){return to_string(power(a,b));}
    return "!~!NONE";
}
int precedence(string op){
    if(op == "+" || op == "-"){return 1;}
    if(op == "*" || op == "/" || op == "%" || op=="^"){return 2;}
    return 0;
}
string math_eval(string text,string scope){
    //gets operations
    vector<string> values;
    vector<string> ops;
    vector<Token> new_tokens = tokens_for_math(text,scope);
    for(int i=0; i<new_tokens.size(); i++){
        Token current_token = new_tokens[i];
        if(current_token.value == "("){ops.push_back(current_token.value);}
        else if(current_token.type == "NUMBER"){values.push_back(current_token.value);}
        else if(current_token.value == ")"){
            while(ops.size() > 0 && ops[ops.size() - 1] != "("){
                string value_2 = values[values.size() - 1];
                values.erase(values.begin()+values.size() - 1);
                string value_1 = values[values.size() - 1];
                values.erase(values.begin()+values.size() - 1);
                string op = ops[ops.size() - 1];
                ops.erase(ops.begin()+ops.size() - 1);
                values.push_back(apply_operations(atof(value_1.c_str()),atof(value_2.c_str()),op));
            }
            ops.erase(ops.begin()+ops.size() - 1);
        }
        else{
            while(ops.size() > 0 && precedence(ops[ops.size()-1]) >= precedence(current_token.value)){
                string value_2 = values[values.size() - 1];
                values.erase(values.begin()+values.size() - 1);
                string value_1 = values[values.size() - 1];
                values.erase(values.begin()+values.size() - 1);
                string op = ops[ops.size() - 1];
                ops.erase(ops.begin()+ops.size() - 1);
                values.push_back(apply_operations(atof(value_1.c_str()),atof(value_2.c_str()),op));
            }
            ops.push_back(current_token.value);
        }
    }
    //gets final value
    while(ops.size() > 0){
        string value_2 = values[values.size() - 1];
        values.erase(values.begin()+values.size() - 1);
        string value_1 = values[values.size() - 1];
        values.erase(values.begin()+values.size() - 1);
        string op = ops[ops.size() - 1];
        ops.erase(ops.begin()+ops.size() - 1);
        values.push_back(apply_operations(atof(value_1.c_str()),atof(value_2.c_str()),op));
    }
    return values[0];
}
