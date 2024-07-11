#ifndef DETECTIONS_HPP
#define DETECTIONS_HPP

#include <vector>
#include <tuple>
#include <opencv2/opencv.hpp> // For cv::Rect2f and cv::Mat
#include <stdexcept> // For std::invalid_argument

class Detections {
public:
    // Constructor
    Detections(
        const std::vector<cv::Rect2f>& xyxy,
        const std::vector<cv::Mat>& mask = std::vector<cv::Mat>(),
        const std::vector<float>& confidence = std::vector<float>(),
        const std::vector<int>& class_id = std::vector<int>(),
        const std::vector<int>& track_id = std::vector<int>()
    );
    // Default constructor
    Detections() = default;
    // Member functions
    void validate() const;
    size_t size() const;
    std::vector<std::tuple<cv::Rect2f, cv::Mat, float, int, int>> iter() const;
    bool operator==(const Detections& other) const;

    static Detections empty();
    Detections at(const std::vector<bool>& index) const;

    // Member variables
    std::vector<cv::Rect2f> xyxy;
    std::vector<cv::Mat> mask;
    std::vector<float> confidence;
    std::vector<int> class_id;
    std::vector<int> track_id;
};

#endif // DETECTIONS_HPP
