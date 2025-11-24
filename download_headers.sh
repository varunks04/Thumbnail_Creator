#!/bin/bash
# Download script for stb headers
# Run this script to download the full stb_image library headers

echo "Downloading stb_image headers..."

BASE_URL="https://raw.githubusercontent.com/nothings/stb/master"
HEADERS=("stb_image.h" "stb_image_resize.h" "stb_image_write.h")

for header in "${HEADERS[@]}"; do
    url="$BASE_URL/$header"
    output="include/$header"
    
    echo "Downloading $header..."
    if curl -L "$url" -o "$output"; then
        echo "  ✓ Downloaded $header"
    else
        echo "  ✗ Failed to download $header"
    fi
done

echo ""
echo "Download complete! You can now build the project."
