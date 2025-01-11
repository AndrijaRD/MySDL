#pragma once
#ifndef MySDL_SYSTEM
#define MySDL_SYSTEM

#include "lib.h"


// A macro for easyer checking of the errors, if there is something working print the error
#define CHECK_ERROR(error) \
    if ((error) != NO_ERROR) \
        std::cout << "[ERROR] " << Sys::checkError((error)) << std::endl;




class Sys{
    friend class Mouse;
    friend class TM;
    friend class GUI;

    private:
    static inline int OS;
    static inline string windowTitle;
    static inline bool isFullscreen;
    static inline SDL_Color clearColor = {21, 20, 21, 255};

    static inline int maxFPS = 120;
    static inline int minFPS = 30;
    static inline bool dynamicFPS = false;  // If this is true, the game will constantly change FPS as
                                            // the game is unable to too well able to catch up
    static inline int FPS = 60;
    static inline uint frameCounter             = 0;
    static inline Uint64 frameStart             = 0;
    static inline Uint64 previousFrameStart     = 0;
    static inline Uint64 deltaTime              = 0;

    static inline SDL_Window* win = nullptr;
    static inline SDL_Renderer* r = nullptr;

    static inline string fontPath = "./assets/fonts/font.ttf";
    static inline TTF_Font* font;

    static inline int wWidth = 0;
    static inline int wHeight = 0;

    static unordered_map<int, string> errorMap;

    // static inline bool showWarnings = true;

    public:
    static int init(
        const string& windowTitle = "MySDL Window",
        const bool& fullscreen = false,
        const int& windowWidth = 1920*0.75,             // 3/4 of the screen
        const int& windowHeight = 1080*0.75             // 3/4 of the screen
    );
    static int handleEvents();
    static int presentFrame();
    static int cleanup();

    static int getOS();
    static void setClearColor(const SDL_Color& color);

    static int getFPS();
    static void setFPS(const int& newFPS);
    static void setDynamicFPS(bool dynamicFPS = false, int maxFPS = 120, int minFPS = 30);
    static int getCurrentFrame();

    static string checkError(int error);

    static inline bool isRunning = true;

    static inline SDL_Window* const& window = win;      // win pointer (SDL_Window*) exposed to the global code, but as read-only
    static inline SDL_Renderer* const& renderer = r;    // r pointer (SDL_Renderer*) exposed to the global code, but as read-only

    static inline int const& winWidth = wWidth;         // wWidth (Window Width int) exposed to the global code, but as read-only
    static inline int const& winHeight = wHeight;       // wHeight (Window Height int) exposed to the global code, but as read-only

    class Mouse {
        friend class Sys;

        protected:
            static inline SDL_Point pos = {0, 0};
            static inline bool clicked = false;
            static inline bool down = false;

        public:
            static SDL_Point getPos();
            static bool isClicked();
            static bool isDown();
            static bool isHovering(const SDL_Rect& area);
    };

    class Keyboard{
        friend class Sys;

        protected:
            static inline SDL_Keycode key;
            static inline string text;

        public:
            static inline SDL_Keycode getKey();  
            static inline string getText();
    };

};

#endif
// Creator: @AndrijaRD