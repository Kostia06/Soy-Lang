#ifndef LANG_MODULE
#define LANG_MODULE
#include <string> 
#include <iostream>
#include <fstream>
#include <vector>
#include "utils.hpp"
#include "SDL.h"
#include "SDL_ttf.h"
#include "SDL_image.h"
using namespace std;

string check_module(Token name,string argument,string scope);

string time_sleep(vector<Token> tokens);
string time_time(vector<Token> tokens);

string math_cos(vector<Token> tokens);
string math_sin(vector<Token> tokens);
string math_tan(vector<Token> tokens);
string math_acos(vector<Token> tokens);
string math_asin(vector<Token> tokens);
string math_atan(vector<Token> tokens);
string math_atan2(vector<Token> tokens);
string math_pow(vector<Token> tokens);
string math_sqrt(vector<Token> tokens);
string math_hypot(vector<Token> tokens);
string math_floor(vector<Token> tokens);
string math_random(vector<Token> tokens);
string math_abs(vector<Token> tokens);
string math_min(vector<Token> tokens);
string math_max(vector<Token> tokens);

string system_system(vector<Token> tokens);
string exec(string command);

string file_read(vector<Token> tokens);
string file_write(vector<Token> tokens);

void window_init(vector<Token> tokens,string scope);
string window_run(vector<Token> tokens,string scope);
string window_state(vector<Token> tokens);

void window_create_rect(vector<Token> tokens);
void window_create_circle(vector<Token> tokens);
void window_create_image(vector<Token> tokens,string scope);
void window_create_text(vector<Token> tokens,string scope);
void draw_circle(SDL_Renderer * renderer, int x, int y, int radius);

string window_event();
void window_render(vector<Token> tokens,string scope);
void window_clear();
#endif