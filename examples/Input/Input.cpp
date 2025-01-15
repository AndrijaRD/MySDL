#include "../../lib/Lumos.h"


int main(){
    int error;
    error = Sys::initWindow("Button Example");
    if(error != NO_ERROR) exit(EXIT_FAILURE);

    error = Sys::initFont("../../assets/fonts/font.ttf");
    if(error != NO_ERROR) exit(EXIT_FAILURE);

    // MAIN APP LOOP ---------------------------------------------------
    cout << "Game Started...\n\n" << endl;
    while(Sys::isRunning){
        Sys::handleEvents();


        // App Code and Logic...

        // Input are special since they requre uniqueId
        // In order for library to recognise the same input filed 
        // accross the frames it uses the uniqueId which is just a 
        // unique string passed as the first argument.
        //
        // The second argument is just dRect where to render and how large
        //
        // You are also able to customize the placeholder
        // The default text that is displayed before any 
        // text has been entered by the user
        //
        // And also the background and foreground color
        // Foreground color is the color of the text
        //
        // And the function returns the input
        string value = GUI::Input(
            "MyInput1", 
            {20, 20, 300, 40},
            "username...",
            {40, 40, 40, 255},
            SDL_COLOR_WHITE
        );
        if(!value.empty()) cout << "You have typed:" << value << endl;


        // CUSTOM STYLES
        // We are also able to push some styles to it
        GUI::pushFontSize(26);
        GUI::pushTextAlignX(GUI_ALIGN_CENTER);
        GUI::pushTextAlignY(GUI_ALIGN_TOP);
        GUI::Input("MyInput2", {30, 100, 300, 40}, "Custom Input");


        Sys::presentFrame();
    }

    TM::cleanup();
    return Sys::cleanup();
}