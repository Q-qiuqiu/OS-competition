FROM openeuler/openeuler:22.03-lts as builder

# 备份并替换YUM仓库文件
RUN mv /etc/yum.repos.d/openEuler.repo /etc/yum.repos.d/openEuler.repo.bak
COPY repo.txt /etc/yum.repos.d/openEuler.repo

# 安装必要的工具，包括cmake和opencv
RUN yum install -y cmake make gcc-c++ && \
    yum install -y opencv

COPY src /yolov5-infer-cpp/src
COPY CMakeLists.txt /yolov5-infer-cpp
COPY acl /usr/local/Ascend/ascend-toolkit/7.0.RC1/aarch64-linux/include/acl
COPY lib64 /usr/local/Ascend/ascend-toolkit/7.0.RC1/aarch64-linux/lib64
WORKDIR /yolov5-infer-cpp

RUN mkdir -p build && \
    cd build && \
    cmake .. -DCMAKE_BUILD_TYPE=Release && \
    make -j

# 构建完成后，构建最终的运行镜像
FROM openeuler/openeuler:22.03-lts

# 备份并替换YUM仓库文件
RUN mv /etc/yum.repos.d/openEuler.repo /etc/yum.repos.d/openEuler.repo.bak
COPY repo.txt /etc/yum.repos.d/openEuler.repo
# 设置Ascend环境变量
ENV ASCEND_TOOLKIT_HOME=/usr/local/Ascend/ascend-toolkit/latest \
    LD_LIBRARY_PATH=/usr/local/Ascend/ascend-toolkit/latest/lib64:/usr/local/Ascend/ascend-toolkit/latest/lib64/plugin/opskernel:/usr/local/Ascend/ascend-toolkit/latest/lib64/plugin/nnengine:/usr/local/Ascend/ascend-toolkit/latest/x86_64-linux/devlib/:/usr/local/Ascend/driver/lib64:${LD_LIBRARY_PATH} \
    PYTHONPATH=/usr/local/Ascend/ascend-toolkit/latest/python/site-packages:/usr/local/Ascend/ascend-toolkit/latest/opp/op_impl/built-in/ai_core/tbe:$PYTHONPATH \
    PATH=/usr/local/Ascend/ascend-toolkit/latest/bin:/usr/local/Ascend/ascend-toolkit/latest/compiler/ccec_compiler/bin:$PATH \
    ASCEND_AICPU_PATH=/usr/local/Ascend/ascend-toolkit/latest \
    ASCEND_OPP_PATH=/usr/local/Ascend/ascend-toolkit/latest/opp \
    TOOLCHAIN_HOME=/usr/local/Ascend/ascend-toolkit/latest/toolkit \
    ASCEND_HOME_PATH=/usr/local/Ascend/ascend-toolkit/latest \
    TMOUT=3600
# 创建目标目录
RUN mkdir -p /yolov5-infer-cpp && \
    yum install -y opencv
# 复制从 builder 阶段构建的应用
COPY --from=builder /yolov5-infer-cpp/build/src/yolov5_infer /yolov5-infer-cpp
COPY model /yolov5-infer-cpp/model
# 设置工作目录
WORKDIR /yolov5-infer-cpp
# 运行推理程序
ENTRYPOINT ["/yolov5-infer-cpp/yolov5_infer"]