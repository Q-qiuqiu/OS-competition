#include <iostream>
#include <sstream>
#include <mutex>
#include <future>
#include <queue>
#include <opencv2/opencv.hpp>
#include "httplib.h"
#include "json.hpp"
#include "infer.h"

using namespace httplib;
using json = nlohmann::json;
//填充结果
static std::string make_result(const std::string &status, const json &result) {
    json j;
    j["status"] = status;
    if (!result.empty()) {
        j["reason"] = result;
    }
    return j.dump();
}
//填充status
static std::string make_success(const json &result) {
    return make_result("success", result);
}

static std::string make_failed(const json &result) {
    return make_result("failed", result);
}
//填充HTTP响应  
static void fill_response(httplib::Response &res, const std::string &content) {
    res.set_content(content, "application/json");
}
//将输出转为json对象
static json boxes_to_json(const std::vector<InferBox> &boxes) {
    json j = json::array();
    for (const auto &box: boxes) {
        j.push_back({box.x0, box.y0, box.x1, box.y1, box.conf, box.cls});
    }
    return j;
}

int main() {
    VideoDecoderGlobalInit global_init;
    if (!global_init.init()) {
        std::cerr << "global init failed" << std::endl;
        return 1;
    }

    std::string infer_url = "http://192.168.137.3:8080";
    int max_video_size = 8;
    int queue_size = 3;
    int width = 640;
    int height = 360;

    InferManager infer_manager(infer_url, max_video_size, queue_size, width, height);

    Server server;

    server.Post("/start", [&](const Request &req, Response &res) {
        std::string url = req.get_param_value("url");
        if (url.empty()) {
            fill_response(res, make_failed("need url"));
            return;
        }
        if (!infer_manager.add_url(url)) {
            fill_response(res, make_failed("add url to infer manager failed"));
            return;
        }
        std::cout << "start decode " << url << std::endl;
        fill_response(res, make_success({}));
    });

    server.Post("/stop", [&](const Request &req, Response &res) {
        std::string url = req.get_param_value("url");
        if (url.empty()) {
            fill_response(res, make_failed("need url"));
            return;
        }
        if (!infer_manager.remove_url(url)) {
            fill_response(res, make_failed("remove url from infer manager failed"));
            return;
        }
        std::cout << "stop decode " << url << std::endl;
        fill_response(res, make_success({}));
    });

    server.Get("/video", [&](const Request &req, Response &res) {
        std::string url = req.get_param_value("url");
        if (url.empty()) {
            fill_response(res, make_failed("need url"));
            return;
        }

        std::future<InferResult> future_result = infer_manager.get_result(url);

        if (!future_result.valid()) {
            fill_response(res, make_failed("get result failed"));
            return;
        }
        InferResult result = future_result.get();

        if (!result.success) {
            fill_response(res, make_failed("infer failed"));
            return;
        }

        json j;
        j["status"] = "success";
        j["width"] = width;
        j["height"] = height;
        j["result"] = boxes_to_json(result.boxes);
        j["decode_time"] = result.decode_time;
        j["infer_time"] = result.infer_time;

        std::stringstream ss;
        ss << j << "!" << result.image;

        res.set_content(ss.str(), "application/octet-stream");
    });

    std::cout << "start http server" << std::endl;
    if (!server.listen("0.0.0.0", 8080)) {
        std::cerr << "http server start failed" << std::endl;
        return 1;
    }
    return 0;
}
