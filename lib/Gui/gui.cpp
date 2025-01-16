#include "./gui.h"
#include "../System/Sys.h"





/** Remove Oldest
 * 
 * INTERNAL USE
 * 
 * Used for removing the oldest text texture of loadedTexts map.
 */
void GUI::removeOldest(){
    // Here will be stored the oldest objest
    LoadedText* oldest = nullptr;
    
    // Iterate trough entier map and memorize the item with lowst frame value
    for(auto& lt : loadedTexts){
        if(oldest == nullptr || *oldest > lt.second){
            oldest = &(lt.second);
        }
    }

    // Free the texture and erase the item from the map
    TM::freeTexture(oldest->td);
    loadedTexts.erase(oldest->title);
}




string color2hex(const SDL_Color& color){
    ostringstream oss;
    oss << "#";
    oss << hex << setw(2) << setfill('0') << static_cast<int>(color.r);
    oss << hex << setw(2) << setfill('0') << static_cast<int>(color.g);
    oss << hex << setw(2) << setfill('0') << static_cast<int>(color.b);
    oss << hex << setw(2) << setfill('0') << static_cast<int>(color.a);
    return oss.str();
}




/** Load New Text
 * 
 * INTERNAL FUNCTION
 * 
 * Funtion for inserting a new text texture into map.
 * It also calles removeOldest if map is full.
 */
GUI::LoadedText* GUI::loadNewText(const string& title, const SDL_Color& color){
    // If there is more loaded Textures then allowed, remove the oldest
    if(loadedTexts.size() >= NUM_OF_LOADED_TEXTURES){
        removeOldest();
    }

    // Create the new LoadedText item
    LoadedText newText;
    newText.color = color;
    newText.frame = Sys::getCurrentFrame();
    newText.title = title;

    // Create the texture
    int err = TM::createTextTexture(newText.td, newText.title, newText.color);
    CHECK_ERROR(err);

    // Insert the item and return the pointer to it
    loadedTexts.insert({newText.getId(), newText});
    return &(loadedTexts.at(newText.getId()));
}




/** GUI Button
 * 
 * This function renders a button. It supports all style pushs.
 * 
 * @param title Button text, which is dinamicly calculated to fit into the button and to be centered
 * @param dRect It represents position and size of the button, must be possitive, all of the value
 * 
 * @returns It returns the state of the button: 
 *  `GUI_CURSOR_CLICKED`, 
 *  `GUI_CURSOR_HOVERING`,
 *  `GUI_CUROSR_DRAGGING`,
 *  `GUI_CURSOR_OUTSIDE`
 * 
 */
