cmake_minimum_required(VERSION 3.10)
project(ParallelThumbnailGenerator)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Find OpenMP
find_package(OpenMP REQUIRED)

# Include directories
include_directories(${CMAKE_SOURCE_DIR}/include)
include_directories(${CMAKE_SOURCE_DIR}/src)

# Source files
set(SOURCES
    src/main.cpp
    src/image_processor.cpp
    src/hash_calculator.cpp
    src/duplicate_detector.cpp
    src/performance_tracker.cpp
)

# Create executable
add_executable(thumbnail_gen ${SOURCES})

# Link OpenMP
if(MSVC)
    target_link_libraries(thumbnail_gen OpenMP::OpenMP_CXX)
else()
    # For MinGW/GCC, explicitly link gomp
    target_link_libraries(thumbnail_gen OpenMP::OpenMP_CXX gomp)
endif()

# Platform-specific settings
if(MSVC)
    # MSVC-specific flags
    target_compile_options(thumbnail_gen PRIVATE /W4)
else()
    # GCC/Clang/MinGW flags
    target_compile_options(thumbnail_gen PRIVATE -Wall -Wextra -O3)
endif()

# Output directory
set_target_properties(thumbnail_gen PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/bin
)
