#ifndef VIDEO_DECODER_VIDEO_DECODER_H
#define VIDEO_DECODER_VIDEO_DECODER_H

#include <string>
#include <functional>
#include <future>

extern "C" {
#include <libavcodec/avcodec.h>
#include <libavformat/avformat.h>
#include <libavutil/avutil.h>
#include <libavutil/imgutils.h>
#include <libswscale/swscale.h>
}

class VideoDecoderGlobalInit {
public:
    bool init();

    ~VideoDecoderGlobalInit();
};

class FrameHandler {
public:
    virtual void handle_frame(uint8_t *data, int width, int height, double decode_time_ms) = 0;

    virtual ~FrameHandler() = default;
};

class VideoDecoder {
public:
    VideoDecoder(const std::string &url, int width, int height, FrameHandler &handler);

    ~VideoDecoder();

    bool init();

    bool run();

    bool stop();

private:
    std::string url{};
    int width{};
    int height{};
    FrameHandler &handler;
    bool init_success{};
    std::atomic_bool want_stop{};
    std::future<bool> run_result{};
    AVFormatContext *av_format_ctx{};
    int video_index{};
    AVCodecParameters *av_codec_params{};
    AVCodecContext *av_codec_ctx{};
    AVCodec *av_codec{};
    AVDictionary *av_opts{};
    SwsContext *sws_ctx{};
    AVPacket *av_packet{};
    AVFrame *av_frame{};
    AVFrame *av_frame_sws{};

    bool extract_video_info();

    bool open_decoder();

    bool create_sws();

    bool run_impl();
};

#endif //VIDEO_DECODER_VIDEO_DECODER_H
