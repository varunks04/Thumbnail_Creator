#ifndef PERFORMANCE_TRACKER_H
#define PERFORMANCE_TRACKER_H

#include <chrono>
#include <string>
#include <vector>

class PerformanceTracker {
public:
    struct Statistics {
        double total_time_ms;
        double images_per_second;
        int total_images;
        int successful_images;
        int failed_images;
        int duplicates_found;
        int threads_used;
        double avg_time_per_image_ms;
        double speedup;
        double efficiency;
    };

    PerformanceTracker();
    
    void start();
    void stop();
    void reset();
    
    void incrementSuccess();
    void incrementFailure();
    void setDuplicatesFound(int count);
    void setThreadsUsed(int count);
    void setTotalImages(int count);
    
    double getElapsedMilliseconds() const;
    Statistics getStatistics() const;
    
    void printStatistics(const std::string& mode_name) const;
    
    // For comparison between serial and parallel
    static void printComparison(const Statistics& serial, const Statistics& parallel);

private:
    std::chrono::high_resolution_clock::time_point start_time;
    std::chrono::high_resolution_clock::time_point end_time;
    bool is_running;
    
    int total_images;
    int successful_images;
    int failed_images;
    int duplicates_found;
    int threads_used;
};

#endif // PERFORMANCE_TRACKER_H
