from kubernetes import client, config
import psycopg2
import json
import requests
from flask import Flask, request, jsonify

# 配置数据库连接信息
DB_HOST = 'localhost'
DB_PORT = '1234'
DB_NAME = 'test'
DB_USER = 'root'
DB_PASSWORD = '1234'

def get_k8s_nodes():
    # 配置加载kubeconfig文件或集群内配置
    config.load_kube_config()  # 如果在集群外运行
    # config.load_incluster_config()  # 如果在集群内运行

    # 创建API实例
    v1 = client.CoreV1Api()

    # 获取所有节点的信息
    nodes = v1.list_node().items

    devices = []
    for node in nodes:
        node_name = node.metadata.name
        allocatable = node.status.allocatable
        cpu_capacity = allocatable['cpu']
        mem_capacity = allocatable['memory']
        
        # 获取节点的资源使用情况（需要metrics-server支持）
        metrics_api = client.CustomObjectsApi()
        node_metrics = metrics_api.list_cluster_custom_object(
            "metrics.k8s.io", "v1beta1", "nodes")['items']

        for metrics in node_metrics:
            if metrics['metadata']['name'] == node_name:
                cpu_usage = metrics['usage']['cpu'].replace('n', '')  # 纳核 (nano-core)
                mem_usage = metrics['usage']['memory'].replace('Ki', '')  # KiB
                break

        device_info = {
            "id": node_name,
            "cpu_usage": float(cpu_usage) / (1000000000 * int(cpu_capacity)),  # 转换纳核为比例
            "npu_usage": 0,  # Kubernetes 默认不支持 NPU，需自定义资源
            "latency": 0,  # 初始延时
            "running_tasks": 0  # 初始运行任务数
        }
        
        devices.append(device_info)
    
    return devices
	
def insert_devices_into_db(devices):
    try:
        # 连接数据库
        conn = psycopg2.connect(
            host=DB_HOST,
            port=DB_PORT,
            dbname=DB_NAME,
            user=DB_USER,
            password=DB_PASSWORD
        )
        cursor = conn.cursor()

        # 创建表（如果不存在）
        cursor.execute("""
            CREATE TABLE IF NOT EXISTS devices (
                id VARCHAR(255) PRIMARY KEY,
                cpu_usage FLOAT,
                npu_usage FLOAT,
                latency FLOAT,
                running_tasks INT
            )
        """)
        conn.commit()

        # 插入数据
        for device in devices:
            cursor.execute("""
                INSERT INTO devices (id, cpu_usage, npu_usage, latency, running_tasks)
                VALUES (%s, %s, %s, %s, %s)
                ON CONFLICT (id) DO UPDATE SET
                    cpu_usage = EXCLUDED.cpu_usage,
                    npu_usage = EXCLUDED.npu_usage,
                    latency = EXCLUDED.latency,
                    running_tasks = EXCLUDED.running_tasks
            """, (device['id'], device['cpu_usage'], device['npu_usage'], device['latency'], device['running_tasks']))

        # 提交事务
        conn.commit()

        print("Devices data inserted/updated successfully")

    except Exception as e:
        print(f"Error inserting data into the database: {e}")
    
    finally:
        cursor.close()
        conn.close()

def filter_devices(devices):
    return [device for device in devices if device["cpu_usage"] <= (1 - required_cpu) and device["npu_usage"] <= (1 - required_npu)]

def assign_task(devices):
    # 首先进行过滤操作
    filtered_devices = filter_devices(devices)
    if not filtered_devices:
        return None, None, None

    # 根据运行任务数和延迟进行排序
    filtered_devices.sort(key=lambda d: (d["running_tasks"], d["latency"]))

    # 获取运行任务数最少的设备
    min_running_tasks = filtered_devices[0]["running_tasks"]
    candidate_devices = [device for device in filtered_devices if device["running_tasks"] == min_running_tasks]

    # 选择延迟最小的设备
    selected_device = min(candidate_devices, key=lambda d: d["latency"])

    # 模拟更新延迟和运行任务数
    selected_device["latency"] = random.uniform(1, 5)
    selected_device["running_tasks"] += 1

    return selected_device["id"], selected_device["cpu_usage"], selected_device["npu_usage"], selected_device["latency"], selected_device["running_tasks"]

def main():
	devices = get_k8s_nodes()
    while True:
        device_id, cpu_usage, npu_usage, latency, running_tasks = assign_task(devices)
        if device_id:
			url = "http://192.168.137.100:31314/yanshi"

			response = requests.get(url)
			response.raise_for_status()  # 检查请求是否成功
			
			data = response.json()
			latencytemp = data.get("latency", latency)  
			
			devicetemp = {
				"id": device_id,
				"cpu_usage": cpu_usage,
				"npu_usage": npu_usage,
				"latency": latencytemp,
				"running_tasks": running_tasks + 1
			}
			
			deviceinfo = [devicetemp]
			
			insert_devices_into_db(deviceinfo)

            print(f"Task assigned to device {device_id} with latency {latencytemp} and running tasks {running_tasks + 1}")
        else:
            print("No suitable devices found")
            break
