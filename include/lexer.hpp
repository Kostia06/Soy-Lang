#ifndef LANG_LEXER
#define LANG_LEXER
#include <iostream>
#include <fstream>
#include <string> 
#include <vector>
#include "utils.hpp"
using namespace std;





bool is_end(int c);
bool is_letter(int c);
bool is_number(int c);
bool is_space(int c);
bool is_quote(int c);
bool is_box(int c);
bool is_sign(int c);

string sign_type(string result);


void get_files(string directory);
string get_import_position(string directory);
void import_file(vector<Token> tokens,string directory);
string read_and_execute(string file, string scope);

void lexer();
Token token_init(string value, string type);
string tokenizer_action(vector<Token> tokens, string scope, string type);
vector<Component> tokenizer(string text, string scope, int variable_check);

Token change_index(Token item,Token index,string scope);
#endif