#include "../../lib/Lumos.h"


int main(){
    int error;

    // First thing that needs to be done is for general system
    // to be initialized which created some variabled like:
    //      - Sys::renderer
    //      - Sys::window
    // As arguments it takes window title, fullscreen, and 
    // width and height of the windiw
    error = Sys::initWindow(
        "Empty Window Example", // WINDOW TITLE
        false,                  // SHOULD WINDOW BE FULLSCREEN
        (1920*0.75),            // WINDOW WIDTH
        (1080*0.75)             // WINDOW HEIGHT
    );

    // Now if the error code is not 0 (NO_ERROR) we exit as 
    // that means something went wrong
    if(error != NO_ERROR) exit(EXIT_FAILURE);



    // We can set the window background color using setClearColor
    // and then passing the SDL_Color object.
    // We can also use some built in color:
    //      - SDL_COLOR_BLACK
    //      - SDL_COLOR_WHITE
    //      - ... (SDL_COLOR_*)
    Sys::setClearColor({21, 20, 21, 255});


    // Now the fonts needs to be initialized
    // They allow the app to render GUI elements like
    //      - GUI::Button
    //      - GUI::Text
    //      - GUI::Input
    // The function takes the path to the font
    error = Sys::initFont("../../assets/fonts/font.ttf");
    

    // If the access to the Posgresql Database is needed
    // then this function needs to be runned as it
    // establishes the connection to the database
    error = DB::init("projectdata", "postgres", "root");
    CHECK_ERROR(error);

    // Using CHECK_ERROR(err_code) we can check what
    // error message did the function return the macro
    // will, if error is present, print out the message



    // MAIN APP LOOP ---------------------------------------------------
    cout << "Game Started...\n\n" << endl;
    while(Sys::isRunning){
        // This function needs to be runned
        // at the start of every frame, it:
        //      - Clears the screen
        //      - Updates the variables like mouse position and window size
        //      - Handles events like keypresses and window exit code
        Sys::handleEvents();

        // App Code and Logic...

        // TAnd this is runned at the end of the each frame to make
        // sure that frame rate is stable and to draw the current 
        // renderer onto the screen
        Sys::presentFrame();
    }

    // FPS that app is running on can be changed using:
    //              Sys::setFPS(newFPS)
    // Or you can view the current FPS using:
    //                  Sys::getFPS()




    // At the end of the app these two lines need to be
    // called to makes sure that all of the textures
    // and resources are freed
    TM::cleanup();
    return Sys::cleanup();
}