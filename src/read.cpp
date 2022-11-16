#include <string>
#include <stdio.h>
#include <stdlib.h>
#include <iostream>
#include <fstream>

#include "include/read.hpp"
#include "include/error.hpp"


std::string read_file(char* filename){
    FILE* file = fopen(filename, "r");
    if(file == NULL){
        display_error(filename,"File not found",0);
    }
    fseek(file, 0, SEEK_END);
    long size = ftell(file);
    rewind(file);
    char* buffer = (char*)malloc(sizeof(char) * (size + 1));
    fread(buffer, sizeof(char), size, file);
    buffer[size] = '\0';
    fclose(file);
    return buffer;
}



