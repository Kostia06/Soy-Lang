#include <iostream>
#include <fstream>
#include <stdio.h>
#include <string>
#include <vector>
#include <regex>
#include "../include/lexer.hpp"
#include "../include/utils.hpp"
using namespace std;




vector<Variable> variable_list;
vector<Function> function_list;
vector<Bind> bind_list;
vector<Object> object_list;
string version = "1.0.0";

int main(int argc, char *argv[]){
    int start = clock();
    if(argc < 2){cout << "Not enough arguments";return 1;}
    if(strcmp(argv[1],"run")==0 || strcmp(argv[1],"drun")==0){
        get_files(argv[2]);
        lexer();
    }
    if(strcmp(argv[1],"help")==0){
        cout << "\033[1m\033[37msoy-lang" << endl;
        cout << "\t v \t tells the current version of the soy-lang"  <<endl;
        cout << "\t run + directory \t runs your code"  <<endl;
        cout << "\t drun + directory \t runs your code and tells every variable,function and objects that were declared"  <<endl;
    }
    if(strcmp(argv[1],"v")==0){cout <<"\033[1m\033[37mSoy-lang version "+ version << endl;}
    if(strcmp(argv[1],"drun")==0){
        cout << "\n\033[1m\033[37mExecuted in \033[1m\033[32m" << ((float)clock()-start)/CLOCKS_PER_SEC << "\033[1m\033[37m seconds\n"<<endl;

        cout << "\033[1m\033[32mVariable\033[1m\033[37m"<<endl;
        for(int i = 0; i < variable_list.size(); i++){
            Variable item = variable_list[i];
            cout << item.name << "\t" << regex_replace(item.value,regex("\n"),"\\n") << "\t" << item.type << "\t" <<item.scope << endl;
        }
        cout << "\033[1m\033[32mFunction\033[1m\033[37m"<<endl;
        for(int i = 0; i < function_list.size(); i++){
            Function item = function_list[i];
            cout << item.name << "\t" << item.arguments << "\t"<< item.scope<< "\t" <<  endl;
        }
        cout << "\033[1m\033[32mObject\033[1m\033[37m"<<endl;
        for(int i = 0; i < object_list.size(); i++){
            Object item = object_list[i];
            cout << item.name << "\t" << item.arguments << "\t"<< item.scope <<  endl;
        }
    }
    
    return 0;
}
