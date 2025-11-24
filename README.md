# Parallel Thumbnail Generator and Duplicate Detector

A high-performance C++ application that generates image thumbnails and detects duplicates using both serial and parallel processing. Compares performance metrics between serial and parallel execution modes using OpenMP.

## Features

- **Parallel Image Processing**: Uses OpenMP to process multiple images simultaneously
- **Thumbnail Generation**: Creates resized thumbnails from large photo collections
- **Duplicate Detection**: 
  - Exact duplicates using MD5 hashing
  - Similar images using perceptual hashing (dHash algorithm)
- **Performance Comparison**: Detailed statistics comparing serial vs parallel execution
- **Metrics Tracked**:
  - Total execution time
  - Images processed per second
  - Average time per image
  - Speedup factor
  - Efficiency percentage
  - Thread utilization

## Requirements

### Windows
- **Compiler**: Visual Studio 2019+ (with C++17 support) or MinGW-w64
- **CMake**: Version 3.10 or higher
- **OpenMP**: Included with MSVC and MinGW

### Linux/Mac
- **Compiler**: GCC 7+ or Clang 5+
- **CMake**: Version 3.10 or higher
- **OpenMP**: Install via package manager
  ```bash
  # Ubuntu/Debian
  sudo apt-get install libomp-dev
  
  # macOS (using Homebrew)
  brew install libomp
  ```

## Installation

### Step 1: Download Full stb Headers

The stub headers in `include/` need to be replaced with full versions:

```powershell
# Download stb_image.h
Invoke-WebRequest -Uri "https://raw.githubusercontent.com/nothings/stb/master/stb_image.h" -OutFile "include/stb_image.h"

# Download stb_image_resize.h
Invoke-WebRequest -Uri "https://raw.githubusercontent.com/nothings/stb/master/stb_image_resize.h" -OutFile "include/stb_image_resize.h"

# Download stb_image_write.h
Invoke-WebRequest -Uri "https://raw.githubusercontent.com/nothings/stb/master/stb_image_write.h" -OutFile "include/stb_image_write.h"
```

### Step 2: Build the Project

#### Windows (PowerShell)

```powershell
# Create build directory
mkdir build
cd build

# Configure with CMake
cmake ..

# Build the project
cmake --build . --config Release

# The executable will be in bin/Release/thumbnail_gen.exe
```

#### Linux/Mac

```bash
# Create build directory
mkdir build
cd build

# Configure and build
cmake -DCMAKE_BUILD_TYPE=Release ..
make

# The executable will be in bin/thumbnail_gen
```

## Usage

### Basic Usage

```bash
# Windows
.\bin\Release\thumbnail_gen.exe -i <input_directory> -o <output_directory>

# Linux/Mac
./bin/thumbnail_gen -i <input_directory> -o <output_directory>
```

### Command Line Options

```
Options:
  -i <dir>     Input directory with images (required)
  -o <dir>     Output directory for thumbnails (default: ./output/thumbnails)
  -s <size>    Thumbnail size in pixels (default: 256)
  -t <value>   Hamming distance threshold for duplicates (default: 8)
  -n <num>     Number of threads for parallel mode (default: all available)
  --serial     Run only serial mode
  --parallel   Run only parallel mode
  -h, --help   Show this help message
```

### Examples

```powershell
# Process photos with default settings (runs both serial and parallel)
.\bin\Release\thumbnail_gen.exe -i C:\Photos -o C:\Thumbnails

# Use 512px thumbnails and stricter duplicate detection
.\bin\Release\thumbnail_gen.exe -i C:\Photos -o C:\Thumbnails -s 512 -t 5

# Run only parallel mode with 8 threads
.\bin\Release\thumbnail_gen.exe -i C:\Photos --parallel -n 8

# Run only serial mode for baseline measurement
.\bin\Release\thumbnail_gen.exe -i C:\Photos --serial
```

## Sample Output

