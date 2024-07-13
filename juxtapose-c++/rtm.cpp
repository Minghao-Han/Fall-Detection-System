#include <opencv2/opencv.hpp>
#include <onnxruntime/core/session/onnxruntime_cxx_api.h>
#include <vector>
#include <string>
#include <filesystem>
#include <chrono>
#include <memory>
#include <iostream>
#include "PolygonZone.h"
#include "/mnt/juxtapose-c++/detection.hpp"
#include "rtmpose.hpp"
#include "BYTETracker.h"
#include "rtmdet.hpp"
#include "utils.hpp"


// Typedefs for convenience
using namespace std;
using namespace cv;
namespace fs = std::filesystem;

struct Result {
    vector<PosePoint> kpts;
    vector<Rect> bboxes;
    map<string, double> speed;
    string save_dirs;
    string name;
};

class RTM {
public:
    RTM(
        string det = "rtmdet-m",
        string pose = "rtmpose-m",
        string tracker = "bytetrack",
        string device = "cuda",
        string captions = "person ."
    ) {
        if (device == "cuda" && (ort::GetAvailableProviders().size() == 0)) {
            cout << "Auto switch to CPU, as you are running without CUDA" << endl;
            device = "cpu";
        }

        this->det = setup_detector(det, device, captions);
        this->rtmpose = RTMPose(pose.split("-")[1], device);
        this->tracker_type = tracker;

        if (TRACKER_MAP.find(tracker) == TRACKER_MAP.end()) {
            this->tracker_type = "";
        }

        this->dataset = nullptr;
        this->vid_path = "";
    }

    void setup_source(string source, int imgsz = 640, int vid_stride = 1) {
        this->dataset = load_inference_source(source, imgsz, vid_stride);
        this->source_type = this->dataset->source_type;
        this->vid_path = "";
    }

    void setup_tracker() {
        this->tracker = make_unique<BYTETracker>(this->tracker_type);
    }

    unique_ptr<Detector> setup_detector(string det, string device, string captions) {
        return get_detector(det, device, captions);
    }

    vector<pair<PolygonZone, Scalar>> setup_zones(int w, int h, vector<vector<Point>> zones) {
        vector<pair<PolygonZone, Scalar>> polygon_zones;
        for (size_t idx = 0; idx < zones.size(); ++idx) {
            polygon_zones.push_back({
                PolygonZone(zones[idx], Size(w, h), Position::CENTER),
                Scalar::all(0).from_hex(DEFAULT_COLOR_PALETTE[idx])
            });
        }
        return polygon_zones;
    }

    vector<Result> stream_inference(
        string source,
        vector<int> timer = {},
        string poi = "",
        string roi = "",
        vector<vector<Point>> zones = {},
        vector<int> framestamp = {},
        bool show = true,
        bool plot = true,
        bool plot_bboxes = true,
        bool save = false,
        string save_dirs = "",
        bool verbose = true
    ) {
        vector<chrono::high_resolution_clock::time_point> profilers(3);

        setup_source(source);

        if (!this->tracker_type.empty()) {
            setup_tracker();
        }

        fs::path current_source;
        int index = 0;
        vector<Result> results;

        for (auto& batch : *this->dataset) {
            string path = batch.path;
            Mat im0s = batch.im0s;
            VideoCapture vid_cap = batch.vid_cap;
            string s = batch.s;
            bool is_image = s.starts_with("image");

            fs::path p(path);
            Mat im = im0s.clone();
            Mat im0 = im0s.clone();

            index += 1;

            if (!framestamp.empty()) {
                if (framestamp[0] >= index) {
                    continue;
                }
                if (index > framestamp[1] + 1) {
                    break;
                }
            }

            // @param const cv::Mat& input_mat,float threshold
            vector<DetectBox> boxes= this->det(im, 0.4);//boxes include x,y,w,h,score
            // Detections detections
            //update
            if (!this->tracker_type.empty()) {
                this->tracker->update(boxes);
            }

            // kpt detection
            vector<vector<Point2f>> kpts;
            vector<float> scores;
            if (!detections.empty()) {
                // @param cv::Mat& input_mat, const vector<DetectBox>& boxs
                vector<PosePoint> kpts = this->rtmpose(im, boxes);//kpts include x,y,score
                // scores = get<1>(kptAscores);
            }

            // build result 
            Result result;
            if (!detections.empty()) {
                result.kpts = kpts;
                result.bboxes = detections.xyxy;
                result.speed["bboxes"] = chrono::duration<double, milli>(profilers[0]).count();
                result.speed["track"] = chrono::duration<double, milli>(profilers[1]).count();
                result.speed["kpts"] = chrono::duration<double, milli>(profilers[2]).count();
                result.save_dirs = save ? save_dirs : "";
                result.name = p.filename().string();
            } else {
                result.kpts = {};
                result.bboxes = {};
                result.speed["bboxes"] = chrono::duration<double, milli>(profilers[0]).count();
                result.speed["track"] = chrono::duration<double, milli>(profilers[1]).count();
                result.speed["kpts"] = chrono::duration<double, milli>(profilers[2]).count();
                result.save_dirs = save ? save_dirs : "";
                result.name = p.filename().string();
            }

            results.push_back(result);
        }

        return results;
    }

    vector<Result> operator()(
        string source,
        bool stream = false,
        vector<int> timer = {},
        string poi = "",
        string roi = "",
        vector<vector<Point>> zones = {},
        vector<int> framestamp = {},
        bool show = true,
        bool plot = true,
        bool plot_bboxes = true,
        bool save = false,
        string save_dirs = "",
        bool verbose = true,
    ) {
        if (stream) {
            return stream_inference(
                source, timer, poi, roi, zones, framestamp,
                show, plot, plot_bboxes, save, save_dirs, verbose
            );
        } else {
            return stream_inference(
                source, timer, poi, roi, zones, framestamp,
                show, plot, plot_bboxes, save, save_dirs, verbose
            );
        }
    }

private:
    unique_ptr<RTMDet> det;
    unique_ptr<RTMPose> rtmpose;
    string tracker_type;
    unique_ptr<BYTETracker> tracker;
    unique_ptr<Dataset> dataset;
    string vid_path;
};
