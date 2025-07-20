#!/usr/bin/env bash
set -e

BUILD_DIR=$1
ZIP_NAME=$2

echo "[INFO] Packaging game assets into $ZIP_NAME..."

# Copy assets into build directory
mkdir -p "$BUILD_DIR/assets"
cp -r assets/* "$BUILD_DIR/assets/"

# Zip the build binary + assets
cd "$BUILD_DIR"
if [[ "$OS" == "windows-latest" ]]; then
    7z a "../$ZIP_NAME" PixelInvaders.exe *.dll assets
else
    zip -r "../$ZIP_NAME" PixelInvaders assets
fi

cd ..
