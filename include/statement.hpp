#ifndef LANG_STATEMENT
#define LANG_STATEMENT
#include <string> 
#include <iostream>
#include <fstream>
#include <vector>
#include "utils.hpp"
using namespace std;


string statement(vector<Token> tokens, string scope);
bool get_equality(vector<Token> tokens);
bool get_value_statement(vector<Token> tokens,string scope);


string loop(vector<Token> tokens,string scope);
#endif