#include <fstream>
#include <vector>
#include <time.h>
#include <unistd.h> 
#include <cmath>
#include <math.h> 
#include <variant>
#include <any>
#include <iostream>
#include <stdlib.h>
#include <algorithm>
#include "SDL.h"
#include "SDL_ttf.h"
#include "SDL_image.h"
#include "../include/module.hpp"
using namespace std;


SDL_Window *window;
SDL_Renderer *renderer;
SDL_Event event;
bool window_running = false;
int FPS,frame_delay,frame_time;
Uint32 frame_start;
int mouse_x, mouse_y;
Uint64 old_time;
Uint64 new_time;
struct Rect{
    int x,y,w,h;
    int r,b,g,a;
};
struct Circle{
    int x,y,s;
    int r,b,g,a;
};
struct Text{
    string title;
    int x,y,w,h,size;
    string directory;
};
struct Image{
    string directory;
    int x,y,w,h,angle;
};
vector<Rect> rect_list;
vector<Circle> circle_list;
vector<Text> text_list;
vector<Image> image_list;
string module_list[] ={
    "@sleep","@time",
    "@cos","@sin","@tan","@acos","@asin","@atan","@atan2","@sqrt","@hypot","@floor","@random","@abs","@min","@max",
    "@system",
    "@read","@write",
    "@window_init","@window_run","@window_state","@window_event","@window_rect","@window_circle","@window_clear","@window_text","@window_image"
};

string check_module(Token name,string argument,string scope){

    int size = sizeof(module_list)/sizeof(module_list[0]);
    for(int i=0;i<size;i++){
        string current_module = module_list[i];
        if(current_module == name.value){
            vector<Token> values;
            vector<vector<Token> >list = split_tokens(argument,scope);
            for(int j=0;j<list.size();j++){
                Token token;
                token.value = get_value(list[j],scope,-1,true);
                values.push_back(token);
            }
            if(current_module == "@sleep"){return time_sleep(values);}
            else if(current_module == "@time"){return time_time(values);}

            else if(current_module == "@cos"){return math_cos(values);}
            else if(current_module == "@sin"){return math_sin(values);}
            else if(current_module == "@tan"){return math_atan(values);}
            else if(current_module == "@acos"){return math_cos(values);}
            else if(current_module == "@asin"){return math_asin(values);}
            else if(current_module == "@atan"){return math_atan(values);}
            else if(current_module == "@atan2"){return math_atan2(values);}
            else if(current_module == "@sqrt"){return math_sqrt(values);}
            else if(current_module == "@hypot"){return math_hypot(values);}
            else if(current_module == "@floor"){return math_floor(values);}
            else if(current_module == "@random"){return math_random(values);}
            else if(current_module == "@abs"){return math_abs(values);}
            else if(current_module == "@min"){return math_min(values);}
            else if(current_module == "@max"){return math_max(values);}

            else if(current_module == "@system"){return system_system(values);}

            else if(current_module == "@read"){return file_read(values);}
            else if(current_module == "@write"){return file_write(values);}

            else if(current_module == "@window_init"){window_init(values,scope);}
            else if(current_module == "@window_run"){return window_run(values,scope);}
            else if(current_module == "@window_state"){return window_state(values);}
            else if(current_module == "@window_event"){return window_event();}
            else if(current_module == "@window_rect"){window_create_rect(values);}
            else if(current_module == "@window_circle"){window_create_circle(values);}
            else if(current_module == "@window_clear"){window_clear();}
            else if(current_module == "@window_text"){window_create_text(values,scope);}
            else if(current_module == "@window_image"){window_create_image(values,scope);}
        }
    }
    return "";
}

string time_sleep(vector<Token> tokens){
    if(get_type(tokens[0].value) == "NUMBER"){
        usleep(stoi(tokens[0].value)*1000000);
    }
    return "";
}
string time_time(vector<Token> tokens){
    return "";
}

