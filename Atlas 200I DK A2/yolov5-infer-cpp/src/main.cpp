#include <iostream>
#include <fstream>
#include <memory>
#include <opencv2/opencv.hpp>
#include <sched.h>
#include "model.h"
#include "yolov5.h"
#include "httplib.h"
#include "json.hpp"

using namespace nlohmann;
//测试
static void test_infer(Yolov5 &model) {
    std::ifstream f("test/frame.bin", std::ios::binary);
    size_t width = 640;
    size_t height = 640;
    std::unique_ptr<uint8_t[]> frame(new uint8_t[width * height * 3]);
    f.read(reinterpret_cast<char *>(frame.get()), width * height * 3);

    cv::Mat img(height, width, CV_8UC3, frame.get());

    auto boxes = model.infer(img);

    std::cout << "boxes: " << boxes.size() << std::endl;
}
//填充http结果
static std::string make_result(const std::string &status, const json &result) {
    json j;
    j["status"] = status;
    j["result"] = result;
    return j.dump();
}

//填充成功的信息
static std::string make_success(const json &result) {
    return make_result("success", result);
}
//填充失败的信息
static std::string make_failed(const json &result) {
    return make_result("failed", result);
}
//检测框转为json对象
static void fill_response(httplib::Response &res, const std::string &content) {
    res.set_content(content, "application/json");
}

static json boxes_to_json(const std::vector<BBox> &boxes) {
    json j = json::array();
    for (const auto &box: boxes) {
        j.push_back({box.x0, box.y0, box.x1, box.y1, box.score, box.class_id});
    }
    return j;
}

static std::string read_file(const std::string &filename) {
    std::ifstream f(filename);
    return {(std::istreambuf_iterator<char>(f)), std::istreambuf_iterator<char>()};
}

class ThreadContext {
public:
    ThreadContext(void *model_data, size_t model_size) :
            acl_context(),
            model_loader(model_data, model_size),
            model(acl_context, model_loader) {
        if (!acl_context.init()) {
            std::cout << "acl context init failed" << std::endl;
            return;
        }
        if (!model_loader.init()) {
            std::cout << "model loader init failed" << std::endl;
            return;
        }
        if (!model.init()) {
            std::cout << "model init failed" << std::endl;
            return;
        }
        valid = true;
    }

    bool is_valid() {
        return valid;
    }

    Yolov5 &get_model() {
        return model;
    }

private:
    bool valid{};
    AclContext acl_context;
    ModelLoader model_loader;
    Yolov5 model;
};

int main() {
    AclInit acl_init;//初始化
    if (!acl_init.init()) {
        std::cout << "acl init failed" << std::endl;
        return 1;
    }
    //读取模型文件
    std::string model_file = read_file("model/yolov5s_bs1.om");
    //初始化模型加载模型等
    auto get_thread_context = [&model_file]() -> ThreadContext & {
        thread_local ThreadContext context(model_file.data(), model_file.size());
        return context;
    };

    // 启动 HTTP服务
    httplib::Server server;

    static std::atomic<uint64_t> request_count = 0;

    server.Post("/predict", [&](const httplib::Request &req, httplib::Response &res) {
        ThreadContext &context = get_thread_context();
        if (!context.is_valid()) {
            fill_response(res, make_failed("init model failed"));
            return;
        }

        httplib::MultipartFormData http_image = req.get_file_value("image");
        // 解码图像
        std::vector<uint8_t> image_data(http_image.content.begin(), http_image.content.end());
        cv::Mat image = cv::imdecode(image_data, cv::IMREAD_COLOR);
        if (image.empty()) {
            fill_response(res, make_failed("decode image failed"));
            return;
        }
        request_count++;
        std::vector<BBox> boxes = context.get_model().infer(image);
        fill_response(res, make_success(boxes_to_json(boxes)));
    });

    server.Post("/predict_raw", [&](const httplib::Request &req, httplib::Response &res) {
        ThreadContext &context = get_thread_context();
        if (!context.is_valid()) {
            fill_response(res, make_failed("init model failed"));
            return;
        }

        httplib::MultipartFormData http_width = req.get_file_value("width");
        httplib::MultipartFormData http_height = req.get_file_value("height");
        httplib::MultipartFormData http_image = req.get_file_value("image");
        if (http_width.content.empty() || http_height.content.empty() || http_image.content.empty()) {
            fill_response(res, make_failed("invalid params"));
            return;
        }

        size_t width = std::stoul(http_width.content);
        size_t height = std::stoul(http_height.content);

        cv::Mat image(height, width, CV_8UC3, http_image.content.data());

        request_count++;
        std::vector<BBox> boxes = context.get_model().infer(image);
        fill_response(res, make_success(boxes_to_json(boxes)));
    });
    //输出日志
    std::thread([] {
        while (true) {
            std::this_thread::sleep_for(std::chrono::seconds(1));
            std::cout << "request count: " << request_count << std::endl;
        }
    }).detach();

    std::cout << "start http server" << std::endl;
    if (!server.listen("0.0.0.0", 8080)) {
        std::cout << "http server start failed" << std::endl;
        return 1;
    }

    return 0;
}
