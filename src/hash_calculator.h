#ifndef HASH_CALCULATOR_H
#define HASH_CALCULATOR_H

#include <string>
#include <vector>
#include <cstdint>

class HashCalculator {
public:
    // Calculate MD5 hash of file (for exact duplicate detection)
    static std::string calculateMD5(const std::string& filepath);
    
    // Calculate perceptual hash (difference hash - dHash) from image data
    // Returns 64-bit hash suitable for comparing similar images
    static uint64_t calculatePerceptualHash(const unsigned char* image_data, 
                                           int width, int height, int channels);
    
    // Calculate Hamming distance between two hashes (number of different bits)
    static int hammingDistance(uint64_t hash1, uint64_t hash2);
    
private:
    // Helper: Convert grayscale for perceptual hashing
    static std::vector<unsigned char> convertToGrayscale(const unsigned char* image_data,
                                                         int width, int height, int channels);
    
    // Helper: Resize image to 9x8 for dHash calculation
    static std::vector<unsigned char> resizeForHash(const std::vector<unsigned char>& gray_data,
                                                    int width, int height);
    
    // Simple MD5 implementation
    static void md5(const unsigned char* data, size_t length, unsigned char* digest);
};

#endif // HASH_CALCULATOR_H
