使用 Atlas 200I DK A2 开发者套件部署 k3s 等系统，对接收的信息进行分析，并调用合适的系统进行资源调度。

```
📁Atlas 200I DK A2
  ├─ 📁yolov5-infer-cpp
  │  ├─ 📁acl                             // 华为Ascend AI处理器的ACL（Ascend Computing Language）API
  │  ├─ 📁lib64                           // 相关库文件
  │  ├─ 📁model                           // yolov5的om模型
  │  ├─ 📁src                             // 调用NPU进行推理的程序代码
  │  ├─ 📄CMakeLists.txt
  │  ├─ 📄Dockerfile                      // 制作包含所有运行环境和库文件的 docker image 语句 
  │  ├─ 📄README.md                       // 启动docker的挂载命令和程序接口说明等
  │  └─ 📄repo.txt                        // 与 dockerfile 配套使用，用于更换 OpenEuler 的 yum 源
```

根据 Yolov5 模型使用 C++ 制作了一个图像推理程序。便于NPU推理开发板接收视频解码开发板提供的图像并调用容器内部的推理程序进行分析。程序框架内提供HTTP接口支持，使用华为在昇腾平台上开发深度神经网络推理应用的C语言API库AscendCL，其能提供运行资源管理、内存管理、模型加载与执行、算子加载与执行、媒体数据处理等API，能够实现利用昇腾硬件计算资源、在昇腾CANN平台上进行深度学习推理计算、图形图像预处理、单算子加速计算等能力。此外还借助OpenCV库和yolov5模型将图像进行推理，将推理结果填充HTTP响应后返回。

具体NPU推理部分流程如下图所示：

<img src="/image/NPU开发板流程.png" alt="NPU推理开发板流程" width="50%" />

NPU推理程序函数调用关系如下图所示：

<img src="/image/NPU推理函数调用.png" alt="NPU推理函数调用" width="50%" />

部署步骤如下：

构建镜像
```
docker build -t yolov5-infer-cpp:latest .
```

创建容器
```
docker run -itd --privileged \
    -e ASCEND_VISIBLE_DEVICES=0 \
    -e ASCEND_ALLOW_LINK=True \
    --device=/dev/svm0 \
    --device=/dev/ts_aisle \
    --device=/dev/upgrade \
    --device=/dev/sys \
    --device=/dev/vdec \
    --device=/dev/vpc \
    --device=/dev/pngd \
    --device=/dev/venc \
    --device=/dev/dvpp_cmdlist \
    --device=/dev/log_drv \
    -v /etc/sys_version.conf:/etc/sys_version.conf \
    -v /etc/hdcBasic.cfg:/etc/hdcBasic.cfg \
    -v /usr/local/sbin/npu-smi:/usr/local/sbin/npu-smi \
    -v /usr/local/Ascend/driver/lib64:/usr/local/Ascend/driver/lib64 \
    -v /usr/lib64/aicpu_kernels/:/usr/lib64/aicpu_kernels/ \
    -v /var/slogd:/var/slogd \
    -v /var/dmp_daemon:/var/dmp_daemon \
    -v /usr/lib64/libaicpu_processer.so:/usr/lib64/libaicpu_processer.so \
    -v /usr/lib64/libaicpu_prof.so:/usr/lib64/libaicpu_prof.so \
    -v /usr/lib64/libaicpu_sharder.so:/usr/lib64/libaicpu_sharder.so \
    -v /usr/lib64/libadump.so:/usr/lib64/libadump.so \
    -v /usr/lib64/libtsd_eventclient.so:/usr/lib64/libtsd_eventclient.so \
    -v /usr/lib64/libaicpu_scheduler.so:/usr/lib64/libaicpu_scheduler.so \
    -v /usr/lib64/libdcmi.so:/usr/lib64/libdcmi.so \
    -v /usr/lib64/libmpi_dvpp_adapter.so:/usr/lib64/libmpi_dvpp_adapter.so \
    -v /usr/lib64/libstackcore.so:/usr/lib64/libstackcore.so \
    -v /usr/local/Ascend/driver:/usr/local/Ascend/driver \
    -v /etc/ascend_install.info:/etc/ascend_install.info \
    -v /var/log/ascend_seclog:/var/log/ascend_seclog \
    -v /var/davinci/driver:/var/davinci/driver \
    -v /usr/lib64/libc_sec.so:/usr/lib64/libc_sec.so \
    -v /usr/lib64/libdevmmap.so:/usr/lib64/libdevmmap.so \
    -v /usr/lib64/libdrvdsmi.so:/usr/lib64/libdrvdsmi.so \
    -v /usr/lib64/libslog.so:/usr/lib64/libslog.so \
    -v /usr/lib64/libmmpa.so:/usr/lib64/libmmpa.so \
    -v /usr/lib64/libascend_hal.so:/usr/lib64/libascend_hal.so \
    -v /usr/local/Ascend/ascend-toolkit:/usr/local/Ascend/ascend-toolkit \
    --name yolov5-infer-cpp \
    -p 8080:8080 \
    yolov5-infer-cpp 
```
运行容器即可自动启动推理程序

predict测试：
post访问http://192.168.137.100:8080/predict，请求体为form-data，key为image，value为文件 