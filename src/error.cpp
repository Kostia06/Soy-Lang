#include <string>
#include <stdlib.h>
#include <vector>
#include <stdexcept> 

#include "include/error.hpp"
#include "include/utils.hpp"



static std::string get_line_error(std::string directory, int range){
    FILE* file = fopen(directory.c_str(),"r");
    fseek(file, 0, SEEK_END);
    long size = ftell(file);
    rewind(file);
    char* buffer = (char*)malloc(sizeof(char) * (size + 1));
    fread(buffer, sizeof(char), size, file);
    buffer[size] = '\0';
    fclose(file);
    std::string result = buffer;
    std::vector<std::string> lines = tools::split(result,"\n");
    std::string error = "";
    for(int i=range-5;i<range+5;i++){
        if(i < 0 || i >= lines.size()){continue;}
        error += lines[i] + "\n";
    }
    return error;
}
void display_error(std::string scope,std::string error,int line){
    std::vector<std::string> scopes = tools::split(scope,"!~!");
    std::string directory = tools::split(scope,"!~!")[0];
    printf("\n\033[1m\033[31m%s\033[1m\033[32m\n",error.c_str());
    if(line == 0){exit(1);}
    //loop though scopes
    for(int i = 0; i < scopes.size(); i++){
        if(scopes[i] == "0"){break;}
        for(int j=0;j<i;j++){printf("  ");}
        printf("> %s\n",scopes[i].c_str());
    }
    printf("%i\n",line);
    directory = get_line_error(directory,line);
    std::vector<std::string> errors = tools::split(directory,"\n");
    int size = errors.size()/2;
    if(errors.size()%2==1){size--;}
    for(int i=0;i<errors.size();i++){
        if(line == i+line-size){printf("\033[1m\033[32m%i >>\033[1m\033[31m\t%s\n",i+line-size,errors[i].c_str());}
        else{printf("\033[1m\033[32m%i >>\033[37m\t%s\n",i+line-size,errors[i].c_str());}
    }
    printf("\033[0m");
    exit(1);
}