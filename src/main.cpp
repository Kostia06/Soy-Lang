#include <stdio.h>
#include <string>
#include <vector>
#include <map>

#include "include/read.hpp"
#include "include/lexer.hpp"
#include "include/module.hpp"


std::vector<Variable> variables;
std::vector<Function> functions;
std::vector<Object> objects;

typedef std::string (*fun)(std::string,std::string,int);
typedef std::map<std::string,fun> module_item;
module_item module_list;

int main(int argc, char *argv[]){
    if(argc < 2){ printf("Not enough arguments\n");return 1;}

    module_list["system"] = &modules::system_system;
    module_list["exit"] = &modules::system_exit;
    module_list["execute"] = &modules::execute;

    module_list["sleep"] = &modules::time_sleep;
    module_list["time"] = &modules::time_time;

    module_list["cos"] = &modules::math_cos;
    module_list["sin"] = &modules::math_sin;
    module_list["tan"] = &modules::math_tan;
    module_list["acos"] = &modules::math_acos;
    module_list["asin"] = &modules::math_asin;
    module_list["atan"] = &modules::math_atan;
    module_list["atan2"] = &modules::math_atan2;
    module_list["sqrt"] = &modules::math_sqrt;
    module_list["hypot"] = &modules::math_hypot;
    module_list["floor"] = &modules::math_floor;
    module_list["random"] = &modules::math_random;
    module_list["abs"] = &modules::math_abs;
    module_list["min"] = &modules::math_min;
    module_list["max"] = &modules::math_max;


    if(strcmp(argv[1],"drun") == 0){
        lexer::tokenize((char*)read_file(argv[2]).c_str(),argv[2],1);
        
        for(Variable i:variables){
            printf("%s\t%s\t%u\t%s\n",i.name.c_str(),i.value.c_str(),i.type,i.scope.c_str());
        }
        for(Function i:functions){
            printf("%s\t%s\t%s\n",i.name.c_str(),tools::join(i.arguments,",").c_str(),i.scope.c_str());
        }
        for(Object i:objects){
            printf("%s\t%s\t%s\n",i.name.c_str(),tools::join(i.arguments,",").c_str(),i.scope.c_str());
        }

    }

    return 1;
}