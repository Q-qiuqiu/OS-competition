#!/bin/sh
pip3 install -r ${SRC_PKG}/requirements.txt -i https://pypi.tuna.tsinghua.edu.cn/simple -t ${SRC_PKG} && cp -r ${SRC_PKG} ${DEPLOY_PKG}
