#ifndef _UTILS_H
#define _UTILS_H 1
#include <vector>
#include <opencv2/opencv.hpp>

using namespace cv;
using namespace std;
const std::vector<float> IMAGE_MEAN{ 123.675, 116.28, 103.53 };
const std::vector<float> IMAGE_STD{ 58.395, 57.12, 57.375 };


typedef struct DetectBox
{
	float left;
	float top;
	float right;
	float bottom;
	float score;
	int label;

	DetectBox()
	{
		left = -1.0;
		top = -1.0;
		right = -1.0;
		bottom = -1.0;
		score = -1.0;
		label = -1;
	}

	bool IsValid() const
	{
		return left != -1 && top != -1 && right != -1 && bottom != -1 && score != -1.0 && label != -1;
	}
	DetectBox(float left, float top, float right, float bottom, float score, int label)
		: left(left), top(top), right(right), bottom(bottom), score(score), label(label) {}
}DetectBox;
bool BoxCompare(const DetectBox& a,const DetectBox& b);
typedef struct PosePoint
{
	float x;
	float y;
	float score;

	PosePoint()
	{
		x = 0.0;
		y = 0.0;
		score = 0.0;
	}
}PosePoint;


typedef PosePoint Vector2D;
cv::Mat GetAffineTransform(float center_x, float center_y, float scale_width, float scale_height, int output_image_width, int output_image_height, bool inverse = false);

vector<vector<Point>> xyxy2xyxyxyxy(const vector<Rect>& xyxy);
cv::Mat box_iou_batch(const cv::Mat& boxes_true, const cv::Mat& boxes_detection);
std::vector<int> non_max_suppression(const cv::Mat& predictions, float iou_threshold = 0.5);
tuple<vector<vector<Point2f>>, vector<vector<float>>> get_simcc_maximum(const vector<Mat>& simcc_x,const vector<Mat>& simcc_y);
tuple<vector<Point2f>, vector<Point2f>> bbox_xyxy2cs(const vector<Rect>& bbox,float padding = 1.0);
tuple<Mat, Point2f> top_down_affine(const Size& input_size,const Point2f& bbox_scale,const Point2f& bbox_center,const Mat& img);
#endif