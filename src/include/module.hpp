#ifndef LANG_MODULE
#define LANG_MODULE

#include <string>
#include <vector>
#include <string>
#include <map>

#include "utils.hpp"

namespace modules{
    std::string module_init(std::string name,std::string result,std::string scope,int line);
    std::string time_sleep(std::string value,std::string scope,int line);

    std::string time_time(std::string value,std::string scope,int line);

    std::string math_cos(std::string value,std::string scope,int line);
    std::string math_sin(std::string value,std::string scope,int line);
    std::string math_tan(std::string value,std::string scope,int line);
    std::string math_acos(std::string value,std::string scope,int line);
    std::string math_asin(std::string value,std::string scope,int line);
    std::string math_atan(std::string value,std::string scope,int line);
    std::string math_atan2(std::string value,std::string scope,int line);
    std::string math_sqrt(std::string value,std::string scope,int line);
    std::string math_hypot(std::string value,std::string scope,int line);
    std::string math_floor(std::string value,std::string scope,int line);
    std::string math_random(std::string value,std::string scope,int line);
    std::string math_abs(std::string value,std::string scope,int line);
    std::string math_min(std::string value,std::string scope,int line);
    std::string math_max(std::string value,std::string scope,int line);

    std::string system_system(std::string value,std::string scope,int line);
    std::string execute(std::string value,std::string scope,int line);
    std::string system_exit(std::string value,std::string scope,int line);


    

}

typedef std::string (*fun)(std::string,std::string,int);
typedef std::map<std::string,fun> module_item;
extern module_item module_list;
#endif