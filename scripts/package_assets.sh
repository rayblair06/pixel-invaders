#!/usr/bin/env bash
set -e

BUILD_DIR=$1
ZIP_NAME=$2

echo "[INFO] Packaging assets and binaries into: $ZIP_NAME"
echo "[INFO] Build directory: $BUILD_DIR"

# Ensure build directory exists
if [ ! -d "$BUILD_DIR" ]; then
    echo "[ERROR] Build directory '$BUILD_DIR' does not exist!"
    exit 1
fi

# Copy assets into the build directory
mkdir -p "$BUILD_DIR/assets"
cp -r assets/* "$BUILD_DIR/assets/"

# Detect platform
OS="unknown"
case "$OSTYPE" in
  linux*)   OS="linux" ;;
  darwin*)  OS="macos" ;;
  msys*|cygwin*|win32*) OS="windows" ;;
esac

echo "[INFO] Detected platform: $OS"

# Package using the appropriate tool
cd "$BUILD_DIR"
if [ "$OS" = "windows" ]; then
    echo "[INFO] Using 7zip to package on Windows"
    7z a "../$ZIP_NAME" *  >/dev/null
else
    echo "[INFO] Using zip to package on $OS"
    zip -r "../$ZIP_NAME" * >/dev/null
fi

cd ..
echo "[INFO] Package created: $ZIP_NAME"
