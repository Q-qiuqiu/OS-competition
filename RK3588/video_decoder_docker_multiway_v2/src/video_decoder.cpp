#include <iostream>
#include <chrono>
#include "video_decoder.h"


bool VideoDecoderGlobalInit::init() {
    // 初始化网络
    if (avformat_network_init() < 0) {
        return false;
    }
    return true;
}

VideoDecoderGlobalInit::~VideoDecoderGlobalInit() {
    avformat_network_deinit();
}

VideoDecoder::VideoDecoder(const std::string &url, int width, int height, FrameHandler &handler)
        : url(url), width(width), height(height), handler(handler) {}
//初始化视频解码器，打开输入视频文件并准备解码
bool VideoDecoder::init() {
    // 分配上下文
    av_format_ctx = avformat_alloc_context();
    if (!av_format_ctx) {
        return false;
    }

    // 打开url
    if (avformat_open_input(&av_format_ctx, url.c_str(), nullptr, nullptr) < 0) {
        return false;
    }

    // 获取流信息
    if (avformat_find_stream_info(av_format_ctx, nullptr) < 0) {
        return false;
    }

    // 提取视频信息
    if (!extract_video_info()) {
        return false;
    }

    // 打开解码器
    if (!open_decoder()) {
        return false;
    }

       // 创建 sws
    if (!create_sws()) {
        return false;
    }

    // 分配数据包和帧
    av_packet = av_packet_alloc();
    if (!av_packet) {
        return false;
    }
    av_frame = av_frame_alloc();
    if (!av_frame) {
        return false;
    }

    init_success = true;
    return true;
}
 //提取视频信息
bool VideoDecoder::extract_video_info() {
    for (int i = 0; i < av_format_ctx->nb_streams; i++) {//遍历所有流，找到视频流
        if (av_format_ctx->streams[i]->codecpar->codec_type == AVMEDIA_TYPE_VIDEO) {
            video_index = i;
            av_codec_params = av_format_ctx->streams[i]->codecpar;
            return true;
        }
    }
    return false;
}
//打开视频解码器，并初始化相关的解码器上下文
bool VideoDecoder::open_decoder() {
    av_codec = const_cast<AVCodec *>(avcodec_find_decoder(av_codec_params->codec_id));
    if (!av_codec) {
        return false;
    }
    // 尝试查找 rkmpp 解码器
    std::string new_name = std::string(av_codec->name) + "_rkmpp";
    const AVCodec *rkmpp_decoder = avcodec_find_decoder_by_name(new_name.c_str());
    if (rkmpp_decoder) {
        av_codec = const_cast<AVCodec *>(rkmpp_decoder);
    }

    //  打印解码器名称
    std::cout << "decoder name: " << av_codec->name << std::endl;

    //  设置选项
    av_dict_set(&av_opts, "rtsp_flags", "+prefer_tcp", 0);
    av_dict_set(&av_opts, "allowed_media_types", "video", 0);

    // 分配上下文
    av_codec_ctx = avcodec_alloc_context3(av_codec);
    if (!av_codec_ctx) {
        return false;
    }

    //复制编解码参数
    if (avcodec_parameters_to_context(av_codec_ctx, av_codec_params) < 0) {
        return false;
    }

    // 打开编解码器
    if (avcodec_open2(av_codec_ctx, av_codec, &av_opts) < 0) {
        return false;
    }

    return true;
}
//创建 sws 上下文用于图像格式转换
bool VideoDecoder::create_sws() {
    sws_ctx = sws_getContext(
            av_codec_ctx->width,
            av_codec_ctx->height,
            av_codec_ctx->pix_fmt,
            width,
            height,
            AV_PIX_FMT_BGR24,
            SWS_BILINEAR,
            nullptr,
            nullptr,
            nullptr);

    if (!sws_ctx) {
        return false;
    }

    // 分配帧
    av_frame_sws = av_frame_alloc();
    if (!av_frame_sws) {
        return false;
    }
    // 分配数据
    av_image_alloc(av_frame_sws->data,
                   av_frame_sws->linesize,
                   width,
                   height,
                   AV_PIX_FMT_BGR24,
                   1);

    return true;
}
//将错误码转换为字符串
static std::string av_err2string(int errnum) {
    char errbuf[AV_ERROR_MAX_STRING_SIZE];
    av_strerror(errnum, errbuf, AV_ERROR_MAX_STRING_SIZE);
    return {errbuf};
}
//计算并显示每秒的帧率 (FPS)
class FpsCounter {
public:
    FpsCounter(std::string name) : name(std::move(name)), count(0),
                                   last_time(std::chrono::high_resolution_clock::now()) {}

