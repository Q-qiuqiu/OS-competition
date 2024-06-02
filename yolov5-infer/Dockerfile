FROM openeuler/openeuler:22.03-lts 
RUN mv /etc/yum.repos.d/openEuler.repo /etc/yum.repos.d/openEuler.repo.bak

# 导入openEuler.repo 文件,更换yum源
COPY repo.txt /etc/yum.repos.d/openEuler.repo

RUN yum clean all && \
    yum makecache && \
    yum -y install openssl && \
    yum -y install python3-pip && \
    pip3 config set global.index-url https://pypi.tuna.tsinghua.edu.cn/simple && \
    pip3 install torch torchvision && \
    yum -y install gcc python3-devel && \
    yum -y install mesa-libGL && \
    yum -y install dos2unix && \
    pip3 install opencv-python && \
    pip3 install attrs numpy decorator sympy cffi pyyaml pathlib2 psutil protobuf scipy requests absl-py Pillow flask

#设置SCNN环境变量
ENV ASCEND_TOOLKIT_HOME=/usr/local/Ascend/ascend-toolkit/latest \
    LD_LIBRARY_PATH=/usr/local/Ascend/ascend-toolkit/latest/lib64:/usr/local/Ascend/ascend-toolkit/latest/lib64/plugin/opskernel:/usr/local/Ascend/ascend-toolkit/latest/lib64/plugin/nnengine:/usr/local/Ascend/ascend-toolkit/latest/x86_64-linux/devlib/:$LD_LIBRARY_PATH \
    PYTHONPATH=/usr/local/Ascend/ascend-toolkit/latest/python/site-packages:/usr/local/Ascend/ascend-toolkit/latest/opp/op_impl/built-in/ai_core/tbe:$PYTHONPATH \
    PATH=/usr/local/Ascend/ascend-toolkit/latest/bin:/usr/local/Ascend/ascend-toolkit/latest/compiler/ccec_compiler/bin:$PATH \
    ASCEND_AICPU_PATH=/usr/local/Ascend/ascend-toolkit/latest \
    ASCEND_OPP_PATH=/usr/local/Ascend/ascend-toolkit/latest/opp \
    TOOLCHAIN_HOME=/usr/local/Ascend/ascend-toolkit/latest/toolkit \
    ASCEND_HOME_PATH=/usr/local/Ascend/ascend-toolkit/latest \
    LD_LIBRARY_PATH=/usr/local/Ascend/driver/lib64:${LD_LIBRARY_PATH} \
    TMOUT=3600

COPY infer /infer
WORKDIR /infer

RUN chmod +x start.sh
CMD ["./start.sh"]