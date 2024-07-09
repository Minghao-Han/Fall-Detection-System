#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <string.h>
#include <stdbool.h>
#include <opencv2/opencv.hpp>
#include <cjson/cJSON.h>

/**
 * @brief Convert LightTrack into C and use it as framework
 * @note instead of using yolov3(light) and MobileNetv1-Deconv to detect bbox and keypoints, here we use yolo-nas-pose to perform these two task at the same time.
 * @article{ning2019lighttrack,
  author    = {Ning, Guanghan and Huang, Heng},
  title     = {LightTrack: A Generic Framework for Online Top-Down Human Pose Tracking,
  journal   = {Proceedings of CVPRW 2020 on Towards Human-Centric Image/Video Synthesis and the 4th Look Into Person (LIP) Challenge},
  year      = {2020},
}
 */

// 全局变量定义
double total_time_POSE = 0.0, total_time_DET = 0.0, total_time_ALL = 0.0;enlarge_scale=0.2;
int total_num_frames = 0, total_num_persons = 0;keyframe_interval=10;

// 函数声明
char** get_immediate_childfile_paths(const char* folder, int* num_imgs);
bool is_keyframe(int img_id, int keyframe_interval);
std::vector<BBox_det> inference_yolov3(const char* img_path);
bool bbox_invalid(BBox_det bbox);
BBox_det enlarge_bbox(BBox_det bbox, float scale);
Keypoints inference_keypoints(PoseEstimator pose_estimator, BBox_det bbox_det_dict);
int get_track_id_SpatialConsistency(BBox_det bbox_det, std::vector<BBox_det> bbox_list_prev_frame,int *match_index);
int get_track_id_SGCN(int *match_index,BBox_det bbox, std::vector<BBox_det> bbox_list_prev_frame, Keypoints keypoints, std::vector<Keypoints> keypoints_list_prev_frame);
BBox_det get_bbox_from_keypoints(Keypoints keypoints);
bool is_target_lost(Keypoints keypoints_next);
void write_json_to_file(cJSON* json, const char* output_json_path);
void create_folder(const char* path);
void make_video_from_images(char** img_paths, int num_imgs, const char* output_video_path, int fps);



typedef struct { // include [x1, y1, box_w, box_h]
    double x1;
    double y1;
    double box_w;
    double box_h;
} Human_rect;
typedef struct {
    int img_id;
    int det_id;
    int track_id;
    const char* img_path;
    Human_rect bbox;
} BBox_det;
typedef struct {
    int img_id;
    int det_id;
    int track_id;
    const char* img_path;
    std::vector<cv::Point> keypoints;
} Keypoints;

typedef struct {} PoseEstimator;