string math_cos(vector<Token> tokens){
    if(get_type(tokens[0].value) == "NUMBER"){
        int result = cos(stoi(tokens[0].value));
        return to_string(result);
    }
    return "";
}
string math_sin(vector<Token> tokens){
    if(get_type(tokens[0].value) == "NUMBER"){
        int result = sin(stoi(tokens[0].value));
        return to_string(result);
    }
    return "";
}
string math_tan(vector<Token> tokens){
    if(get_type(tokens[0].value) == "NUMBER"){
        int result = tan(stoi(tokens[0].value));
        return to_string(result);
    }
    return "";
}
string math_acos(vector<Token> tokens){
    if(get_type(tokens[0].value) == "NUMBER"){
        int result = acos(stoi(tokens[0].value));
        return to_string(result);
    }
    return "";
}
string math_asin(vector<Token> tokens){
    if(get_type(tokens[0].value) == "NUMBER"){
        int result = asin(stoi(tokens[0].value));
        return to_string(result);
    }
    return "";
}
string math_atan(vector<Token> tokens){
    if(get_type(tokens[0].value) == "NUMBER"){
        int result = atan(stoi(tokens[0].value));
        return to_string(result);
    }
    return "";
}
string math_atan2(vector<Token> tokens){
    if(get_type(tokens[0].value) == "NUMBER"&&get_type(tokens[1].value) == "NUMBER"){
        double result = atan2(atoi(tokens[0].value.c_str()),atoi(tokens[1].value.c_str()));
        return to_string(result);
    }
    return "";
}
string math_sqrt(vector<Token> tokens){
    // if(get_type(tokens[0].value) == "NUMBER"){
    //     int result = cos(stoi(tokens[0].value));
    //     return to_string(result);
    // }
    return "";
}
string math_hypot(vector<Token> tokens){
    if(tokens.size() >= 2 && get_type(tokens[0].value) == "NUMBER" && get_type(tokens[1].value) == "NUMBER"){
        int result = hypot(stoi(tokens[0].value),stoi(tokens[1].value));
        return to_string(result);
    }
    return "";
}
string math_floor(vector<Token> tokens){
    if(get_type(tokens[0].value) == "NUMBER"){
        int result = floor(stoi(tokens[0].value));
        return to_string(result);
    }
    return "";
}
string math_random(vector<Token> tokens){
    srand(time(NULL));
    if(tokens.size() >= 2 && get_type(tokens[0].value) == "NUMBER" && get_type(tokens[1].value) == "NUMBER"){
        int min = stoi(tokens[0].value);
        int max = stoi(tokens[1].value);
        int range = max - min + 1;
        int result = (rand() % range) + min;
        return to_string(result);
    }
    return "";
}
string math_abs(vector<Token> tokens){
    if(get_type(tokens[0].value)=="NUMBER"){
        return to_string(abs(atof(tokens[0].value.c_str())));
    }
    return "";
}
string math_min(vector<Token> tokens){
    if(get_type(tokens[0].value) == "NUMBER" && get_type(tokens[1].value) == "NUMBER"){
        return to_string(min(atof(tokens[0].value.c_str()),atof(tokens[1].value.c_str())));
    }
    return "";
}
string math_max(vector<Token> tokens){
    if(get_type(tokens[0].value) == "NUMBER" && get_type(tokens[1].value) == "NUMBER"){
        return to_string(max(atof(tokens[0].value.c_str()),atof(tokens[1].value.c_str())));
    }
    return "";
}


