# Quick Start Guide

## Step 1: Download Required Headers

### Windows (PowerShell):
```powershell
.\download_headers.ps1
```

### Linux/Mac:
```bash
chmod +x download_headers.sh
./download_headers.sh
```

## Step 2: Build

### Windows:
```powershell
mkdir build
cd build
cmake ..
cmake --build . --config Release
```

### Linux/Mac:
```bash
mkdir build
cd build
cmake -DCMAKE_BUILD_TYPE=Release ..
make
```

## Step 3: Run

### Create Test Images Directory
```powershell
# Put some test images in a directory, e.g., C:\TestPhotos
```

### Execute
```powershell
# Windows
.\bin\Release\thumbnail_gen.exe -i C:\TestPhotos -o .\output\thumbnails

# Linux/Mac
./bin/thumbnail_gen -i /path/to/photos -o ./output/thumbnails
```

## Expected Output

The program will:
1. Scan the input directory for images
2. Process all images in SERIAL mode first
3. Process all images in PARALLEL mode with multiple threads
4. Display detailed statistics comparing both modes
5. Show duplicate detection results
6. Save thumbnails to the output directory

## Sample Statistics You'll See

- **Execution Time**: How long each mode took
- **Throughput**: Images processed per second
- **Speedup**: How much faster parallel was than serial
- **Efficiency**: How well the threads were utilized
- **Duplicate Count**: Number of duplicate/similar images found

Enjoy your parallel image processing! 🚀
