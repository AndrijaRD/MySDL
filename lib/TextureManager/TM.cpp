#include "./TM.h"
#include "../System/Sys.h"
#include "../Gui/gui.h"



/** Load Texture
 * 
 * Load an image from filesystem into TextureData object.
 * 
 * @param td TextureData object into which image should be loaded.
 * @param path Path to the image on the filesystem
 * @return Returns 0 on success and positive number on error, coresponding to the ERROR DEFINITIONS 
 */
int TM::loadTexture(
    TextureData& td, 
    const string& path
){
    // Ensure that privous Texture is de-loaded -----------------------------------------
    if(td.tex != nullptr) TM::freeTexture(td);

    // Load the image -------------------------------------------------------------------
    SDL_Surface* surface = IMG_Load(path.c_str());
    if(surface == nullptr) return TM_SURFACE_CREATE_ERROR;

    // Check the format of the image ----------------------------------------------------
    if(surface->format->BitsPerPixel != 32){
        surface = SDL_ConvertSurfaceFormat(surface, SDL_PIXELFORMAT_RGBA32, 0);
        if(surface == nullptr){
            return TM_SURFACE_CONVERT_ERROR;
        }
    }


    // CREATING THE TEXTURE --------------------------------------------------------------
    td.tex = SDL_CreateTexture(
        Sys::renderer,
        SDL_PIXELFORMAT_RGBA32,
        SDL_TEXTUREACCESS_TARGET,
        surface->w,
        surface->h
    );
    if(td.tex == nullptr) {
        SDL_FreeSurface(surface);
        return TM_TEXTURE_CREATE_ERROR;
    }


    // FILL THE TEXTURE WITH THE IMAGE DATA -----------------------------------------------
    int s = SDL_UpdateTexture(td.tex, NULL, surface->pixels, surface->pitch);
    if(s != 0){
        SDL_FreeSurface(surface);
        SDL_DestroyTexture(td.tex);
        return TM_TEXTURE_UPDATE_ERROR;
    }


    // CLEAN UP ---------------------------------------------------------------------------
    SDL_FreeSurface(surface);
    loadedTextures.push_back(td.tex);

    // MAKE TEXTURE UPDATATBLE/MODIFIABLE -------------------------------------------------
    if(SDL_SetTextureBlendMode(td.tex, SDL_BLENDMODE_BLEND)){
        SDL_FreeSurface(surface);
        SDL_DestroyTexture(td.tex);
        return TM_TEXTURE_SET_BLENDMODE_ERROR;
    }

    // GET TEXTURE DIMENSIONS -------------------------------------------------------------
    SDL_QueryTexture(td.tex, &td.format, NULL, &td.width, &td.height);
    
    return NO_ERROR;
}




/** Free Texture
 * 
 * Frees the texture from memory and sets the TextureData properties to null
 * @param td TextureData to be freeed
 */
void TM::freeTexture(TextureData& td){
    if(td.tex != nullptr){

        auto it = std::remove(loadedTextures.begin(), loadedTextures.end(), td.tex);
        loadedTextures.erase(it, loadedTextures.end());

        SDL_DestroyTexture(td.tex);
        td.tex = nullptr;
    }

    td.width = 0;
    td.height = 0;
    td.format = 0;
}

// This should be avoided as it leaves dangling pointer and risks the use-after-free segmentation fault

// void TM::freeTexture(SDL_Texture* tex){
//     if(tex != nullptr){
//         auto it = std::remove(loadedTextures.begin(), loadedTextures.end(), tex);
//         loadedTextures.erase(it, loadedTextures.end());
//
//         SDL_DestroyTexture(tex);
//     }
// }





/** Cleanup 
 * 
 * Called at the end of the program to free all of the textures
*/
void TM::cleanup(){
    for(auto tex : loadedTextures){
        SDL_DestroyTexture(tex);
    }
    loadedTextures.clear();
}





