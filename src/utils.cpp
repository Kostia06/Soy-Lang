#include <string> 
#include <iostream>
#include <vector>
#include "../include/utils.hpp"
#include "../include/lexer.hpp"
#include "../include/math.hpp"
using namespace std;


vector<string> split (string text, string target) {
    size_t pos_start = 0, pos_end, delim_len = target.length();
    string token;
    vector<string> res;
    if(target == ""){
        for(int i=0;i<text.size();i++){
            string answer = "";answer += text[i];
            res.push_back(answer);
        }  
    }else{
        while ((pos_end = text.find(target, pos_start)) != string::npos) {
            token = text.substr(pos_start, pos_end - pos_start);
            pos_start = pos_end + delim_len;
            res.push_back(token);
        }
    }

    res.push_back(text.substr (pos_start));
    return res;
}
string join(vector<string> list, string target){
    if (list.empty()) return "";

    string str;
    for (int i=0; i<list.size();i++){
        str += list[i] + target;
    } 
    str = str.substr(0, str.size() - target.size());
    return str;
}
string replace_chars(string text, string target, string to){
    string result = "";
    for(int i = 0; i < text.length(); i++){
        if(text[i] == target[0]){result += to;}
        else{result +=text[i];}
    }
    return result;
}

bool in_function_list(string name,string scope){
    for(int i=0;i<function_list.size();i++){
        if(function_list[i].name == name && check_scope(scope, function_list[i].scope)){return true;}
    }
    return false;
}
bool in_object_list(string name,string scope){
    for(int i=0;i<object_list.size();i++){
        if(object_list[i].name == name && check_scope(scope, object_list[i].scope)){return true;}
    }
    return false;
}

bool check_scope(string from, string to){
    vector<string> from_list = split(from, "!~!");
    vector<string> to_list = split(to, "!~!");
    if(to == "PUBLIC"){return true;}
    if(to == from){return true;}
    for(int i=0; i<to_list.size(); i++){
        if(from_list[i] != to_list[i]){return false;}
    }
    return true;
}
bool find_item_in_array(string list[], string item){
    int count = 0;
    while(list[count] != "\0"){
        if(list[count] == item){return true;}
        count ++;
    }
    return false;
}

void output(vector<Token> tokens, string scope){
    string value = "";
    vector<string> items = split(tokens[1].value,"");
    items.erase(items.begin());
    items.pop_back();
    items.pop_back();
    vector<Token> token_list = tokenizer(join(items, ""),scope,0)[0].tokens;
    vector<vector<Token> > new_tokens;
    vector<Token> current_tokens;
    //separates tokens by comma
    for(int i=0;i<token_list.size();i++){
        Token current_token = token_list[i];
        if(current_token.type == "PUNCTUATION" && current_token.value == ","){
            new_tokens.push_back(current_tokens);
            current_tokens.clear();
        }else{current_tokens.push_back(current_token);}
        if(i+1 == token_list.size()){new_tokens.push_back(current_tokens);}
    }
    //gets value of each group of tokens
    for(int i = 0; i < new_tokens.size();i++){
        value += get_value(new_tokens[i],scope,-1,true);
    }
    cout << value << endl;
}
string input(Token token, string scope){
    string value = "";;
    vector<vector<Token> > values = split_tokens(token.value,scope);
    //gets value of each group of tokens
    for(int i = 0; i < values.size();i++){
        value += get_value(values[i],scope,-1,true);
    }
    cout << value;
    string respond;cin >> respond;
    return respond;
}

string indexing(Token token, Token index_token, string scope){
    vector<string> list_index = split(index_token.value, "");
    list_index.erase(list_index.begin());
    list_index.erase(list_index.begin()+list_index.size()-1);
    int index = stoi(get_value(tokenizer(join(list_index, ""),scope,0)[0].tokens,scope,-1));
    vector<Token> error;error.push_back(token);error.push_back(index_token);
    if(index < 0){display_error(error,scope,"Out of index");}
    if(token.type == "ARRAY"){
        vector<vector<Token> > tokens = split_tokens(token.value,scope);
        if(tokens.size() <= index){display_error(error,scope,"Out of index");}
        return get_value(split_tokens(token.value,scope)[index],scope,-1);
    }
    else if(token.type == "STRING"){
        vector<string> value = split(token.value,"");
        value.pop_back();
        value.pop_back();
        value.erase(value.begin());
        if(value.size() <= index){display_error(error,scope,"Out of index");}
        return value[index];
    }
    else{
        vector<string> result = split(token.value,"");
        if(result.size() <= index){display_error(error,scope,"Out of index");}
        return result[index];
    }
    return "!~!NONE";
}

void display_error(vector<Token> tokens, string scope,string text){
    vector<string> directory = get_scope(scope);
    string line;
    for(int i=0;i<tokens.size();i++){line += tokens[i].value + " ";}
    cout << "\n\033[1m\033[32m";
    for(int i=0;i<directory.size();i++){
        cout << ">";
        for(int j =0;j<=i;j++){cout << "  ";}
        cout << directory[i] << endl;
    }
    cout << "\033[1m\033[31m"<<text << " in: \033[37m\033[4m" << line << "\n" << endl; 

    //  \033[0;32m" << directory << "\033[0;37m"<< endl;
    exit(1);
}
vector<string> get_scope(string scope){
    vector<string> result;
    vector<string> list = split(scope,"/");
    for(int i=0;i<list.size();i++){
        string item = list[i];
        vector<string> list2 = split(item,"!~!");
        for(int j=0;j<list2.size();j++){
            result.push_back(list2[j]);
        }
    }
    return result;
}

