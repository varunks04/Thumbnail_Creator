#ifndef DUPLICATE_DETECTOR_H
#define DUPLICATE_DETECTOR_H

#include <string>
#include <vector>
#include <map>
#include <cstdint>

class DuplicateDetector {
public:
    struct ImageHash {
        std::string filepath;
        std::string md5_hash;
        uint64_t perceptual_hash;
    };
    
    struct DuplicateGroup {
        std::vector<std::string> filepaths;
        int similarity_score;  // Hamming distance (lower = more similar)
    };
    
    DuplicateDetector(int hamming_threshold = 8);
    
    // Add image hash to database
    void addImageHash(const std::string& filepath, const std::string& md5, uint64_t phash);
    
    // Find duplicates based on MD5 (exact) and perceptual hash (similar)
    std::vector<DuplicateGroup> findDuplicates();
    
    // Get total number of duplicate images found
    int getDuplicateCount() const;
    
    // Clear all stored hashes
    void clear();
    
    // Print duplicate report
    void printDuplicateReport() const;

private:
    std::vector<ImageHash> image_hashes;
    int hamming_threshold;
    std::vector<DuplicateGroup> duplicate_groups;
};

#endif // DUPLICATE_DETECTOR_H
