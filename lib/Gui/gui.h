#pragma once
#ifndef MySDL_GUI
#define MySDL_GUI

#include "lib.h"
#include "TextureManager/TM.h"

#define NUM_OF_LOADED_TEXTURES 50

#define GUI_CURSOR_OUTSIDE     0
#define GUI_CURSOR_CLICKED     1
#define GUI_CURSOR_HOVERING    2
#define GUI_CURSOR_DRAGGING    3



string color2hex(const SDL_Color& color);

class GUI{
private:
    struct LoadedText {
        int frame;
        TextureData td;
        SDL_Color color;
        string title;

        LoadedText(
            int frame = 0,
            TextureData td = TextureData(),
            SDL_Color color = SDL_COLOR_WHITE,
            string title = ""
        ):  frame(frame), 
            td(td),
            color(color),
            title(title) {}
        

        string getId(){
            return title + color2hex(this->color);
        }


        // Comparator: sort by value
        bool operator<(const LoadedText& other) const { return frame < other.frame; }
        bool operator>(const LoadedText& other) const { return frame > other.frame; }
    };

    static inline unordered_map<string, LoadedText> loadedTexts;
    static LoadedText* loadNewText(const string& title, const SDL_Color& color);
    //static LoadedText* getLoadedText(const string& title);

public:
    static int Button(
        const string& title, 
        const SDL_Rect& dRect,
        const SDL_Color& textColor = SDL_COLOR_WHITE,
        const SDL_Color& buttonColor = SDL_COLOR_BLACK
    );

    static void Text(
        const string& title, 
        SDL_Rect& dRect, 
        const SDL_Color& color = SDL_COLOR_WHITE
    );
    
    static void Rect(
        const SDL_Rect& dRect,
        const SDL_Color& color = SDL_COLOR_WHITE,
        const int thickness = -1
    );

    static void Line(
        const SDL_Point& p1,
        const SDL_Point& p2,
        const SDL_Color& color,
        const int thickness = -1
    );
    
};

#endif
// Creator: @AndrijaRD