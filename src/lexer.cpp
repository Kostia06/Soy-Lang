#include <stdlib.h>
#include <string.h> 
#include <vector>
#include <iostream>
#include <algorithm>
#include <thread>

#include "include/lexer.hpp"
#include "include/variable.hpp"
#include "include/block.hpp"
#include "include/function.hpp"
#include "include/error.hpp"
#include "include/utils.hpp"
#include "include/module.hpp"
#include "include/read.hpp"

static std::string keywords[14] = {"out","as","if","elif","input","fn","obj","do","pub","return","stop","skip","end","use"};


static void use(std::vector<Token> tokens,std::string scope, int line){
    int count = 0;
    std::vector<Token> current_tokens = stuff::split_tokens(tokens,scope,1,line);
    
    std::vector<std::string> directory = tools::split(tools::split(scope,"!~!")[0],"/");
    directory.pop_back();
    std::string path = tools::join(directory,"/");
    for(int i=0;i<current_tokens.size();i++){
        std::string result = path+current_tokens[i].value;
        char buffer[50];
        strcpy(buffer, result.c_str());
        lexer::tokenize((char*)read_file(buffer).c_str(),result,1,1);
    }
}
static Token parser(std::vector<Token>& tokens,std::string& current_scope,std::string& scope, enum types& type,int& line){
    switch(type){
        case VARIABLE_DECLARATION:
            var::declaration(tokens,current_scope,scope,line);return (Token){"!~!CLEAR!~!",STRING};
        case RETURN:
            return stuff::get_value(tokens,scope,0,line);
        case STOP:
            return (Token){"!~!STOP!~!",STRING};
        case SKIP:
            return (Token){"!~!SKIP!~!",STRING};
        case USE:
            use(tokens,scope,line);return (Token){"!~!CLEAR!~!",STRING};
        default:
            return (Token){"!~!NONE!~!",STRING};
    }
}
static std::string get_token_type(enum types type){
    switch(type){
        case ACTION: return "ACTION";
        case STRING: return "STRING";
        case INTEGER: return "INTEGER";
        case ARRAY: return "ARRAY";
        case OBJECT: return "OBJECT";
        case ARGUMENT: return "ARGUMENT";
        default: return "NONE";
    }
}
namespace lexer{
    std::vector<Token> tokenize(char* text, std::string scope,int line,int variable_check){
        std::vector<Token> tokens;
        enum types parser_type = NONE;
        std::string current_scope = scope;
        char c = *text++;
        for(;;){
            switch(c){
                case '\0':{
                    if(parser_type != NONE){
                        Token result = parser(tokens,current_scope,scope,parser_type,line);
                        if(result.value == "!~!CLEAR!~!"){tokens.clear();}
                        else if(result.value != "!~!NONE!~!"){
                            tokens.clear();tokens.push_back(result);
                            return tokens;
                        }
                    }
                    if(variable_check == 1){tokens.clear();tokens.push_back((Token){"!~!NONE!~!",STRING});}
                    return tokens;
                }
                case ' ':case '\t':case '\v':case '\f':{
                    c = *text++;break;
                }
                case '\r':case '\n':case ';':{
                    if(parser_type == FUNCTION_DECLARATION || parser_type == IF || parser_type == LOOP){
                        c= *text++;
                        break;
                    }
                    else if(parser_type == SKIP){
                        tokens.clear();
                        tokens.push_back((Token){"!~!SKIP!~!",SKIP});
                        return tokens;
                    }
                    else if(parser_type == STOP){
                        tokens.clear();
                        tokens.push_back((Token){"!~!STOP!~!",STOP});
                        return tokens;
                    }
                    else if(parser_type != NONE){
                        Token result = parser(tokens,current_scope,scope,parser_type,line);
                        if(result.value == "!~!CLEAR!~!"){tokens.clear();}
                        else if(result.value != "!~!NONE!~!"){
                            tokens.clear();tokens.push_back(result);
                            return tokens;
                        }
                    }  
                    if(c == '\n'){line++;}
                    parser_type = NONE;
                    c = *text++;break;
                }
                case '\'': case '\"':case '`':case '~':{
                    char end =c;
                    std::string result = "";result+=c;
                    c = *text++;
                    for(;;){
                        if(c == end || c=='\0'){break;}
                        if(c == '\n'){line++;}
                        result += c;
                        c = *text++;
                    }
                    result += c;
                    c = *text++;
                    if(end == '~'){break;}
                    tokens.push_back((Token){result,STRING});
                    break;
                }
                case '0':case '1':case '2':case '3':case '4':case '5':case '6':case '7':case '8':case '9':{
                    std::string result = "";result += c;
                    c = *text++;
                    for(int i=1;;i++){
                        if(c == '\0'){break;}
                        if(!tools::is_number(c) && c!='.' && c!='_'){result[i]='\0';break;}
                        result += c;
                        c = *text++;
                    }
                    tokens.push_back((Token){tools::replace(result,"_",""),INTEGER});
                    break;
                }
                case '[':case '(':case '{':{
                    char end = c == '[' ? ']' : c == '(' ? ')' : '}';
                    char start = c;
                    std::string result = "";result+=c;
                    c = *text++;
                    int count = 1;
                    for(;;){
                        if(c == '\0' || count ==0){break;}
                        if(c == '\n'){line++;}
                        if(end == c){count--;}
                        else if(start == c){count++;}
                        result += c;
                        c = *text++;
                    }
                    if(count != 0){display_error(scope,"Missing end of block",line);}
                    result.pop_back();
                    result.erase(result.begin());
                    if(end == ')'){
                        if(tokens.size()>0){
                            if(tokens[tokens.size()-1].type == MODULE){
                                result = modules::module_init(tokens[tokens.size()-1].value,result,scope,line);
                                tokens[tokens.size()-1].value = result;
                                tokens[tokens.size()-1].type = stuff::get_type(result);
                                break;
                            }
                            if(tokens[tokens.size()-1].type == INTEGER){
                                std::vector<Token> new_tokens = tokenize((char*)result.c_str(),scope,line,0);
                                new_tokens = stuff::split_tokens(new_tokens,scope,0,line);
                                Token token = func::call(new_tokens,scope,stoi(tokens[tokens.size()-1].value),line);
                                tokens[tokens.size()-1] = token;
                                break;
                            }
                            if(tokens[tokens.size()-1].type == KEYWORD){
                                if(tokens[tokens.size()-1].value == "out"){
                                    std::vector<Token> new_tokens = tokenize((char*)result.c_str(),scope,line,0);
                                    new_tokens = stuff::split_tokens(new_tokens,scope,1,line);
                                    stuff::output(new_tokens);
                                    tokens.pop_back();
                                    break;
                                }
                                if(tokens[tokens.size()-1].value == "input"){
                                    std::vector<Token> new_tokens = tokenize((char*)result.c_str(),scope,line,0);
                                    new_tokens = stuff::split_tokens(new_tokens,scope,1,line);
                                    //get input
                                    std::string input = "";
                                    std::getline(std::cin,input);
                                    stuff::output(new_tokens);
                                    tokens[tokens.size()-1] = (Token){input,stuff::get_type(input)};
                                    break;;
                                }
                            }
                        }
                        if(parser_type == IF){
                            std::vector<Token> new_tokens = tokenize((char*)result.c_str(),scope,line,0);
                            std::vector<std::string> array;
                            for(Token i:new_tokens){array.push_back(i.value);}
                            tokens.push_back((Token){tools::join(array,""),ARGUMENT});
                            break;
                        }
                        tokens.push_back((Token){result,ARGUMENT});
                        break;  
                    }
                    if(end == '}'){
                        if(tokens.size() > 0 && tokens[tokens.size()-1].type == INTEGER){
                            std::vector<Token> new_tokens = tokenize((char*)result.c_str(),scope,line,0);
                            new_tokens = stuff::split_tokens(new_tokens,scope,0,line);
                            result = "{";
                            for(Token i:new_tokens){result += i.value + ",";}
                            result[result.size()-1] = '}';
                            tokens[tokens.size()-1].value += result;
                            tokens[tokens.size()-1].type = OBJECT;
                            break;
                        } 
                        if(parser_type == FUNCTION_DECLARATION){
                            func::declaration(tokens,result,current_scope,scope,line);
                            tokens.clear();
                            parser_type = NONE;
                            current_scope = scope;
                            break;
                        }
                        if(parser_type == OBJECT_DECLARATION){
                            result = tools::replace(result,"\n","");
                            obj::declaration(tokens,result,current_scope,scope,line);
                            tokens.clear();
                            parser_type = NONE;
                            current_scope = scope;
                            break;
                        }
                        if(parser_type == IF){
                            Token token = block::statement(tokens,result,scope,line);
                            tokens.clear();
                            parser_type = NONE;
                            current_scope = scope;
                            if(token.value != "!~!NONE!~!"){tokens.push_back(token);return tokens;}
                            break;
                        }
                        if(parser_type == LOOP){
                            Token token = block::loop(tokens,result,scope,line);
                            tokens.clear();
                            parser_type = NONE;
                            current_scope = scope;
                            if(token.value != "!~!NONE!~!"){tokens.push_back(token);return tokens;}
                            break;
                        }
                        tokens.push_back((Token){result,BIND});
                        break;
                    }
                    if(end == ']'){
                        std::vector<Token> new_tokens = tokenize((char*)result.c_str(),scope,line,0);
                        new_tokens = stuff::split_tokens(new_tokens,scope,0,line);     
                        std::string final_result ;
                        for(Token i:new_tokens){final_result += i.value +",";}
                        final_result.pop_back();
                        if(new_tokens.size() == 1 && (parser_type != NONE || variable_check == 0) && tokens.size() > 0 && (tokens[tokens.size()-1].type == STRING || tokens[tokens.size()-1].type == ARRAY)){
                            std::string value = new_tokens[0].value;
                            if(tokens[tokens.size()-1].type == STRING){
                                if(stoi(value) >= tokens[tokens.size()-1].value.size()){display_error(scope,"Index out of range",line);}
                                final_result = tokens[tokens.size()-1].value[stoi(value)];
                                tokens[tokens.size()-1] = (Token){final_result,STRING};
                                break;
                            }
                            else{
                                std::string target = tokens[tokens.size()-1].value;
                                target.pop_back();
                                target.erase(target.begin());
                                std::vector<Token> array = tokenize((char*)target.c_str(),scope,line,0);
                                array = stuff::split_tokens(array,scope,0,line);
                                if(stoi(value) >= array.size()){display_error(scope,"Index out of range",line);}
                                Token final_result = array[stoi(value)];
                                tokens[tokens.size()-1] = (Token){final_result.value,final_result.type};
                                break;
                            }
                        }
                        tokens.push_back((Token){"["+final_result+"]",ARRAY});
                        break;
                    }
                }
                case '+':case '-':case '*':case '/':case '%':case '^':{
                    std::string result = "";result += c;
                    c = *text++;
                    if(c == '='){
                        result += c;c = *text++;
                        if(parser_type == NONE){parser_type = VARIABLE_DECLARATION;}
                        tokens.push_back((Token){result,OPERATOR});
                        break;
                    }
                    tokens.push_back((Token){result,ACTION});
                    break;
                }
                case '<':case '>':case '!':{
                    std::string result = "";result += c;
                    c = *text++;
                    if(c == '='){result += c;c = *text++;}
                    tokens.push_back((Token){result,COMPARATOR});
                    break;
                }
                case '&':case '|':{
                    std::string result = "";result += c;
                    c = *text++;
                    if(result[0] == c){result += c;c = *text++;}
                    tokens.push_back((Token){result,COMPARATOR});
                    break;
                }
                case '=':{
                    std::string result = "";result += c;
                    c = *text++;
                    if(c == '='){
                        result += c;c = *text++;
                        tokens.push_back((Token){result,COMPARATOR});
                        break;
                    }
                    if(parser_type == NONE){parser_type = VARIABLE_DECLARATION;}
                    tokens.push_back((Token){result,OPERATOR});
                    break;
                }
                case ',':{
                    std::string result = "";result += c;
                    c = *text++;
                    tokens.push_back((Token){result,PUNCTUATION});
                    break;
                }
                case '.':{
                    c = *text++;
                    tokens.push_back((Token){".",TARGET});
                    break;
                }
                case ':':{
                    c = *text++;
                    if(c !=':'){display_error(scope,"Invalid string function, must have \"::\" not \":\"",line);}
                    tokens.push_back((Token){"::",STRING_FUNCTION});
                    break;
                }
                case '#':{
                    c = *text++;
                    if(tokens.size()>0){
                        enum types type = tokens[tokens.size()-1].type;
                        if(type == STRING){
                            std::string value = tokens[tokens.size()-1].value;
                            tokens[tokens.size()-1] = (Token){std::to_string(value.size()-2),INTEGER};
                            break;
                        }
                        if(type == ARRAY){
                            std::string value = tokens[tokens.size()-1].value;
                            value.pop_back();value.erase(value.begin());
                            std::vector<Token> array = tokenize((char*)value.c_str(),scope,line,0);
                            int size = stuff::split_tokens(array,scope,0,line).size();
                            tokens[tokens.size()-1] = (Token){std::to_string(size),INTEGER};
                            break;
                        }
                        if(type == INTEGER){
                            tokens[tokens.size()-1] = (Token){std::to_string(tokens[tokens.size()-1].value.size()),INTEGER};
                            break;
                        }
                        else{display_error(scope,"Cannot get size of "+get_token_type(tokens[tokens.size()-1].type),line);}
                    }
                    break;
         
                }
                case '?':{
                    if(tokens.size() >0){tokens[tokens.size()-1] = (Token){"\""+get_token_type(tokens[tokens.size()-1].type)+"\"",STRING};}
                    c = *text++;
                    break;
                }
                case '@':{
                    c = *text++;
                    tokens.push_back((Token){"",MODULE});
                    break;
                }
                default:{
                    if(tools::is_letter(c) || c == '_'){
                        std::string result = "";result += c;
                        c = *text++;
                        for(int i=1;;i++){
                            if(c == '\0'){break;}
                            if(!tools::is_letter(c) && !tools::is_number(c) && c!='_' ){result[i]='\0';break;}
                            result += c;
                            c = *text++;
                        }
                        if(tokens.size() > 1 && tokens[tokens.size()-1].type == TARGET && tokens[tokens.size()-2].type == OBJECT){
                            Token final_result = obj::get_value(result,tokens[tokens.size()-2].value,scope,line);
                            tokens.pop_back();
                            tokens[tokens.size()-1] = final_result;
                            break;
                        }
                        if(tokens.size() > 0){
                            if(tokens[tokens.size()-1].type == TARGET){
                                tokens[tokens.size()-1].value += result;
                                tokens[tokens.size()-1].type = TARGET;
                                break;
                            }
                            if(tokens[tokens.size()-1].type == MODULE){
                                tokens[tokens.size()-1].value = result;
                                tokens[tokens.size()-1].type = MODULE;
                                break;
                            }
                            if(tokens[tokens.size()-1].type == STRING_FUNCTION){
                                tokens[tokens.size()-1].value = result;
                                tokens[tokens.size()-1].type = STRING_FUNCTION;
                                break;
                            }
                        }
                        if(std::find(std::begin(keywords), std::end(keywords), result) != std::end(keywords)){
                            if(result == "if"){tokens.push_back((Token){result,KEYWORD});parser_type = IF;}
                            else if(result == "elif"){tokens.push_back((Token){result,KEYWORD});parser_type = IF;}
                            else if(result == "use"){parser_type = USE;}
                            else if(result == "fn"){parser_type = FUNCTION_DECLARATION;}
                            else if(result == "obj"){parser_type = OBJECT_DECLARATION;}
                            else if(result == "do"){parser_type = LOOP;}
                            else if(result == "pub"){current_scope = "PUBLIC";}
                            else if(result == "return"){parser_type = RETURN;}
                            else if(result == "stop"){parser_type = STOP;}
                            else if(result == "skip"){parser_type = SKIP;}
                            else{tokens.push_back((Token){result,KEYWORD});}
                            break;
                        }
                        //variable
                        int found = var::in_list(result,scope);
                        if(found > -1){
                            if(parser_type != NONE || variable_check == 0 || (variable_check == 1 && tokens.size() > 2) || (tokens.size() > 0 && tokens[tokens.size()-1].value == "!")){
                                tokens.push_back((Token){variables[found].value,variables[found].type});
                                break;    
                            }
                            tokens.push_back((Token){std::to_string(found),INTEGER});
                            break;
                        }
                        //function
                        found = func::in_list(result,scope);
                        if(found > -1){
                            tokens.push_back((Token){std::to_string(found),INTEGER});
                            break;
                        }
                        //object
                        found = obj::in_list(result,scope);
                        if(found > -1){
                            tokens.push_back((Token){std::to_string(found),INTEGER});
                            break;
                        }
                        tokens.push_back((Token){result,CUSTOM});
                        break; 
                    }                
                }
            }
        }
    }
}

