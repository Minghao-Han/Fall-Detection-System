#include "BYTETracker.h"
#include <iostream>
#include <chrono>

int main() {
    BYTETracker tracker(10,30);
    Detections detections;
    detections.xyxy = {
        {176.64, 130.05, 405.93, 572.22},
        {418.3, 163.69, 725.85, 541.73},
        {559.03, 81.396, 815.69, 545.73}
    };
    detections.confidence={0.87704,0.86072,0.83821};

    auto start = std::chrono::high_resolution_clock::now();
    tracker.update(detections);
    auto end = std::chrono::high_resolution_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::microseconds>(end - start);
    std::cout << "Update runtime: " << duration.count() << " microseconds" << std::endl;
    return 0;
}