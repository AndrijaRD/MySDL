#include "Sys.h"


unordered_map<int, string> Sys::errorMap = {
    {NO_ERROR,                          "NO_ERROR"},

    {SYS_SDL_INIT_ERROR,                "SYS_SDL_INIT_ERROR"},
    {SYS_FONT_INIT_ERROR,               "SYS_FONT_INIT_ERROR"},
    {SYS_FONT_PATH_ERROR,               "SYS_FONT_PATH_ERROR"},
    {SYS_WINDOW_INIT_ERROR,             "SYS_WINDOW_INIT_ERROR"},
    {SYS_RENDERER_INIT_ERROR,           "SYS_RENDERER_INIT_ERROR"},
    {SYS_FPS_TOO_LOW,                   "SYS_FPS_TOO_LOW"},
    {SYS_FPS_TOO_HIGH,                  "SYS_FPS_TOO_HIGH"},

    {TM_SURFACE_CREATE_ERROR,           "TM_SURFACE_CREATE_ERROR"},
    {TM_SURFACE_CONVERT_ERROR,          "TM_SURFACE_CONVERT_ERROR"},
    {TM_TEXTURE_CREATE_ERROR,           "TM_TEXTURE_CREATE_ERROR"},
    {TM_TEXTURE_SET_BLENDMODE_ERROR,    "TM_TEXTURE_SET_BLENDMODE_ERROR"},
    {TM_TEXTURE_UPDATE_ERROR,           "TM_TEXTURE_UPDATE_ERROR"},
    {TM_GOT_NULLPTR_TEX,                "TM_GOT_NULLPTR_TEX"},
    {TM_INVALID_DRECT,                  "TM_INVALID_DRECT"},
    {TM_RCPY_FAILED,                    "TM_RCPY_FAILED"},
    {TM_SRT_FAILED,                     "TM_SRT_FAILED"},
    {TM_SRDC_FAILED,                    "TM_SRDC_FAILED"},
    {TM_RCLR_FAILED,                    "TM_RCLR_FAILED"},
    {TM_FILL_RECT_ERROR,                "TM_FILL_RECT_ERROR"},
    {TM_INVALID_LINE_LENGTH,            "TM_INVALID_LINE_LENGTH"},
    

    {DB_CONNECTION_ERROR,               "DB_CONNECTION_ERROR"},
    {DB_PREPARE_ERROR,                  "DB_PREPARE_ERROR"},
    {DB_EXEC_RESULT_ERROR,              "DB_EXEC_RESULT_ERROR"},
    {DB_EXEC_NOT_PREPARED_ERROR,        "DB_EXEC_NOT_PREPARED_ERROR"},
    {DB_INVALID_RESULT,                 "DB_INVALID_RESULT"},
    {DB_INVALID_ROW_COLUMN,             "DB_INVALID_ROW_COLUMN"},
    {DB_INVALID_RES_VALUE,              "DB_INVALID_RES_VALUE"}
};


/** System Init
 * 
 * Sets up the entier SDL system.
 * Creates Window.
 * Creates Renderer.
 * 
 * @return 0 on success and positive on error, coresponding to ERROR DEFINITIONS
 */
