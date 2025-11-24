#include "performance_tracker.h"
#include <iostream>
#include <iomanip>
#include <cmath>

PerformanceTracker::PerformanceTracker() 
    : is_running(false), total_images(0), successful_images(0), 
      failed_images(0), duplicates_found(0), threads_used(1) {
}

void PerformanceTracker::start() {
    start_time = std::chrono::high_resolution_clock::now();
    is_running = true;
}

void PerformanceTracker::stop() {
    end_time = std::chrono::high_resolution_clock::now();
    is_running = false;
}

void PerformanceTracker::reset() {
    is_running = false;
    total_images = 0;
    successful_images = 0;
    failed_images = 0;
    duplicates_found = 0;
    threads_used = 1;
}

void PerformanceTracker::incrementSuccess() {
    successful_images++;
}

void PerformanceTracker::incrementFailure() {
    failed_images++;
}

void PerformanceTracker::setDuplicatesFound(int count) {
    duplicates_found = count;
}

void PerformanceTracker::setThreadsUsed(int count) {
    threads_used = count;
}

void PerformanceTracker::setTotalImages(int count) {
    total_images = count;
}

double PerformanceTracker::getElapsedMilliseconds() const {
    auto end = is_running ? std::chrono::high_resolution_clock::now() : end_time;
    auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(end - start_time);
    return duration.count();
}

PerformanceTracker::Statistics PerformanceTracker::getStatistics() const {
    Statistics stats;
    stats.total_time_ms = getElapsedMilliseconds();
    stats.total_images = total_images;
    stats.successful_images = successful_images;
    stats.failed_images = failed_images;
    stats.duplicates_found = duplicates_found;
    stats.threads_used = threads_used;
    
    double total_time_sec = stats.total_time_ms / 1000.0;
    stats.images_per_second = (total_time_sec > 0) ? (successful_images / total_time_sec) : 0;
    stats.avg_time_per_image_ms = (successful_images > 0) ? (stats.total_time_ms / successful_images) : 0;
    
    // Speedup and efficiency will be calculated in comparison
    stats.speedup = 0.0;
    stats.efficiency = 0.0;
    
    return stats;
}

void PerformanceTracker::printStatistics(const std::string& mode_name) const {
    Statistics stats = getStatistics();
    
    std::cout << "\n========================================\n";
    std::cout << "  " << mode_name << " Mode Statistics\n";
    std::cout << "========================================\n";
    std::cout << std::fixed << std::setprecision(2);
    std::cout << "Total Time:          " << stats.total_time_ms / 1000.0 << " seconds\n";
    std::cout << "Total Images:        " << stats.total_images << "\n";
    std::cout << "Successful:          " << stats.successful_images << "\n";
    std::cout << "Failed:              " << stats.failed_images << "\n";
    std::cout << "Duplicates Found:    " << stats.duplicates_found << "\n";
    std::cout << "Threads Used:        " << stats.threads_used << "\n";
    std::cout << "Throughput:          " << stats.images_per_second << " images/sec\n";
    std::cout << "Avg Time/Image:      " << stats.avg_time_per_image_ms << " ms\n";
    std::cout << "========================================\n";
}

void PerformanceTracker::printComparison(const Statistics& serial, const Statistics& parallel) {
    double speedup = (parallel.total_time_ms > 0) ? (serial.total_time_ms / parallel.total_time_ms) : 0;
    double efficiency = (parallel.threads_used > 0) ? (speedup / parallel.threads_used) * 100.0 : 0;
    
    std::cout << "\n========================================\n";
    std::cout << "  SERIAL vs PARALLEL COMPARISON\n";
    std::cout << "========================================\n";
    std::cout << std::fixed << std::setprecision(2);
    
    std::cout << "\nExecution Time:\n";
    std::cout << "  Serial:            " << serial.total_time_ms / 1000.0 << " seconds\n";
    std::cout << "  Parallel:          " << parallel.total_time_ms / 1000.0 << " seconds\n";
    
    std::cout << "\nThroughput:\n";
    std::cout << "  Serial:            " << serial.images_per_second << " images/sec\n";
    std::cout << "  Parallel:          " << parallel.images_per_second << " images/sec\n";
    
    std::cout << "\nAverage Time per Image:\n";
    std::cout << "  Serial:            " << serial.avg_time_per_image_ms << " ms\n";
    std::cout << "  Parallel:          " << parallel.avg_time_per_image_ms << " ms\n";
    
    std::cout << "\nParallelization Metrics:\n";
    std::cout << "  Threads Used:      " << parallel.threads_used << "\n";
    std::cout << "  Speedup:           " << speedup << "x\n";
    std::cout << "  Efficiency:        " << efficiency << "%\n";
    
    std::cout << "\nDuplicates Detection:\n";
    std::cout << "  Serial Found:      " << serial.duplicates_found << "\n";
    std::cout << "  Parallel Found:    " << parallel.duplicates_found << "\n";
    
    if (serial.duplicates_found == parallel.duplicates_found) {
        std::cout << "  ✓ Results Match!\n";
    } else {
        std::cout << "  ✗ Results Differ!\n";
    }
    
    std::cout << "========================================\n";
}
