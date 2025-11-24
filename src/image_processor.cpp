#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"
#define STB_IMAGE_RESIZE_IMPLEMENTATION
#define STBIR_DEFAULT_FILTER_DOWNSAMPLE  STBIR_FILTER_MITCHELL
#include "stb_image_resize.h"
#define STB_IMAGE_WRITE_IMPLEMENTATION
#include "stb_image_write.h"

#include "image_processor.h"
#include "hash_calculator.h"
#include <algorithm>
#include <iostream>

ImageProcessor::ImageData ImageProcessor::loadImage(const std::string& filepath) {
    ImageData img;
    
    img.data = stbi_load(filepath.c_str(), &img.width, &img.height, &img.channels, 0);
    
    if (img.data == nullptr) {
        std::cerr << "Failed to load image: " << filepath << " - " << stbi_failure_reason() << std::endl;
        img.is_valid = false;
    } else {
        img.is_valid = true;
    }
    
    return img;
}

void ImageProcessor::freeImage(ImageData& img) {
    if (img.data != nullptr) {
        stbi_image_free(img.data);
        img.data = nullptr;
        img.is_valid = false;
    }
}

ImageProcessor::ImageData ImageProcessor::createThumbnail(const ImageData& original, int thumbnail_size) {
    ImageData thumbnail;
    
    if (!original.is_valid) {
        return thumbnail;
    }
    
    // Calculate aspect ratio
    float aspect_ratio = static_cast<float>(original.width) / static_cast<float>(original.height);
    
    int thumb_w, thumb_h;
    if (original.width > original.height) {
        thumb_w = thumbnail_size;
        thumb_h = static_cast<int>(thumbnail_size / aspect_ratio);
    } else {
        thumb_h = thumbnail_size;
        thumb_w = static_cast<int>(thumbnail_size * aspect_ratio);
    }
    
    // Allocate memory for thumbnail
    thumbnail.width = thumb_w;
    thumbnail.height = thumb_h;
    thumbnail.channels = original.channels;
    thumbnail.data = new unsigned char[thumb_w * thumb_h * original.channels];
    
    // Resize using stb_image_resize2
    void* result = stbir_resize_uint8_linear(
        original.data, original.width, original.height, 0,
        thumbnail.data, thumb_w, thumb_h, 0,
        (stbir_pixel_layout)original.channels
    );
    
    if (result) {
        thumbnail.is_valid = true;
    } else {
        delete[] thumbnail.data;
        thumbnail.data = nullptr;
        thumbnail.is_valid = false;
    }
    
    return thumbnail;
}

bool ImageProcessor::saveThumbnail(const ImageData& thumbnail, const std::string& output_path) {
    if (!thumbnail.is_valid) {
        return false;
    }
    
    // Save as JPEG with quality 85
    int result = stbi_write_jpg(output_path.c_str(), thumbnail.width, thumbnail.height,
                                thumbnail.channels, thumbnail.data, 85);
    
    return result != 0;
}

uint64_t ImageProcessor::processSingleImage(const std::string& input_path,
                                            const std::string& output_path,
                                            int thumbnail_size,
                                            bool& success) {
    success = false;
    uint64_t hash = 0;
    
    // Load original image
    ImageData original = loadImage(input_path);
    if (!original.is_valid) {
        return hash;
    }
    
    // Calculate perceptual hash from original
    hash = HashCalculator::calculatePerceptualHash(original.data, original.width, 
                                                   original.height, original.channels);
    
    // Create thumbnail
    ImageData thumbnail = createThumbnail(original, thumbnail_size);
    if (!thumbnail.is_valid) {
        freeImage(original);
        return hash;
    }
    
    // Save thumbnail
    success = saveThumbnail(thumbnail, output_path);
    
    // Cleanup
    freeImage(original);
    if (thumbnail.data != nullptr) {
        delete[] thumbnail.data;
    }
    
    return hash;
}

std::string ImageProcessor::getFileExtension(const std::string& filepath) {
    size_t dot_pos = filepath.find_last_of('.');
    if (dot_pos == std::string::npos) {
        return "";
    }
    
    std::string ext = filepath.substr(dot_pos + 1);
    std::transform(ext.begin(), ext.end(), ext.begin(), ::tolower);
    return ext;
}

bool ImageProcessor::isImageFile(const std::string& filepath) {
    std::string ext = getFileExtension(filepath);
    
    return (ext == "jpg" || ext == "jpeg" || ext == "png" || 
            ext == "bmp" || ext == "tga" || ext == "gif");
}
