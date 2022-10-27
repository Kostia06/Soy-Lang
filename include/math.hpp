#ifndef LANG_MATH
#define LANG_MATH
#include <string> 
#include <iostream>
#include <fstream>
#include <vector>
#include "utils.hpp"
#include "lexer.hpp"
using namespace std;


vector<Token> tokens_for_math(string text,string scope);
float power(float from , float to);
string apply_operations(float a, float b, string op);
int precedence(string op);
string math_eval(string text,string scope);

#endif