int Sys::initWindow(
    const string& winTitle,
    const bool& fullscreen,
    const int& windowWidth,
    const int& windowHeight
){
    Sys::windowTitle = winTitle;
    Sys::isFullscreen = fullscreen;
    Sys::wWidth = windowWidth;
    Sys::wHeight = windowHeight;

    // SDL INIT ----------------------------------------------------------
    int status = SDL_Init(SDL_INIT_EVERYTHING);
    if(status == 0){
        cout << "[INIT] Subsystem Initialized..." << endl;
    } else {
        cout << "[FATAL] Failed to initialize subsystems!" << endl;
        return SYS_SDL_INIT_ERROR;
    }


    // CREATE WINDOW ------------------------------------------------------
    int flags = 0;
    if(isFullscreen) flags = SDL_WINDOW_FULLSCREEN;

    win = SDL_CreateWindow(
        windowTitle.c_str(), 
        SDL_WINDOWPOS_CENTERED, 
        SDL_WINDOWPOS_CENTERED, 
        wWidth, 
        wHeight, 
        flags
    );
    if(win){
        cout << "[INIT] Window created..." << endl;
    } else{
        cout << "[FATAL] Failed to create window!" << endl;
        return SYS_WINDOW_INIT_ERROR;
    }


    // CREATE RENDERER -----------------------------------------------------
    r = SDL_CreateRenderer(win, -1, 0);
    if(r){
        cout << "[INIT] Renderer created..." << endl;
    }
    else{
        cout << "[FATAL] Failed to create rederer!" << endl;
        return SYS_RENDERER_INIT_ERROR;
    }
    SDL_SetRenderDrawBlendMode(r, SDL_BLENDMODE_BLEND);

    return NO_ERROR;
}




/** Font Init
 * 
 * Initializes the Fonts.
 * 
 * @return 0 on success and positive on error, coresponding to ERROR DEFINITIONS
 */
int Sys::initFont(const string& fontPath){
    // FONT INIT ---------------------------------------------------------
    int status = TTF_Init();
    if(status != 0){
        cout << "[FATAL] Failed to initialize fonts!" << endl;
        return SYS_FONT_INIT_ERROR;
    }

    // Create Font Object
    font = TTF_OpenFont(fontPath.c_str(), 108);
    if(font == nullptr){
        cout << "[FATAL] Failed to load font!" << endl;
        return SYS_FONT_PATH_ERROR;
    }

    SDL_SetHint(SDL_HINT_RENDER_SCALE_QUALITY, "1");

    cout << "[INIT] Fonts Initialized..." << endl;
    return NO_ERROR;
}




/** Handle Events
 * 
 * This function should be runned at the start of each frame.
 * 
 * It updates all of the values accordingly and clears the screen.
 * 
 * @return 0 on success and positive on error, coresponding to ERROR DEFINITIONS
 */
int Sys::handleEvents(){
    uint64_t error = NO_ERROR;

    // Calculate the delta Time --------------------------------------------------------------------------------------
    frameStart = SDL_GetTicks();

    // Avoid calculation in the first frame
    if (previousFrameStart != 0) 
        deltaTime = (frameStart - previousFrameStart);
    
    previousFrameStart = frameStart;



    // Clear the screen ----------------------------------------------------------------------------------------------
    SDL_SetRenderDrawColor(Sys::r, clearColor);
    SDL_RenderClear(Sys::r);


    // Keyboard Focus
    if(Keyboard::pendingUnFocus) Keyboard::focused = false;
    if(Keyboard::pendingFocus) Keyboard::focused = true;
    Keyboard::pendingFocus = false;
    Keyboard::pendingUnFocus = false;


    // GET NEW FRAME VALUES -------------------------------------------------------------------------------------------
    SDL_GetWindowSize(Sys::win, &Sys::wWidth, &Sys::wHeight);               // Getting window width and height
    Uint32 mouseState = SDL_GetMouseState(&Mouse::pos.x, &Mouse::pos.y);    // Getting mouse states and position

    // Calculating the new frame mouse status, is it down
    bool isMouseDown = mouseState & SDL_BUTTON(SDL_BUTTON_LEFT);
    
    // If the mouse was clicked last frame (Mouse::down) but now its not
    // that means that it was released this frame, making it a compleat "click"
    Mouse::clicked = false; // Firstly reset it
    if(Mouse::down && !isMouseDown) Mouse::clicked = true;

    // Now update the Mouse::down
    Mouse::down = isMouseDown;



    // HANDLE EVENTS --------------------------------------------------------------------------------------------------
    SDL_Event event;
    Keyboard::text.clear();
    Keyboard::keyUp = 0;
    Keyboard::keyDown = 0;

    while(SDL_PollEvent(&event)){
        if(event.type == SDL_QUIT){
            isRunning = false;
        }

        if(event.type == SDL_KEYUP){
            Keyboard::keyUp = event.key.keysym.sym;
        }

        if(event.type == SDL_KEYDOWN){
            Keyboard::keyDown = event.key.keysym.sym;
        }

        if(event.type == SDL_TEXTINPUT){
            Keyboard::text += event.text.text;
        }
    }

    return error;
}




