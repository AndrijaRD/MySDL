#!/bin/bash

# Define installation directories
INCLUDE_DIR="/usr/local/include/Lumos"
LIB_DIR="/usr/local/lib"

# Define source paths
SRC_INCLUDE_DIR="lib"
BUILD_LIB_DIR="build/lib"

# Ensure the script is run as root
if [ "$EUID" -ne 0 ]; then
    echo "Please run as root"
    exit 1
fi

# Install headers
echo "Installing headers to $INCLUDE_DIR..."
mkdir -p "$INCLUDE_DIR"
cp -r $SRC_INCLUDE_DIR/*.h "$INCLUDE_DIR/"
cp -r $SRC_INCLUDE_DIR/*/*.h "$INCLUDE_DIR/"

# Install compiled library objects
echo "Installing compiled objects to $LIB_DIR..."
mkdir -p "$LIB_DIR"
cp -r $BUILD_LIB_DIR/*/*.o "$LIB_DIR/"

# Create a pkg-config file for the library
echo "Generating pkg-config file..."
PKG_CONFIG_FILE="/usr/local/lib/pkgconfig/Lumos.pc"
mkdir -p "$(dirname "$PKG_CONFIG_FILE")"
cat <<EOL > "$PKG_CONFIG_FILE"
prefix=/usr/local
exec_prefix=\${prefix}
includedir=\${prefix}/include/Lumos
libdir=\${prefix}/lib

Name: Lumos
Description: A wrapper library for SDL2
Version: 1.0
Cflags: -I\${includedir}
Libs: -L\${libdir}
EOL

echo "Installation complete!"
echo "You can now include Lumos headers using:"
echo "#include <Lumos>"
echo "Ensure you add '-lLumos' to link with the library."