```
Found 1000 image files.
Thumbnail size: 256px
Hamming threshold: 8

[SERIAL MODE] Processing 1000 images...

========================================
  SERIAL Mode Statistics
========================================
Total Time:          45.23 seconds
Total Images:        1000
Successful:          998
Failed:              2
Duplicates Found:    15
Threads Used:        1
Throughput:          22.07 images/sec
Avg Time/Image:      45.32 ms
========================================

[PARALLEL MODE] Processing 1000 images with 8 threads...

========================================
  PARALLEL Mode Statistics
========================================
Total Time:          8.71 seconds
Total Images:        1000
Successful:          998
Failed:              2
Duplicates Found:    15
Threads Used:        8
Throughput:          114.58 images/sec
Avg Time/Image:      8.73 ms
========================================

========================================
  SERIAL vs PARALLEL COMPARISON
========================================

Execution Time:
  Serial:            45.23 seconds
  Parallel:          8.71 seconds

Throughput:
  Serial:            22.07 images/sec
  Parallel:          114.58 images/sec

Average Time per Image:
  Serial:            45.32 ms
  Parallel:          8.73 ms

Parallelization Metrics:
  Threads Used:      8
  Speedup:           5.19x
  Efficiency:        64.88%

Duplicates Detection:
  Serial Found:      15
  Parallel Found:    15
  ✓ Results Match!
========================================
```

## Project Structure

```
PC/
├── CMakeLists.txt                 # Build configuration
├── Plan.md                        # Project plan
├── README.md                      # This file
├── include/                       # Third-party headers
│   ├── stb_image.h
│   ├── stb_image_resize.h
│   └── stb_image_write.h
├── src/                           # Source files
│   ├── main.cpp                   # Entry point and CLI
│   ├── image_processor.h/cpp      # Image loading and thumbnail creation
│   ├── hash_calculator.h/cpp      # MD5 and perceptual hashing
│   ├── duplicate_detector.h/cpp   # Duplicate detection logic
│   └── performance_tracker.h/cpp  # Performance metrics
├── output/                        # Generated thumbnails
│   └── thumbnails/
└── build/                         # Build artifacts (generated)
```

## How It Works

1. **Image Collection**: Scans input directory recursively for image files (JPG, PNG, BMP, TGA, GIF)

2. **Serial Processing**:
   - Loads each image sequentially
   - Generates thumbnail using stb_image_resize
   - Calculates MD5 hash (exact duplicates) and perceptual hash (similar images)
   - Saves thumbnail as JPEG

3. **Parallel Processing**:
   - Uses OpenMP `#pragma omp parallel for` to distribute images across threads
   - Each thread processes images independently (embarrassingly parallel)
   - Results are aggregated after processing

4. **Duplicate Detection**:
   - **Exact duplicates**: Compares MD5 hashes of original files
   - **Similar images**: Compares perceptual hashes using Hamming distance
   - Groups duplicates and reports findings

5. **Performance Analysis**:
   - Tracks execution time with `std::chrono::high_resolution_clock`
   - Calculates speedup: `serial_time / parallel_time`
   - Calculates efficiency: `(speedup / threads) × 100%`

## Algorithm Details

### Perceptual Hashing (dHash)
1. Convert image to grayscale
2. Resize to 9×8 pixels
3. Compare adjacent pixels horizontally
4. Create 64-bit hash based on brightness differences
5. Similar images produce similar hashes

### Duplicate Detection
- **Hamming Distance Threshold**: Number of differing bits between hashes
  - `0`: Exact match
  - `1-5`: Very similar (minor edits)
  - `6-10`: Similar (moderate changes)
  - `>10`: Different images

## Supported Image Formats

- JPEG (.jpg, .jpeg)
- PNG (.png)
- BMP (.bmp)
- TGA (.tga)
- GIF (.gif)

## Performance Tips

1. **SSD vs HDD**: Use SSD for better I/O performance
2. **Thread Count**: Default uses all CPU cores; adjust with `-n` if needed
3. **Thumbnail Size**: Smaller thumbnails process faster
4. **Image Count**: Performance gains are more noticeable with 100+ images

## Troubleshooting

### Build Errors

**Error: OpenMP not found**
```bash
# Windows: Use Visual Studio or install MinGW-w64 with OpenMP support
# Linux: sudo apt-get install libomp-dev
# macOS: brew install libomp
```

**Error: stb_image headers missing**
- Download full headers as shown in Installation Step 1

### Runtime Issues

**No images found**
- Ensure input directory contains supported image formats
- Check file extensions match supported types

**Failed to load image**
- Image file may be corrupted
- Format may not be supported by stb_image

## License

This project uses the following libraries:
- **stb_image/stb_image_resize/stb_image_write**: Public Domain

## Future Enhancements

- GPU acceleration using CUDA or OpenCL
- Web interface for visualization
- More hash algorithms (pHash, aHash)
- Video thumbnail support
- Database storage for large collections

## Author

Parallel Thumbnail Generator - C++ Image Processing Project
