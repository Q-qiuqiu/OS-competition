网络结构：

- 192.168.137.1：电脑（网关）
- 192.168.137.2：master节点
- 192.168.137.3：atlas200-0节点

为了实现内网容器分发，需要在master节点上搭建docker仓库，用于存储docker镜像。在master节点执行：

```bash
docker run -d -p 5000:5000 --restart always --name registry registry:2
```

由于docker仓库未启用https，因此需要改变docker daemon安全配置。在master节点修改`/etc/docker/daemon.json`文件，加入：

```yaml
{
  "insecure-registries":["192.168.137.2:5000"]
} 
```

然后重启docker服务：

```bash
systemctl restart docker
```

还需要改变master节点和计算节点的k3s镜像拉取安全配置。分别在master节点和计算节点上执行：

```bash
mkdir -p /etc/rancher/k3s/
cat << EOF > /etc/rancher/k3s/registries.yaml
mirrors:
  "192.168.137.2:5000":
    endpoint:
      - "http://192.168.137.2:5000"
EOF
```

在master节点上执行：

```bash
systemctl restart k3s
```

在计算节点上执行：

```bash
systemctl restart k3s-agent
```

在master节点构建镜像（在yolov5-infer目录下执行，参考另一个群文件），并推送到仓库：

```bash
docker build -t 192.168.137.2:5000/yolov5-infer:latest .
docker push 192.168.137.2:5000/yolov5-infer:latest
```

在master节点上给计算节点标记label，表示设备类型：

```bash
kubectl label nodes atlas200-0 type=atlas200
```

在master节点上应用k3s配置文件：

```bash
kubectl apply -f deployment.yml
```

post请求http://192.168.137.2/predict，请求体为form-data，key为image，value为图片，即可访问到计算集群。