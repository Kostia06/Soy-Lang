#ifndef LANG_FUNCTION
#define LANG_FUNCTION
#include <string>
#include <vector>

#include "utils.hpp"



namespace func{
    void declaration(std::vector<Token>& tokens,std::string& text,std::string& current_scope,std::string& scope,int line);
    Token call(std::vector<Token> parameters,std::string scope,int name,int line);
    int in_list(std::string& name,std::string& scope);
    void destroy_scope(int index,std::string scope);
}

namespace obj{
    void declaration(std::vector<Token>& tokens,std::string& parameters,std::string& current_scope,std::string& scope,int line);
    Token get_value(std::string& name,std::string& object,std::string& scope,int line);
    int in_list(std::string& name, std::string& scope);
    std::string change_object_value(std::string object,std::string target,std::string value,std::string scope,int line);
    void destroy_scope(int index,std::string scope);
}

#endif