/** Get Number of Loaded Textures
 * 
 * Returns the number of currently loaded textures.
 */
int TM::getLoadedTextures(){ return loadedTextures.size(); }





/** Render Texture
 * 
 * Renders the texture on the screen width the specific x and y coordinates and custom width and height
 * 
 * As dr param function can take either width or hight as -1 meaning the function will determene that 
 * dimension automaticly using the aspect ratio of the image, and the dr propertie will be updated with
 * that newly calculated dimension.
 * 
 * Example: If func gets {10, 10, 100, -1} that means that height will be automaticly caluclated and
 * dr will be updated with that and then rendered, so after function call you have compeat dRect.
 * 
 * @param td TextureData to be rendered on the screen
 * @param dr SDL_Rect specifing where and how big should Texture be rendered
 * @return 0 on success and positive on error coresponding to the ERROR DEFINITIONS
 */
int TM::renderTexture(const TextureData& td, SDL_Rect& dr) {
    // CHECK IF TEXTURE IS VALID ---------------------------------------------------------
    if(td.tex == nullptr) return TM_GOT_NULLPTR_TEX;

    // CHECK IF DIMENSIONS ARE VALID ------------------------------------------------------
    if(dr.w < 0 && dr.h < 0) return TM_INVALID_DRECT;


    // CALCULATE IF ONE DIMENSION IS MISSING ----------------------------------------------
    if(dr.w == -1) dr.w = dr.h * (float)td.width / td.height;
    if(dr.h == -1) dr.h = dr.w * (float)td.height / td.width;


    // RENDER THE TEXTURE ON SCREEN -------------------------------------------------------
    if(SDL_RenderCopy(Sys::renderer, td.tex, NULL, &dr)) return TM_RCPY_FAILED;


    return NO_ERROR;
}





/** Create Text Texture
 * 
 * Creates a texture from a text using TTF_RenderUTF8_Blended.
 * It saves the texture into td TextureData and fills the rest of the object data.
 * 
 * @param td TextureData object where texture and side data will be placed
 * @param text The text to be compiled into texture
 * @param colo SDL_Color object, representing the color of the text
 * @return 0 on success and positive error code on error
 */
int TM::createTextTexture(
    TextureData& td, 
    const string& text,
    const SDL_Color& color
){
    // Ensure that privous Texture is de-loaded -----------------------------------------
    if(td.tex != nullptr) TM::freeTexture(td);

    // Create the text texture and store it as surface ----------------------------------
    SDL_Surface* surface = TTF_RenderUTF8_Blended(Sys::font, text.c_str(), color);
    if(surface == nullptr) return TM_SURFACE_CREATE_ERROR;

    // Create texture from it and store it in TextureData
    td.tex = SDL_CreateTextureFromSurface(Sys::renderer, surface);
    if(td.tex == nullptr) {
        SDL_FreeSurface(surface);
        return TM_TEXTURE_CREATE_ERROR;
    }

    // MAKE TEXTURE UPDATATBLE/MODIFIABLE -------------------------------------------------
    if(SDL_SetTextureBlendMode(td.tex, SDL_BLENDMODE_BLEND)){
        SDL_FreeSurface(surface);
        SDL_DestroyTexture(td.tex);
        return TM_TEXTURE_SET_BLENDMODE_ERROR;
    }

    // GET TEXTURE DIMENSIONS -------------------------------------------------------------
    SDL_QueryTexture(td.tex, &td.format, NULL, &td.width, &td.height);

    SDL_FreeSurface(surface);
    loadedTextures.push_back(td.tex);

    return NO_ERROR;
}





/** COPY
 * 
 * Creates a new Texture, copy, of the src, and places
 * it into dst TextureData. This function frees the old
 * dst Texture and creates a new one same as src.
 * 
 * If you just want to re-draw the src ontop of the dst
 * without creating a whole new texture from the start
 * you can use src.drawOverlayTexture() wich does exacly that.
 * 
 * @param src The source TextureData object, original
 * @param dst The destinaion object, the copy
 * @return 0 on success and positive error code on error
 */
