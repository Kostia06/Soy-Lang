#include <string>
#include <vector>

#include "include/function.hpp"
#include "include/utils.hpp"
#include "include/lexer.hpp"
#include "include/variable.hpp"
#include "include/error.hpp"

static int working_functions = 0;
namespace func{
    void declaration(std::vector<Token>& tokens,std::string& text,std::string& current_scope,std::string& scope,int line){
        Function function;
        std::vector<std::string> args = tools::split(tokens[1].value,",");
        if(args[0] == ""){args.clear();}
        function.name = tokens[0].value;
        function.current_scope = current_scope;
        function.scope = scope;
        function.text = text;
        function.arguments = args;
        function.line = line;
        functions.push_back(function);
    }
    Token call(std::vector<Token> parameters,std::string scope,int name,int line){
        Function function = functions[name];
        std::string current_scope = scope + "!~!"+function.name+std::to_string(working_functions++);
        if(parameters.size() < function.arguments.size()){display_error(scope,"Not enough arguments for function "+function.name+" on line ",line);}
        for(int i =0;i<function.arguments.size();i++){
            std::string value = parameters[i].value;
            enum types type = parameters[i].type;
            variables.push_back((Variable){function.arguments[i],value,current_scope,type});
        }
        int var_index = variables.size(), func_index = functions.size(), obj_index = objects.size();
        std::vector<Token> tokens = lexer::tokenize((char*)function.text.c_str(),current_scope,function.line);
        var::destroy_scope(var_index,current_scope);
        destroy_scope(func_index,current_scope);
        obj::destroy_scope(obj_index,current_scope);
        if(tokens.size() > 0){return tokens[0];}
        return (Token){"0",INTEGER};
    }
    int in_list(std::string& name,std::string& scope){
        for(int i=0;i<functions.size();i++){
            if(functions[functions.size()-i-1].name == name && tools::check_scope(scope,functions[functions.size()-i-1].scope)){
                return functions.size()-i-1;
            }
        }
        return -1;
    }
    void destroy_scope(int index,std::string scope){
        int count = 0;
        for(;;){
            if(count == functions.size()){break;}
            if(tools::check_scope(functions[functions.size()-count-1].scope,scope)){
                if(variables.size()-count-1 == index){
                    functions.erase(functions.begin()+functions.size()-count-1);
                    count--;
                    break;
                }
                functions.erase(functions.begin()+functions.size()-count-1);
                count--;
            }
            count++;
        }
    }
}

namespace obj{
    void declaration(std::vector<Token>& tokens,std::string& parameters, std::string& current_scope,std::string& scope,int line){
        if(tokens.size() > 1){display_error(scope,"Too many tokens to declare an object",line);}
        if(tokens.size() < 1){display_error(scope,"Not enough tokens to declare an object",line);}
        objects.push_back((Object){tokens[0].value,current_scope,tools::split(parameters,",")});
    }
    int in_list(std::string& name, std::string& scope){
        for(int i=0;i<objects.size();i++){
            if(objects[objects.size()-i-1].name==name && tools::check_scope(scope,objects[objects.size()-i-1].scope)){
                return objects.size()-i-1;
            }
        }
        return -1;    
    }
    Token get_value(std::string& name,std::string& object,std::string& scope,int line){
        int index = stoi(tools::split(object,"{")[0]);
        std::vector<std::string> items = objects[index].arguments;
        std::string object_list = tools::split(object,"{")[1].c_str();
        object_list.pop_back();
        std::vector<std::string> values = tools::split(object_list,",");
        for(int i=0;i<items.size();i++){
            if(tools::replace(items[i]," ","") == name){
                return (Token){values[i],stuff::get_type(values[i])};
            }
        }
        display_error(scope,"Object does not have a value with that name \""+name+"\"",0);
        return (Token){"0",INTEGER};
    }
    std::string change_object_value(std::string object,std::string target,std::string value,std::string scope,int line){
        int find = atoi(&object[0]);
        Object find_object = objects[find];
        std::vector<std::string> items = find_object.arguments;
        std::string new_object = std::to_string(find) + "{";
        std::string object_items = tools::split(object,"{")[1].c_str();
        object_items.pop_back();
        int found = 0;
        std::vector<Token> tokens = lexer::tokenize((char*)object_items.c_str(),scope,line,0);
        std::vector<Token> values = stuff::split_tokens(tokens,scope,0,line);
        for(int i=0;i<items.size();i++){
            if(items[i] == tools::replace(target,".","")){
                new_object += value + ",";
                found = 1;
            }else{
                new_object += values[i].value + ",";
            }
        }
        if(found == 0){display_error(scope,"Object does not have a value with that name \""+target+"\"",0);}
        new_object.pop_back();new_object += "}";
        return new_object;
    }
    void destroy_scope(int index,std::string scope){
        int count = 0;
        for(;;){
            if(count == objects.size()){break;}
            if(tools::check_scope(objects[objects.size()-count-1].scope,scope)){
                if(variables.size()-count-1 == index){
                    objects.erase(objects.begin()+objects.size()-count-1);
                    count--;
                    break;
                }
                objects.erase(objects.begin()+objects.size()-count-1);
                count--;
            }
            count++;
        }
    }

}