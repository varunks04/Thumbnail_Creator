#include <iostream>
#include <string>
#include <vector>
#include <filesystem>
#include <omp.h>

#include "image_processor.h"
#include "hash_calculator.h"
#include "duplicate_detector.h"
#include "performance_tracker.h"

namespace fs = std::filesystem;

struct Config {
    std::string input_dir;
    std::string output_dir;
    int thumbnail_size = 256;
    int hamming_threshold = 8;
    int num_threads = 0;  // 0 = use all available
    bool run_serial = true;
    bool run_parallel = true;
    bool compare_modes = true;
};

void printUsage(const char* program_name) {
    std::cout << "Usage: " << program_name << " [options]\n\n";
    std::cout << "Options:\n";
    std::cout << "  -i <dir>     Input directory with images (required)\n";
    std::cout << "  -o <dir>     Output directory for thumbnails (default: ./output/thumbnails)\n";
    std::cout << "  -s <size>    Thumbnail size in pixels (default: 256)\n";
    std::cout << "  -t <value>   Hamming distance threshold for duplicates (default: 8)\n";
    std::cout << "  -n <num>     Number of threads for parallel mode (default: all available)\n";
    std::cout << "  --serial     Run only serial mode\n";
    std::cout << "  --parallel   Run only parallel mode\n";
    std::cout << "  -h, --help   Show this help message\n\n";
    std::cout << "Example:\n";
    std::cout << "  " << program_name << " -i ./photos -o ./thumbnails -s 256 -t 8\n";
}

bool parseArguments(int argc, char* argv[], Config& config) {
    for (int i = 1; i < argc; i++) {
        std::string arg = argv[i];
        
        if (arg == "-h" || arg == "--help") {
            printUsage(argv[0]);
            return false;
        }
        else if (arg == "-i" && i + 1 < argc) {
            config.input_dir = argv[++i];
        }
        else if (arg == "-o" && i + 1 < argc) {
            config.output_dir = argv[++i];
        }
        else if (arg == "-s" && i + 1 < argc) {
            config.thumbnail_size = std::stoi(argv[++i]);
        }
        else if (arg == "-t" && i + 1 < argc) {
            config.hamming_threshold = std::stoi(argv[++i]);
        }
        else if (arg == "-n" && i + 1 < argc) {
            config.num_threads = std::stoi(argv[++i]);
        }
        else if (arg == "--serial") {
            config.run_serial = true;
            config.run_parallel = false;
            config.compare_modes = false;
        }
        else if (arg == "--parallel") {
            config.run_serial = false;
            config.run_parallel = true;
            config.compare_modes = false;
        }
    }
    
    return !config.input_dir.empty();
}

std::vector<std::string> collectImageFiles(const std::string& directory) {
    std::vector<std::string> image_files;
    
    try {
        for (const auto& entry : fs::recursive_directory_iterator(directory)) {
            if (entry.is_regular_file()) {
                std::string filepath = entry.path().string();
                if (ImageProcessor::isImageFile(filepath)) {
                    image_files.push_back(filepath);
                }
            }
        }
    } catch (const std::exception& e) {
        std::cerr << "Error scanning directory: " << e.what() << std::endl;
    }
    
    return image_files;
}

void processImagesSerial(const std::vector<std::string>& image_files,
                        const Config& config,
                        PerformanceTracker& tracker,
                        DuplicateDetector& detector) {
    std::cout << "\n[SERIAL MODE] Processing " << image_files.size() << " images...\n";
    
    tracker.reset();
    tracker.setTotalImages(image_files.size());
    tracker.setThreadsUsed(1);
    detector.clear();
    
    tracker.start();
    
    for (const auto& filepath : image_files) {
        // Generate output path
        fs::path input_path(filepath);
        std::string output_filename = input_path.stem().string() + "_thumb.jpg";
        fs::path output_path = fs::path(config.output_dir) / output_filename;
        
        // Process image
        bool success = false;
        uint64_t phash = ImageProcessor::processSingleImage(
            filepath, output_path.string(), config.thumbnail_size, success
        );
        
        if (success) {
            tracker.incrementSuccess();
            std::string md5 = HashCalculator::calculateMD5(filepath);
            detector.addImageHash(filepath, md5, phash);
        } else {
            tracker.incrementFailure();
        }
    }
    
    // Find duplicates
    detector.findDuplicates();
    tracker.setDuplicatesFound(detector.getDuplicateCount());
    
    tracker.stop();
    tracker.printStatistics("SERIAL");
}

