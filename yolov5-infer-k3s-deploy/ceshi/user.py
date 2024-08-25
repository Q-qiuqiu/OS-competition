from kubernetes import client, config
from flask import Flask, request, jsonify
import time

app = Flask(__name__)

@app.route('/create', methods=['POST'])
def create_pod_and_service():
    config.load_incluster_config()
    api = client.CoreV1Api()

    pod_body = client.V1Pod(
        api_version="v1",
        kind="Pod",
        metadata=client.V1ObjectMeta(name="yolov5-infer-cpp"),
        spec=client.V1PodSpec(
            containers=[
                client.V1Container(
                    name="yolov5-infer-cpp",
                    image="yolov5-infer-cpp",
                    image_pull_policy="IfNotPresent",
                    ports=[client.V1ContainerPort(container_port=8080)],
                    security_context=client.V1SecurityContext(privileged=True),
                    env=[
                        client.V1EnvVar(name="ASCEND_VISIBLE_DEVICES", value="0"),
                        client.V1EnvVar(name="ASCEND_ALLOW_LINK", value="True"),
                    ],
                    volume_mounts=[
                        client.V1VolumeMount(name="sys-version-conf", mount_path="/etc/sys_version.conf"),
                        client.V1VolumeMount(name="hdc-basic-cfg", mount_path="/etc/hdcBasic.cfg"),
                        client.V1VolumeMount(name="npu-smi", mount_path="/usr/local/sbin/npu-smi"),
                        client.V1VolumeMount(name="ascend-driver-lib64", mount_path="/usr/local/Ascend/driver/lib64"),
                        client.V1VolumeMount(name="aicpu-kernels", mount_path="/usr/lib64/aicpu_kernels"),
                        client.V1VolumeMount(name="slogd", mount_path="/var/slogd"),
                        client.V1VolumeMount(name="dmp-daemon", mount_path="/var/dmp_daemon"),
                        client.V1VolumeMount(name="libaicpu-processer", mount_path="/usr/lib64/libaicpu_processer.so"),
                        client.V1VolumeMount(name="libaicpu-prof", mount_path="/usr/lib64/libaicpu_prof.so"),
                        client.V1VolumeMount(name="libaicpu-sharder", mount_path="/usr/lib64/libaicpu_sharder.so"),
                        client.V1VolumeMount(name="libadump", mount_path="/usr/lib64/libadump.so"),
                        client.V1VolumeMount(name="libtsd-eventclient", mount_path="/usr/lib64/libtsd_eventclient.so"),
                        client.V1VolumeMount(name="libaicpu-scheduler", mount_path="/usr/lib64/libaicpu_scheduler.so"),
                        client.V1VolumeMount(name="libdcmi", mount_path="/usr/lib64/libdcmi.so"),
                        client.V1VolumeMount(name="libmpi-dvpp-adapter", mount_path="/usr/lib64/libmpi_dvpp_adapter.so"),
                        client.V1VolumeMount(name="libstackcore", mount_path="/usr/lib64/libstackcore.so"),
                        client.V1VolumeMount(name="ascend-driver", mount_path="/usr/local/Ascend/driver"),
                        client.V1VolumeMount(name="ascend-install-info", mount_path="/etc/ascend_install.info"),
                        client.V1VolumeMount(name="ascend-seclog", mount_path="/var/log/ascend_seclog"),
                        client.V1VolumeMount(name="davinci-driver", mount_path="/var/davinci/driver"),
                        client.V1VolumeMount(name="libc-sec", mount_path="/usr/lib64/libc_sec.so"),
                        client.V1VolumeMount(name="libdevmmap", mount_path="/usr/lib64/libdevmmap.so"),
                        client.V1VolumeMount(name="libdrvdsmi", mount_path="/usr/lib64/libdrvdsmi.so"),
                        client.V1VolumeMount(name="libslog", mount_path="/usr/lib64/libslog.so"),
                        client.V1VolumeMount(name="libmmpa", mount_path="/usr/lib64/libmmpa.so"),
                        client.V1VolumeMount(name="libascend-hal", mount_path="/usr/lib64/libascend_hal.so"),
                        client.V1VolumeMount(name="ascend-toolkit", mount_path="/usr/local/Ascend/ascend-toolkit")
                    ]
                )
            ],
            volumes=[
                client.V1Volume(name="sys-version-conf", host_path=client.V1HostPathVolumeSource(path="/etc/sys_version.conf")),
                client.V1Volume(name="hdc-basic-cfg", host_path=client.V1HostPathVolumeSource(path="/etc/hdcBasic.cfg")),
                client.V1Volume(name="npu-smi", host_path=client.V1HostPathVolumeSource(path="/usr/local/sbin/npu-smi")),
                client.V1Volume(name="ascend-driver-lib64", host_path=client.V1HostPathVolumeSource(path="/usr/local/Ascend/driver/lib64")),
                client.V1Volume(name="aicpu-kernels", host_path=client.V1HostPathVolumeSource(path="/usr/lib64/aicpu_kernels")),
                client.V1Volume(name="slogd", host_path=client.V1HostPathVolumeSource(path="/var/slogd")),
                client.V1Volume(name="dmp-daemon", host_path=client.V1HostPathVolumeSource(path="/var/dmp_daemon")),
                client.V1Volume(name="libaicpu-processer", host_path=client.V1HostPathVolumeSource(path="/usr/lib64/libaicpu_processer.so")),
                client.V1Volume(name="libaicpu-prof", host_path=client.V1HostPathVolumeSource(path="/usr/lib64/libaicpu_prof.so")),
                client.V1Volume(name="libaicpu-sharder", host_path=client.V1HostPathVolumeSource(path="/usr/lib64/libaicpu_sharder.so")),
                client.V1Volume(name="libadump", host_path=client.V1HostPathVolumeSource(path="/usr/lib64/libadump.so")),
                client.V1Volume(name="libtsd-eventclient", host_path=client.V1HostPathVolumeSource(path="/usr/lib64/libtsd_eventclient.so")),
                client.V1Volume(name="libaicpu-scheduler", host_path=client.V1HostPathVolumeSource(path="/usr/lib64/libaicpu_scheduler.so")),
                client.V1Volume(name="libdcmi", host_path=client.V1HostPathVolumeSource(path="/usr/lib64/libdcmi.so")),
                client.V1Volume(name="libmpi-dvpp-adapter", host_path=client.V1HostPathVolumeSource(path="/usr/lib64/libmpi_dvpp_adapter.so")),
                client.V1Volume(name="libstackcore", host_path=client.V1HostPathVolumeSource(path="/usr/lib64/libstackcore.so")),
                client.V1Volume(name="ascend-driver", host_path=client.V1HostPathVolumeSource(path="/usr/local/Ascend/driver")),
                client.V1Volume(name="ascend-install-info", host_path=client.V1HostPathVolumeSource(path="/etc/ascend_install.info")),
                client.V1Volume(name="ascend-seclog", host_path=client.V1HostPathVolumeSource(path="/var/log/ascend_seclog")),
                client.V1Volume(name="davinci-driver", host_path=client.V1HostPathVolumeSource(path="/var/davinci/driver")),
                client.V1Volume(name="libc-sec", host_path=client.V1HostPathVolumeSource(path="/usr/lib64/libc_sec.so")),
                client.V1Volume(name="libdevmmap", host_path=client.V1HostPathVolumeSource(path="/usr/lib64/libdevmmap.so")),
                client.V1Volume(name="libdrvdsmi", host_path=client.V1HostPathVolumeSource(path="/usr/lib64/libdrvdsmi.so")),
                client.V1Volume(name="libslog", host_path=client.V1HostPathVolumeSource(path="/usr/lib64/libslog.so")),
                client.V1Volume(name="libmmpa", host_path=client.V1HostPathVolumeSource(path="/usr/lib64/libmmpa.so")),
                client.V1Volume(name="libascend-hal", host_path=client.V1HostPathVolumeSource(path="/usr/lib64/libascend_hal.so")),
                client.V1Volume(name="ascend-toolkit", host_path=client.V1HostPathVolumeSource(path="/usr/local/Ascend/ascend-toolkit"))
            ]
        )
    )

    service_body = client.V1Service(
        api_version="v1",
        kind="Service",
        metadata=client.V1ObjectMeta(name="yolov5-infer-cpp-service"),
        spec=client.V1ServiceSpec(
            type="NodePort",
            selector={"app": "yolov5-infer-cpp"},
            ports=[client.V1ServicePort(port=8080, target_port=8080, node_port=30000)]
        )
    )
    
    start_time = time.time()

    # Create Pod
    api.create_namespaced_pod(namespace="default", body=pod_body)
    # Create Service
    api.create_namespaced_service(namespace="default", body=service_body)

    pod_creation_time = time.time() - start_time

    return jsonify(message="Pod and Service created successfully.",
                   pod_creation_time=f"{pod_creation_time:.2f} seconds")

if __name__ == '__main__':
    app.run(debug=True, host='0.0.0.0')

