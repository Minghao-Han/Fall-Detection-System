#include <algorithm>
#include "utils.hpp"

using namespace std;
using namespace cv;

vector<vector<Point>> xyxy2xyxyxyxy(const vector<Rect2f>& xyxy) {
    vector<vector<Point>> xyxyxyxy;
    for (const auto& _xyxy : xyxy) {
        int x1 = _xyxy.x;
        int y1 = _xyxy.y;
        int x2 = _xyxy.x + _xyxy.width;
        int y2 = _xyxy.y + _xyxy.height;

        int x_min = min(x1, x2);
        int y_min = min(y1, y2);
        int x_max = max(x1, x2);
        int y_max = max(y1, y2);

        xyxyxyxy.push_back({
            Point(x_min, y_min), 
            Point(x_min, y_max), 
            Point(x_max, y_max), 
            Point(x_max, y_min)
        });
    }
    return xyxyxyxy;
}

Mat box_iou_batch(const Mat& boxes_true, const Mat& boxes_detection) {
    auto box_area = [](const Mat& box) {
        return (box.col(2) - box.col(0)).mul(box.col(3) - box.col(1));
    };

    Mat area_true = box_area(boxes_true);
    Mat area_detection = box_area(boxes_detection);

    Mat top_left = max(boxes_true.colRange(0, 2), boxes_detection.colRange(0, 2).t());
    Mat bottom_right = min(boxes_true.colRange(2, 4), boxes_detection.colRange(2, 4).t());

    Mat area_inter = (bottom_right - top_left).mul(bottom_right - top_left);
    area_inter.setTo(0, area_inter < 0);

    return area_inter / (area_true + area_detection.t() - area_inter);
}

vector<int> non_max_suppression(const Mat& predictions, float iou_threshold) {
    assert(0 <= iou_threshold && iou_threshold <= 1 && "iou_threshold must be between 0 and 1");

    Mat predictions_copy = predictions.clone();
    int rows = predictions_copy.rows;
    int columns = predictions_copy.cols;

    if (columns == 5) {
        Mat zeros = Mat::zeros(rows, 1, CV_32F);
        hconcat(predictions_copy, zeros, predictions_copy);
    }

    Mat scores;
    predictions_copy.col(4).copyTo(scores);
    Mat sorted_idx;
    sortIdx(scores, sorted_idx, SORT_EVERY_COLUMN + SORT_DESCENDING);

    Mat sorted_predictions(rows, columns, predictions_copy.type());
    for (int i = 0; i < rows; ++i) {
        sorted_predictions.row(i) = predictions_copy.row(sorted_idx.at<int>(i));
    }

    Mat boxes = sorted_predictions.colRange(0, 4);
    Mat categories = sorted_predictions.col(5);
    Mat ious = box_iou_batch(boxes, boxes) - Mat::eye(rows, rows, CV_32F);

    vector<int> keep(rows, 1);

    for (int i = 0; i < rows; ++i) {
        if (!keep[i]) continue;

        for (int j = i + 1; j < rows; ++j) {
            if (ious.at<float>(i, j) > iou_threshold && categories.at<float>(i) == categories.at<float>(j)) {
                keep[j] = 0;
            }
        }
    }

    vector<int> result;
    for (int i = 0; i < rows; ++i) {
        if (keep[i]) {
            result.push_back(sorted_idx.at<int>(i));
        }
    }

    return result;
}


