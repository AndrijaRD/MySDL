# Compiler
CXX := g++
WIN_CXX := x86_64-w64-mingw32-g++

# Flags
CXXFLAGS := -Wall -Wextra -g3 -std=c++23 -MMD -MP
CXXFLAGS += -I./lib $(shell sdl2-config --cflags)

LDFLAGS := $(shell sdl2-config --libs) -lSDL2_image -lSDL2_ttf
LDFLAGS += `pkg-config --libs libcurl` -ldl -lpq -L/usr/local/lib -ldlib -llapack -lblas -lcblas -lgif

WIN_CXXFLAGS := -Wall -Wextra -std=c++23 -I./lib
WIN_CXXFLAGS += -I/usr/x86_64-w64-mingw32/include/SDL2
WIN_LDFLAGS := -L/usr/x86_64-w64-mingw32/lib -lSDL2main -lSDL2 -lSDL2_image -lSDL2_ttf -lws2_32 -lwinmm -lgdi32 -mconsole

# Directories
SRCDIR := .
LIBS_DIR := $(SRCDIR)/lib
BUILDDIR := $(SRCDIR)/build

# Files
SRC := $(wildcard $(LIBS_DIR)/**/*.cpp) ./main.cpp

OBJ := $(patsubst $(SRCDIR)/%.cpp, $(BUILDDIR)/%.o, $(SRC))
WIN_OBJ := $(patsubst $(SRCDIR)/%.cpp, $(BUILDDIR)/%.win.o, $(SRC))

# Target
TARGET := main
WIN_TARGET := main.exe

# Rules
.PHONY: all clean linux windows

all: linux

# Linux target
linux: $(TARGET)

$(TARGET): $(OBJ)
	$(CXX) $(OBJ) -o $@ $(LDFLAGS)

$(BUILDDIR)/%.o: $(SRCDIR)/%.cpp
	@mkdir -p $(dir $@)
	$(CXX) $(CXXFLAGS) -c $< -o $@

# Windows target
windows: $(WIN_TARGET)

$(WIN_TARGET): $(WIN_OBJ)
	$(WIN_CXX) $(WIN_OBJ) -o $@ $(WIN_LDFLAGS)

$(BUILDDIR)/%.win.o: $(SRCDIR)/%.cpp
	@mkdir -p $(dir $@)
	$(WIN_CXX) $(WIN_CXXFLAGS) -c $< -o $@

clean:
	rm -rf $(BUILDDIR) $(TARGET) $(WIN_TARGET)

-include $(OBJ:.o=.d)
