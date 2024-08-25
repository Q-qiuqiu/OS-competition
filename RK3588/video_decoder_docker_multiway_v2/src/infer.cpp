#include <iostream>
#include <utility>
#include "httplib.h"
#include "json.hpp"
#include "infer.h"

using namespace httplib;
using json = nlohmann::json;
//重载输出
std::ostream &operator<<(std::ostream &os, const InferResult &result) {
    if (!result.success) {
        os << "infer failed" << std::endl;
        return os;
    }

    os << "infer success" << std::endl;
    for (const auto &box: result.boxes) {
        os << "    [" << box.x0 << ", " << box.y0 << ", " << box.x1 << ", " << box.y1 << ", " << box.conf << ", "
           << box.cls << "]" << std::endl;
    }
    return os;
}

InferFrameHandler::InferFrameHandler(std::string infer_url, FutureQueue<InferResult> &queue)
        : infer_url(std::move(infer_url)), queue(queue) {}//queue是一个队列，用于存储推理结果
//处理输入的图像帧数据，将其转换为字符串并启动异步推理任务
void InferFrameHandler::handle_frame(uint8_t *data, int width, int height, double decode_time_ms) {
    std::string frame_content(reinterpret_cast<const char *>(data), width * height * 3);

    //启动异步任务，并将未来结果推入队列中
    std::future<InferResult> future_result = thread_pool.submit_task(
            [this, frame_content = std::move(frame_content), width, height, decode_time_ms] {
                return infer_impl(frame_content, width, height, decode_time_ms);
            });
    queue.push(std::move(future_result));
}
//将图像数据发送到推理服务器并解析返回结果
InferResult
InferFrameHandler::infer_impl(const std::string &frame_content, int width, int height, double decode_time_ms) {
    Client client(infer_url);//推理开发板的url
    client.set_compress(true);

    MultipartFormDataItems args = {
            {"width",  std::to_string(width),  "",          ""},
            {"height", std::to_string(height), "",          ""},
            {"image",  frame_content,          "image.bin", "application/octet-stream"},
    };

    auto begin = std::chrono::high_resolution_clock::now();
    Result res = client.Post("/predict_raw", args);
    auto end = std::chrono::high_resolution_clock::now();
    double infer_time_ms = (double) std::chrono::duration_cast<std::chrono::microseconds>(end - begin).count() / 1000.0;

    if (!res || res->status != 200) {
        return {false, {}};
    }

    json j = json::parse(res->body);
    if (j["status"] != "success") {
        return {false, {}};
    }

    InferResult result;
    result.success = true;
    for (const auto &box: j["result"]) {
        result.boxes.push_back({box[0].get<float>(),
                                box[1].get<float>(),
                                box[2].get<float>(),
                                box[3].get<float>(),
                                box[4].get<float>(),
                                std::lround(box[5].get<float>()),});
    }
    result.image = frame_content;
    result.decode_time = decode_time_ms;
    result.infer_time = infer_time_ms;
    return result;
}

InferItem::InferItem(const std::string &video_url,
                     const std::string &infer_url,
                     int queue_size,
                     int width,
                     int height)
        : queue(queue_size),
          handler(infer_url, queue),
          decoder(video_url, width, height, handler) {}
//解码开始
bool InferItem::start() {
    if (!decoder.init()) {
        return false;
    }
    return decoder.run();
}
//解码停止
bool InferItem::stop() {
    return decoder.stop();
}

std::future<InferResult> InferItem::get_result() {
    return queue.pop();
}

InferManager::InferManager(const std::string &infer_url, int max_video_size, int queue_size, int width, int height)
        : infer_url(infer_url),
          max_video_size(max_video_size),
          queue_size(queue_size),
          width(width),
          height(height) {}
//HTTP START
bool InferManager::add_url(const std::string &url) {
    std::lock_guard lock(mutex);

    if (items.size() >= max_video_size) {
        return false;
    }

    if (items.find(url) != items.end()) {
        return false;
    }

    auto item = std::make_unique<InferItem>(url, infer_url, queue_size, width, height);
    if (!item->start()) {
        return false;
    }
    items[url] = std::move(item);
    return true;
}
//HTTP STOP
bool InferManager::remove_url(const std::string &url) {
    std::lock_guard lock(mutex);

    auto it = items.find(url);
    if (it == items.end()) {
        return false;
    }

    bool result = it->second->stop();
    items.erase(it);
    return result;
}
//HTTP GET
std::future<InferResult> InferManager::get_result(const std::string &url) {
    std::lock_guard lock(mutex);

    auto it = items.find(url);
    if (it == items.end()) {
        return {};
    }

    return it->second->get_result();
}
