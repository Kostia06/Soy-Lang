#include <string>
#include <vector>
#include <iostream>

#include "include/block.hpp"
#include "include/utils.hpp"
#include "include/error.hpp"
#include "include/lexer.hpp"
#include "include/variable.hpp"
#include "include/function.hpp"

static int state = 1; //if 1; elif 2
static std::string equalities[6] = {"==","!=","<=",">=",">","<"};
namespace block{
    static int get_equality(Token value_1, Token value_2, std::string op,std::string scope,int line){
        // printf("%u <-> %s\t",value_1.type,value_1.value.c_str());
        // printf("%s <-> %i\t\t",op.c_str(),value_1.value == value_2.value);
        // printf("%u <-> %s\n",value_2.type,value_2.value.c_str());
        if(value_1.type != value_2.type){return 0;}
        if(value_1.type ==STRING || value_1.type == ACTION){
            if(op == "=="){return value_1.value == value_2.value;}
            if(op == "!="){return value_1.value != value_2.value;}
            else{display_error(scope,"Invalid operator for string in a condition",line);}
        }else if(value_1.type == INTEGER){
            if(op == "=="){return atoi(value_1.value.c_str()) == atoi(value_2.value.c_str());}
            if(op == "!="){return atoi(value_1.value.c_str()) != atoi(value_2.value.c_str());}
            if(op == ">="){return atoi(value_1.value.c_str()) >= atoi(value_2.value.c_str());}
            if(op == "<="){return atoi(value_1.value.c_str()) <= atoi(value_2.value.c_str());}
            if(op == "<"){return atoi(value_1.value.c_str()) < atoi(value_2.value.c_str());}
            if(op == ">"){return atoi(value_1.value.c_str()) > atoi(value_2.value.c_str());}
        }
        return 0;
    }
    static int get_value_statement(std::string condition,std::string scope,int line){   
        std::vector<Token> tokens = lexer::tokenize((char*)condition.c_str(),scope,line,0);
        if(tokens.size() == 1){return atoi(tokens[0].value.c_str());}
        else if(tokens.size() == 0){return 1;}
        std::vector<Token> new_tokens;
        std::vector<Token> hold_tokens;
        std::vector<std::string> equality_tokens;
        //get full values
        for(int i=0;i<tokens.size();i++){
            if(i==tokens.size()-1){
                if(tokens[i].type == ARGUMENT){new_tokens.push_back(tokens[i]);continue;}
                hold_tokens.push_back(tokens[i]);
                Token result = stuff::get_value(hold_tokens,scope,0,line);
                new_tokens.push_back(result);
            }
            else if(tokens[i].type == COMPARATOR){
                Token result = stuff::get_value(hold_tokens,scope,0,line);
                new_tokens.push_back(result);
                hold_tokens.clear();
                new_tokens.push_back(tokens[i]);
            }
            else if(tokens[i].type == ARGUMENT){new_tokens.push_back(tokens[i]);}
            else{hold_tokens.push_back(tokens[i]);}
        }
        //get equality
        for(int i=0;i<new_tokens.size();i++){
            if(new_tokens[i].type == COMPARATOR && std::find(std::begin(equalities), std::end(equalities), new_tokens[i].value) != std::end(equalities)){
                int result = get_equality(new_tokens[i-1],new_tokens[i+1],new_tokens[i].value,scope,line);
                equality_tokens.push_back(std::to_string(result));
            }
            else if(new_tokens[i].type == COMPARATOR){
                equality_tokens.push_back(new_tokens[i].value);
            }
            else if(new_tokens[i].type == ARGUMENT){
                equality_tokens.push_back(std::to_string(get_value_statement(new_tokens[i].value,scope,line)));
            }
        }
        int count =0;
        for(;;){
            if(count >= equality_tokens.size()){break;}
            if(equality_tokens[count] == "!"){
                if(equality_tokens[count+1] =="1"){
                    equality_tokens.erase(equality_tokens.begin()+count);
                    equality_tokens[count] = "0";
                }
                else{
                    equality_tokens.erase(equality_tokens.begin()+count);
                    equality_tokens[count] = "0";
                }
            }
            count++;
        }
        count = 0;
        if(equality_tokens.size() == 1){return stoi(equality_tokens[0]);}
        for(int _=0;_<equality_tokens.size();_++){
            if(equality_tokens[count] == "||"){
                if(equality_tokens[count-1]=="1"){
                    equality_tokens.erase(equality_tokens.begin()+1);
                    equality_tokens.erase(equality_tokens.begin()+1);
                    equality_tokens[0] = "1";
                    count = 0;
                }
                else if(equality_tokens[count+1]=="1"){
                    equality_tokens.erase(equality_tokens.begin()+1);
                    equality_tokens.erase(equality_tokens.begin()+1);
                    equality_tokens[0] = "1";
                    count = 0;
                }else{
                    equality_tokens.erase(equality_tokens.begin()+1);
                    equality_tokens.erase(equality_tokens.begin()+1);
                    equality_tokens[0] = "0";
                    count = 0;
                }
            }
            if(equality_tokens[count] == "&&"){
                if(equality_tokens[count-1]=="0"){
                    equality_tokens.erase(equality_tokens.begin()+1);
                    equality_tokens.erase(equality_tokens.begin()+1);
                    equality_tokens[0] = "0";
                    count = 0;
                }
                else if(equality_tokens[count+1]=="0"){
                    equality_tokens.erase(equality_tokens.begin()+1);
                    equality_tokens.erase(equality_tokens.begin()+1);
                    equality_tokens[0] = "0";
                    count = 0;
                }else{
                    equality_tokens.erase(equality_tokens.begin()+1);
                    equality_tokens.erase(equality_tokens.begin()+1);
                    equality_tokens[0] = "1";
                    count = 0;
                }
            }
            count++;
        }
        return stoi(equality_tokens[0]);
        return 0;
    }
    Token statement(std::vector<Token>& tokens,std::string& text,std::string& scope,int line){
        if(tokens.size() < 1){display_error(scope,"Not enough tokens to create a statement",line);}
        if(tokens.size() > 2){display_error(scope,"Too many tokens to create a statement",line);}
        int count = 1; //if 1; elif 2 
        if(tokens[0].value == "elif"){count = 2;}
        if(state == 1 && count == 2){return (Token){"!~!NONE!~!",NONE};}
        int value = get_value_statement(tokens[1].value,scope,line);
        if(value){
            state = 1;
           int var_index = variables.size(), func_index = functions.size(), obj_index = objects.size();
            tokens =  lexer::tokenize((char*)text.c_str(),scope+"!~!if",line);
            Token result = (Token){"!~!NONE!~!",NONE};
            if(tokens.size() > 0){result = tokens[0];}
            var::destroy_scope(var_index,scope+"!~!if");
            func::destroy_scope(func_index,scope+"!~!if");
            obj::destroy_scope(obj_index,scope+"!~!if");
            return result;
        }else{state = 2;}
        return (Token){"!~!NONE!~!",NONE};
    }
    static int check_condition(std::string condition){
        for(int i=0;i<6;i++){
            if(condition.find(equalities[i]) != std::string::npos){return 1;}
        }
        return 0;
    }
    Token loop(std::vector<Token>& tokens,std::string& text,std::string& scope,int line){
        if(tokens.size() < 1){display_error(scope,"Not enough tokens to create a statement",line);}
        if(tokens.size() > 3){display_error(scope,"Too many tokens to create a statement",line);}
        std::string current_scope = scope + "!~!do",variable_name;
        int var_index = variables.size(), func_index = functions.size(), obj_index = objects.size();
        if(tokens.size() == 3){variable_name = tokens[2].value;}
        if(check_condition(tokens[0].value)){
            int value = get_value_statement(tokens[0].value,current_scope,line);
            while(value){
                std::vector<Token> new_tokens =  lexer::tokenize((char*)text.c_str(),current_scope,line);
                Token result = (Token){"!~!NONE!~!",NONE};
                if(new_tokens.size() > 0){result = new_tokens[0];}
                if(result.value == "!~!SKIP!~!"){continue;}
                else if(result.value == "!~!STOP!~!"){break;}
                else if(result.value != "!~!NONE!~!"){
                    var::destroy_scope(var_index,current_scope);
                    func::destroy_scope(func_index,current_scope);
                    obj::destroy_scope(obj_index,current_scope);
                    return result;
                }
                value = get_value_statement(tokens[0].value,current_scope,line);
            }
        }
        //for loop
        else{
            tokens = lexer::tokenize((char*)tokens[0].value.c_str(),current_scope,line,0);
            Token value = stuff::get_value(tokens,scope,0,line);
            int index;
            if(variable_name != ""){
                index = variables.size();
                variables.push_back((Variable){variable_name,"",current_scope,value.type});
            }
            if(value.type == INTEGER){
                for(int i=0;i<std::stoi(value.value);i++){
                    if(variable_name != ""){
                        variables[index].value = std::to_string(i);
                        variables[index].type = INTEGER;
                    }
                    tokens = lexer::tokenize((char*)text.c_str(),current_scope,line);
                    Token result = (Token){"!~!NONE!~!",NONE};
                    if(tokens.size() > 0){result = tokens[0];}
                    if(result.value == "!~!SKIP!~!"){continue;}
                    else if(result.value == "!~!STOP!~!"){break;}
                    else if(result.value != "!~!NONE!~!"){
                        var::destroy_scope(var_index,current_scope);
                        func::destroy_scope(func_index,current_scope);
                        obj::destroy_scope(obj_index,current_scope);
                        return result;
                    }
                }
            }
            else if(value.type == STRING){
                for(int i=0;i<value.value.size();i++){
                    if(variable_name != ""){
                        variables[index].value = value.value[i];
                        variables[index].type = STRING;
                    }
                    tokens = lexer::tokenize((char*)text.c_str(),current_scope,line,1);
                    Token result = (Token){"!~!NONE!~!",NONE};
                    if(tokens.size() > 0){result = tokens[0];}
                    if(result.value == "!~!SKIP!~!"){continue;}
                    else if(result.value == "!~!STOP!~!"){break;}
                    else if(result.value != "!~!NONE!~!"){
                        var::destroy_scope(var_index,current_scope);
                        func::destroy_scope(func_index,current_scope);
                        obj::destroy_scope(obj_index,current_scope);
                        return result;
                    }
                }
            }
            else if(value.type == ARRAY){
                value.value.pop_back();
                value.value.erase(value.value.begin());
                std::vector<Token> array = stuff::split_tokens(tokens,scope,0,line);
                for(int i=0;i<array.size();i++){
                    if(variable_name != ""){
                        variables[index].value = array[i].value;
                        variables[index].type = array[i].type;
                    }
                    tokens = lexer::tokenize((char*)text.c_str(),current_scope,line,1);
                    Token result = (Token){"!~!NONE!~!",NONE};
                    if(tokens.size() > 0){result = tokens[0];}
                    if(result.value == "!~!SKIP!~!"){continue;}
                    else if(result.value == "!~!STOP!~!"){break;}
                    else if(result.value != "!~!NONE!~!"){
                        var::destroy_scope(var_index,current_scope);
                        func::destroy_scope(func_index,current_scope);
                        obj::destroy_scope(obj_index,current_scope);
                        return result;
                    }
                }
            }
        }
        var::destroy_scope(var_index,current_scope);
        func::destroy_scope(func_index,current_scope);
        obj::destroy_scope(obj_index,current_scope);
        return (Token){"!~!NONE!~!",NONE};
    }
}



