import csv
import time
import threading
import requests
from threading import Event
import json

# RTSP URL
rtsp_url = "rtsp://192.168.137.1:8554/camera_test"
http_base_url = "http://192.168.137.2:8081"


def get_response(http_base_url, rtsp_url, results, decode_time, infer_time, index, stop_event, start_event):
    start_url = f"{http_base_url}/start?url={rtsp_url}" + str(index + 1)
    start_response = requests.post(start_url, timeout=10)
    if start_response.status_code == 200:
        print(f"Thread {index}: 传输启动成功。")
    url = f"{http_base_url}/video?url={rtsp_url}" + str(index + 1)

    # 等待所有线程准备好
    start_event.wait()

    while not stop_event.is_set():
        start_time = time.time()
        try:
            response = requests.get(url, timeout=10)
            end_time = time.time()
            latency = end_time - start_time

            try:
                # 获取响应内容并移除 '!' 之后的部分
                data = response.content.decode('ISO-8859-1')
                json_data_str = data.split('!')[0]
                json_data = json.loads(json_data_str)
                if json_data.get("status") != "failed":
                    decode_time_temp = json_data.get("decode_time")
                    infer_time_temp = json_data.get("infer_time")
                    results[index].append(latency)
                    decode_time[index].append(int(decode_time_temp))
                    infer_time[index].append(int(infer_time_temp))
                    print(f"Thread {index}: Latency = {latency:.4f} seconds")
                    print(f"Thread {index}: decode_time = {decode_time_temp:.4f} mseconds")
                else:
                    print(f"Thread {index}: Status is 'failed', latency not recorded")
            except ValueError as e:
                print(f"Thread {index}: Latency = {latency:.4f} seconds")

                results[index].append(latency)  # Still count latency if parsing fails
                decode_time[index].append(int(decode_time_temp))
                infer_time[index].append(int(infer_time_temp))

            if response.status_code != 200:
                print(f"Thread {index}: Failed to get response, status code: {response.status_code}")
                break
        except requests.exceptions.RequestException as e:
            print(f"Thread {index}: Exception occurred: {e}")
            break


# Number of threads for the test
num_threads = int(input())

# Shared list to store results
results = [[] for _ in range(num_threads)]
decode_time = [[] for _ in range(num_threads)]
infer_time = [[] for _ in range(num_threads)]

# Event to signal the threads to stop
stop_event = Event()
# Event to signal all threads to start
start_event = Event()

try:
    for i in range(num_threads):
        stop_url = f"{http_base_url}/stop?url={rtsp_url}" + str(i + 1)
        stop_response = requests.post(stop_url, timeout=10)
        if stop_response.status_code == 200:
            print(f"Thread {i}: 传输停止成功。")
    # Create and start threads
    threads = []
    for i in range(num_threads):
        thread = threading.Thread(target=get_response,
                                  args=(http_base_url, rtsp_url, results, decode_time, infer_time, i, stop_event,
                                        start_event))
        threads.append(thread)
        thread.start()

    # Let the threads run for a specified duration
    time.sleep(2)  # 等待所有线程启动

    # Signal all threads to start
    start_event.set()

    # Run the threads for a specified duration
    time.sleep(60)

    # Signal threads to stop
    stop_event.set()

    # Wait for all threads to complete
    for thread in threads:
        thread.join()

    # Calculate average latency for each thread
    for i in range(num_threads):
        avg_latency = sum(results[i]) / len(results[i]) if results[i] else float('inf')
        print(f"Thread {i}: Average Latency = {avg_latency:.4f} seconds")
        avg_decode_time = sum(decode_time[i]) / len(decode_time[i]) if decode_time[i] else float('inf')
        print(f"Thread {i}: Average decode_time = {avg_decode_time:.4f} mseconds")
        avg_infer_time = sum(infer_time[i]) / len(infer_time[i]) if infer_time[i] else float('inf')
        print(f"Thread {i}: Average infer_time = {avg_infer_time:.4f} mseconds")

    # 向 /stop 发送 POST 请求
    for i in range(num_threads):
        stop_url = f"{http_base_url}/stop?url={rtsp_url}" + str(i + 1)
        stop_response = requests.post(stop_url, timeout=10)
        if stop_response.status_code == 200:
            print(f"Thread {i}: 传输停止成功。")


 #保存数据
    with open('test_result.csv', 'w', newline='') as csvfile:
        fieldnames = ['Thread', 'Latency (s)', 'Decode Time (ms)', 'Infer Time (ms)']
        writer = csv.DictWriter(csvfile, fieldnames=fieldnames)
        writer.writeheader()
        for i in range(num_threads):
            for j in range(len(results[i])):
                writer.writerow({'Thread': i, 'Latency (s)': results[i][j], 'Decode Time (ms)': decode_time[i][j], 'Infer Time (ms)': infer_time[i][j]})

except requests.exceptions.RequestException as e:
    print(f"启动请求失败: {e}")
