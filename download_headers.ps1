# Download script for stb headers
# Run this script to download the full stb_image library headers

Write-Host "Downloading stb_image headers..." -ForegroundColor Green

$baseUrl = "https://raw.githubusercontent.com/nothings/stb/master"
$headers = @(
    "stb_image.h",
    "stb_image_resize.h",
    "stb_image_write.h"
)

foreach ($header in $headers) {
    $url = "$baseUrl/$header"
    $output = "include\$header"
    
    Write-Host "Downloading $header..." -ForegroundColor Yellow
    try {
        Invoke-WebRequest -Uri $url -OutFile $output
        Write-Host "  ✓ Downloaded $header" -ForegroundColor Green
    }
    catch {
        Write-Host "  ✗ Failed to download $header" -ForegroundColor Red
        Write-Host "  Error: $_" -ForegroundColor Red
    }
}

Write-Host "`nDownload complete! You can now build the project." -ForegroundColor Green
