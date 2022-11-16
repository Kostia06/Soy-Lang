#ifndef LANG_LEXER
#define LANG_LEXER

#include <string.h> 
#include <vector>
#include "utils.hpp"

namespace lexer{
    std::vector<Token> tokenize(char* text, std::string scope,int line,int variable_check=1);
}



#endif