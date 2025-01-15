# Compiler
CXX := g++
CXXFLAGS := -Wall -Wextra -g3 -std=c++23 -I../../lib $(shell sdl2-config --cflags)
LDFLAGS := $(shell sdl2-config --libs) -lSDL2_image -lSDL2_ttf `pkg-config --libs libcurl` -ldl -lpq -L/usr/local/lib -ldlib -llapack -lblas -lcblas -lgif

# Directories
SRCDIR := .
BUILDDIR := ../../build/examples/Inpit
LIBDIR := ../../lib
LIBBUILDDIR := ../../build/lib

# Files
SRC := $(SRCDIR)/Input.cpp
LIB_SRC := $(wildcard $(LIBDIR)/**/*.cpp)

OBJ := $(patsubst $(SRCDIR)/%.cpp, $(BUILDDIR)/%.o, $(SRC))
LIB_OBJ := $(patsubst $(LIBDIR)/%.cpp, $(LIBBUILDDIR)/%.o, $(LIB_SRC))

# Target
TARGET := Input

# Rules
.PHONY: all clean

all: $(TARGET)

$(TARGET): $(OBJ) $(LIB_OBJ)
	$(CXX) $^ -o $@ $(LDFLAGS)

$(BUILDDIR)/%.o: $(SRCDIR)/%.cpp
	@mkdir -p $(dir $@)
	$(CXX) $(CXXFLAGS) -c $< -o $@

$(LIBBUILDDIR)/%.o: $(LIBDIR)/%.cpp
	@mkdir -p $(dir $@)
	$(CXX) $(CXXFLAGS) -c $< -o $@

clean:
	rm -rf $(BUILDDIR) $(TARGET)
	rm -rf $(LIBBUILDDIR)
