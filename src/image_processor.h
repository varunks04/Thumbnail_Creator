#ifndef IMAGE_PROCESSOR_H
#define IMAGE_PROCESSOR_H

#include <string>
#include <vector>

class ImageProcessor {
public:
    struct ImageData {
        unsigned char* data;
        int width;
        int height;
        int channels;
        bool is_valid;
        
        ImageData() : data(nullptr), width(0), height(0), channels(0), is_valid(false) {}
    };
    
    // Load image from file
    static ImageData loadImage(const std::string& filepath);
    
    // Free image data
    static void freeImage(ImageData& img);
    
    // Generate thumbnail from image
    static ImageData createThumbnail(const ImageData& original, int thumbnail_size);
    
    // Save thumbnail to file
    static bool saveThumbnail(const ImageData& thumbnail, const std::string& output_path);
    
    // Process single image: load, create thumbnail, save, return perceptual hash
    static uint64_t processSingleImage(const std::string& input_path,
                                       const std::string& output_path,
                                       int thumbnail_size,
                                       bool& success);
    
    // Get file extension
    static std::string getFileExtension(const std::string& filepath);
    
    // Check if file is an image
    static bool isImageFile(const std::string& filepath);
};

#endif // IMAGE_PROCESSOR_H
