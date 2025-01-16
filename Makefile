# Set the compiler and flags
CXX = g++
CXXFLAGS = -Wall -fPIC -std=c++17  # -fPIC for position-independent code

# Define the output shared library name
LIB_NAME = libLumos.so
LIB_DIR = /usr/local/lib

# Define the source files
SRC = \
    lib/System/Sys.cpp \
    lib/TextureManager/TM.cpp \
    lib/PqDB/db.cpp \
    lib/Gui/gui.cpp

# Define the object files
OBJ = $(SRC:.cpp=.o)

# Define the installation directories
INCLUDE_DIR = /usr/include/Lumos

# Default rule to build the shared library
all: $(LIB_NAME)

# Build the shared library from object files
$(LIB_NAME): $(OBJ)
	$(CXX) -shared -o $(LIB_NAME) $(OBJ) -Wl,-soname,$(LIB_NAME)

# Rule to create object files from source files
%.o: %.cpp
	$(CXX) $(CXXFLAGS) -I$(INCLUDE_DIR) -c $< -o $@

# Install the shared library and headers
install: $(LIB_NAME)
	# Install the library to the system
	mkdir -p $(LIB_DIR)
	cp $(LIB_NAME) $(LIB_DIR)
	# Install the header files
	mkdir -p $(INCLUDE_DIR)
	cp -r lib/* $(INCLUDE_DIR)/

# Clean up build files
clean:
	rm -f $(OBJ) $(LIB_NAME)

# Make install the default rule
.PHONY: all install clean