/** Present Frame
 * 
 * This function should be called at the end of each frame cycle.
 * 
 * It presents the frame and and makes a delay for stable FPS and delta time.
 * 
 * @return 0 on success and positive on error, coresponding to ERROR DEFINITIONS
 */
int Sys::presentFrame(){
    uint64_t error = NO_ERROR;

    // PRESENT THE NEW FRAME ON THE SCREEN ----------------------------------------------------------------------------
    SDL_RenderPresent(Sys::r);


    // FRAME DELAY ----------------------------------------------------------------------------------------------------
    Uint64 currentFrameDuration = SDL_GetTicks() - frameStart;
    if((Uint64)(1000 / FPS) > currentFrameDuration){
        uint64_t timeRemaining = (1000 / FPS) - currentFrameDuration;
        SDL_Delay(timeRemaining);

        // if(dynamicFPS){
        //     int additionalFrames = timeRemaining / (1000 / FPS);
        //     FPS = std::min(maxFPS, FPS + additionalFrames);
        // }
    } else {
        error = SYS_FPS_TOO_HIGH;
        // if(dynamicFPS) {
        //     FPS = std::max(minFPS, FPS - 5);
        // }
    }


    // UPDATE FRAME COUNTER -------------------------------------------------------------------------------------------
    frameCounter++;
    
    // Since the int has a finite size if it comes to its limit, the counter is restarted
    if(frameCounter >= 0xFFFFFFFF) frameCounter = 0; 

    return error;
}




/** Cleanup
 * 
 * This function should be called at the end of the application to properly shutdown all of the systems.
 *
 * @return 0 on success and positive on error, coresponding to ERROR DEFINITIONS
 */
int Sys::cleanup(){
    // DESTROY AND FREE EVERYTHING ------------------------------------------------------------------------------------
    SDL_DestroyWindow(win);
    SDL_DestroyRenderer(r);
    TTF_Quit();
    SDL_Quit();

    cout << "Game Finished." << endl;
    return NO_ERROR;
}



string Sys::checkError(int error){
    if (errorMap.count(error)) {
        return errorMap[error];
    } else {
        return "Unknown Error";
    }
}


int Sys::getOS() { return OS; }
void Sys::setClearColor(const SDL_Color& color) { clearColor = color; }


int Sys::getFPS() { return FPS; }
void Sys::setFPS(const int& newFPS ) { FPS = min(max(20, newFPS), 144); }
// void Sys::setDynamicFPS(bool dFPS, int maxF, int minF){
//     dynamicFPS = dFPS;
//     maxFPS = std::max(20, maxF);
//     minFPS = std::min(20, minF);
// }
int Sys::getCurrentFrame() { return frameCounter; }


SDL_Point Sys::Mouse::getPos() { return pos; }
bool Sys::Mouse::isClicked() { return clicked; }
bool Sys::Mouse::isDown() { return down; }
bool Sys::Mouse::isHovering(const SDL_Rect& rect) { 
    return pos.x >= rect.x && pos.x <= rect.x+rect.w &&
        pos.y >= rect.y && pos.y <= rect.y+rect.h; 
}


SDL_Keycode Sys::Keyboard::getKeyUp() { return keyUp; }
SDL_Keycode Sys::Keyboard::getKeyDown() { return keyDown; }
string Sys::Keyboard::getText() { return text; }

bool Sys::Keyboard::isFocused() { return focused; }
void Sys::Keyboard::focus() { pendingFocus = true; }
void Sys::Keyboard::unfocus() { pendingUnFocus = true; }