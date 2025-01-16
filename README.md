# LUMOS

A bunddle of System Manager, Texture Manager, Database Manager and custom GUI elements.
Made easy and very fast to use.

## HOW TO SETUP ##
1. `make`
2. `sudo make install`
3. `sudo chmod +x ./install.sh`
4. `sudo ./install.sh`

## HOW TO USE ##
1. `g++ main.cpp -o main -I/usr/include/Lumos -L/usr/local/lib -lLumos -lSDL2 -lSDL2_ttf -lSDL2_image -lpq`
2. Makefile example:
```

# Use pkg-config to get the necessary flags
CXXFLAGS = $(shell pkg-config --cflags Lumos)
LDFLAGS = $(shell pkg-config --libs Lumos)

# Compiler
CXX = g++

# Build the main executable
main: main.cpp
    $(CXX) $(CXXFLAGS) $(LDFLAGS) main.cpp -o main

# Clean
clean:
    rm -f main

```

## ABOUT THE MODULES

System Manager (Sys):  
    - Initiates the system  
    - Holds the important variables (renderer, window, font, ...)  
    - Has a job of handling events and updating the values accordingly  
    - Presents the drawn window every frame  
    - Runs stable FPS  
    - Handles a well organised error definitions  

Texture Manager (TM):  
    - Has special functions for loading textures in a safe manner  
    - Keeps track of all loaded textures  
    - Has special function for rendering textures and dinamicly calculating there dimensions  
    - Handles freeing of the textures  
    - Maganges a custom data structure called TextureData which holds textures with some helpfull data alnog side it  
  
Database Manager (DB):  
    - Handles the connection creation to the posgresql db  
    - Holds special data structures that makes it easy to use  
    - Has build it optimization of prepared statements  

GUI Lib (GUI):  
    - Allows a very efficient way of rendering buttons, texts and other elements  

Creator: AndrijaRD  

To view the amount of lines written use:
` find . -type f \( -name "*.cpp" -o -name "*.h" \) | xargs wc -l `
