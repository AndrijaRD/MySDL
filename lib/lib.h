#pragma once
#ifndef MySDL
#define MySDL


#include <iostream>         // std
#include <filesystem>       // std::filesystem
#include <SDL2/SDL.h>       // Global SDL
#include <SDL2/SDL_ttf.h>   // Fonts
#include <SDL2/SDL_image.h> // IMG_Load
#include <vector>           // vector<>
#include <algorithm>        // For std::remove
#include <libpq-fe.h>       // For PosgreSQL DB
#include <unordered_map>    // For unordered_map<type, type>
#include <set>              // For sets (gui.h)


using namespace std;
namespace fs = std::filesystem;



// OS ---------------------------------------------------------------------------
#define OS_LINUX    0
#define OS_WINDOWS  1


// COLORS -----------------------------------------------------------------------
#define SDL_COLOR_WHITE         {255,   255,    255,    255}
#define SDL_COLOR_BLACK         {0,     0,      0,      255}
#define SDL_COLOR_BLUE          {0,     0,      255,    255}
#define SDL_COLOR_GREEN         {0,     255,    0,      255}
#define SDL_COLOR_RED           {255,   0,      0,      255}
#define SDL_COLOR_PURPLE        {128,   0,      128,    255}
#define SDL_COLOR_YELLOW        {255,   255,    0,      255}
#define SDL_COLOR_CYAN          {0,     255,    255,    255}
#define SDL_COLOR_PINK          {255,   192,    203,    255}
#define SDL_COLOR_SEMI_TRANSP   {120,   120,    120,    127}
#define SDL_COLOR_TRANSP        {255,   255,    255,    0  }
#define SDL_COLOR_GRAY          {120,   120,    120,    255}

inline Uint32 toUint32(const SDL_Color& color) {
    return (static_cast<Uint32>(color.r) << 24) |
           (static_cast<Uint32>(color.g) << 16) |
           (static_cast<Uint32>(color.b) << 8)  |
            static_cast<Uint32>(color.a);
}


// ERRORS ----------------------------------------------------------------------
#define NO_ERROR                        0x00

#define SYS_SDL_INIT_ERROR              0x01
#define SYS_FONT_INIT_ERROR             0x02
#define SYS_FONT_PATH_ERROR             0x03
#define SYS_WINDOW_INIT_ERROR           0x04
#define SYS_RENDERER_INIT_ERROR         0x05
#define SYS_FPS_TOO_LOW                 0x06
#define SYS_FPS_TOO_HIGH                0x07
//  SYS RESERVED                        0x1f

#define TM_SURFACE_CREATE_ERROR         0x20
#define TM_SURFACE_CONVERT_ERROR        0x21
#define TM_TEXTURE_CREATE_ERROR         0x22
#define TM_TEXTURE_SET_BLENDMODE_ERROR  0x23
#define TM_TEXTURE_UPDATE_ERROR         0x24
#define TM_GOT_NULLPTR_TEX              0x25
#define TM_INVALID_DRECT                0x26        // SDL_Rect dRect           Invalid
#define TM_RCPY_FAILED                  0x27        // SDL_RenderCopy           Failed
#define TM_SRT_FAILED                   0x28        // SDL_SetRenderTarget      Failed
#define TM_SRDC_FAILED                  0x29        // SDL_SetRenderDrawColor   Failed
#define TM_RCLR_FAILED                  0x2a        // SDL_RenderClear          Failed
#define TM_FILL_RECT_ERROR              0x2b        // SDL_RenderFillRect       Failed
#define TM_INVALID_LINE_LENGTH          0x2c
//  TM RESERVED                         0x3f

#define DB_CONNECTION_ERROR             0x40
#define DB_PREPARE_ERROR                0x41
#define DB_EXEC_RESULT_ERROR            0x42
#define DB_EXEC_NOT_PREPARED_ERROR      0x43
#define DB_INVALID_RESULT               0x44
#define DB_INVALID_ROW_COLUMN           0x45
#define DB_INVALID_RES_VALUE            0x46
//  DB RESERVED                         0x5f



// DATE ------------------------------------------------------------------------
typedef struct {
    uint year;
    uint month;
    uint day;
} dateT;

inline std::ostream& operator<<(std::ostream& os, const dateT& date){
    std::ostringstream oss;
    oss << std::setw(4) << std::setfill('0') << date.year << "-"
        << std::setw(2) << std::setfill('0') << date.month << "-"
        << std::setw(2) << std::setfill('0') << date.day;
    os << oss.str();
    return os;
}

inline dateT string2date(const string& dateStr){
    dateT date;
    sscanf(dateStr.c_str(), "%4u-%2u-%2u", &date.year, &date.month, &date.day);
    return date;
}

// TIME ------------------------------------------------------------------------
typedef struct {
    uint hour;
    uint minute;
    uint second;
} timeT;

inline std::ostream& operator<<(std::ostream& os, const timeT time){
    std::ostringstream oss;
    oss << std::setw(2) << std::setfill('0') << time.hour << ":"
        << std::setw(2) << std::setfill('0') << time.minute << ":"
        << std::setw(2) << std::setfill('0') << time.second;
    os << oss.str();
    return os;
}

inline timeT string2time(const string& timeStr){
    timeT time;
    sscanf(timeStr.c_str(), "%2u:%2u:%2u", &time.hour, &time.minute, &time.second);
    return time;
}



// WRAPPERS -------------------------------------------------------------------

inline int SDL_SetRenderDrawColor(SDL_Renderer* renderer, const SDL_Color& color){
    return SDL_SetRenderDrawColor(renderer, color.r, color.g, color.b, color.a);
}

inline int SDL_RenderDrawLine(SDL_Renderer* renderer, const SDL_Point p1, const SDL_Point p2){
    return SDL_RenderDrawLine(renderer, p1.x, p1.y, p2.x, p2.y);
}

#endif

// Creator: @AndrijaRD