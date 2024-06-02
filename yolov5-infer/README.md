构建docker：

```bash
docker build -t yolov5-infer:latest .
```

启动docker：

```bash
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
	--name yolov5-infer \
	-p 8080:80 \
    yolov5-infer
```

停止docker：

```bash
docker stop yolov5-infer && docker rm yolov5-infer
```

predict测试：

post访问http://ip:8080/predict，请求体为form-data，key为image，value为文件
