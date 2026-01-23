#!/bin/bash

# Build script for 3DS Zip Extractor
# This script checks prerequisites and builds the application

set -e

echo "================================"
echo "3DS Zip Extractor Build Script"
echo "================================"
echo ""

# Check if DEVKITARM is set
if [ -z "$DEVKITARM" ]; then
    echo "ERROR: DEVKITARM environment variable is not set!"
    echo "Please install devkitPro and set the environment variables."
    echo "See INSTALL.md for details."
    exit 1
fi

echo "✓ DEVKITARM found at: $DEVKITARM"

# Check if make is available
if ! command -v make &> /dev/null; then
    echo "ERROR: make command not found!"
    echo "Please install make."
    exit 1
fi

echo "✓ make found"

# Check if required tools are available
if ! command -v arm-none-eabi-gcc &> /dev/null; then
    echo "ERROR: arm-none-eabi-gcc not found!"
    echo "Please make sure devkitARM is properly installed and in your PATH."
    exit 1
fi

echo "✓ arm-none-eabi-gcc found"

# Clean previous builds
echo ""
echo "Cleaning previous builds..."
make clean 2>/dev/null || true

# Build
echo ""
echo "Building 3ds-zip-extractor..."
if make; then
    echo ""
    echo "================================"
    echo "✓ Build successful!"
    echo "================================"
    echo ""
    echo "Output file: 3ds-zip-extractor.3dsx"
    echo ""
    echo "Next steps:"
    echo "1. Copy 3ds-zip-extractor.3dsx to your SD card's /3ds/ folder"
    echo "2. Insert the SD card into your 3DS"
    echo "3. Launch the Homebrew Launcher"
    echo "4. Run 3DS Zip Extractor"
    echo ""
else
    echo ""
    echo "================================"
    echo "✗ Build failed!"
    echo "================================"
    echo ""
    echo "Common issues:"
    echo "- Missing libraries: Install with 'sudo dkp-pacman -S 3ds-curl 3ds-libarchive 3ds-mbedtls'"
    echo "- Wrong devkitPro version: Update with 'sudo dkp-pacman -Syu'"
    echo "- See INSTALL.md for detailed troubleshooting"
    echo ""
    exit 1
fi
