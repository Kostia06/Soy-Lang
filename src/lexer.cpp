#include <iostream>
#include <fstream>
#include <string> 
#include <vector>
#include <cctype>
#include <regex>
#include "../include/lexer.hpp"
#include "../include/parser.hpp"
#include "../include/utils.hpp"
#include "../include/function.hpp"
#include "../include/variable.hpp"
#include "../include/statement.hpp"
#include "../include/module.hpp"
using namespace std;

vector<File> file_list;
string keywords[] = {"out","as","if","else","input","fn","obj","use","do","pub","return","stop","skip"};

bool is_end(int c){return c == '\n' || c == '\r';}
bool is_letter(int c){return (c >= 'A' && c <= 'Z') || (c >= 'a' && c <= 'z') || c == '_';}
bool is_quote(int c){return c == '\'' || c == '\"' || c=='`';}
bool is_number(int c){return c >= '0' && c <= '9';}
bool is_space(int c){return c == ' '|| c == '\t' || c == '\f' || c == '\v';}
bool is_box(int c){return  c == '{'|| c == '[' || c == '~';}
bool is_sign(int c){
    switch(c){
        case '+':case '-':case '*':case '/':case '^':case '%':
        case '<':case '>':case '=':case '!':case '|':case '&':
        case '#': case '?':
        case '.':case ',': case ';':
            return true;
        default:
            return false;
    }
}

string sign_type(string result){
    const string actions[] = {"+", "-", "*", "/", "%","^"};
    const string statements[] = {"==", "!=", "<=", ">=", ">", "<", "&&", "||", "!"};
    const string ids = "#?";
    const string punctuations = ".,";
    if(find(begin(actions), end(actions),result)!=end(actions)){return "ACTION";}
    if(find(begin(statements), end(statements),result)!=end(statements)){return "STATEMENT";}
    if(result.length()>1){return "!~!NONE";}
    if(ids.find(result)!= std::string::npos){return "ID";}
    if(result == "="){return "EQUAL";}
    if(result == ";"){return "NEXT";}
    if(punctuations.find(result)!= std::string::npos){return "PUNCTUATION";}
    return "!~!NONE!~!";
}

void get_files(string directory){
    File current_file;
    current_file.name = directory;
    ifstream read_file(directory);
    string file,current_line;
    while (getline(read_file, current_line)){
        if(current_line[0] == 'u' && current_line[1] == 's' && current_line[2] == 'e'){
            file += "\n";
            import_file(tokenizer(current_line, directory,1)[0].tokens,directory);
        }else{file += current_line + "\n";}
    }
    current_file.file = file;
    read_file.close();
    file_list.push_back(current_file);
}
void lexer(){
    for(int i=0;i<file_list.size();i++){
        string directory = file_list[i].name;
        parser(tokenizer(file_list[i].file, directory, 1),directory,directory);
    }
}
string get_import_position(string directory){
	vector<string> result;
	vector<string> list = split(directory,"/");
    for(int i=0;i<list.size()-1;i++){
        result.push_back(list[i]);
    }
	return  join(result,"/");
}
void import_file(vector<Token> tokens,string directory){
    vector<Token> new_tokens;
    string starting_pos = get_import_position(directory);
    for(int i=0;i<tokens.size();i++){if(tokens[i].type == "STRING"){new_tokens.push_back(tokens[i]);}}

    for(int i=0;i<new_tokens.size();i++){
        get_files(starting_pos+get_token_value(new_tokens[i]));
    }
}
string read_and_execute(string file, string scope){
    file.erase(file.begin());
    file.pop_back();
    string pop = parser(tokenizer(file, scope, 1),scope,scope);
    return pop;
}