vector<vector<Token> > split_tokens(string value, string scope){
    vector<string> items = split(value, "");
    items.erase(items.begin());
    items.pop_back();items.pop_back();
    vector<Component> components = tokenizer(join(items, ""),scope,0);
    vector<vector<Token> > new_tokens;
    if(components.size() == 0){return new_tokens;}
    vector<Token> current_tokens;
    vector<Token> tokens = components[0].tokens;
    int count = 0;
    for(int i=0;i<tokens.size();i++){
        if(count == tokens.size()){break;}
        Token current_token = tokens[count];
        if(current_token.type == "PUNCTUATION" && current_token.value == ","){
            new_tokens.push_back(current_tokens);
            current_tokens.clear();
        }else{current_tokens.push_back(current_token);}
        if(count+1 == tokens.size()){new_tokens.push_back(current_tokens);}
        count++;
    }
    return new_tokens;
}
string get_token_value(Token token){
    if(token.type == "STRING" || token.type == "ARRAY"){
        vector<string> value = split(token.value, "");
        value.pop_back();value.pop_back();value.erase(value.begin());
        return join(value,"");
    }
    return "";
}
string get_value(vector<Token> tokens,string scope,int index,bool stringify){
    for(int i=0; i<=index;i++){tokens.erase(tokens.begin());}
    if(tokens.size() == 1){
        Token token = tokens[0];
        if(token.type == "ARRAY"){
            vector<vector<Token> > array = split_tokens(token.value,scope);
            if(array.size()==0){return "[]";}
            string result = "[";
            for(int i = 0; i < array.size(); i++){
                string value;
                if(array[i].size()==1 && array[i][0].type == "OBJECT"){value = array[i][0].value;}
                else{value = get_value(array[i],scope,-1);}
                result += value + ",";
            }
            result[result.size()-1] =']';
            return result;
        }
        else{
            if(stringify && token.type == "STRING"){
                vector<string> value = split(token.value,"");
                value.pop_back();
                value.pop_back();
                value.erase(value.begin());
                return join(value,"");
            }
            return token.value;
        }
    }
    if(tokens.size() == 2 && tokens[0].value == "-" && tokens[1].type == "NUMBER"){
        return "-"+tokens[1].value;
    }
    if(tokens.size() == 2 && tokens[0].type == "OBJECT" && tokens[1].type =="BIND"  ){
        vector<vector<Token> > values = split_tokens(tokens[1].value,scope);
        string result = "{";
        for(int i=0; i<values.size(); i++){
            result += get_value(values[i],scope,-1) + ",";
        }
        result[result.size()-1] = '}';
        return tokens[0].value + result;
    }else if(tokens.size() == 2 && tokens[0].type == "OBJECT" && tokens[1].type =="STRING_FUNCTION"  ){
        return get_value(tokens,scope,index);
    }
    string type = "NUMBER";
    for(int i = 0; i < tokens.size();i++){
        Token token = tokens[i];
        if(token.type == "STRING"){type = "STRING";break;}
    }
    if(type == "NUMBER"){
        string text = "";
        for(int i = 0; i < tokens.size(); i++){
            text += tokens[i].value;
        }
        string result = math_eval(text,scope);
        return result;
    }
    else if(type == "STRING"){
        vector<string> value = split(tokens[0].value,"");
        value.pop_back();value.pop_back();
        value.erase(value.begin());
        string result = join(value,"");
        for(int i=1; i < tokens.size(); i++){
            if(tokens[i].type == "ACTION" && tokens[i].value == "+"){
                vector<string> value = split(tokens[i+1].value,"");
                value.pop_back();value.pop_back();
                value.erase(value.begin());
                result += join(value,"");
            }else if(tokens[i].type == "ACTION" && tokens[i].value == "-"){
                result.erase(result.begin()+result.size()-stoi(tokens[i+1].value));
            }
        }
        if(stringify){
            return result;
        }
        return "\""+result+"\"";
    }
    return "!~!NONE"; 
}
string get_type(string value){
    if(is_quote(value[0]) && is_quote(value[value.size()-1])){return "STRING";}
    if(value[0] == '[' && value[value.size()-1] == ']'){return "ARRAY";}
    if(value[0] == '(' && value[value.size()-1] == ')'){return "ARGUMENT";}
    if(value[0] == '{' && value[value.size()-1] == '}'){return "BIND";}
    if(value[value.size()-1] == '}'){return "OBJECT";}
    bool number = true;
    if(value.size() > 1 && value[0] == '-'){
        value.erase(value.begin());
    }
    for(int i=0;i<value.size();i++){if(!is_number(value[i]) && value[i] != '.'){return "STRING";}}
    return "NUMBER";
}