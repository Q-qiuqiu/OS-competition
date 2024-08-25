#include <list>
#include <algorithm>
#include "yolov5.h"
//计算图像缩放和填充的信息
ScaleInfo Yolov5::calc_scale_info(const cv::Mat &image) {
    size_t width = image.cols;
    size_t height = image.rows;

    double scale_w = (double) wanted_width / width;
    double scale_h = (double) wanted_height / height;
    double scale = std::min(scale_w, scale_h);

    size_t new_width = std::min(wanted_width, static_cast<size_t>(width * scale));
    size_t new_height = std::min(wanted_height, static_cast<size_t>(height * scale));

    size_t offset_x = (wanted_width - new_width) / 2;
    size_t offset_y = (wanted_height - new_height) / 2;

    return {scale, new_width, new_height, offset_x, offset_y};
}
//创建并拷贝新图像
cv::Mat Yolov5::letterbox(const cv::Mat &image, const ScaleInfo &scale_info, const cv::Scalar &pad_color) {
    size_t width = image.cols;
    size_t height = image.rows;
    if (width == wanted_width && height == wanted_height) {
        return image;
    }

    cv::Mat resize_image;
    cv::resize(image, resize_image, cv::Size(scale_info.new_width, scale_info.new_height));

    cv::Mat new_image(wanted_height, wanted_width, CV_8UC3, pad_color);
    resize_image.copyTo(
            new_image(cv::Rect(scale_info.offset_x, scale_info.offset_y, scale_info.new_width, scale_info.new_height)));

    return new_image;
}
//BGR 转 RGB
//将输入图像转换格式
static std::unique_ptr<float[]> get_infer_input(const cv::Mat &image) {
    size_t width = image.cols;
    size_t height = image.rows;
    uint8_t *frame = image.data;

    std::unique_ptr<float[]> new_frame(new float[width * height * 3]);
    size_t channels = 3; // Assuming 3 channels (BGR)

    for (size_t c = 0; c < channels; ++c) {
        for (size_t h = 0; h < height; ++h) {
            for (size_t w = 0; w < width; ++w) {
                size_t src_idx = (h * width + w) * channels + (2 - c); // Reverse the channels: BGR to RGB
                size_t dst_idx = (c * height + h) * width + w;         // Transpose HWC to CHW

                new_frame[dst_idx] = static_cast<float>(frame[src_idx]) / 255.0f;
            }
        }
    }

    return new_frame;
}
//运行推理
std::vector<BBox> Yolov5::infer(const cv::Mat &image) {
    ScaleInfo scale_info = calc_scale_info(image);
    cv::Mat letterbox_image = letterbox(image, scale_info, cv::Scalar(114, 114, 114));
    std::unique_ptr<float[]> infer_frame = get_infer_input(letterbox_image);

    MemRange input_range{infer_frame.get(), wanted_width * wanted_height * 3 * sizeof(float)};
    std::vector<MemRange> input_ranges = {input_range};

    if (!execute(input_ranges)) {
        return {};
    }

    std::vector<BBox> result = parse_output();
    resize_output(result, scale_info);
    return result;
}
//解析模型输出
std::vector<BBox> Yolov5::parse_output() {
    aclError ret;

    aclmdlDesc *model_desc = model_loader.get_model_desc();//获取模型描述符:
    size_t num_outputs = aclmdlGetNumOutputs(model_desc);
    if (num_outputs != 1) {
        ACL_APP_LOG(ACL_ERROR, "output num mismatch");//获取输出数量并检查:
        return {};
    }

    size_t output_index = 0;

    aclDataBuffer *output_buffer = aclmdlGetDatasetBuffer(output_dataset.get(), output_index);//获取输出数据缓冲区:
    if (output_buffer == nullptr) {
        ACL_APP_LOG(ACL_ERROR, "get output buffer failed");
        return {};
    }

    void *data = aclGetDataBufferAddr(output_buffer);//获取数据缓冲区地址和大小:
    if (data == nullptr) {
        ACL_APP_LOG(ACL_ERROR, "get output data failed");
        return {};
    }
    size_t data_size = aclGetDataBufferSizeV2(output_buffer);

    aclmdlIODims output_dims;
    ret = aclmdlGetOutputDims(model_desc, output_index, &output_dims);//获取输出维度信息:
    if (ret != ACL_SUCCESS) {
        ACL_APP_LOG(ACL_ERROR, "get output dims failed");
        return {};
    }

    aclDataType data_type = aclmdlGetOutputDataType(model_desc, output_index);//检查数据类型:
    if (data_type != ACL_FLOAT) {
        ACL_APP_LOG(ACL_ERROR, "unsupported data type");
        return {};
    }

    if (output_dims.dims[0] != 1) {//检查批处理大小:
        ACL_APP_LOG(ACL_ERROR, "batch size mismatch");
        return {};
    }
//获取输出的行数和列数:
    size_t rows = output_dims.dims[1];
    size_t cols = output_dims.dims[2];
//调用 filter_output 函数过滤输出
    return filter_output(static_cast<float *>(data), rows, cols);
}
//过滤输出
std::vector<BBox>
Yolov5::filter_output(float *data, size_t rows, size_t cols) {

    std::vector<BBox> temp_vec_result;

    // 过滤置信框
    for (size_t i = 0; i < rows; i++) {
        float class_conf = data[i * cols + 4];

        if (class_conf >= conf_threshold) {
            float x = data[i * cols + 0];
            float y = data[i * cols + 1];
            float w = data[i * cols + 2];
            float h = data[i * cols + 3];

            float *conf_start = data + i * cols + 5;
            float *conf_end = data + i * cols + cols;
            size_t max_index = std::distance(conf_start, std::max_element(conf_start, conf_end));
            float max_score = conf_start[max_index];

            temp_vec_result.push_back({x - w / 2, y - h / 2, x + w / 2, y + h / 2, class_conf * max_score, max_index});
        }
    }

    // 过滤 iou
    std::sort(temp_vec_result.begin(), temp_vec_result.end(), [](const BBox &a, const BBox &b) {
        return a.score > b.score;
    });

    std::list<BBox> temp_list_result(temp_vec_result.begin(), temp_vec_result.end());
    for (auto it = temp_list_result.begin(); it != temp_list_result.end(); ++it) {
        for (auto it_cmp = std::next(it); it_cmp != temp_list_result.end();) {
            if (iou(*it, *it_cmp) > iou_threshold) {
                it_cmp = temp_list_result.erase(it_cmp);
            } else {
                ++it_cmp;
            }
        }
    }

    return {temp_list_result.begin(), temp_list_result.end()};
}
//计算交并比
float Yolov5::iou(const BBox &a, const BBox &b) {
    if (a.class_id != b.class_id) {
        return 0.0f;
    }

    float x0 = std::max(a.x0, b.x0);
    float y0 = std::max(a.y0, b.y0);
    float x1 = std::min(a.x1, b.x1);
    float y1 = std::min(a.y1, b.y1);

    float intersection_area = std::max(0.0f, x1 - x0) * std::max(0.0f, y1 - y0);
    float area_a = (a.x1 - a.x0) * (a.y1 - a.y0);
    float area_b = (b.x1 - b.x0) * (b.y1 - b.y0);
    float union_area = area_a + area_b - intersection_area;

    return intersection_area / union_area;
}
//格式化输出
void Yolov5::resize_output(std::vector<BBox> &boxes, const ScaleInfo &scale_info) {
    for (BBox &box: boxes) {
        box.x0 = (box.x0 - scale_info.offset_x) / scale_info.scale;
        box.y0 = (box.y0 - scale_info.offset_y) / scale_info.scale;
        box.x1 = (box.x1 - scale_info.offset_x) / scale_info.scale;
        box.y1 = (box.y1 - scale_info.offset_y) / scale_info.scale;
    }
}
