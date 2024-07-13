#include "/mnt/juxtapose-c++/detection.hpp"
#include <stdexcept>
#include <algorithm>
#include "utils.hpp"

Detections::Detections(
    const std::vector<cv::Rect2f>& xyxy,
    const std::vector<cv::Mat>& mask,
    const std::vector<float>& confidence,
    const std::vector<int>& class_id,
    const std::vector<int>& track_id
) : xyxy(xyxy), mask(mask), confidence(confidence), class_id(class_id), track_id(track_id) {
    validate();
}

void Detections::validate() const {
    size_t n = xyxy.size();
    if (!mask.empty() && mask.size() != n) {
        throw std::invalid_argument("mask must be 3d np.ndarray with (n, W, H) shape");
    }
    if (!class_id.empty() && class_id.size() != n) {
        throw std::invalid_argument("class_id must be 1d np.ndarray with (n,) shape");
    }
    if (!confidence.empty() && confidence.size() != n) {
        throw std::invalid_argument("confidence must be 1d np.ndarray with (n,) shape");
    }
    if (!track_id.empty() && track_id.size() != n) {
        throw std::invalid_argument("track_id must be 1d np.ndarray with (n,) shape");
    }
}

size_t Detections::size() const {
    return xyxy.size();
}

std::vector<std::tuple<cv::Rect2f, cv::Mat, float, int, int>> Detections::iter() const {
    std::vector<std::tuple<cv::Rect2f, cv::Mat, float, int, int>> results;
    for (size_t i = 0; i < xyxy.size(); ++i) {
        results.push_back(std::make_tuple(
            xyxy[i],
            mask.empty() ? cv::Mat() : mask[i],
            confidence.empty() ? 0.0f : confidence[i],
            class_id.empty() ? -1 : class_id[i],
            track_id.empty() ? -1 : track_id[i]
        ));
    }
    return results;
}

bool Detections::operator==(const Detections& other) const {
    return xyxy == other.xyxy &&
           mask == other.mask &&
           confidence == other.confidence &&
           class_id == other.class_id &&
           track_id == other.track_id;
}

Detections Detections::empty() {
    return Detections(std::vector<cv::Rect2f>());
}

Detections Detections::at(const std::vector<bool>& index) const {
    std::vector<cv::Rect2f> xyxy_selected;
    std::vector<cv::Mat> mask_selected;
    std::vector<float> confidence_selected;
    std::vector<int> class_id_selected;
    std::vector<int> tracker_id_selected;

    for (size_t i = 0; i < index.size(); ++i) {
        if (index[i]) {
            xyxy_selected.push_back(xyxy[i]);
            if (!mask.empty()) mask_selected.push_back(mask[i]);
            if (!confidence.empty()) confidence_selected.push_back(confidence[i]);
            if (!class_id.empty()) class_id_selected.push_back(class_id[i]);
            if (!track_id.empty()) tracker_id_selected.push_back(track_id[i]);
        }
    }
    return Detections(xyxy_selected, mask_selected, confidence_selected, class_id_selected, tracker_id_selected);
}

// Detections Detections::with_nms(float threshold, bool class_agnostic) const {
//     if (xyxy.empty()) return *this;

//     if (confidence.empty()) {
//         throw std::runtime_error("Detections confidence must be given for NMS to be executed.");
//     }

//     vector<int> indices;
//     if (class_agnostic) {
//         vector<vector<float>> predictions;
//         for (size_t i = 0; i < xyxy.size(); ++i) {
//             vector<float> pred = {
//                 static_cast<float>(xyxy[i].x),
//                 static_cast<float>(xyxy[i].y),
//                 static_cast<float>(xyxy[i].width + xyxy[i].x),
//                 static_cast<float>(xyxy[i].height + xyxy[i].y),
//                 confidence[i]
//             };
//             predictions.push_back(pred);
//         }
//         indices = non_max_suppression(predictions, threshold, class_agnostic);
//     } else {
//         if (class_id.empty()) {
//             throw std::runtime_error("Detections class_id must be given for NMS to be executed. If you intended to perform class agnostic NMS set class_agnostic=True.");
//         }
//         vector<vector<float>> predictions;
//         for (size_t i = 0; i < xyxy.size(); ++i) {
//             vector<float> pred = {
//                 static_cast<float>(xyxy[i].x),
//                 static_cast<float>(xyxy[i].y),
//                 static_cast<float>(xyxy[i].width + xyxy[i].x),
//                 static_cast<float>(xyxy[i].height + xyxy[i].y),
//                 confidence[i],
//                 static_cast<float>(class_id[i])
//             };
//             predictions.push_back(pred);
//         }
//         indices = non_max_suppression(predictions, threshold, class_agnostic);
//     }

//     vector<bool> index(xyxy.size(), false);
//     for (int idx : indices) {
//         index[idx] = true;
//     }
//     return at(index);
// }