string system_system(vector<Token> tokens){
    if(get_type(tokens[0].value) == "STRING"){
        string value = exec(tokens[0].value);
        if(value.find('\n') != std::string::npos){
            vector<string> items = split(value, "\n");
            string result = "[";
            for(int i = 0; i < items.size(); i++){
                result += "\""+items[i] + "\",";
            }
            result = result.substr(0, result.size() - 4);
            result += "]";
            return result;
        }
        return value;
    }
    return "";
}
string exec(string command) {
   char buffer[128];
   string result = "";
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

string file_read(vector<Token> tokens){
    if(get_type(tokens[0].value) == "STRING"){
        ifstream file(tokens[0].value);
        string line;
        vector<string> lines;
        while (getline(file, line)){
            lines.push_back(line);
        }
        file.close();
        string result = "\"";result += join(lines,"\n");
        result += "\"";
        return result;
    }
    return "";
}
string file_write(vector<Token> tokens){
    if(get_type(tokens[0].value) == "STRING" && get_type(tokens[1].value) == "STRING"){
        ofstream file(tokens[0].value);
        file<<tokens[1].value<<endl;
    }
    return "";
}

void window_init(vector<Token> tokens,string scope){
    if(get_type(tokens[0].value) == "STRING" && get_type(tokens[1].value) == "NUMBER" && get_type(tokens[2].value) == "NUMBER"){
        char title[150];
        strcpy(title, tokens[0].value.c_str());    
        TTF_Init();
        if(SDL_Init(SDL_INIT_VIDEO) == 0){
            window = SDL_CreateWindow(title,SDL_WINDOWPOS_CENTERED,SDL_WINDOWPOS_CENTERED,stoi(tokens[1].value),stoi(tokens[2].value),SDL_WINDOW_SHOWN);
            if(!window){display_error(tokens,scope,"Window refused to initialize");}
            renderer = SDL_CreateRenderer(window,-1,0);
            if(!renderer){display_error(tokens,scope,"Renderer refused to initialize");}
            SDL_SetRenderDrawColor(renderer,255,255,255,0);
            window_running = true;
            old_time  = SDL_GetTicks();
        }
    }
}
string window_state(vector<Token> tokens){
    if(window_running){return "1";}
    else{return "0";}
}
string window_run(vector<Token> tokens,string scope){
    double delta_time=0;
    if(get_type(tokens[0].value) == "NUMBER" ){
        FPS = stoi(tokens[0].value);
        frame_delay = 1000/FPS;
        frame_start = SDL_GetTicks();
        window_render(tokens,scope);
        frame_time = SDL_GetTicks() - frame_start;

        new_time = SDL_GetTicks();
        delta_time = new_time - old_time;
        old_time = new_time;
        if(frame_delay > frame_time){SDL_Delay(frame_delay-frame_time);}
    }
    return to_string(delta_time/FPS);
}
string window_event(){
    string current_key = "";
    SDL_GetMouseState(&mouse_x, &mouse_y);
    SDL_PollEvent (&event);
    int type = event.type;
    if(type == SDL_QUIT){window_running = false;return "";}
    if(type == SDL_KEYDOWN){current_key += (char)event.key.keysym.sym;}
    if(type == SDL_TEXTEDITING){cout << event.text.text << endl;}
    return "[\"" + current_key + "\",[" + to_string(mouse_x) + ","+to_string(mouse_y) + ","+ "0"+ "]]";
}


void window_create_rect(vector<Token> tokens){
    if(get_type(tokens[0].value)=="NUMBER" && tokens.size() == 8){
        Rect rect = {
            atof(tokens[0].value.c_str()),atof(tokens[1].value.c_str()),atof(tokens[2].value.c_str()),atof(tokens[3].value.c_str()),
            stoi(tokens[4].value),stoi(tokens[5].value),stoi(tokens[6].value),stoi(tokens[7].value)
        };
        rect_list.push_back(rect);
    }    
}
void window_create_circle(vector<Token> tokens){
    if(get_type(tokens[0].value)=="NUMBER" && tokens.size() == 7){
        Circle circle = {
            atof(tokens[0].value.c_str()),atof(tokens[1].value.c_str()),atof(tokens[2].value.c_str()),
            stoi(tokens[3].value),stoi(tokens[4].value),stoi(tokens[5].value),stoi(tokens[6].value)
        };
        circle_list.push_back(circle);
    }    
}
void window_create_image(vector<Token> tokens,string scope){
    if(get_type(tokens[0].value)=="STRING" && get_type(tokens[1].value)=="NUMBER" && get_type(tokens[2].value)=="NUMBER" && get_type(tokens[3].value)=="NUMBER"&& get_type(tokens[4].value)=="NUMBER"&& get_type(tokens[5].value)=="NUMBER"){
        string directory = split(scope,"!~!")[0]+"/"+tokens[0].value;
        int x=atoi(tokens[1].value.c_str());
        int y=atoi(tokens[2].value.c_str());
        int w=atoi(tokens[3].value.c_str());
        int h=atoi(tokens[4].value.c_str());
        int angle=atoi(tokens[5].value.c_str());
        Image image = {directory,x,y,w,h,angle};

        image_list.push_back(image);
    }
}
void window_create_text(vector<Token> tokens,string scope){
    if( get_type(tokens[1].value)=="NUMBER" && get_type(tokens[2].value)=="NUMBER" && get_type(tokens[3].value)=="NUMBER"){
        string directory = split(scope,"!~!")[0]+"/"+tokens[0].value;
        string title = tokens[0].value.c_str();  
        int size  = stoi(tokens[1].value);
        int x  = stoi(tokens[2].value);
        int y  = stoi(tokens[3].value);
        int w  = stoi(tokens[3].value);
        int h  = stoi(tokens[4].value);
        Text text ={title,x,y,w,h,size,directory};
        text_list.push_back(text);
    }
}
void draw_circle(SDL_Renderer * renderer, int x, int y, int radius){
    for (int w = 0; w < radius * 2; w++){
        for (int h = 0; h < radius * 2; h++){
            int dx = radius - w; 
            int dy = radius - h;
            if ((dx*dx + dy*dy) <= (radius * radius)){
                SDL_RenderDrawPoint(renderer, x + dx, y + dy);
            }
        }
    }
}
void window_render(vector<Token> tokens,string scope){
    int r =255;
    int b =255;
    int g =255;
    int a =255;
    if(tokens.size() > 1 && get_type(tokens[1].value)=="NUMBER"){r = stoi(tokens[1].value);}
    if(tokens.size() > 2 &&get_type(tokens[2].value)=="NUMBER"){g = stoi(tokens[2].value);}
    if( tokens.size() > 3 &&get_type(tokens[3].value)=="NUMBER"){b = stoi(tokens[3].value);}
    if( tokens.size() > 4 &&get_type(tokens[4].value)=="NUMBER"){a = stoi(tokens[4].value);}

    SDL_SetRenderDrawColor( renderer,r,g,b,a);
    SDL_RenderClear(renderer);
    for(int i=0;i<rect_list.size();i++){
        Rect current_rect = rect_list[i];
        SDL_Rect rect;
        rect.x = current_rect.x;
        rect.y = current_rect.y;
        rect.h = current_rect.h;
        rect.w = current_rect.w;
        SDL_SetRenderDrawColor( renderer, current_rect.r, current_rect.g, current_rect.b, current_rect.a );
        SDL_RenderFillRect( renderer, &rect );
    }
    for(int i =0;i<circle_list.size();i++){
        Circle current_circle = circle_list[i];
        SDL_SetRenderDrawColor( renderer, current_circle.r, current_circle.g, current_circle.b, current_circle.a );
        draw_circle(renderer,current_circle.x,current_circle.y,current_circle.s);
    }
    for(int i =0;i<image_list.size();i++){
        Image current_image = image_list[i];
        char dir[150];
        strcpy(dir, current_image.directory.c_str());
        SDL_Surface* image = IMG_Load(dir);
        if(!image){display_error(tokens,scope,"Directory \""+current_image.directory+"\" doesn't exist");}
        SDL_Rect rect;
        rect.x = current_image.x;
        rect.y = current_image.y;
        rect.w = current_image.w;
        rect.h = current_image.h;
        SDL_Texture* texture = SDL_CreateTextureFromSurface(renderer,image);
        SDL_RenderCopyEx(renderer,texture,NULL,&rect,current_image.angle,NULL,SDL_FLIP_NONE);
        SDL_FreeSurface(image);
        SDL_DestroyTexture(texture);
    }
    for(int i=0;i<text_list.size();i++){
        Text current_text = text_list[i];
        char title[150];
        char dir[150];
        strcpy(title, current_text.title.c_str());
        strcpy(dir, current_text.directory.c_str());
        SDL_Color color = { 0, 0, 0 };
        TTF_Font* font = TTF_OpenFont(dir, current_text.size);
        if(!font){display_error(tokens,scope,"Font did not load");}
        SDL_Surface* text = TTF_RenderText_Solid(font,title,color);
        SDL_Texture* Message = SDL_CreateTextureFromSurface(renderer,text);
        SDL_Rect Message_rect;
        Message_rect.x = current_text.x;   
        Message_rect.y = current_text.y; 
        Message_rect.w = current_text.w; 
        Message_rect.h = current_text.h; 
        SDL_RenderCopy(renderer, Message, NULL, &Message_rect);
        SDL_FreeSurface(text);
        SDL_DestroyTexture(Message);   
        TTF_CloseFont(font);     
    }
    circle_list.clear();
    rect_list.clear();
    image_list.clear();
    text_list.clear();
    SDL_RenderPresent(renderer);
}
void window_clear(){
    SDL_DestroyWindow(window);
    SDL_DestroyRenderer(renderer);
    IMG_Quit();
    SDL_Quit();
}


