#include "gui.h"
#include "System/Sys.h"



string color2hex(const SDL_Color& color){
    ostringstream oss;
    oss << "#";
    oss << hex << setw(2) << setfill('0') << static_cast<int>(color.r);
    oss << hex << setw(2) << setfill('0') << static_cast<int>(color.g);
    oss << hex << setw(2) << setfill('0') << static_cast<int>(color.b);
    oss << hex << setw(2) << setfill('0') << static_cast<int>(color.a);
    return oss.str();
}


GUI::LoadedText* GUI::loadNewText(const string& title, const SDL_Color& color){
    // If there is more loaded Textures then allowed, remove the oldest
    if(loadedTexts.size() >= NUM_OF_LOADED_TEXTURES){
        
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
 * This function renders a button.
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
    // Improvements:
    //      - Text color:
    //          To identifier of the unrodered_map loadedText add the text color like:
    //          "Click Me!" + "#ffffffff" so in the map its "Click me!#ffffffff"
    //          So user can render buttons with custom colors and have them PreCompiled
    //      - Backgound color:
    //          Just to be able to change the color of the button it self, 
    //          insted of it being black like only current state now


    if(dRect.w < 1 || dRect.h < 1) return GUI_CURSOR_OUTSIDE;

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

    // Render the buttons background, filled rect, 
    // and then on top of it will be white text
    SDL_SetRenderDrawColor(Sys::renderer, buttonColor);
    SDL_RenderFillRect(Sys::renderer, &dRect);
    cout << "Draw from y:" << dRect.y << " to " << dRect.y+dRect.h << endl;

    // Add a 5px padding on the sides, unless the total 
    // width or height of the button is less then 10x10
    int padding = 5;
    if(dRect.w < 10 || dRect.h < 10) padding = 0;

    // Calculate what is the max width and height of 
    // the text that can fit in the button without overflow
    int maxTextWidth = dRect.w - padding*2;
    int maxTextHeight = dRect.h - padding*2;

    // Now for the first check, we set up the texts height to
    // maximal height and then using aspect ratio calculate the width
    // so we can check if that with can fit in the maxTextWidth
    int textWidth = textPointer->td.width * (float)maxTextHeight / textPointer->td.height;
    int textHeight = maxTextHeight;

    // If it can fit, render it
    if(textWidth <= maxTextWidth){
        // Also calculate the difference between maxWidth and realWidth
        // so the text can be horizontaly centered in the button
        int difference = maxTextWidth - textWidth;
        SDL_Rect text_dRect = {
            dRect.x + padding + difference/2,
            dRect.y + padding,
            textWidth,
            textHeight
        };

        // Now render the text
        int err = TM::renderTexture(textPointer->td, text_dRect);
        CHECK_ERROR(err);

    } else {
        // If the text wasnt able to fit in the button with the
        // max height, then we will make text as wide as possible
        // and height will be calculated dinamicly using aspect
        // ratio, just like width was in the begginging.
        textWidth = maxTextWidth;
        textHeight = textPointer->td.height * (float)maxTextWidth / textPointer->td.width;

        // Find the difference to make the text verticly centered in the button
        int difference = maxTextHeight - textHeight;
        SDL_Rect text_dRect = {
            dRect.x + padding,
            dRect.y + padding + difference/2,
            textWidth,
            textHeight
        };

        // Now render the text
        int err = TM::renderTexture(textPointer->td, text_dRect);
        CHECK_ERROR(err);
    }

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




void GUI::Rect(
    const SDL_Rect& dRect,
    const SDL_Color& color,
    const int thickness
){
    SDL_SetRenderDrawColor(Sys::renderer, color);
    cout << "Draw from y:" << dRect.y << " to " << dRect.y+dRect.h << endl;

    if(thickness == -1){
        SDL_RenderFillRect(Sys::renderer, &dRect);
    } else {
        SDL_RenderDrawRect(Sys::renderer, &dRect);
    }
}



void GUI::Line(
    const SDL_Point& p1,
    const SDL_Point& p2,
    const SDL_Color& color,
    const int thickness
){
    SDL_SetRenderDrawColor(Sys::renderer, color);
    SDL_RenderDrawLine(Sys::renderer, p1, p2);
}