int GUI::Button(
    const string& title, 
    const SDL_Rect& dRect,
    const SDL_Color& textColor,
    const SDL_Color& buttonColor
){
    // COPY STYLES -------------------------------------------------------
    // First we copy the pushed styles
    int fontSize = GUI::pFontSize;
    GUI::pFontSize = -1;

    int textAlignX = GUI::pTextAlignX;
    if(textAlignX == -1) textAlignX = GUI_ALIGN_CENTER; // Default
    GUI::pTextAlignX = -1;

    int textAlignY = GUI::pTextAlignY;
    if(textAlignY == -1) textAlignY = GUI_ALIGN_CENTER; // Default
    GUI::pTextAlignY = -1;



    // CHECK VALIDITY OF dRect -------------------------------------------
    if(dRect.w < 1 || dRect.h < 1) return GUI_CURSOR_OUTSIDE;



    // FIND BUTTON TEXTURE
    LoadedText* textPointer = nullptr;
    string id = title + color2hex(textColor);

    auto it = loadedTexts.find(id);
    if (it != loadedTexts.end()) {
        // Save the pointer to that LOadedText item
        textPointer = &it->second;
        
        // Update with the current frame
        textPointer->frame = Sys::getCurrentFrame();
    } else {
        // Create new Text Texture
        textPointer = loadNewText(title, textColor);
    }



    // BUTTON BACK -------------------------------------------------------
    // Render the buttons background, filled rect, 
    // and then on top of it will be white text
    SDL_SetRenderDrawColor(Sys::renderer, buttonColor);
    SDL_RenderFillRect(Sys::renderer, &dRect);
    


    // Add a 5px padding on the sides, unless the total 
    // width or height of the button is less then 10x10
    int padding = 5;
    if(dRect.w < 10 || dRect.h < 10) padding = 0;

    // Calculate what is the max width and height of 
    // the text that can fit in the button without overflow
    int maxTextWidth = dRect.w - padding*2;
    int maxTextHeight = dRect.h - padding*2;

    // TEXT DRECT --------------------------------------------------------
    SDL_Rect text_dRect;

    if(fontSize == -1){
        // if required width is less then maximal allowed then
        // the height is the limiting factor, so its max height

        // We calc the text width in case of max height and compare it to max allowed width
        int textWidth = textPointer->td.width * (float)maxTextHeight / textPointer->td.height;
        if(textWidth <= maxTextWidth){
            // This means that text is maximal height, 
            // and width is less then max, there will be
            // left-right free space
            text_dRect.h = maxTextHeight;
        } else {
            // This means that text is maximal width, 
            // and height is less then max, there will be
            // top-bottom free space
            textWidth = maxTextWidth;
            text_dRect.h = textPointer->td.height * (float)maxTextWidth / textPointer->td.width;
        }
    } else{
        text_dRect.h = fontSize;
    }


    // DRECT WIDTH -----------------------------------------------------------------
    text_dRect.w = text_dRect.h * (float)textPointer->td.width / textPointer->td.height;

    // DRECT X POS
    if(textAlignX == GUI_ALIGN_LEFT){
        text_dRect.x = dRect.x + padding;
    } else if(textAlignX == GUI_ALIGN_CENTER){
        text_dRect.x = dRect.x + dRect.w/2 - text_dRect.w/2;
    } else if(textAlignX == GUI_ALIGN_RIGHT){
        text_dRect.x = dRect.x + dRect.w - padding - text_dRect.w;
    }

    // DRECT Y POS
    if(textAlignY == GUI_ALIGN_TOP){
        text_dRect.y = dRect.y + padding;
    } else if(textAlignY == GUI_ALIGN_CENTER) {
        text_dRect.y = dRect.y + dRect.h/2 - text_dRect.h/2;
    } else if(textAlignY == GUI_ALIGN_BOTTOM) {
        text_dRect.y = dRect.y + dRect.h - padding - text_dRect.h;
    }


    // Now render the text
    int err = TM::renderTexture(textPointer->td, text_dRect);
    CHECK_ERROR(err);

    if(Sys::Mouse::isHovering(dRect)){
        if(Sys::Mouse::isClicked()){
            return GUI_CURSOR_CLICKED;
        }
        if(Sys::Mouse::isDown()){
            return GUI_CURSOR_DRAGGING;
        }
        return GUI_CURSOR_HOVERING;
    }

    return GUI_CURSOR_OUTSIDE;
}





/** Text
 * 
 * Draws a optimized verion of text on the screen.
 * It doesnt respond to any push styles since all of them
 * can be controlled trough dRect param.
 * 
 * @param title Text to be rendered.
 * @param dRect position and size of the text, one dimension can be left as -1 for
 *              dynamic calculation, eg. set height to font size and width as -1
 *              and after the render you can check dRect that was passed and the
 *              width will be calculated and dRect will be updated, replacing -1
 * @param color The color of the text
 * 
 */
void GUI::Text(const string& title, SDL_Rect& dRect, const SDL_Color& color){
    if(dRect.w < 1 && dRect.h < 1) return;

    LoadedText* textPointer = nullptr;
    string id = title + color2hex(color);

    auto it = loadedTexts.find(id);
    if (it != loadedTexts.end()) {
        // Save the pointer to that LoadedText item
        textPointer = &it->second;
        
        // Update with the current frame
        textPointer->frame = Sys::getCurrentFrame();
    } else {
        // Create new Text Texture
        textPointer = loadNewText(title, color);
    }

    // Render the texture
    TM::renderTexture(textPointer->td, dRect);
}