std::tuple<cv::Mat, cv::Mat> get_simcc_maximum(
    const cv::Mat& simcc_x, 
    const cv::Mat& simcc_y
) {
    // Check if input matrices have the same size
    CV_Assert(simcc_x.size == simcc_y.size);

    int N = simcc_x.size[0];
    int K = simcc_x.size[1];
    int Wx = simcc_x.size[2];
    int Wy = simcc_y.size[2];

    // Reshape matrices to 2D (N*K, Wx) and (N*K, Wy)
    cv::Mat simcc_x_reshaped = simcc_x.reshape(1, N * K);
    cv::Mat simcc_y_reshaped = simcc_y.reshape(1, N * K);

    // Initialize matrices for locations and values
    cv::Mat locs = cv::Mat::zeros(N * K, 2, CV_32F);
    cv::Mat vals = cv::Mat::zeros(N * K, 1, CV_32F);

    // Iterate over each row to find the max location and value
    for (int i = 0; i < N * K; ++i) {
        double minVal, maxVal;
        cv::Point minLoc, maxLoc;

        // Find max location and value for x-axis
        cv::minMaxLoc(simcc_x_reshaped.row(i), &minVal, &maxVal, &minLoc, &maxLoc);
        int x_locs = maxLoc.x;
        float max_val_x = static_cast<float>(maxVal);

        // Find max location and value for y-axis
        cv::minMaxLoc(simcc_y_reshaped.row(i), &minVal, &maxVal, &minLoc, &maxLoc);
        int y_locs = maxLoc.x;
        float max_val_y = static_cast<float>(maxVal);

        // Set locations and values
        locs.at<float>(i, 0) = static_cast<float>(x_locs);
        locs.at<float>(i, 1) = static_cast<float>(y_locs);
        vals.at<float>(i, 0) = 0.5f * (max_val_x + max_val_y);
    }

    // Reshape locs and vals to (N, K, 2) and (N, K)
    locs = locs.reshape(2, N * K);
    vals = vals.reshape(1, N * K);

    // Set invalid values to -1
    for (int i = 0; i < vals.rows; ++i) {
        if (vals.at<float>(i, 0) <= 0.0f) {
            locs.at<float>(i, 0) = -1.0f;
            locs.at<float>(i, 1) = -1.0f;
        }
    }

    // Reshape locs and vals to (N, K, 2) and (N, K)
    locs = locs.reshape(2, N);
    vals = vals.reshape(1, N);

    return std::make_tuple(locs, vals);
}


tuple<Point2f, Point2f> bbox_xyxy2cs(const Rect2f& bbox, float padding) {
    float x1 = bbox.x;
    float y1 = bbox.y;
    float x2 = bbox.width;
    float y2 = bbox.height;

    Point2f center((x1 + x2) * 0.5f, (y1 + y2) * 0.5f);
    Point2f scale((x2 - x1) * padding, (y2 - y1) * padding);
    return make_tuple(center, scale);
}

tuple<vector<Point2f>, vector<Point2f>> bbox_xyxy2cs_multi(
    const vector<Rect2f>& bbox,
    float padding
) {
    size_t dim = bbox.size();
    vector<Point2f> center(dim);
    vector<Point2f> scale(dim);

    for (size_t i = 0; i < dim; ++i) {
        float x1 = bbox[i].x;
        float y1 = bbox[i].y;
        float x2 = bbox[i].x + bbox[i].width;
        float y2 = bbox[i].y + bbox[i].height;

        center[i] = Point2f((x1 + x2) * 0.5f, (y1 + y2) * 0.5f);
        scale[i] = Point2f((x2 - x1) * padding, (y2 - y1) * padding);
    }

    return make_tuple(center, scale);
}

//top_down_affine

Point2f rotate_point(const Point2f& pt, float angle_rad) {
    float sn = sin(angle_rad);
    float cs = cos(angle_rad);
    Matx22f rot_mat(cs, -sn, sn, cs);
    return rot_mat * pt;
}

Point2f get_3rd_point(const Point2f& a, const Point2f& b) {
    Point2f direction = a - b;
    return b + Point2f(-direction.y, direction.x);
}

cv::Mat get_warp_matrix(
    const cv::Point2f& center,
    const cv::Point2f& scale,
    float rot,
    const cv::Size& output_size,
    const cv::Point2f& shift,
    bool inv
) {
    float src_w = scale.x;
    float dst_w = static_cast<float>(output_size.width);
    float dst_h = static_cast<float>(output_size.height);

    // Convert degrees to radians
    float rot_rad = rot * CV_PI / 180.0f;
    cv::Point2f src_dir = rotate_point(cv::Point2f(0.0f, src_w * -0.5f), rot_rad);
    cv::Point2f dst_dir(0.0f, dst_w * -0.5f);

    // Get corners of the source rectangle
    cv::Point2f src[3];
    src[0] = center + cv::Point2f(scale.x * shift.x, scale.y * shift.y);
    src[1] = center + src_dir + cv::Point2f(scale.x * shift.x, scale.y * shift.y);
    src[2] = get_3rd_point(src[0], src[1]);

    // Get corners of the destination rectangle
    cv::Point2f dst[3];
    dst[0] = cv::Point2f(dst_w * 0.5f, dst_h * 0.5f);
    dst[1] = dst[0] + dst_dir;
    dst[2] = get_3rd_point(dst[0], dst[1]);

    cv::Mat warp_mat;
    if (inv) {
        warp_mat = cv::getAffineTransform(dst, src);
    } else {
        warp_mat = cv::getAffineTransform(src, dst);
    }

    return warp_mat;
}


