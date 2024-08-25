#ifndef YOLOV5_INFER_SRC_YOLOV5_H
#define YOLOV5_INFER_SRC_YOLOV5_H

#include <memory>
#include <vector>
#include <cstdint>
#include <opencv2/opencv.hpp>
#include "model.h"

struct BBox {
    float x0;
    float y0;
    float x1;
    float y1;
    float score;
    size_t class_id;
};

struct ScaleInfo {
    double scale;
    size_t new_width;
    size_t new_height;
    size_t offset_x;
    size_t offset_y;
};

class Yolov5 : public Model {
public:
    using Model::Model;

    std::vector<BBox> infer(const cv::Mat &image);

private:
    size_t wanted_width = 640;
    size_t wanted_height = 640;
    float conf_threshold = 0.25f;
    float iou_threshold = 0.45f;

    ScaleInfo calc_scale_info(const cv::Mat &image);

    cv::Mat letterbox(const cv::Mat &image, const ScaleInfo &scale_info, const cv::Scalar &pad_color);

    std::vector<BBox> parse_output();

    std::vector<BBox> filter_output(float *data, size_t rows, size_t cols);

    void resize_output(std::vector<BBox> &boxes, const ScaleInfo &scale_info);

    float iou(const BBox &a, const BBox &b);
};

#endif //YOLOV5_INFER_SRC_YOLOV5_H