/** Text Dynamic
 * 
 * Draws a text that is not cached anywhere, the texture is created,
 * rendered and then destroyed, its not optimal, but it shold be used for 
 * highly changable texts, like the ones that change every frame so there
 * is no point is storing them as they will never be reused unline regular
 * GUI::Text which is optimised so it stores all texture in case of re-use.
 * 
 * @param title Text to be rendered.
 * @param dRect position and size of the text, one dimension can be left as -1 for
 *              dynamic calculation, eg. set height to font size and width as -1
 *              and after the render you can check dRect that was passed and the
 *              width will be calculated and dRect will be updated, replacing -1
 * @param color The color of the text
 * 
 */
void GUI::TextDynamic(const string& title, SDL_Rect& dRect, const SDL_Color& color){
    if(dRect.w < 1 && dRect.h < 1) return;

    // Create the texture
    TextureData td;
    int err = TM::createTextTexture(td, title, color);
    CHECK_ERROR(err);

    // Render the texture
    TM::renderTexture(td, dRect);

    // Free Texture Data
    TM::freeTexture(td); 
}






/** Rect
 * 
 * This function can currently draw either
 * a filled rect or a rect outline with 1px lines.
 * 
 * @param dRect the position and size of the rect
 * @param colo the rects color
 * @param thickness if its -1 then it will be filled rect, otherwise outline
 * 
 */
void GUI::Rect(
    const SDL_Rect& dRect,
    const SDL_Color& color,
    const int thickness
){
    SDL_SetRenderDrawColor(Sys::renderer, color);

    if(thickness == -1){
        SDL_RenderFillRect(Sys::renderer, &dRect);
    } else {
        SDL_RenderDrawRect(Sys::renderer, &dRect);
    }
}





/** Line
 * 
 * Draws a 1px thick line from p1 to p2 with custom color
 * 
 * @param p1 SDL_Point line start coordinates
 * @param p2 SDL_Point line end coordinates
 * @param color SDL_Color line color
 */
void GUI::Line(
    const SDL_Point& p1,
    const SDL_Point& p2,
    const SDL_Color& color
){
    SDL_SetRenderDrawColor(Sys::renderer, color);
    SDL_RenderDrawLine(Sys::renderer, p1, p2);
}





/** Input
 * 
 * Renders a Input field using a unique id.
 * 
 * The UniqueId should not be changes and needs to be
 * unique to each input field as it is used for accessing
 * the stored state of the field. Returing the value of
 * the filed, the user inputed text.
 * 
 * @param uniqueId A unique string intrenaly used to itentify the input field acrros diffrent frames
 * @param dRect A SDL_Rect representing where and what size should field be rendered
 * @param placeholder A text displayed on the input filed when input is empty
 * @param background SDL_Color representing the background field color
 * @param foreground SDL_Color representing the text color 
 * @return The value of the field
 */
