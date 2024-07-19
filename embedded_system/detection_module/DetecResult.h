#ifndef _DETECT_RESULT_H
#define _DETECT_RESULT_H 1
#include <vector>
#include <array>

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


struct Result {
    std::array<PosePoint,17> kpts;
    DetectBox bboxes;
    // map<string, double> speed;
};
struct Results {
    std::vector<std::array<PosePoint,17>> kptss;
    std::vector<DetectBox> bboxes;
    // map<string, double> speed;
};
#endif