#!/bin/bash

INCLUDE_DIR="/usr/include/Lumos"
LIB_DIR="/usr/local/lib"

SRC_INCLUDE_DIR="./lib"

if [ "$EUID" -ne 0 ]; then
    echo "Please run as root"
    exit 1
fi

echo "Installing dependencies..."
pacman -S --noconfirm sdl2 sdl2_ttf sdl2_image postgresql

echo "Installing headers to $INCLUDE_DIR..."
mkdir -p "$INCLUDE_DIR"
cp -r $SRC_INCLUDE_DIR/* "$INCLUDE_DIR/"

echo "Installing compiled objects to $LIB_DIR..."
mkdir -p "$LIB_DIR"

echo "Generating pkg-config file..."
PKG_CONFIG_FILE="/usr/lib/pkgconfig/Lumos.pc"
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
Libs: -L\${libdir} -lLumos -lSDL2 -lSDL2_ttf -lSDL2_image -lpq
EOL

ldconfig
echo "Installation complete!"