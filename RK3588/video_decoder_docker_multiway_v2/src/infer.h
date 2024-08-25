#ifndef VIDEO_DECODER_INFER_H
#define VIDEO_DECODER_INFER_H

#include <vector>
#include <string>
#include <queue>
#include <future>
#include <condition_variable>
#include "BS_thread_pool.hpp"
#include "video_decoder.h"

struct InferBox {
    float x0;
    float y0;
    float x1;
    float y1;
    float conf;
    int cls;
};

struct InferResult {
    bool success;
    std::vector<InferBox> boxes;
    std::string image;
    double decode_time;
    double infer_time;

    friend std::ostream &operator<<(std::ostream &os, const InferResult &result);
};

template<typename T>
class FutureQueue {
public:
    explicit FutureQueue(size_t max_size) : max_size(max_size) {}

    void push(std::future<T> &&future) {
        std::lock_guard lock(mutex);
        if (futures.size() >= max_size) {
            futures.pop();
        }
        futures.push(std::move(future));
        cond.notify_one();
    }

    std::future<T> pop() {
        std::unique_lock lock(mutex);
        if (!cond.wait_for(lock,
                           std::chrono::milliseconds(500),
                           [this] { return !futures.empty(); })) {
            return {};
        }
        std::future<T> future = std::move(futures.front());
        futures.pop();
        return future;
    }

private:
    size_t max_size;
    std::queue<std::future<T>> futures;
    std::mutex mutex;
    std::condition_variable cond;
};

class InferFrameHandler : public FrameHandler {
public:
    InferFrameHandler(std::string infer_url, FutureQueue<InferResult> &queue);

    void handle_frame(uint8_t *data, int width, int height, double decode_time_ms) override;

private:
    std::string infer_url;
    FutureQueue<InferResult> &queue;
    BS::thread_pool thread_pool{16};

    InferResult infer_impl(const std::string &frame_content, int width, int height, double decode_time_ms);
};

class InferItem {
public:
    InferItem(const std::string &video_url, const std::string &infer_url, int queue_size, int width, int height);

    bool start();

    bool stop();

    std::future<InferResult> get_result();

private:
    FutureQueue<InferResult> queue;
    InferFrameHandler handler;
    VideoDecoder decoder;
};

class InferManager {
public:
    InferManager(const std::string &infer_url, int max_video_size, int queue_size, int width, int height);

    bool add_url(const std::string &url);

    bool remove_url(const std::string &url);

    std::future<InferResult> get_result(const std::string &url);

private:
    std::string infer_url;
    int max_video_size;
    int queue_size;
    int width;
    int height;
    std::map<std::string, std::unique_ptr<InferItem>> items;
    std::mutex mutex;
};

#endif //VIDEO_DECODER_INFER_H
