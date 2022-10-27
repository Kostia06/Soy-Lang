#ifndef LANG_LOOP
#define LANG_LOOP
#include <string> 
#include <iostream>
#include <fstream>
#include <vector>
#include "utils.hpp"
using namespace std;

void function_init(vector<Token> tokens, string scope);
string call_function(vector<Token> tokens, string scope);

void bind_init(vector<Token> tokens,string scope);
void bind_execute(Bind bind,string scope, string directory);

void object_init(vector<Token> tokens,string scope);
string get_object_value(Token name,Token argument,string scope);
string access_object(Token token, Token target,string scope);
string change_object(Token token,string target,string change_value,string scope);
#endif