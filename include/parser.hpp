#ifndef LANG_PARSER
#define LANG_PARSER
#include <string> 
#include <iostream>
#include <fstream>
#include <vector>
#include "lexer.hpp"
#include "utils.hpp"
using namespace std;

string parser(vector<Component> component_list, string scope, string directory);


#endif