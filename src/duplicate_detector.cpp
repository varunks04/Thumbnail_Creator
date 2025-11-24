#include "duplicate_detector.h"
#include "hash_calculator.h"
#include <iostream>
#include <algorithm>
#include <set>

DuplicateDetector::DuplicateDetector(int hamming_threshold) 
    : hamming_threshold(hamming_threshold) {
}

void DuplicateDetector::addImageHash(const std::string& filepath, const std::string& md5, uint64_t phash) {
    ImageHash img_hash;
    img_hash.filepath = filepath;
    img_hash.md5_hash = md5;
    img_hash.perceptual_hash = phash;
    
    image_hashes.push_back(img_hash);
}

std::vector<DuplicateDetector::DuplicateGroup> DuplicateDetector::findDuplicates() {
    duplicate_groups.clear();
    std::set<size_t> processed_indices;
    
    // First pass: Find exact duplicates using MD5
    std::map<std::string, std::vector<std::string>> md5_groups;
    for (const auto& img : image_hashes) {
        if (!img.md5_hash.empty()) {
            md5_groups[img.md5_hash].push_back(img.filepath);
        }
    }
    
    // Add exact duplicate groups
    for (const auto& pair : md5_groups) {
        if (pair.second.size() > 1) {
            DuplicateGroup group;
            group.filepaths = pair.second;
            group.similarity_score = 0;  // Exact match
            duplicate_groups.push_back(group);
        }
    }
    
    // Second pass: Find similar images using perceptual hash
    for (size_t i = 0; i < image_hashes.size(); i++) {
        if (processed_indices.count(i)) continue;
        
        std::vector<std::string> similar_group;
        similar_group.push_back(image_hashes[i].filepath);
        processed_indices.insert(i);
        
        for (size_t j = i + 1; j < image_hashes.size(); j++) {
            if (processed_indices.count(j)) continue;
            
            // Skip if already in exact duplicate group
            if (image_hashes[i].md5_hash == image_hashes[j].md5_hash && 
                !image_hashes[i].md5_hash.empty()) {
                continue;
            }
            
            // Calculate Hamming distance
            int distance = HashCalculator::hammingDistance(
                image_hashes[i].perceptual_hash,
                image_hashes[j].perceptual_hash
            );
            
            if (distance <= hamming_threshold && distance > 0) {
                similar_group.push_back(image_hashes[j].filepath);
                processed_indices.insert(j);
            }
        }
        
        // Add group if we found similar images
        if (similar_group.size() > 1) {
            DuplicateGroup group;
            group.filepaths = similar_group;
            group.similarity_score = hamming_threshold;  // Approximate
            duplicate_groups.push_back(group);
        }
    }
    
    return duplicate_groups;
}

int DuplicateDetector::getDuplicateCount() const {
    int count = 0;
    for (const auto& group : duplicate_groups) {
        // Count all images except the first one in each group as duplicates
        count += group.filepaths.size() - 1;
    }
    return count;
}

void DuplicateDetector::clear() {
    image_hashes.clear();
    duplicate_groups.clear();
}

void DuplicateDetector::printDuplicateReport() const {
    if (duplicate_groups.empty()) {
        std::cout << "\nNo duplicates found.\n";
        return;
    }
    
    std::cout << "\n========================================\n";
    std::cout << "  DUPLICATE DETECTION REPORT\n";
    std::cout << "========================================\n";
    std::cout << "Total duplicate groups: " << duplicate_groups.size() << "\n";
    std::cout << "Total duplicate images: " << getDuplicateCount() << "\n\n";
    
    for (size_t i = 0; i < duplicate_groups.size(); i++) {
        const auto& group = duplicate_groups[i];
        std::cout << "Group " << (i + 1) << " (";
        if (group.similarity_score == 0) {
            std::cout << "Exact duplicates";
        } else {
            std::cout << "Similar images, threshold: " << group.similarity_score;
        }
        std::cout << "):\n";
        
        for (const auto& filepath : group.filepaths) {
            std::cout << "  - " << filepath << "\n";
        }
        std::cout << "\n";
    }
    
    std::cout << "========================================\n";
}
