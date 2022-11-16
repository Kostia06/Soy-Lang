#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <map>
#include <vector>
#include <variant>
#include <cmath>



#include "include/module.hpp"
#include "include/utils.hpp"
#include "include/lexer.hpp"
#include "include/error.hpp"









namespace modules{
    std::string time_sleep(std::string value,std::string scope,int line){
        if(stuff::get_type(value) == INTEGER){
            usleep(stoi(value)*1000000);
        }
        display_error(scope,"Invalid argument for @sleep",line);
        return "";
    }
    std::string time_time(std::string value,std::string scope,int line){
        return std::to_string(clock());
    }

    std::string math_cos(std::string value,std::string scope,int line){
        if(stuff::get_type(value) == INTEGER){
            return std::to_string(cos(atof(value.c_str())));
        }
        display_error(scope,"Invalid argument for @cos",line);
        return "";
    }
    std::string math_sin(std::string value,std::string scope,int line){
        if(stuff::get_type(value) == INTEGER){
            return std::to_string(sin(atof(value.c_str())));
        }
        display_error(scope,"Invalid argument for @sin",line);
        return "";
    }
    std::string math_tan(std::string value,std::string scope,int line){
        if(stuff::get_type(value) == INTEGER){
            return std::to_string(tan(atof(value.c_str())));
        }
        display_error(scope,"Invalid argument for @tan",line);
        return "";
    }
    std::string math_acos(std::string value,std::string scope,int line){
        if(stuff::get_type(value) == INTEGER){
            return std::to_string(acos(atof(value.c_str())));
        }
        display_error(scope,"Invalid argument for @acos",line);
        return "";
    }
    std::string math_asin(std::string value,std::string scope,int line){
        if(stuff::get_type(value) == INTEGER){
            return std::to_string(asin(atof(value.c_str())));
        }
        display_error(scope,"Invalid argument for @asin",line);
        return "";
    }
    std::string math_atan(std::string value,std::string scope,int line){
        if(stuff::get_type(value) == INTEGER){
            return std::to_string(atan(atof(value.c_str())));
        }
        display_error(scope,"Invalid argument for @atan",line);
        return "";
    }
    std::string math_atan2(std::string value,std::string scope,int line){
        std::vector<Token> tokens = lexer::tokenize((char*)value.c_str(),scope,line,0);
        std::vector<Token>array = stuff::split_tokens(tokens,scope,0,line);
        if(array[0].type == INTEGER && array[1].type == INTEGER){
            double result = atan2(atoi(array[0].value.c_str()),atoi(array[1].value.c_str()));
            return std::to_string(result);
        }
        display_error(scope,"Invalid argument for @atan2",line);
        return "";
    }
    std::string math_sqrt(std::string value,std::string scope,int line){
        if(stuff::get_type(value) == INTEGER){
            int result = cos(atof(value.c_str()));
            return std::to_string(result);
        }
        display_error(scope,"Invalid argument for @sqrt",line);
        return "";
    }
    std::string math_hypot(std::string value,std::string scope,int line){
        std::vector<Token> tokens = lexer::tokenize((char*)value.c_str(),scope,line,0);
        std::vector<Token> array = stuff::split_tokens(tokens,scope,0,line);
        if(array.size() >= 2 && array[0].type == INTEGER && array[1].type == INTEGER){
            int result = hypot(atof(array[0].value.c_str()),atof(array[1].value.c_str()));
            return std::to_string(result);
        }
        display_error(scope,"Invalid argument for @hypot",line);
        return "";
    }
    std::string math_floor(std::string value,std::string scope,int line){
        if(stuff::get_type(value) == INTEGER){
            return std::to_string(floor(atof(value.c_str())));
        }
        display_error(scope,"Invalid argument for @floor",line);
        return "";
    }
    std::string math_random(std::string value,std::string scope,int line){
        srand(time(NULL));
        std::vector<Token> tokens = lexer::tokenize((char*)value.c_str(),scope,line,0);
        std::vector<Token> array = stuff::split_tokens(tokens,scope,0,line);
        if(array.size() >= 2 && array[0].type == INTEGER && array[1].type == INTEGER){
            int min = atof(array[0].value.c_str());
            int max = atof(array[1].value.c_str());
            int range = max - min + 1;
            int result = (rand() % range) + min;
            return std::to_string(result);
        }
        display_error(scope,"Invalid argument for @random",line);
        return "";
    }
    std::string math_abs(std::string value,std::string scope,int line){
        if(stuff::get_type(value)==INTEGER){
            return std::to_string(abs(atof(value.c_str())));
        }
        display_error(scope,"Invalid argument for @abs",line);
        return "";
    }
    std::string math_min(std::string value,std::string scope,int line){
        std::vector<Token> tokens = lexer::tokenize((char*)value.c_str(),scope,line,0);
        std::vector<Token> array = stuff::split_tokens(tokens,scope,0,line);
        if(array[0].type == INTEGER && array[1].type == INTEGER){
            return std::to_string(std::min(atof(array[0].value.c_str()),atof(array[1].value.c_str())));
        }
        display_error(scope,"Invalid argument for @min",line);
        return "0";
    }
    std::string math_max(std::string value,std::string scope,int line){
        std::vector<Token> tokens = lexer::tokenize((char*)value.c_str(),scope,line,0);
        std::vector<Token> array = stuff::split_tokens(tokens,scope,0,line);
        if(array[0].type == INTEGER && array[1].type == INTEGER){
            return std::to_string(std::max(atof(array[0].value.c_str()),atof(array[1].value.c_str())));
        }
        display_error(scope,"Invalid argument for @max",line);
        return "";
    }

    static std::string exec(std::string command){
        char buffer[128];
        std::string result = "";
        FILE* pipe = popen(command.c_str(), "r");
        if (!pipe) {
            return "popen failed!";
        }
        while (!feof(pipe)) {
                if (fgets(buffer, 128, pipe) != NULL){
                    result += buffer;
                }
        }
        pclose(pipe);
        return result;
    }
    std::string system_system(std::string value,std::string scope,int line){
        std::vector<Token> tokens =  lexer::tokenize((char*)value.c_str(),scope,line,0);
        value = stuff::get_value(tokens,scope,1,line).value;
        std::string result = exec(value);
        if(result.find('\n') != std::string::npos){
            std::vector<std::string> items = tools::split(result, "\n");
            result = "[";
            for(int i = 0; i < items.size(); i++){
                result += "\""+items[i] + "\",";
            }
            result = result.substr(0, result.size() - 4);
            result += "]";
            return result;
        }
        return result;
    }
    std::string execute(std::string value,std::string scope,int line){
        if(stuff::get_type(value) == STRING){
            value.pop_back();value.erase(value.begin());
            lexer::tokenize((char*)value.c_str(),scope+"!~!execute",line,1);
        }
        return "0";
    }
    std::string system_exit(std::string value,std::string scope,int line){
        exit(0);
        return "0";
    }

    

    std::string module_init(std::string name,std::string result,std::string scope,int line){
        if(module_list.find(name) != module_list.end()){
            return module_list[name](result,scope,line);
        }   
        display_error(scope,"Module \"" +name+"\" not found",line);
        return "0";
    }

}