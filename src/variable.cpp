#include <string>
#include <vector>

#include "include/variable.hpp"
#include "include/function.hpp"
#include "include/utils.hpp"
#include "include/lexer.hpp"
#include "include/error.hpp"


namespace var{
    void declaration(std::vector<Token>& tokens, std::string& current_scope, std::string& scope, int& line){
        if(tokens.size() < 3){display_error(scope,"Not enough tokens to declare a variable",line);}
        std::vector<Token> names,values;
        Token op;
        for(Token token : tokens){
            if(token.type == OPERATOR){op = token;}
            else if(op.value != ""){values.push_back(token);}
            else{names.push_back(token);}
        }
        if(names.size() == 1){
            if(op.value == "="){
                Token result = stuff::get_value(values,scope,0,line);
                if(names[0].type == INTEGER){
                    variables[stoi(names[0].value)] = (Variable){variables[stoi(names[0].value)].name,result.value,variables[stoi(names[0].value)].scope,result.type};
                }
                else{variables.push_back((Variable){names[0].value,result.value,current_scope,result.type});}
                return;
            }
            else{
                if(names[0].type != INTEGER || stoi(names[0].value) == -1){display_error(scope,"To use operators, variable has to be declared",line);}
                if(op.value == "+="){values.insert(values.begin(),(Token){"+",ACTION});}
                else if(op.value == "-="){values.insert(values.begin(),(Token){"-",ACTION});}
                else if(op.value == "*="){values.insert(values.begin(),(Token){"*",ACTION});}
                else if(op.value == "/="){values.insert(values.begin(),(Token){"/",ACTION});}
                else if(op.value == "^="){values.insert(values.begin(),(Token){"^",ACTION});}
                else if(op.value == "%="){values.insert(values.begin(),(Token){"%",ACTION});}
                values.insert(values.begin(),(Token){variables[stoi(names[0].value)].value,variables[stoi(names[0].value)].type});
                Token result = stuff::get_value(values,scope,0,line);
                variables[stoi(names[0].value)] = (Variable){variables[stoi(names[0].value)].name,result.value,variables[stoi(names[0].value)].scope,result.type};
                return;
            }
            
        }
        else if(names.size() > 0){
            if(op.value == "+="){values.insert(values.begin(),(Token){"+",ACTION});}
            else if(op.value == "-="){values.insert(values.begin(),(Token){"-",ACTION});}
            else if(op.value == "*="){values.insert(values.begin(),(Token){"*",ACTION});}
            else if(op.value == "/="){values.insert(values.begin(),(Token){"/",ACTION});}
            else if(op.value == "^="){values.insert(values.begin(),(Token){"^",ACTION});}
            else if(op.value == "%="){values.insert(values.begin(),(Token){"%",ACTION});}
            if(op.value != "="){
                values.insert(values.begin(),names.begin(),names.end());
                values[0] = (Token){variables[stoi(names[0].value)].value,variables[stoi(names[0].value)].type};
            }
            values.insert(values.begin(),(Token){"=",OPERATOR});
            names.insert(names.end(), values.begin(), values.end());
            pointer(names,scope,line);
        }
        else{display_error(scope,"No variable name given",line);}

    }
    int in_list(std::string name,std::string scope){
        for(int i=0;i<variables.size();i++){
            if(variables[variables.size()-i-1].name == name && tools::check_scope(scope,variables[variables.size()-i-1].scope)){
                return variables.size()-i-1;
            }
        }
        return -1;
    }
    void destroy_scope(int index,std::string scope){
        int count = 0;
        for(;;){
            if(count == variables.size()){break;}
            if(tools::check_scope(variables[variables.size()-count-1].scope,scope)){
                if(variables.size()-count-1 == index){
                    variables.erase(variables.begin()+variables.size()-count-1);
                    count--;
                    break;
                }
                variables.erase(variables.begin()+variables.size()-count-1);
                count--;
            }
            count++;
        }
    }
    void pointer(std::vector<Token> tokens, std::string scope,int line){
        if(tokens.size()<3){display_error(scope,"Not enough tokens to declare a pointer",line);}
        int found = atoi(tokens[0].value.c_str());
        Variable variable = variables[found];
        std::string values = "";
        std::vector<Token>pointers;
        std::vector<std::string> holders, new_values;
        int work = 0;
        for(int i=1;i<tokens.size();i++){
            if(tokens[i].type == OPERATOR){work = 1;}
            else if(work){values += tokens[i].value;}
            else{pointers.push_back(tokens[i]);}
        }
        if(values.size() == 0){display_error(scope,"No value given to pointer",line);}
        std::vector<Token> values_tokens = lexer::tokenize((char*)values.c_str(),scope,line,0);
        std::string value = stuff::get_value(values_tokens,scope,0,line).value;
        holders.push_back(variable.value);
        new_values.push_back(value);
        for(int i=0;i<pointers.size();i++){
            std::string pointer = holders[holders.size()-1] + pointers[i].value;
            std::vector<Token> current_tokens = lexer::tokenize((char*)pointer.c_str(),scope,line,0);
            std::string result = stuff::get_value(current_tokens,scope,0,line).value;
            holders.push_back(result);
        }
        for(int i=2;i<holders.size()+1;i++){
            enum types type = stuff::get_type(holders[holders.size()-i]);
            if(type == OBJECT){
                new_values.push_back(obj::change_object_value(holders[holders.size()-i],pointers[pointers.size()-i+1].value,new_values[new_values.size()-1],scope,line));
            }
            else if(type == ARRAY){
                new_values.push_back(index_array(holders[holders.size()-i],pointers[pointers.size()-i+1].value,new_values[new_values.size()-1],scope,line));
            }
            else{
                display_error(scope,"Invalid pointer",line);
            }
        }
        variable.name = variables[found].name;
        variable.value = new_values[new_values.size()-1];
        variable.type = stuff::get_type(variable.value);
        variables[found] = variable;
    }
    std::string index_array(std::string array, std::string pointer, std::string value, std::string scope, int line){
        array.pop_back();
        array.erase(array.begin());
        pointer.erase(pointer.begin());
        pointer.pop_back();
        if(stuff::get_type(pointer)!= INTEGER){display_error(scope,"Array index must be a number",line);}
        std::vector<Token> tokens = lexer::tokenize((char*)array.c_str(),scope,line,0);
        std::vector<Token> items = stuff::split_tokens(tokens,scope,0,line);
        if(stoi(pointer) >= items.size()){display_error(scope,"Index out of range",line);}
        items[stoi(pointer)] = (Token){value,stuff::get_type(value)};
        std::string result = "[";
        for(int i=0;i<items.size();i++){
            result += items[i].value + ",";
        }
        result[result.size()-1] = ']';
        return result;
    }
}




