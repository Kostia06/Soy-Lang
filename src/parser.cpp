#include <string> 
#include <iostream>
#include <fstream>
#include <vector>
#include "../include/parser.hpp"
#include "../include/variable.hpp"
#include "../include/function.hpp"
using namespace std;

string parser(vector<Component> component_list, string scope, string directory){
    for(int i = 0; i < component_list.size();i++){
        vector<Token> tokens = component_list[i].tokens;
        string type = component_list[i].type;
        if(type == "SKIP"){return "!~!SKIP!~!";}
        if(type == "STOP"){return "!~!STOP!~!";}
        if(type == "RETURN"){return tokens[0].value;}
    }
    return "!~!NONE!~!";
}