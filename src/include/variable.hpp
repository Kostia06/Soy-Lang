#ifndef LANG_VARIABLE
#define LANG_VARIABLE

#include <vector>
#include <string>

#include "utils.hpp"

namespace var{
    void declaration(std::vector<Token>& tokens,std::string& current_scope,std::string& scope, int& line);
    int in_list(std::string name,std::string scope);
    void destroy_scope(int index,std::string scope);
    void pointer(std::vector<Token> tokens, std::string scope,int line);
    std::string index_array(std::string array, std::string pointer, std::string value, std::string scope, int line);
}



#endif