void light_track(PoseEstimator pose_estimator, const char* image_folder, const char* output_json_path, const char* visualize_folder, const char* output_video_path) {
    //  | 统计处理总时间
    clock_t st_time_total = clock();

    // process the frames sequentially | 初始化变量
    std::vector<Keypoints> keypoints_list;
    std::vector<BBox_det> bbox_dets_list;
    int frame_prev = -1;
    int frame_cur = 0;
    int img_id = -1;
    int next_id = 0;
    std::vector<std::vector<BBox_det>> bbox_dets_list_list;
    std::vector<std::vector<Keypoints>> keypoints_list_list;

    bool flag_mandatory_keyframe = false;

    int num_imgs;
    char** img_paths = get_immediate_childfile_paths(image_folder, &num_imgs);
    total_num_frames = num_imgs;

    while (img_id < num_imgs - 1) {
        img_id++;
        char* img_path = img_paths[img_id];
        printf("Current tracking: [image_id:%d]\n", img_id);

        frame_cur = img_id;
        if (frame_cur == frame_prev) {
            frame_prev--;
        }

        // KEYFRAME: loading results from other modules | 关键帧处理
        if (is_keyframe(img_id, keyframe_interval) || flag_mandatory_keyframe) {
            flag_mandatory_keyframe = false;
            bbox_dets_list.clear();
            keypoints_list.clear();

            // perform detection at keyframes | 关键帧检测
            clock_t st_time_detection = clock();
            std::vector<Human_rect> human_candidates = inference_yolov3(img_path);
            clock_t end_time_detection = clock();
            total_time_DET += (double)(end_time_detection - st_time_detection) / CLOCKS_PER_SEC;

            int num_dets = human_candidates.size();
            printf("Keyframe: %d detections\n", num_dets);

            // if nothing detected at keyframe, regard next frame as keyframe because there is nothing to track | 如果关键帧没有检测到目标，则将下一帧作为关键帧
            if (num_dets <= 0) {
                // add empty result | 添加空结果
                BBox_det bbox_det = {img_id, 0, -1, img_path, {0, 0, 2, 2}};
                bbox_dets_list.push_back(bbox_det);
                Keypoints keypoints = {img_id, 0, -1, img_path, {}};
                keypoints_list.push_back(keypoints);

                bbox_dets_list_list.push_back(bbox_dets_list);
                keypoints_list_list.push_back(keypoints_list);

                flag_mandatory_keyframe = true;
                continue;
            }

            // 2. statistics: get total number of detected persons | 统计：获取检测到的总人数
            total_num_persons += num_dets;

            std::vector<BBox_det> bbox_list_prev_frame;
            std::vector<Keypoints> keypoints_list_prev_frame;

            // First frame does not have previous frame | 第一帧没有前一帧
            if (img_id > 0) {
                bbox_list_prev_frame = bbox_dets_list_list[img_id - 1];
                keypoints_list_prev_frame = keypoints_list_list[img_id - 1];
            }

            for (int det_id = 0; det_id < num_dets; det_id++) {
                //  obtain bbox position and track id | 获取bbox位置和track id
                Human_rect human_rect = human_candidates[det_id];

                // enlarge bbox by 20% with same center position | 以相同中心位置将bbox扩大20%
                // xywh_to_x1y1x2y2(bbox_det),enlarge_bbox(bbox_x1y1x2y2, enlarge_scale),x1y1x2y2_to_xywh(bbox_in_xywh)
                human_rect = enlarge_bbox(human_rect, enlarge_scale);

                BBox_det bbox_det;
                if (bbox_invalid(bbox_det)) {
                    // update current frame bbox | 更新当前帧bbox
                    human_rect = {0, 0, 2, 2};
                    bbox_det = {img_id, det_id, -1, img_path,human_rect};
                    bbox_dets_list.push_back(bbox_det);
                    // update current frame keypoints | 更新当前帧关键点
                    Keypoints keypoints = {img_id, det_id, -1, img_path, {}};
                    keypoints_list.push_back(keypoints);
                    
                    continue;
                }
                // update current frame bbox | 更新当前帧bbox
                bbox_det.img_id = img_id;
                bbox_det.det_id = det_id;
                bbox_det.bbox=human_rect;
                bbox_det.track_id = next_id; // ?
                next_id++;

                clock_t st_time_pose = clock();
                Keypoints keypoints = inference_keypoints(pose_estimator, bbox_det);
                clock_t end_time_pose = clock();
                total_time_POSE += (double)(end_time_pose - st_time_pose) / CLOCKS_PER_SEC;

                if (img_id == 0) {
                    track_id = next_id;
                    next_id++;
                } else {
                    int match_index;
                    track_id = get_track_id_SpatialConsistency(human_rect, bbox_list_prev_frame,&match_index);

                    if (track_id != -1) {
                        bbox_list_prev_frame.erase(bbox_list_prev_frame.begin() + match_index);
                        keypoints_list_prev_frame.erase(keypoints_list_prev_frame.begin() + match_index);
                    }
                }
                // update current frame bbox | 更新当前帧bbox
                bbox_det={img_id,det_id,track_id,img_path,bbox_det};
                bbox_dets_list.push_back(bbox_det);
                // update current frame keypoints | 更新当前帧关键点
                keypoints={img_id,det_id,track_id, img_path,keypoints}
                keypoints_list.push_back(keypoints);
            }
            // For candidate that is not assopciated yet, perform data association based on Pose Similarity (SGCN) | 对尚未关联的候选对象，基于姿态相似性（SGCN）进行数据关联
            for (int det_id = 0; det_id < num_dets; det_id++) {
                BBox_det bbox_det = bbox_dets_list[det_id];
                Keypoints keypoints = keypoints_list[det_id];

                if (bbox_det.track_id == -1) { // this id means matching not found yet | 此id表示尚未找到匹配
                    int match_index;
                    int track_id = get_track_id_SGCN(&match_index,bbox_det.bbox, bbox_list_prev_frame, keypoints.keypoints, keypoints_list_prev_frame);

                    if (track_id != -1) { // if candidate from prev frame matched, prevent it from matching another | 如果前一帧的候选对象匹配成功，则防止其匹配另一个
                        bbox_list_prev_frame.erase(bbox_list_prev_frame.begin() + match_index);
                        keypoints_list_prev_frame.erase(keypoints_list_prev_frame.begin() + match_index);
                        bbox_det.track_id = track_id;
                        keypoints.track_id = track_id;
                        next_id++;
                    }
                    // if still can not find a match from previous frame, then assign a new id | 如果仍然无法找到前一帧的匹配项，则分配一个新的id
                    if (track_id == -1 && !bbox_invalid(bbox_det.bbox)){
                        bbox_det.track_id = next_id;
                        keypoints.track_id = next_id;
                        next_id++;
                    }
                }
            }
            // update frame | 更新帧
            bbox_dets_list_list.push_back(bbox_dets_list);
            keypoints_list_list.push_back(keypoints_list);
            frame_prev = frame_cur;

        } 
        // NOT KEYFRAME: multi-target pose tracking | 非关键帧：多目标姿态跟踪
        else {
            std::vector<BBox_det> bbox_dets_list_next;
            std::vector<Keypoints> keypoints_list_next;

            int num_dets = keypoints_list.size();
            total_num_persons += num_dets;

            if (num_dets == 0) {
                flag_mandatory_keyframe = true;
            }

            for (int det_id = 0; det_id < num_dets; det_id++) {
                Keypoints keypoints = keypoints_list[det_id];

                // for non-keyframes, the tracked target preserves its track_id | 对于非关键帧，跟踪目标保留其track_id
                int track_id = keypoints_list[det_id].track_id;
                // next frame bbox | 下一帧bbox
                BBox_det bbox_det_next = get_bbox_from_keypoints(keypoints);

                if (bbox_det_next.width == 0 || bbox_det_next.height == 0) {
                    bbox_det_next = {0, 0, 2, 2};
                    total_num_persons--;
                }

                bbox_det_next.img_id = img_id;
                bbox_det_next.det_id = det_id;
                bbox_det_next.track_id = track_id;

                clock_t st_time_pose = clock();
                Keypoints keypoints_next = inference_keypoints(pose_estimator, bbox_det_next);
                clock_t end_time_pose = clock();
                total_time_POSE += (double)(end_time_pose - st_time_pose) / CLOCKS_PER_SEC;

                bool target_lost = is_target_lost(keypoints_next);

                if (!target_lost) {
                    bbox_dets_list_next.push_back(bbox_det_next);
                    Keypoints keypoints_dict_next = {img_id, det_id, track_id, img_path, keypoints_next};
                    keypoints_list_next.push_back(keypoints_dict_next);
                } else {
                    BBox_det bbox_det_dict_next = {img_id, det_id, -1, img_path, {0, 0, 2, 2}};
                    bbox_dets_list_next.push_back(bbox_det_dict_next);
                    Keypoints keypoints_null = {img_id, det_id, -1, img_path, {}};
                    keypoints_list_next.push_back(keypoints_null);
                    printf("Target lost. Process this frame again as keyframe.\n\n\n");
                    flag_mandatory_keyframe = true;
                    total_num_persons--;

                    if (img_id != 0) {
                        img_id--;
                    }
                    break;
                }
            }

            if (!flag_mandatory_keyframe) {
                bbox_dets_list = bbox_dets_list_next;
                keypoints_list = keypoints_list_next;
                bbox_dets_list_list.push_back(bbox_dets_list);
                keypoints_list_list.push_back(keypoints_list);
                frame_prev = frame_cur;
            }
        }
    }

    clock_t end_time_total = clock();
    total_time_ALL += (double)(end_time_total - st_time_total) / CLOCKS_PER_SEC;

    // 转换结果为openSVAI格式
    printf("Exporting Results in openSVAI Standard Json Format...\n");
    cJSON* poses_standard = pose_to_standard_mot(keypoints_list_list, bbox_dets_list_list);
    write_json_to_file(poses_standard, output_json_path);
    printf("Json Export Finished!\n");

    // 可视化
    if (flag_visualize) {
        printf("Visualizing Pose Tracking Results...\n");
        create_folder(visualize_folder);
        show_all_from_standard_json(output_json_path, classes, joint_pairs, joint_names, image_folder, visualize_folder, true);
        printf("Visualization Finished!\n");

        int num_visualize_imgs;
        char** img_paths = get_immediate_childfile_paths(visualize_folder, &num_visualize_imgs);
        int avg_fps = total_num_frames / total_time_ALL;
        make_video_from_images(img_paths, num_visualize_imgs, output_video_path, 25);
    }
}

int main() {
    PoseEstimator pose_estimator;
    light_track(pose_estimator, "image_folder", "output.json", "visualize_folder", "output_video.avi");
    return 0;
}
