#include <string>
#include <stdio.h>
#include <vector>
#include <regex>

#include <iomanip>
#include <iostream>
#include <ranges>
#include <string_view>

#include "include/utils.hpp"
#include "include/math.hpp"
#include "include/error.hpp"
#include "include/variable.hpp"
#include "include/function.hpp"

namespace tools{
    std::vector<std::string> split(std::string str,std::string delimiter){
        std::vector<std::string> result;
        size_t pos = 0;
        std::string token;
        while ((pos = str.find(delimiter)) != std::string::npos) {
            token = str.substr(0, pos);
            result.push_back(token);
            str.erase(0, pos + delimiter.length());
        }
        result.push_back(str);
        return result;
    }
    std::string replace(std::string str,std::string old,std::string new_str){
        size_t pos = 0;
        while ((pos = str.find(old)) != std::string::npos) {
            str.replace(pos, old.length(), new_str);
        }
        return str;
    }
    std::string join(std::vector<std::string>& vec, std::string delimiter){
        std::string newStr;
        for (int i = 0; i < (int)vec.size(); i++){
            newStr += vec[i];
            if (i != (int)vec.size() - 1){newStr += delimiter;}
        }
        return newStr;
    }
    int check_scope(std::string& from, std::string& to){
        return to == "PUBLIC" || to == from || from.find(to) != std::string::npos;
    }
    int is_number(int c){return (c >= '0' && c <= '9');}
    int is_letter(int c){return (c >= 'a' && c <= 'z') || (c >= 'A' && c <= 'Z');}
}

static int is_string(int c){return c == '\'' || c == '\"' || c=='`';}
static std::string actions[6] = {"+", "-", "*", "/", "%","^"};
namespace stuff{
    void output(std::vector<Token> tokens){
        for(Token i:tokens){
            if(i.type == KEYWORD && i.value == "end"){printf("\n");}
            else{printf("%s",i.value.c_str());}
        }
    }
    std::vector<Token> split_tokens(std::vector<Token>& tokens,std::string& scope,int stringify,int line){
        std::vector<Token> new_tokens,array;
        int count = 0;
        for(;;){
            if(count == tokens.size()){break;}
            Token token = tokens[count];
            if(token.type == PUNCTUATION && token.value == ","){
                array.push_back(get_value(new_tokens,scope,stringify,line));
                new_tokens.clear();
            }
            else{new_tokens.push_back(token);}
            if(count+1 == tokens.size()){
                array.push_back(get_value(new_tokens,scope,stringify,line));
            }
            count++;
        }
        return array;
    }
    Token get_value(std::vector<Token>& tokens, std::string& scope,int stringify,int& line){
        if(tokens.size() == 1){
            if(stringify && tokens[0].type == STRING){
                tokens[0].value.erase(tokens[0].value.begin());
                tokens[0].value.pop_back();
                return tokens[0];
            }
        return tokens[0];
        }
        enum types type = INTEGER;
        if(tokens[0].type == ARRAY){type = ARRAY;}
        else{for(int i=1;i<tokens.size();i++){if(tokens[i].type == STRING){type = STRING;break;}}}

        if(type == STRING){
            std::string result = tokens[0].value;
            if(tokens[0].type == STRING){result.erase(result.begin());result.pop_back();}
            for(int i=1;i<tokens.size();i++){
                if(tokens[i].type == ACTION){
                    if(tokens[i].value == "+"){
                        std::string value = tokens[i+1].value;
                        if(tokens[i+1].type == STRING){
                            value.erase(value.begin());
                            value.pop_back();
                        }
                        result += value;
                        continue;
                    }
                    else if(tokens[i].value == "-"){
                        int value = stoi(tokens[i+1].value);
                        if(tokens[i+1].type != INTEGER){display_error(scope,"Can't subtract a non-integer",line);}
                        if(value < 0 || value >= result.length()){display_error(scope,"Index out of range",line);}
                        result.erase(result.length()+value);
                        continue;
                    }
                    else if(tokens[i].value == "*"){
                        int value = stoi(tokens[i+1].value);
                        if(tokens[i+1].type != INTEGER){display_error(scope,"Can't multiply a non-integer",line);}
                        if(value < 0){display_error(scope,"Can't multiply by a negative number",line);}
                        std::string new_result = "";
                        for(int j=0;j<value;j++){
                            new_result += result;
                        }
                        result = new_result;
                        continue;
                    }
                    else{display_error(scope,"Unknown action to use on string",line);}
                }
            }
            if(!stringify){
                return (Token){"\""+result+"\"",STRING};
         }
            return (Token){result,STRING};
        }
        else if(type == ARRAY){
            std::string result = tokens[0].value;
            for(int i=1;i<tokens.size();i++){
                if(tokens[i].type == ACTION){
                    if(tokens[i].type == ACTION && tokens[i].value == "+"){
                        std::string value1 = tokens[i+1].value;
                        if(tokens[i+1].type == STRING){
                            value1.erase(value1.begin());
                            value1.pop_back();
                        }
                        result += ","+value1;
                        continue;
                    }
                    else if(tokens[i].type == ACTION && tokens[i].value == "-"){
                        int value = stoi(tokens[i+1].value);
                        if(tokens[i+1].type != INTEGER){display_error(scope,"Can't subtract a non-integer",line);}
                        if(value < 0 || value >= result.length()){display_error(scope,"Index out of range",line);}
                        std::vector<std::string> array = tools::split(result,",");
                        array.erase(array.begin()+value);
                        result = tools::join(array,",");
                        continue;
                    }
                    else{display_error(scope,"Invalid action to use on array",line);}
                }
            }
            return (Token){result,ARRAY};
        }
        else if(type == INTEGER){
            std::string value = "0";
            for(int i=0;i<tokens.size();i++){value += tokens[i].value;}
            return (Token){std::to_string(evaluate(value)), INTEGER};
        }
        display_error(scope,"Unknown action",line);
        return (Token){"",NONE};
    }
    types get_type(std::string value){
        if(is_string(value[0]) && is_string(value[value.size()-1])){return STRING;}
        if(value[0] == '[' && value[value.size()-1]==']'){return ARRAY;}
        if(value[value.size()-1] =='}'){return OBJECT;}
        if(std::find(std::begin(actions), std::end(actions), value) != std::end(actions)){return ACTION;}
        return INTEGER;
    }
}