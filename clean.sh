#!/bin/sh

if [ -d "build/" ]; then
    echo "Found build/, removing..."
    rm -rf build
    echo "Done."
else
    echo "Not Found build/, it seems the build files are already deleted!"
fi