int TM::copy(const TextureData& src, TextureData& dst){
    int err;
    if(dst.tex != nullptr){
        TM::freeTexture(dst);
    }

    // Create a new texture with the same format and size as the original
    dst.tex = SDL_CreateTexture(
        Sys::renderer, 
        src.format, 
        SDL_TEXTUREACCESS_TARGET, 
        src.width, 
        src.height
    );
    if (dst.tex == nullptr) {
        return TM_TEXTURE_CREATE_ERROR;
    }
    
    // Set the render target to the new texture to copy from the source texture
    err = SDL_SetRenderTarget(Sys::renderer, dst.tex);
    if(err) return TM_SRT_FAILED;

    // Clear the target texture
    err = SDL_RenderClear(Sys::renderer);
    if(err) return TM_RCLR_FAILED;

    // Copy the content of the source texture to the new one
    err = SDL_RenderCopy(Sys::renderer, src.tex, NULL, NULL);
    if(err) return TM_RCPY_FAILED;

    // Reset the render target to the default (the screen)
    err = SDL_SetRenderTarget(Sys::renderer, nullptr);
    if(err) return TM_SRT_FAILED;

    dst.width = src.width;
    dst.height = src.height;
    dst.format = src.format;

    loadedTextures.push_back(dst.tex);

    return NO_ERROR;
};





/** Draw Overlay Texture
 * 
 * Draws a texture on top of this texture.
 * Just like the TM::renderTexture it is able to dinamicly calculate one
 * missing dimension of the dRect, {10, 10, 100, -1}, and it calculates the
 * missing height and it updates it and then renderes the texture
 * 
 * @param td TextureData object to be rendered on top
 * @param dr SDL_Rect specifing where and how big should Texture be rendered
 * @return 0 on success and positive on error coresponding to the ERROR DEFINITIONS
 */
int TextureData::drawOverlayTexture(const TextureData& td, SDL_Rect& dr){
    int err;

    // CHECK IF DIMENSIONS ARE VALID ------------------------------------------------------
    if(dr.w < 0 && dr.h < 0) return TM_INVALID_DRECT;

    // CALCULATE IF ONE DIMENSION IS MISSING ----------------------------------------------
    if(dr.w == -1) dr.w = dr.h * (float)td.width / td.height;
    if(dr.h == -1) dr.h = dr.w * (float)td.height / td.width;

    // Set the render target to the new texture ----------------------------------------
    err = SDL_SetRenderTarget(Sys::renderer, this->tex);
    if(err != 0) return TM_SRT_FAILED;

    // Copy the content of the source texture to the new one ---------------------------
    err = SDL_RenderCopy(Sys::renderer, td.tex, NULL, &dr);
    if(err != 0) return TM_RCPY_FAILED;

    // Reset the render target to the default (the screen) -----------------------------
    err = SDL_SetRenderTarget(Sys::renderer, nullptr);
    if(err != 0) return TM_SRT_FAILED;

    return NO_ERROR;
}


/** Draw Overlay Texture
 * 
 * Draws a texture on top of this texture.
 * This drawOverlayTexture overload draws the 
 * texture asuming that this->td and param td
 * have at least the same aspect ratio and its
 * drawing from top-left croner to bottom-right.
 * 
 * @param td TextureData object to be rendered on top
 * @return 0 on success and positive on error coresponding to the ERROR DEFINITIONS
 */
int TextureData::drawOverlayTexture(const TextureData& td){
    SDL_Rect dRect = {0, 0, this->width, this->height};
    return drawOverlayTexture(td, dRect);
}





/** Draw Overlay Filled Rect
 * 
 * Draws a filled rect on top of this texture.
 * 
 * @param rect SDL_Rect specifing where and how big should rect be
 * @param color SDL_Color representing what color should rect be
 * @return 0 on success and positive on error coresponding to the ERROR DEFINITIONS
 */