    void tick() {
        count++;
        auto now = std::chrono::high_resolution_clock::now();
        auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(now - last_time).count();
        if (duration >= 1000) {
            std::cout << "fps: " << count << std::endl;
            count = 0;
            last_time = now;
        }
    }

private:
    std::string name;
    int count;
    std::chrono::time_point<std::chrono::high_resolution_clock> last_time;
};
//计算并显示解码的持续时间
class DecodeDurationCounter {
public:
    DecodeDurationCounter(std::string name) : name(std::move(name)), count(0), count_duration(0),
                                              last_time(std::chrono::high_resolution_clock::now()) {}

    void begin() {
        begin_time = std::chrono::high_resolution_clock::now();
    }

    void end() {
        end_time = std::chrono::high_resolution_clock::now();
        count_duration += std::chrono::duration_cast<std::chrono::milliseconds>(end_time - begin_time).count();

        count++;
        auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(end_time - last_time).count();
        if (duration >= 1000) {
            if (count == 0) {
                std::cout << name << " decode duration: UNKNOWN" << std::endl;
            } else {
                std::cout << name << " decode duration: " << ((double) count_duration / count) << "ms" << std::endl;
            }
            count_duration = 0;
            count = 0;
            last_time = end_time;
        }
    }
//获取全部时间
    double get_ms() {
        return (double) std::chrono::duration_cast<std::chrono::microseconds>(end_time - begin_time).count() / 1000.0;
    }

private:
    std::string name;
    int count;
    std::chrono::time_point<std::chrono::high_resolution_clock> last_time;
    std::chrono::time_point<std::chrono::high_resolution_clock> begin_time;
    std::chrono::time_point<std::chrono::high_resolution_clock> end_time;
    long long count_duration;
};

//启动解码线程
bool VideoDecoder::run() {
    if (!init_success) {
        return false;
    }
    run_result = std::async(std::launch::async, &VideoDecoder::run_impl, this);
    return true;
}
//停止解码线程
bool VideoDecoder::stop() {
    want_stop = true;
    return run_result.get();
}

static bool need_retry(int ret) {
    return ret == AVERROR(EAGAIN) || ret == AVERROR_EXTERNAL;
}
//实际执行解码过程
bool VideoDecoder::run_impl() {
    int ret = 0;
    DecodeDurationCounter decode_duration_counter(url);
    FpsCounter fps_counter(url);

    // 读取帧
    while (!want_stop) {
        decode_duration_counter.begin();
        if ((ret = av_read_frame(av_format_ctx, av_packet)) < 0) {
            if (need_retry(ret)) {
                std::cout << "av_read_frame retry: " << av_err2string(ret) << std::endl;
                continue;
            }
            std::cout << "av_read_frame failed: " << av_err2string(ret) << std::endl;
            return false;
        }
        if (av_packet->stream_index == video_index) {
            // 发送包到解码器
            if ((ret = avcodec_send_packet(av_codec_ctx, av_packet)) < 0) {
                if (need_retry(ret)) {
                    avcodec_flush_buffers(av_codec_ctx);
                    std::cout << "avcodec_send_packet retry: " << av_err2string(ret) << std::endl;
                    continue;
                }
                std::cout << "avcodec_send_packet failed: " << av_err2string(ret) << std::endl;
                return false;
            }

            // 从解码器接收帧
            if ((ret = avcodec_receive_frame(av_codec_ctx, av_frame)) < 0) {
                if (need_retry(ret)) {
                    std::cout << "avcodec_receive_frame retry: " << av_err2string(ret) << std::endl;
                    continue;
                }
                std::cout << "avcodec_receive_frame failed: " << av_err2string(ret) << std::endl;
                return false;
            }

            // 转换帧
            if ((ret = sws_scale(sws_ctx,
                                 av_frame->data,
                                 av_frame->linesize,
                                 0,
                                 av_codec_ctx->height,
                                 av_frame_sws->data,
                                 av_frame_sws->linesize
            )) < 0) {
                std::cout << "sws_scale failed: " << av_err2string(ret) << std::endl;
                return false;
            }

            decode_duration_counter.end();
            fps_counter.tick();
            // 处理转换后的帧
            handler.handle_frame(av_frame_sws->data[0], width, height, decode_duration_counter.get_ms());
        }
        av_packet_unref(av_packet);
    }
    return true;
}

VideoDecoder::~VideoDecoder() {
    if (av_frame) {
        av_frame_free(&av_frame);
    }
    if (av_packet) {
        av_packet_free(&av_packet);
    }
    if (av_frame_sws) {
        av_freep(&av_frame_sws->data[0]);
        av_frame_free(&av_frame_sws);
    }
    if (sws_ctx) {
        sws_freeContext(sws_ctx);
    }
    if (av_codec_ctx) {
        avcodec_free_context(&av_codec_ctx);
    }
    if (av_opts) {
        av_dict_free(&av_opts);
    }
    if (av_format_ctx) {
        avformat_close_input(&av_format_ctx);
    }
}