Token token_init(string value, string type){
    Token token;token.value =value;token.type = type;
    return token;
}
string tokenizer_action(vector<Token> tokens, string scope, string type){
    string current_scope = scope;
    if(tokens[0].value == "pub" && tokens[0].type == "KEYWORD"){
        current_scope = "PUBLIC";
        tokens.erase(tokens.begin());
    }
    if(type == "FUNCTION_DECLARATION"){function_init(tokens,current_scope);}
    else if(type == "VARIABLE_DECLARATION"){variable_init(tokens,current_scope,scope,scope);}
    else if(type == "BIND"){bind_init(tokens,scope);}
    else if(type == "OUT"){output(tokens,scope);}
     else if(type == "OBJECT_DECLARATION"){object_init(tokens,current_scope);}
    else if(type == "IF"){return statement(tokens,scope);}
    else if(type == "ELSE"){return statement(tokens,scope);}
    else if(type == "LOOP"){return loop(tokens,scope);}
    else if(type == "RETURN"){return get_value(tokens,scope,0);}
    else if(type == "SKIP"){return "!~!SKIP!~!";}
    else if(type == "STOP"){return "!~!STOP!~!";}
    return "!~!NONE!~!";
}
vector<Component> tokenizer(string text, string scope,int variable_check){
    int size = text.size(), count = 0,argument_count = 0;
    vector<Component> component_list;
    vector<Token> argument_tokens;
    string argument_type ="";
    Component component;
    component.type = "";
    vector<Token> tokens;
    for(int _=0;_<size;_++){
        //if index is at the end of the file then end
        if(count == size){break;}
        char current_char = text.at(count);
        //new line
        if(is_end(current_char)){
            if(tokens.size() > 0 && argument_count == 0){
                string type = component.type;
                if(type != ""){
                    string return_result = tokenizer_action(tokens,scope,component.type);
                    if(return_result != "!~!NONE!~!"){
                        if(return_result == "!~!SKIP!~!"){component.type = "SKIP";}
                        else if(return_result == "!~!STOP!~!"){component.type = "STOP";}
                        else{component.type = "RETURN";}
                        tokens.clear();
                        tokens.push_back(token_init(return_result,get_type(return_result)));
                        component.tokens = tokens;
                        component_list.push_back(component);
                        component.type = "";
                        tokens.clear();
                        break;
                    }
                    component.type = "";
                    tokens.clear();
                }
                else{
                    component.tokens = tokens;
                    tokens.clear();
                    component_list.push_back(component);
                    component.type = "";
                }
            }
            count++;
            continue;
        }
        //check for white space
        if(is_space(current_char)){count++;continue;}
        //check for customs, variables, keywords, etc...
        if(current_char == ':' || current_char == '@' || is_letter(current_char)){
            string result;result+=current_char;
            count++;
            bool found_variable = false;
            for(int _=0;count<size;){
                if(result.size() == 1 && current_char == ':' && text.at(count)==':'){
                    result += ':';count++;
                    continue;
                }
                if(!is_letter(text.at(count)) && !is_number(text.at(count))){break;}
                result += text.at(count);
                count++;
            }
            //keywords
            if(find(begin(keywords), end(keywords),result)!=end(keywords)){
                tokens.push_back(token_init(result,"KEYWORD"));
                if(component.type == ""){
                    if(result == "fn"){component.type = "FUNCTION_DECLARATION";}
                    else if(result == "obj"){component.type = "OBJECT_DECLARATION";}
                    else if(result == "else"){component.type = "ELSE";}
                    else if(result == "if"){component.type = "IF";}
                    else if(result == "do"){component.type = "LOOP";}
                    else if(result == "return"){component.type = "RETURN";}
                    else if(result == "stop"){component.type = "STOP";}
                    else if(result == "skip"){component.type = "SKIP";}
                    else if(result == "out"){component.type = "OUT";}
                }
                continue;
            }
            //string function
            else if(result[0] == ':'){
                if(tokens.size() > 0 && tokens[tokens.size()-1].type == "OBJECT"){
                    string value = access_object(tokens[tokens.size()-1], token_init(result,"STRING_FUNCTION"),scope);
                    tokens[tokens.size()-1] = token_init(value,get_type(value));
                    continue;
                }else{
                    tokens.push_back(token_init(result,"STRING_FUNCTION"));
                    continue;
                }
            }
            //import functions
            else if(result[0] == '@'){
                tokens.push_back(token_init(result,"MAIN"));
                continue;
            }
            else if(in_function_list(result,scope)){
                tokens.push_back(token_init(result,"FUNCTION"));
                continue;
            }
            else if(in_object_list(result,scope)){
                tokens.push_back(token_init(result,"OBJECT"));
                continue;
            }
            for(int i=0;i<variable_list.size();i++){
                if(variable_list[i].name == result && check_scope(scope, variable_list[i].scope)){
                    found_variable = true;
                    if(variable_check == 0 || (variable_check == 1 && (tokens.size() > 2 || (tokens.size() != 0 &&tokens[0].value != "pub") ))){
                        tokens.push_back(token_init(variable_list[i].value,variable_list[i].type));
                        break;
                    }
                    else{
                        tokens.push_back(token_init(result,"VARIABLE"));
                        break;
                    }
                }
            }
            if(found_variable){continue;}
            tokens.push_back(token_init(result,"CUSTOM"));
            continue;    
        }
        //arguments
        if(current_char == '(' && component.type == "FUNCTION_DECLARATION"){
            string result = "(";
            int quote_count = 1;
            char quote = ')';
            count ++;
            for(int _=0;count <size;){
                if(quote_count == 0){break;}
                if(text.at(count) == current_char){quote_count ++;}
                else if(text.at(count) == quote){quote_count --;}
                result += text.at(count);count ++;
            } 
            tokens.push_back(token_init(result,"ARGUMENT"));
            continue;
        }   
        if(current_char == '('){
            if(argument_tokens.size()==0 && argument_type == "" && argument_count==0){
                argument_tokens=tokens;
                argument_type = component.type;
                component.type = "";
                tokens.clear();
            }
            argument_count ++;
            tokens.push_back(token_init("(","ARGUMENT"));
            count++;
            continue;
        }
        if(current_char == ')'){
            argument_count --;
            tokens.push_back(token_init(")","ARGUMENT"));
            count++;
            if(argument_count != 0){
                string result;
                int work_count = argument_count+1;
                int num = 0;
                for(int i=0;i<tokens.size();i++){
                    if(tokens[i].type=="ARGUMENT"){work_count--;}
                    if(work_count == 0){result+=tokens[i].value;num++;}
                }
                for(int i=0;i<=num;i++){tokens.pop_back();}
                result += ")";
                if(tokens.size()>0 && tokens[tokens.size()-1].type == "MAIN"){
                    string value = check_module(tokens[tokens.size()-1],result,scope);
                    tokens[tokens.size()-1] = token_init(value,get_type(value));
                    continue;
                }
                else if(tokens.size()>0 && tokens[tokens.size()-1].type == "FUNCTION"){
                    vector<Token> new_tokens;
                    new_tokens.push_back(tokens[tokens.size()-1]);
                    new_tokens.push_back(token_init(result,"ARGUMENT"));
                    string value = call_function(new_tokens,scope);
                    tokens[tokens.size()-1] = token_init(value, get_type(value));
                    continue;
                }
                else if(tokens.size() > 1 && tokens[tokens.size()-1].type == "STRING_FUNCTION" && tokens[tokens.size()-1].value == "::find"){
                    string current_type = tokens[tokens.size()-2].type;
                    vector<string> target = split(split_tokens(result, scope)[0][0].value,"");
                    target.pop_back();target.pop_back();
                    target.erase(target.begin());
                    string find_item = join(target,"");
                    if(current_type == "STRING"){
                        vector<string> value = split(tokens[tokens.size()-2].value,"");
                        value.pop_back();value.pop_back();
                        value.erase(value.begin());
                        string new_value = join(value,"");
                        new_value = to_string(split(new_value, find_item).size()-1);
                        tokens.pop_back();
                        tokens[tokens.size()-1] = token_init(new_value,"NUMBER");
                        continue;
                    }
                    if(current_type == "NUMBER"){
                        string value = tokens[tokens.size()-2].value;
                        value = to_string(split(value, find_item).size()-1);

                        tokens.pop_back();
                        tokens[tokens.size()-1] = token_init(value,"NUMBER");
                        continue;
                    }
                    if(current_type == "ARRAY"){
                        vector<vector<Token> > values = split_tokens(tokens[tokens.size()-2].value, scope);
                        string list_value = "";
                        for(int i = 0; i < values.size(); i++){
                            list_value += get_value(values[i],scope,-1);
                        }
                        string value = to_string(split(list_value, find_item).size()-1);

                        tokens.pop_back();
                        tokens[tokens.size()-1] = token_init(value,"NUMBER");;
                        continue;
                    }
                }
                else if(tokens.size() > 1 && tokens[tokens.size()-1].type == "STRING_FUNCTION" && tokens[tokens.size()-1].value == "::add"){
                    string current_type = tokens[tokens.size()-2].type;
                    string target = get_value(split_tokens(result, scope)[0],scope,-1);
                    if(current_type == "ARRAY"){
                        vector<vector<Token> > values = split_tokens(tokens[tokens.size()-2].value, scope);
                        string list_value = "[";
                        for(int i = 0; i < values.size(); i++){
                            list_value += get_value(values[i],scope,-1) + ",";
                        }
                        list_value += target;
                        list_value += "]";
                        string value = list_value;
                        tokens.pop_back();
                        tokens[tokens.size()-1] = token_init(value,get_type(value));
                        continue;
                    }
                }
                else if(tokens.size() > 1 && tokens[tokens.size()-1].type == "STRING_FUNCTION" && tokens[tokens.size()-1].value == "::remove"){
                    string current_type = tokens[tokens.size()-2].type;
                    int target = stoi(split_tokens(result, scope)[0][0].value);
                    if(current_type == "ARRAY"){
                        vector<vector<Token> > values = split_tokens(tokens[tokens.size()-2].value, scope);
                        string list_value = "[";
                        for(int i = 0; i < values.size(); i++){
                            if(target != i){
                                list_value += get_value(values[i],scope,-1) + ",";
                            }
                        }
                        list_value[list_value.size()-1] = ']';
                        string value = list_value;
                        tokens.pop_back();
                        tokens[tokens.size()-1] = token_init(value,get_type(value));
                        continue;
                    }
                }
                else if(tokens.size() > 0 &&tokens[tokens.size()-1].type == "KEYWORD" &&tokens[tokens.size()-1].value == "input"){
                    string value = input(token_init(result,"ARGUMENT"),scope);
                    tokens[tokens.size()-1] = token_init(value,get_type(value));
                    continue;
                }
                tokens.push_back(token_init(result,"ARGUMENT"));
                continue;
            }
            if(tokens.size() !=0 && argument_count == 0){
                string result;
                for(int i =0;i<tokens.size();i++){
                    result += tokens[i].value;
                }
                tokens.clear();
                tokens = argument_tokens;argument_tokens.clear();
                component.type = argument_type;argument_type = "";
                if(tokens.size()>0 && tokens[tokens.size()-1].type == "MAIN"){
                    string value = check_module(tokens[tokens.size()-1],result,scope);
                    tokens[tokens.size()-1] = token_init(value,get_type(value));
                    continue;
                }
                else if(tokens.size()>0 && tokens[tokens.size()-1].type == "FUNCTION"){
                    vector<Token> new_tokens;
                    new_tokens.push_back(tokens[tokens.size()-1]);
                    new_tokens.push_back(token_init(result,"ARGUMENT"));
                    string value = call_function(new_tokens,scope);
                    tokens[tokens.size()-1] = token_init(value, get_type(value));
                    continue;
                }
                else if(tokens.size()>1 && tokens[tokens.size()-1].type == "STRING_FUNCTION" && tokens[tokens.size()-1].value == "::at"){
                    string type;
                    int variable_index;
                    string from = tokens[tokens.size()-2].value ;
                    if(tokens[tokens.size()-2].type == "VARIABLE"){
                        for(int i=0;i<variable_list.size();i++){
                            if(variable_list[i].name == from && check_scope(scope, variable_list[i].scope)){   
                                type = variable_list[i].type;
                                from =variable_list[i].value;
                                variable_index = i;
                                break;
                            }
                        }
                    }
                    else{type = tokens[tokens.size()-2].type;}
                    vector<Token> index_list;
                    vector<vector<Token> > values = split_tokens(result,scope);
                    string replace_value = from;
                    for(int i=0;i<values.size();i++){
                        string value = get_value(values[i],scope,-1);
                        index_list.push_back(token_init(value,get_type(value)));
                    }
                    string value = index_list[index_list.size()-1].value;index_list.pop_back(); 
                    if(type == "ARRAY"){
                        Token current_value;
                        for(int j=0;j<=index_list.size();j++){
                            for(int i=0;i<index_list.size()-j;i++){
                                
                                if(index_list[i].type != "NUMBER"){
                                    tokens.push_back(token_init(result,"ARGUMENT"));
                                    display_error(tokens,scope,"Only numbers are allowed");
                                }
                                if(i==0){current_value = change_index(token_init(replace_value,"ARRAY"),index_list[0],scope);}
                                else{current_value = change_index(current_value,index_list[i],scope);}
                            }
                            if(index_list.size()-j==0){current_value.value = from;current_value.type = get_type(from);}
                            if(current_value.type == "ARRAY"){
                                vector<vector<Token> > new_tokens = split_tokens(current_value.value,scope);

                                if(j == 0){result = index_list[index_list.size()-j].value;continue;}
                                int current_index = stoi(index_list[index_list.size()-j].value);
                                // cout << current_value.value <<"\t"<<current_index<< "\t"<< value<<endl;
                                new_tokens[current_index].clear();new_tokens[current_index].push_back(token_init(value,get_type(value)));
                                string result = "[";
                                for(int i =0;i<new_tokens.size();i++){
                                    result += get_value(new_tokens[i],scope,-1) + ",";
                                }
                                result[result.size()-1] = ']';
                                value = result;
                                
                            }
                        } 
                        variable_list[variable_index].value = value;
                        variable_list[variable_index].type = get_type(value);
                        continue;
                    }
                    if(type == "OBJECT"){
                        string change_value = index_list[0].value;
                        if(get_type(change_value)!="STRING"){
                            tokens.push_back(token_init(result,"ARGUMENT"));
                            display_error(tokens,scope,"Only string-functions are allowed");
                        }
                        string result = change_object(token_init(from,type),change_value,value,scope);
                        variable_list[variable_index].value = result;
                        variable_list[variable_index].type = get_type(result);
                        continue;
                    }
                    else{display_error(tokens,scope,"String-function \"::at\" doesn't allow \""+type+"\" type" );}
                }
                else if(tokens.size() > 1 && tokens[tokens.size()-1].type == "STRING_FUNCTION" && tokens[tokens.size()-1].value == "::find"){
                    string current_type = tokens[tokens.size()-2].type;
                    vector<string> target = split(split_tokens(result, scope)[0][0].value,"");
                    target.pop_back();target.pop_back();
                    target.erase(target.begin());
                    string find_item = join(target,"");
                    if(current_type == "STRING"){
                        vector<string> value = split(tokens[tokens.size()-2].value,"");
                        value.pop_back();value.pop_back();
                        value.erase(value.begin());
                        string new_value = join(value,"");
                        new_value = to_string(split(new_value, find_item).size()-1);
                        tokens.pop_back();
                        tokens[tokens.size()-1] = token_init(new_value,"NUMBER");
                        continue;
                    }
                    if(current_type == "NUMBER"){
                        string value = tokens[tokens.size()-2].value;
                        value = to_string(split(value, find_item).size()-1);

                        tokens.pop_back();
                        tokens[tokens.size()-1] = token_init(value,"NUMBER");
                        continue;
                    }
                    if(current_type == "ARRAY"){
                        vector<vector<Token> > values = split_tokens(tokens[tokens.size()-2].value, scope);
                        string list_value = "";
                        for(int i = 0; i < values.size(); i++){
                            list_value += get_value(values[i],scope,-1);
                        }
                        string value = to_string(split(list_value, find_item).size()-1);

                        tokens.pop_back();
                        tokens[tokens.size()-1] = token_init(value,"NUMBER");;
                        continue;
                    }
                }
                else if(tokens.size() > 1 && tokens[tokens.size()-1].type == "STRING_FUNCTION" && tokens[tokens.size()-1].value == "::add"){
                    string current_type = tokens[tokens.size()-2].type;
                    string target = get_value(split_tokens(result, scope)[0],scope,-1);
                    if(current_type == "ARRAY"){
                        vector<vector<Token> > values = split_tokens(tokens[tokens.size()-2].value, scope);
                        string list_value = "[";
                        for(int i = 0; i < values.size(); i++){
                            list_value += get_value(values[i],scope,-1) + ",";
                        }
                        list_value += target;
                        list_value += "]";
                        string value = list_value;
                        tokens.pop_back();
                        tokens[tokens.size()-1] = token_init(value,get_type(value));
                        continue;
                    }
                }
                else if(tokens.size() > 1 && tokens[tokens.size()-1].type == "STRING_FUNCTION" && tokens[tokens.size()-1].value == "::remove"){
                    string current_type = tokens[tokens.size()-2].type;
                    int target = stoi(split_tokens(result, scope)[0][0].value);
                    if(current_type == "ARRAY"){
                        vector<vector<Token> > values = split_tokens(tokens[tokens.size()-2].value, scope);
                        string list_value = "[";
                        for(int i = 0; i < values.size(); i++){
                            if(target != i){
                                list_value += get_value(values[i],scope,-1) + ",";
                            }
                        }
                        list_value[list_value.size()-1] = ']';
                        string value = list_value;
                        tokens.pop_back();
                        tokens[tokens.size()-1] = token_init(value,get_type(value));
                        continue;
                    }
                }
                else if(tokens.size() > 0 &&tokens[tokens.size()-1].type == "KEYWORD" &&tokens[tokens.size()-1].value == "input"){
                    string value = input(token_init(result,"ARGUMENT"),scope);
                    tokens[tokens.size()-1] = token_init(value,get_type(value));
                    continue;
                }
                tokens.push_back(token_init(result,"ARGUMENT"));
            }
            continue;
        }
        //checks for numbers
        if(is_number(current_char)){
            string result;
            for(int _=0;count <size;){
                if(!is_number(text.at(count)) && text.at(count) != '.' && text.at(count) != '_'){break;}
                result += text.at(count);
                count++;
            }
            result  = regex_replace(result,regex("_"),"");
            tokens.push_back(token_init(result,"NUMBER"));
            continue;
            
        }
        //check for signs
        if(is_sign(current_char)){
            string result;
            for(int _=0;count <size;){
                string check = result + text.at(count);
                if(!is_sign(text.at(count)) || result.size() == 2 || sign_type(check)=="!~!NONE"){break;}
                if(result.size()==2){break;}
                result += text.at(count);
                count++;
            }
            string type = sign_type(result);
            if(type == "!~!NONE"){display_error(tokens,scope,"Unknown symbol \"" + result + "\"");}
            if(tokens.size()>0){
                if(result == "#"){
                    Token token = tokens[tokens.size()-1];
                    if(token.type == "ARRAY"){
                        vector<vector<Token> > values = split_tokens(token.value,scope);
                        tokens[tokens.size()-1] = token_init(to_string(values.size()),"NUMBER");
                        continue;
                    }
                    if(token.type == "STRING"){
                        tokens[tokens.size()-1] = token_init(to_string(token.value.size()-2),"NUMBER");
                        continue;
                    }
                    if(token.type == "NUMBER"){
                        tokens[tokens.size()-1] = token_init(to_string(token.value.size()),"NUMBER");
                        continue;
                    }
                }
                if(result == "?"){
                    tokens[tokens.size()-1] = token_init(tokens[tokens.size()-1].value, "STRING");
                }
            }
            if(result == ";"){
                if(tokens.size() > 0){
                    string type = component.type;
                    string return_result = tokenizer_action(tokens,scope,component.type);
                    if(type == ""){
                        if(return_result != "!~!NONE!~!"){
                            if(return_result == "!~!SKIP!~!"){component.type = "SKIP";}
                            else if(return_result == "!~!STOP!~!"){component.type = "STOP";}
                            else{component.type = "RETURN";}
                            tokens.clear();
                            tokens.push_back(token_init(return_result,get_type(return_result)));
                            component.tokens = tokens;
                            component_list.push_back(component);
                            component.type = "";
                            tokens.clear();
                            break;
                        }
                        component.type = "";
                        tokens.clear();
                    }else{
                        component.tokens = tokens;
                        tokens.clear();
                        component_list.push_back(component);
                        component.type = "";
                        continue;
                    }
                }   
            }
            if(type == "EQUAL" || type == "OPERATOR"){component.type = "VARIABLE_DECLARATION";}
            tokens.push_back(token_init(result,type));
            continue;
        }
        //string
        if(is_quote(current_char)){
            char quote = current_char;
            string result = "";result += quote;
            count ++;
            for(int _=0;count<size;){
                if(text.at(count) == quote){break;}
                if(is_end(text.at(count))){count++;}
                else{
                    result += text.at(count);
                    count ++;
                }
            }
            result += quote;
            count ++;
            tokens.push_back(token_init(result,"STRING"));
            continue;
        }
        if(is_box(current_char)){
            char quote;
            int quote_count = 1;
            if(current_char == '['){ quote= ']';}
            else if(current_char == '{'){quote = '}';}
            else if(current_char == '~'){quote = '~';}
            string result; result.push_back(current_char);
            count ++;
            if(quote == '}' &&( component.type == "LOOP" || component.type=="IF" || component.type=="ELSE" || component.type =="FUNCTION_DECLARATION")){
                for(int _=0;count <size;){
                    if(quote_count == 0){break;}
                    if(text.at(count) == current_char){quote_count ++;}
                    else if(text.at(count) == quote){quote_count --;}
                    result += text.at(count);count ++;
                } 
            }else{
                for(int _=0;count <size;){
                    if(quote_count == 0){break;}
                    if(text.at(count) == current_char){quote_count ++;}
                    else if(text.at(count) == quote){quote_count --;}
                    if(is_end(text.at(count))){count++;}
                    else{result += text.at(count);count ++;}
                }   
            }
            //array
            if(quote == '~'){continue;}
            else if(quote == ']'){
                string types[] = {"STRING","NUMBER","ARRAY"};
                Token index = token_init(result,"ARRAY");
                if(tokens.size()>0&&find_item_in_array(types,tokens[tokens.size()-1].type)){
                    string value = indexing(tokens[tokens.size()-1], index,scope);
                    tokens[tokens.size()-1] = token_init(value,get_type(value));
                    continue;
                }else{
                    tokens.push_back(index);
                    continue;
                }
            }
            //bind
            else if(quote == '}'){
                if(component.type == "" && tokens.size()==0){component.type = "BIND";}
                tokens.push_back(token_init(result,"BIND"));

            }
        } 
    }
    if(tokens.size() > 0){
        string type = component.type;
        string return_result = tokenizer_action(tokens,scope,component.type);
        if(return_result != "!~!NONE!~!"){
            if(return_result == "!~!SKIP!~!"){component.type = "SKIP";}
            else if(return_result == "!~!STOP!~!"){component.type = "STOP";}
            else{component.type = "RETURN";}
            tokens.clear();
            tokens.push_back(token_init(return_result,get_type(return_result)));
            component.tokens = tokens;
            component_list.push_back(component);
        }else{
            component.tokens = tokens;
            tokens.clear();
            component_list.push_back(component);
            component.type = "";

        }
    }
    return component_list;
}

Token change_index(Token item, Token index,string scope){
    if(item.type == "ARRAY"){
        vector<vector<Token> >values = split_tokens(item.value,scope);
        if(index.type == "NUMBER"){
            vector<Token> new_tokens = values[stoi(index.value)];
            string value = get_value(new_tokens,scope,-1);
            return token_init(value,get_type(value));
        }
    }
    string result = access_object(item,index,scope);
    return token_init(result,get_type(result));
}