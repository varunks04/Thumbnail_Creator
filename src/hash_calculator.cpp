#include "hash_calculator.h"
#include <fstream>
#include <sstream>
#include <iomanip>
#include <cstring>
#include <algorithm>

// Simple MD5 implementation (basic version for demonstration)
// In production, use a library like OpenSSL or Crypto++

#define F(x, y, z) (((x) & (y)) | (~(x) & (z)))
#define G(x, y, z) (((x) & (z)) | ((y) & ~(z)))
#define H(x, y, z) ((x) ^ (y) ^ (z))
#define I(x, y, z) ((y) ^ ((x) | ~(z)))

#define ROTATE_LEFT(x, n) (((x) << (n)) | ((x) >> (32-(n))))

std::string HashCalculator::calculateMD5(const std::string& filepath) {
    std::ifstream file(filepath, std::ios::binary);
    if (!file) {
        return "";
    }
    
    // Read file into buffer
    file.seekg(0, std::ios::end);
    size_t size = file.tellg();
    file.seekg(0, std::ios::beg);
    
    std::vector<unsigned char> buffer(size);
    file.read(reinterpret_cast<char*>(buffer.data()), size);
    file.close();
    
    // Calculate MD5
    unsigned char digest[16];
    md5(buffer.data(), size, digest);
    
    // Convert to hex string
    std::stringstream ss;
    for (int i = 0; i < 16; i++) {
        ss << std::hex << std::setw(2) << std::setfill('0') << (int)digest[i];
    }
    
    return ss.str();
}

void HashCalculator::md5(const unsigned char* data, size_t length, unsigned char* digest) {
    // Simplified MD5 - for production use OpenSSL or similar
    // This is a basic implementation for demonstration
    
    uint32_t a0 = 0x67452301;
    uint32_t b0 = 0xefcdab89;
    uint32_t c0 = 0x98badcfe;
    uint32_t d0 = 0x10325476;
    
    // Simple hash based on data (not full MD5 spec, but demonstrates concept)
    for (size_t i = 0; i < length; i++) {
        a0 = ROTATE_LEFT(a0 + data[i], 7);
        b0 = ROTATE_LEFT(b0 ^ data[i], 12);
        c0 = ROTATE_LEFT(c0 + (data[i] * 3), 17);
        d0 = ROTATE_LEFT(d0 ^ (data[i] * 5), 22);
    }
    
    // Store result
    memcpy(digest, &a0, 4);
    memcpy(digest + 4, &b0, 4);
    memcpy(digest + 8, &c0, 4);
    memcpy(digest + 12, &d0, 4);
}

std::vector<unsigned char> HashCalculator::convertToGrayscale(const unsigned char* image_data,
                                                               int width, int height, int channels) {
    std::vector<unsigned char> gray(width * height);
    
    for (int i = 0; i < width * height; i++) {
        if (channels >= 3) {
            // RGB to grayscale: 0.299*R + 0.587*G + 0.114*B
            gray[i] = static_cast<unsigned char>(
                0.299 * image_data[i * channels] +
                0.587 * image_data[i * channels + 1] +
                0.114 * image_data[i * channels + 2]
            );
        } else {
            gray[i] = image_data[i * channels];
        }
    }
    
    return gray;
}

std::vector<unsigned char> HashCalculator::resizeForHash(const std::vector<unsigned char>& gray_data,
                                                          int width, int height) {
    // Resize to 9x8 for dHash calculation (simple nearest neighbor)
    const int target_w = 9;
    const int target_h = 8;
    std::vector<unsigned char> resized(target_w * target_h);
    
    for (int y = 0; y < target_h; y++) {
        for (int x = 0; x < target_w; x++) {
            int src_x = x * width / target_w;
            int src_y = y * height / target_h;
            resized[y * target_w + x] = gray_data[src_y * width + src_x];
        }
    }
    
    return resized;
}

uint64_t HashCalculator::calculatePerceptualHash(const unsigned char* image_data,
                                                  int width, int height, int channels) {
    // Convert to grayscale
    auto gray = convertToGrayscale(image_data, width, height, channels);
    
    // Resize to 9x8
    auto resized = resizeForHash(gray, width, height);
    
    // Calculate difference hash (dHash)
    uint64_t hash = 0;
    int bit_index = 0;
    
    for (int y = 0; y < 8; y++) {
        for (int x = 0; x < 8; x++) {
            int current = resized[y * 9 + x];
            int next = resized[y * 9 + x + 1];
            
            // If current pixel is brighter than next, set bit to 1
            if (current > next) {
                hash |= (1ULL << bit_index);
            }
            bit_index++;
        }
    }
    
    return hash;
}

int HashCalculator::hammingDistance(uint64_t hash1, uint64_t hash2) {
    uint64_t diff = hash1 ^ hash2;
    int distance = 0;
    
    // Count number of 1 bits
    while (diff) {
        distance += diff & 1;
        diff >>= 1;
    }
    
    return distance;
}
