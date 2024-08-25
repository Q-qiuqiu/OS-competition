#!/bin/bash

# 循环执行命令40次
for i in {1..40}
do
   echo "Iteration $i" >> results.txt
   kubectl delete service yolov5-infer-cpp-service
   kubectl delete pod yolov5-infer-cpp
   # 休眠一定时间等待 Pod 和 Service 完全删除
   sleep 30
   # 尝试触发函数，并将输出追加到文件
   curl -X POST http://192.168.137.100:31314/ceshi2 >> results.txt 2>&1
   echo -e "\n" >> results.txt
done