// tuple<Mat, Point2f> top_down_affine(
//     const Size& input_size,
//     const Point2f& bbox_scale,
//     const Point2f& bbox_center,
//     const Mat& img
// ) {
//     int w = input_size.width;
//     int h = input_size.height;
//     Size warp_size(w, h);

//     float aspect_ratio = static_cast<float>(w) / h;
//     Point2f b_scale = bbox_scale;

//     if (b_scale.x > b_scale.y * aspect_ratio) {
//         b_scale.y = b_scale.x / aspect_ratio;
//     } else {
//         b_scale.x = b_scale.y * aspect_ratio;
//     }

//     Mat warp_mat = get_warp_matrix(bbox_center, b_scale, 0, Size(w, h));

//     Mat warped_img;
//     warpAffine(img, warped_img, warp_mat, warp_size, INTER_LINEAR);

//     return make_tuple(warped_img, b_scale);
// }



bool BoxCompare(
	const DetectBox& a,
	const DetectBox& b) {
	return a.score > b.score;
}




cv::Mat GetAffineTransform(float center_x, float center_y, float scale_width, float scale_height, int output_image_width, int output_image_height, bool inverse = false)
{
	// solve the affine transformation matrix
	/* 求解仿射变换矩阵 */

	// get the three points corresponding to the source picture and the target picture
	// 获取源图片与目标图片的对应的三个点
	cv::Point2f src_point_1;
	src_point_1.x = center_x;
	src_point_1.y = center_y;

	cv::Point2f src_point_2;
	src_point_2.x = center_x;
	src_point_2.y = center_y - scale_width * 0.5;

	cv::Point2f src_point_3;
	src_point_3.x = src_point_2.x - (src_point_1.y - src_point_2.y);
	src_point_3.y = src_point_2.y + (src_point_1.x - src_point_2.x);


	float alphapose_image_center_x = output_image_width / 2;
	float alphapose_image_center_y = output_image_height / 2;

	cv::Point2f dst_point_1;
	dst_point_1.x = alphapose_image_center_x;
	dst_point_1.y = alphapose_image_center_y;

	cv::Point2f dst_point_2;
	dst_point_2.x = alphapose_image_center_x;
	dst_point_2.y = alphapose_image_center_y - output_image_width * 0.5;

	cv::Point2f dst_point_3;
	dst_point_3.x = dst_point_2.x - (dst_point_1.y - dst_point_2.y);
	dst_point_3.y = dst_point_2.y + (dst_point_1.x - dst_point_2.x);


	cv::Point2f srcPoints[3];
	srcPoints[0] = src_point_1;
	srcPoints[1] = src_point_2;
	srcPoints[2] = src_point_3;

	cv::Point2f dstPoints[3];
	dstPoints[0] = dst_point_1;
	dstPoints[1] = dst_point_2;
	dstPoints[2] = dst_point_3;

	// get affine matrix
	// 获取仿射矩阵
	cv::Mat affineTransform;
	if (inverse)
	{
		affineTransform = cv::getAffineTransform(dstPoints, srcPoints);
	}
	else
	{
		affineTransform = cv::getAffineTransform(srcPoints, dstPoints);
	}

	return affineTransform;
}

static float LetterBoxImage(
	const cv::Mat& image,
	cv::Mat& out_image,
	const cv::Size& new_shape = cv::Size(640, 640),
	int stride = 32,
	const cv::Scalar& color = cv::Scalar(114, 114, 114),
	bool fixed_shape = false,
	bool scale_up = true) 
{
	cv::Size shape = image.size();
	float r = std::min((float)new_shape.height / (float)shape.height, (float)new_shape.width / (float)shape.width);

	if (!scale_up) {
		r = std::min(r, 1.0f);
	}

	int newUnpad[2]{
		(int)std::round((float)shape.width * r), (int)std::round((float)shape.height * r) };

	cv::Mat tmp;
	if (shape.width != newUnpad[0] || shape.height != newUnpad[1]) {
		cv::resize(image, tmp, cv::Size(newUnpad[0], newUnpad[1]));
	}
	else {
		tmp = image.clone();
	}

	float dw = new_shape.width - newUnpad[0];
	float dh = new_shape.height - newUnpad[1];

	if (!fixed_shape) {
		dw = (float)((int)dw % stride);
		dh = (float)((int)dh % stride);
	}

	int top = int(0);
	int bottom = int(std::round(dh + 0.1f));
	int left = int(std::round(0));
	int right = int(std::round(dw + 0.1f));

	cv::copyMakeBorder(tmp, out_image, top, bottom, left, right, cv::BORDER_CONSTANT, color);

	return 1.0f / r;
}
