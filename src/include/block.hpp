#ifndef LANG_BLOCK
#define LANG_BLOCK
#include <string>
#include <vector>
#include "utils.hpp"


namespace block{
    Token statement(std::vector<Token>& tokens,std::string& text,std::string& scope,int line);
    Token loop(std::vector<Token>& tokens,std::string& text,std::string& scope,int line);
}



#endif