string GUI::Input(
    const string& uniqueId,
    const SDL_Rect& dRect,
    const string& placeholder,
    const SDL_Color& background,
    const SDL_Color& foreground
){  
    /**
     * Improvements:
     *      - Char Position changer, like give the arrows some functionality
     *        left and right curosr movement, on each focus it should be set to
     *        max, the last char, but also make it changable using arrows and even
     *        with mouse click, advanced
     *        ! Problem: Each char should then be treated individualy as the program
     *                   must known where to draw the caret, where each char begins
     */


    // COPY STYLES -------------------------------------------------------
    // First we copy the pushed styles
    int fontSize = GUI::pFontSize;
    GUI::pFontSize = -1;

    int textAlignX = GUI::pTextAlignX;
    if(textAlignX == -1) textAlignX = GUI_ALIGN_LEFT; // Default
    GUI::pTextAlignX = -1;

    int textAlignY = GUI::pTextAlignY;
    if(textAlignY == -1) textAlignY = GUI_ALIGN_CENTER; // Default
    GUI::pTextAlignY = -1;



    // FIND STATE --------------------------------------------------------
    // Get the pointer to the buttons state using uniqueId
    InputState* state = nullptr;
    auto it = inputStates.find(uniqueId);
    if(it != inputStates.end()){
        state = &it->second;
    } else{
        // If the state doesnt exist, create it
        inputStates.insert({uniqueId, InputState(uniqueId)});
        state = &inputStates.at(uniqueId);
    }



    // DRAW BACKGROUND ---------------------------------------------------
    GUI::Rect(dRect, background);           // Background field
    GUI::Rect(dRect, SDL_COLOR_BLACK, 1);   // 1px black Outline



    // DEFINE PADDING ----------------------------------------------------
    // This is the paddings for the text relative to the field
    int paddingX = 5;
    int paddingY = 3;



    // TEXT RECT CALCULATION ---------------------------------------------
    SDL_Rect textRect; // Calculate the position and size of the text

    // TEXT RECT HEIGHT --------------------------------------------------
    // If there is no fontSize speicified make the font max height
    if(fontSize == -1){
        textRect.h = dRect.h - paddingY*2;
    } else {
        textRect.h = fontSize;
    }

    // TEXT RECT WIDTH ---------------------------------------------------
    textRect.w = textRect.h * (float)state->td.width / state->td.height;

    // TEXT RECT X POS ---------------------------------------------------
    // Depending on the placement, left, center, right. Default: left
    if(textAlignX == GUI_ALIGN_LEFT){
        textRect.x = dRect.x + paddingX;
    } else if(textAlignX == GUI_ALIGN_RIGHT){
        textRect.x = dRect.x + dRect.w - paddingX - textRect.w;
    } else if(textAlignX == GUI_ALIGN_CENTER){
        textRect.x = dRect.x + dRect.w/2 - textRect.w/2;
    }

    // TEXT RECT Y POS ---------------------------------------------------
    if(textAlignY == GUI_ALIGN_BOTTOM){
        textRect.y = dRect.y + dRect.h - paddingY - textRect.h;
    } else if(textAlignY == GUI_ALIGN_CENTER){
        textRect.y = dRect.y + dRect.h/2 - textRect.h/2;
    } else if(textAlignY == GUI_ALIGN_TOP){
        textRect.y = dRect.y + paddingY;
    }
    

    // Render the text
    TM::renderTexture(state->td, textRect);

    // Check if placeholder was rendered
    bool placeholderActive = false;
    if(state->value == "") placeholderActive = true;

    // If the rendered text is larger then the allowed width
    // and we are also not talking about the placeholder THEN
    // make the removed variable larger by one, signaling to the
    // next frame that it should cut out one more frame from the left
    if(textRect.w > (dRect.w - paddingX*2) && !placeholderActive){
        state->removed++;
        state->change = true;
    }



    // If the keyboard is focused on the field draw the caret, text cursor
    if(state->focused){
        // The cursor would be drawn for half a second and then be not drawn for 
        // another half a second, this is acheved trough clock which changes value
        // every (FPS / 2)frames so it can be devieded using %2 operator
        int blinkClock = static_cast<int>( Sys::getCurrentFrame() / (Sys::FPS / 2) );
        if(blinkClock % 2){
            // Top point of the line, default the
            // x is equal to the starting position of the text
            SDL_Point p1 = {0, dRect.y + paddingY};
            
            // If the placeholder is not active then draw the 
            // line at the end of the text.
            // Else, the placeholder is active, then draw the line
            // just few pixels behind the placeholder, so its vissible
            if(placeholderActive){
                if(textAlignX == GUI_ALIGN_LEFT){
                    p1.x = dRect.x + paddingX;
                } else if(textAlignX == GUI_ALIGN_CENTER){
                    p1.x = dRect.x + dRect.w/2;
                } else if(textAlignX == GUI_ALIGN_RIGHT){
                    p1.x = dRect.x + dRect.w - paddingX;
                }
            } else {
                p1.x = textRect.x + textRect.w + 1;
            }

            // Bottom point of the line
            SDL_Point p2 = {
                p1.x, 
                p1.y + (dRect.h - paddingY*2)
            };
            
            // Draw the line
            GUI::Line(p1, p2, SDL_COLOR_BLACK);
        }
    }
    

    // EVENTS ---------------------------------
    // If the user is hovering the input filed
    // and he clicks then focus the input onto the filed
    if(
        Sys::Mouse::isHovering(dRect) &&
        Sys::Mouse::isClicked()
    ) {
        Sys::Keyboard::focus();
        state->focused = true;
    }

    // If user is not hovering the button, cursor is outside
    // and there is click, that means that there has been unfocus
    if(
        !Sys::Mouse::isHovering(dRect) &&
        Sys::Mouse::isClicked()
    ){
        Sys::Keyboard::unfocus();
        state->focused = false;
    }

    // A small lambda funtion for handling deletion
    auto deleteLastChar = [&]() {
        // If value is already empty skip
        if (state->value.empty()) return;

        // Pop last char and mark change has been made
        state->value.pop_back();
        state->change = true;

        // If there was any chars to be removed, decrease the amount
        if (state->removed != 0) state->removed--;
    };

    // If the input is focused handle key presses
    if(state->focused){
        // Add the text inputed into the value
        if(Sys::Keyboard::getText() != "") {
            state->change = true;
            state->value += Sys::Keyboard::getText();
        }

        // If the backspace has been pressed handle deletion
        if(Sys::Keyboard::getKeyDown() == SDLK_BACKSPACE){
            // If the input is not in the state of deletion
            // that menas that this is the first frame that
            // backspace was pressed, so delete the last char
            // and mark that the hold of the button has begun
            if(!state->deleting){
                deleteLastChar();
                state->deleting = true;
            } else{
                // If the deletion has been going, and the
                // backspace is still down, holding, every
                // 200ms delete one last char from the field
                if(Sys::getCurrentFrame() % (Sys::FPS / 5)){
                    deleteLastChar();
                }
            }
        } else{
            // If the backspace is not pressed end the deletion process
            state->deleting = false;
        }
    }

    // Now handle the value changes and updating the texture
    if(state->change){
        TM::freeTexture(state->td);
        
        string textToCompile;
        SDL_Color colorOfText = foreground;
        if(state->value == ""){
            colorOfText.a *= 0.75;
            textToCompile = placeholder;
        } else {
            // So bc text can get longer then it can fit in the field
            // program also keeps track of how many characters it needs
            // to hide from the left side in order for text from the right
            // side to be vissible and with in the bounds
            textToCompile = state->value;
            textToCompile.erase(0, state->removed);
        }

        TM::createTextTexture(state->td, textToCompile, colorOfText);
        state->change = false;
    }

    // Return the value
    return state->value;
}





/** Destroy Input / Reset Input
 * 
 * This function  should be called if you need to 
 * restart the field or you are sure that its out 
 * of scope and its data is no longer important.
 * 
 * @param uniqueId Id of the input to be reset, destroyed.
 */
void GUI::DestroyInput(const string& uniqueId){
    InputState* state = nullptr;
    auto it = inputStates.find(uniqueId);
    if(it != inputStates.end()){
        state = &it->second;
    } else{
        return;
    }

    TM::freeTexture(state->td);
    inputStates.erase(it);
    return;
}




void GUI::pushFontSize(const int& fontSize) { pFontSize = fontSize; }
void GUI::pushTextAlignX(const int& value)  { pTextAlignX = value;  }
void GUI::pushTextAlignY(const int& value)  { pTextAlignY = value;  }
