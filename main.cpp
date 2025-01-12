#include "lib.h"
#include "System/Sys.h"
#include "TextureManager/TM.h"
#include "PqDB/db.h"
#include "Gui/gui.h"


int main(){
    int error;

    // Init the SDL System, if there is some error, exit
    error = Sys::init("MySDL Window", false, (1920*0.75), (1080*0.75));
    if(error != NO_ERROR) exit(EXIT_FAILURE);
    

    // Init the PosgreSQL DB, check the status using CHECK_ERROR MACRO
    error = DB::init("projectdata", "postgres", "root");
    CHECK_ERROR(error);



    // Just a EXAMPLE of how a statment is prepared for fututre use
    Statement selectUsers = {
        "select-users",             // Name of the statment, anything unique
        "SELECT * FROM users",      // Command to be runned
        0,                          // Numer of parameters that command takes eg. $1, $2, ...   
        SQL_SELECT                  // Type of the command eg. SELECT, INSERT, CREATE, ...
    };
    DB::prepareStatement(selectUsers);  // To make the command ready to use
    


    // Main app loop
    cout << "Game Started...\n\n" << endl;
    while(Sys::isRunning){
        // To update all of the values and clear the screen
        Sys::handleEvents();



        // Code ...

        // example DB Fetch:
        const char* params = {
            // ...
        };
        DBResult res;
        int err = DB::execPrepared(selectUsers, params, res);
        CHECK_ERROR(err);
        if(err) continue;
        cout << res.rowCount() << endl;


        // Example Button ----------------------------------------------------
        int state = GUI::Button(
            "Click me!", 
            {20, 20, 100, 40}, 
            SDL_COLOR_WHITE, 
            {24, 24, 80, 255}
        );

        string text;
        if(state == GUI_CURSOR_HOVERING) text = "Hovering the button";
        if(state == GUI_CURSOR_DRAGGING) text = "Dragging the button";
        if(state == GUI_CURSOR_CLICKED) text = "Clicked the button";
        if(state == GUI_CURSOR_OUTSIDE) text = "Cursor is outside of the button";

        // Example Text ------------------------------------------------------
        SDL_Rect textRect = {20, 140, -1, 40};
        GUI::Text(text, textRect, SDL_COLOR_BLUE);


        // Example Rect ------------------------------------------------------
        SDL_Color color = {
            Sys::getCurrentFrame() % 255,
            255 - (Sys::getCurrentFrame() % 255),
            255,
            180
        };
        GUI::Rect({180, 40, 100, 50}, color);


        
        // Example Input Box -------------------------------------------------
        string value = GUI::Input(
            "input1", 
            {50, 200, 200, 30}, 
            "My Placeholder...",
            SDL_COLOR_WHITE,
            SDL_COLOR_BLACK
        );
        cout << "You Typed: " << value << endl;

        if(state == GUI_CURSOR_CLICKED) GUI::DestroyInput("input1");

        // YOU ARE ABLE TO TRACK THE NUMBER OF LOADED TEXTURES --------------------------
        cout << "Number of loaded Textures: " << TM::getLoadedTextures() << "\n" << endl;
        


        // To render the current frame
        Sys::presentFrame();
    }

    // Free the resources and end the program
    TM::cleanup();
    return Sys::cleanup();
}