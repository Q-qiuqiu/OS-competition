import time
import requests
from flask import Flask, request, jsonify

app = Flask(__name__)

@app.route('/calculate_latency', methods=['GET'])
def calculate_latency():
    url = "http://192.168.137.100:31314/reqidong"

    # 开始计时
    start_time = time.time()

    response = requests.get(url)
    response.raise_for_status()  # 检查请求是否成功

    # 结束计时
    end_time = time.time()

    # 计算延时，单位为秒
    latency = end_time - start_time

    return jsonify({"latency": latency})

if __name__ == '__main__':
    app.run(host='0.0.0.0')