int TextureData::drawOverlayFRect(const SDL_Rect& rect, const SDL_Color& color){
    int err;

    // CHECK IF DIMENSIONS ARE VALID ------------------------------------------------------
    if(rect.w < 0 && rect.h < 0) return TM_INVALID_DRECT;

    // Set the texture as the target for drawing
    err = SDL_SetRenderTarget(Sys::renderer, this->tex);
    if(err != 0) return TM_SRT_FAILED;

    // Set the color for use by renderer
    err = SDL_SetRenderDrawColor(Sys::renderer, color);
    if(err != 0) return TM_SRDC_FAILED;

    // Draw Filled Rect
    err = SDL_RenderFillRect(Sys::renderer, &rect);
    if(err != 0) return TM_FILL_RECT_ERROR;

    // Reset the render target back to window
    err = SDL_SetRenderTarget(Sys::renderer, nullptr);
    if(err != 0) return TM_SRT_FAILED;

    return NO_ERROR;
}





/** Draw Overlay Line
 * 
 * Draws a line with custom thickness on top of this texture.
 * 
 * @param p1 SDL_Point representing coordinates for starting point of the line
 * @param p2 SDL_Point representing coordinates for ending point of the line
 * @param color SDL_Color color of the line
 * @param thickness Thickness of the line
 * @return 0 on success and positive on error coresponding to the ERROR DEFINITIONS
 */
int TextureData::drawOverlayLine(const SDL_Point& p1, const SDL_Point& p2, const SDL_Color& color, const int& thickness){
    int err;

    // Set the texture as the target for drawing
    err = SDL_SetRenderTarget(Sys::renderer, this->tex);
    if(err != 0) return TM_SRT_FAILED;

    // Set the color for use by renderer
    err = SDL_SetRenderDrawColor(Sys::renderer, color);
    if(err != 0) return TM_SRDC_FAILED;

    // DRAWING LINE ------------------------------------------

    // Calculate the line's direction vector
    double dx = p2.x - p1.x;
    double dy = p2.y - p1.y;
    double length = std::sqrt(dx * dx + dy * dy);

    if(length <= 0) return TM_INVALID_LINE_LENGTH;

    // Normalize the direction vector
    dx /= length;
    dy /= length;

    // Calculate the perpendicular vector for thickness
    double px = -dy * (thickness / 2.0);
    double py = dx * (thickness / 2.0);

    // Define the four corners of the rectangle representing the thick line
    SDL_Vertex vertices[4] = {
        {SDL_FPoint{static_cast<float>(p1.x + px), static_cast<float>(p1.y + py)}, color, SDL_FPoint{0, 0}},
        {SDL_FPoint{static_cast<float>(p1.x - px), static_cast<float>(p1.y - py)}, color, SDL_FPoint{0, 0}},
        {SDL_FPoint{static_cast<float>(p2.x - px), static_cast<float>(p2.y - py)}, color, SDL_FPoint{0, 0}},
        {SDL_FPoint{static_cast<float>(p2.x + px), static_cast<float>(p2.y + py)}, color, SDL_FPoint{0, 0}}
    };


    // Draw the thick line as a filled polygon
    SDL_RenderGeometry(Sys::renderer, nullptr, vertices, 4, nullptr, 0);
    
    // Reset the render target back to window
    err = SDL_SetRenderTarget(Sys::renderer, nullptr);
    if(err != 0) return TM_SRT_FAILED;

    return NO_ERROR;
}


int TextureData::drawOverlayText(const string& text, SDL_Rect& dRect, const SDL_Color& color){
    int err;

    err = SDL_SetRenderTarget(Sys::renderer, this->tex);
    if(err != 0) return TM_SRT_FAILED;

    GUI::Text(text, dRect, color);

    err = SDL_SetRenderTarget(Sys::renderer, nullptr);
    if(err != 0) return TM_SRT_FAILED;

    return NO_ERROR;
}