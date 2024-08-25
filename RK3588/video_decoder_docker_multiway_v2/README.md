使用搭载了 Rockchip RK3588 芯片的迅为iTOP-3588开发板，在此开发板上开发一个容器内的多线程解析视频的视频流处理系统。
```
└─ 📁RK3588
   └─ 📁video_decoder_docker_multiway_v2
      ├─ 📁src                            //视频解码程序的源代码
      ├─ 📄Dockerfile                     //制作包含所有运行环境和库文件的 docker image 语句 
      └─ 📄README.md                      //启动docker的挂载命令和程序接口说明等.
```
使用 C++ 编写视频解码程序，框架内提供 HTTP 接口支持。接收到视频设备传输的视频后，使用自身硬件解码的方式将视频流解码成帧图像，再传输给 NPU 推理开发板，接收到前端开发板的HTTP 请求后，会先将请求放入队列，如果接收到NPU推理开发板的返回结果后从队列中选出最新存入的HTTP请求，并将推理结果和帧图像数据传输给前端开发板。如果队列中存在等待超过500 ms 的HTTP请求，会将failed返回，并移出队列。

具体视频解码部分流程如下图所示：

<img src="/image/视频解码开发板流程.png" alt="视频解码开发板流程" width="50%" />

视频解码程序函数调用关系如下图所示：

<img src="/image/视频解码函数调用.png" alt="视频解码函数调用" width="50%" />

部署步骤如下：

构建

```bash
docker build -t video_decoder_multiway . --build-arg "http_proxy=http://192.168.137.1:2080" --build-arg "https_proxy=http://192.168.137.1:2080"
```

启动

```bash
docker run -it --privileged \
    --device=/dev/dri \
    --device=/dev/dma_heap \
    --device=/dev/rga \
    --device=/dev/mpp_service \
    -p 8081:8080 \
    --name=video_decoder_multiway \
    video_decoder_multiway:latest
```

```
docker exec -it video_decoder_multiway /bin/bash
```

停止

```bash
docker stop video_decoder_multiway && docker rm video_decoder_multiway
```

开始处理视频请求：

```
POST http://192.168.137.2:8081/start?url=xxx
```
开始处理视频请求的响应：

```
成功，status为success：
{
    "status": "success"
}
失败，status为failed：
{
    "status": "failed",
    "reason": "<reason>"
}
```

停止处理视频请求：

```
POST http://192.168.137.2:8081/stop?url=xxx
```

停止处理视频请求的响应：

```
成功：
{
    "status": "success"
}
失败：
{
    "status": "failed",
    "reason": "<reason>"
}
```

获取视频帧请求：

```
GET http://192.168.137.2:8081/video?url=xxx
```

获取视频请求的响应：

```
总体形式为：
json数据+!字符+二进制视频帧（BGR格式）
```

测试样例的json数据：

1. 成功

```
{
    "result": [
        [
            269.28125,
            197.75,
            392.71875,
            359.25,
            0.7147121429443359,
            41
        ],
        [
            596.15625,
            246.5625,
            639.84375,
            357.9375,
            0.49903178215026855,
            24
        ],
        [
            15.8125,
            295.171875,
            88.0625,
            358.328125,
            0.27820515632629395,
            41
        ],
        [
            444.40625,
            299.125,
            531.59375,
            360.375,
            0.27074623107910156,
            64
        ]
    ],
    "status": "success",
    "width": 480,
    "height": 360
}
```

成功但没有推理结果：

```
{
    "result": [],
    "status": "success",
    "width": 480,
    "height": 360
}
```

失败：

```
{
    "status": "failed",
    "reason": "<reason>"
}
```