void processImagesParallel(const std::vector<std::string>& image_files,
                          const Config& config,
                          PerformanceTracker& tracker,
                          DuplicateDetector& detector) {
    // Set number of threads
    int num_threads = config.num_threads > 0 ? config.num_threads : omp_get_max_threads();
    omp_set_num_threads(num_threads);
    
    std::cout << "\n[PARALLEL MODE] Processing " << image_files.size() 
              << " images with " << num_threads << " threads...\n";
    
    tracker.reset();
    tracker.setTotalImages(image_files.size());
    tracker.setThreadsUsed(num_threads);
    detector.clear();
    
    // Thread-local counters
    std::vector<int> thread_success(num_threads, 0);
    std::vector<int> thread_failure(num_threads, 0);
    
    // Store hashes temporarily (thread-safe collection)
    std::vector<std::string> filepaths_vec(image_files.size());
    std::vector<std::string> md5_vec(image_files.size());
    std::vector<uint64_t> phash_vec(image_files.size());
    std::vector<bool> success_vec(image_files.size());
    
    tracker.start();
    
    #pragma omp parallel for schedule(dynamic)
    for (int i = 0; i < static_cast<int>(image_files.size()); i++) {
        int thread_id = omp_get_thread_num();
        
        const std::string& filepath = image_files[i];
        
        // Generate output path
        fs::path input_path(filepath);
        std::string output_filename = input_path.stem().string() + "_thumb.jpg";
        fs::path output_path = fs::path(config.output_dir) / output_filename;
        
        // Process image
        bool success = false;
        uint64_t phash = ImageProcessor::processSingleImage(
            filepath, output_path.string(), config.thumbnail_size, success
        );
        
        filepaths_vec[i] = filepath;
        success_vec[i] = success;
        phash_vec[i] = phash;
        
        if (success) {
            thread_success[thread_id]++;
            md5_vec[i] = HashCalculator::calculateMD5(filepath);
        } else {
            thread_failure[thread_id]++;
        }
    }
    
    tracker.stop();
    
    // Aggregate results
    int total_success = 0;
    int total_failure = 0;
    for (int i = 0; i < num_threads; i++) {
        total_success += thread_success[i];
        total_failure += thread_failure[i];
    }
    
    // Update tracker
    for (int i = 0; i < total_success; i++) {
        tracker.incrementSuccess();
    }
    for (int i = 0; i < total_failure; i++) {
        tracker.incrementFailure();
    }
    
    // Add hashes to detector
    for (size_t i = 0; i < image_files.size(); i++) {
        if (success_vec[i]) {
            detector.addImageHash(filepaths_vec[i], md5_vec[i], phash_vec[i]);
        }
    }
    
    // Find duplicates
    detector.findDuplicates();
    tracker.setDuplicatesFound(detector.getDuplicateCount());
    
    tracker.printStatistics("PARALLEL");
}

int main(int argc, char* argv[]) {
    Config config;
    config.output_dir = "./output/thumbnails";
    
    if (!parseArguments(argc, argv, config)) {
        if (config.input_dir.empty()) {
            printUsage(argv[0]);
            return 1;
        }
        return 0;
    }
    
    // Create output directory
    try {
        fs::create_directories(config.output_dir);
    } catch (const std::exception& e) {
        std::cerr << "Error creating output directory: " << e.what() << std::endl;
        return 1;
    }
    
    // Collect image files
    std::cout << "Scanning directory: " << config.input_dir << std::endl;
    auto image_files = collectImageFiles(config.input_dir);
    
    if (image_files.empty()) {
        std::cerr << "No image files found in directory: " << config.input_dir << std::endl;
        return 1;
    }
    
    std::cout << "Found " << image_files.size() << " image files.\n";
    std::cout << "Thumbnail size: " << config.thumbnail_size << "px\n";
    std::cout << "Hamming threshold: " << config.hamming_threshold << "\n";
    
    // Performance trackers and duplicate detectors
    PerformanceTracker serial_tracker, parallel_tracker;
    DuplicateDetector serial_detector(config.hamming_threshold);
    DuplicateDetector parallel_detector(config.hamming_threshold);
    
    // Run serial mode
    if (config.run_serial) {
        processImagesSerial(image_files, config, serial_tracker, serial_detector);
        serial_detector.printDuplicateReport();
    }
    
    // Run parallel mode
    if (config.run_parallel) {
        processImagesParallel(image_files, config, parallel_tracker, parallel_detector);
        parallel_detector.printDuplicateReport();
    }
    
    // Compare modes
    if (config.compare_modes) {
        auto serial_stats = serial_tracker.getStatistics();
        auto parallel_stats = parallel_tracker.getStatistics();
        PerformanceTracker::printComparison(serial_stats, parallel_stats);
    }
    
    std::cout << "\nProcessing complete!\n";
    std::cout << "Thumbnails saved to: " << config.output_dir << "\n";
    
    return 0;
}
