package v1

// This file contains a collection of methods that can be used from go-restful to
// generate Swagger API documentation for its models. Please read this PR for more
// information on the implementation: https://github.com/emicklei/go-restful/pull/215
//
// TODOs are ignored from the parser (e.g. TODO(andronat):... || TODO:...) if and only if
// they are on one line! For multiple line or blocks that you want to ignore use ---.
// Any context after a --- is ignored.
//
// Those methods can be generated by using hack/update-swagger-docs.sh
// AUTO-GENERATED FUNCTIONS START HERE
var map_Archive = map[string]string{
	"":         "Archive contains or references a collection of sources or binary files.",
	"type":     "Type defines how the package is specified: literal or URL. Available value:\n - literal\n - url",
	"literal":  "Literal contents of the package. Can be used for encoding packages below TODO (256 KB?) size.",
	"url":      "URL references a package.",
	"checksum": "Checksum ensures the integrity of packages referenced by URL. Ignored for literals.",
}

func (Archive) SwaggerDoc() map[string]string {
	return map_Archive
}

var map_AuthLogin = map[string]string{
	"": "AuthLogin defines the body for router login",
}

func (AuthLogin) SwaggerDoc() map[string]string {
	return map_AuthLogin
}

var map_Builder = map[string]string{
	"":          "Builder is the setting for environment builder.",
	"image":     "Image for containing the language compilation environment.",
	"command":   "(Optional) Default build command to run for this build environment.",
	"container": "(Optional) Container allows the modification of the deployed builder container using the Kubernetes Container spec. Fission overrides the following fields: - Name - Image; set to the Builder.Image - Command; set to the Builder.Command - TerminationMessagePath - ImagePullPolicy - ReadinessProbe",
	"podspec":   "PodSpec will store the spec of the pod that will be applied to the pod created for the builder",
}

func (Builder) SwaggerDoc() map[string]string {
	return map_Builder
}

var map_CanaryConfig = map[string]string{
	"": "CanaryConfig is for canary deployment of two functions.",
}

func (CanaryConfig) SwaggerDoc() map[string]string {
	return map_CanaryConfig
}

var map_CanaryConfigList = map[string]string{
	"": "CanaryConfigList is a list of CanaryConfigs.",
}

func (CanaryConfigList) SwaggerDoc() map[string]string {
	return map_CanaryConfigList
}

var map_CanaryConfigSpec = map[string]string{
	"":                 "CanaryConfigSpec defines the canary configuration spec",
	"trigger":          "HTTP trigger that this config references",
	"newfunction":      "New version of the function",
	"oldfunction":      "Old stable version of the function",
	"weightincrement":  "Weight increment step for function",
	"duration":         "Weight increment interval, string representation of time.Duration, ex : 1m, 2h, 2d (default: \"2m\")",
	"failurethreshold": "Threshold in percentage beyond which the new version of the function is considered unstable",
}

func (CanaryConfigSpec) SwaggerDoc() map[string]string {
	return map_CanaryConfigSpec
}

var map_CanaryConfigStatus = map[string]string{
	"": "CanaryConfigStatus represents canary config status",
}

func (CanaryConfigStatus) SwaggerDoc() map[string]string {
	return map_CanaryConfigStatus
}

var map_Checksum = map[string]string{
	"": "Checksum of package contents when the contents are stored outside the Package struct. Type is the checksum algorithm; \"sha256\" is the only currently supported one. Sum is hex encoded.",
}

func (Checksum) SwaggerDoc() map[string]string {
	return map_Checksum
}

var map_ConfigMapReference = map[string]string{
	"": "ConfigMapReference is a reference to a kubernetes configmap.",
}

func (ConfigMapReference) SwaggerDoc() map[string]string {
	return map_ConfigMapReference
}

var map_Environment = map[string]string{
	"": "Environment is environment for building and running user functions.",
}

func (Environment) SwaggerDoc() map[string]string {
	return map_Environment
}

var map_EnvironmentList = map[string]string{
	"": "EnvironmentList is a list of Environments.",
}

func (EnvironmentList) SwaggerDoc() map[string]string {
	return map_EnvironmentList
}

var map_EnvironmentReference = map[string]string{
	"": "EnvironmentReference is a reference to an environment.",
}

func (EnvironmentReference) SwaggerDoc() map[string]string {
	return map_EnvironmentReference
}

var map_EnvironmentSpec = map[string]string{
	"":                             "EnvironmentSpec contains with builder, runtime and some other related environment settings.",
	"version":                      "Version is the Environment API version\n\nVersion \"1\" allows user to run code snippet in a file, and it's supported by most of the environments except tensorflow-serving.\n\nVersion \"2\" supports downloading and compiling user function if source archive is not empty.\n\nVersion \"3\" is almost the same with v2, but you're able to control the size of pre-warm pool of the environment.",
	"runtime":                      "Runtime is configuration for running function, like container image etc.",
	"builder":                      "(Optional) Builder is configuration for builder manager to launch environment builder to build source code into deployable binary.",
	"allowedFunctionsPerContainer": "(Optional) defaults to 'single'. Fission workflow uses 'infinite' to load multiple functions in one function pod. Available value: - single - infinite",
	"allowAccessToExternalNetwork": "Istio default blocks all egress traffic for safety. To enable accessibility of external network for builder/function pod, set to 'true'. (Optional) defaults to 'false'",
	"resources":                    "The request and limit CPU/MEM resource setting for poolmanager to set up pods in the pre-warm pool. (Optional) defaults to no limitation.",
	"poolsize":                     "The initial pool size for environment",
	"terminationGracePeriod":       "The grace time for pod to perform connection draining before termination. The unit is in seconds. (Optional) defaults to 360 seconds",
	"keeparchive":                  "KeepArchive is used by fetcher to determine if the extracted archive or unarchived file should be placed, which is then used by specialize handler. (This is mainly for the JVM environment because .jar is one kind of zip archive.)",
	"imagepullsecret":              "ImagePullSecret is the secret for Kubernetes to pull an image from a private registry.",
}

func (EnvironmentSpec) SwaggerDoc() map[string]string {
	return map_EnvironmentSpec
}

var map_ExecutionStrategy = map[string]string{
	"":                      "ExecutionStrategy specifies low-level parameters for function execution, such as the number of instances.\n\nMinScale affects the cold start behavior for a function. If MinScale is 0 then the deployment is created on first invocation of function and is good for requests of asynchronous nature. If MinScale is greater than 0 then MinScale number of pods are created at the time of creation of function. This ensures faster response during first invocation at the cost of consuming resources.\n\nMaxScale is the maximum number of pods that function will scale to based on TargetCPUPercent and resources allocated to the function pod.",
	"ExecutorType":          "ExecutorType is the executor type of function used. Defaults to \"poolmgr\".\n\nAvailable value:\n - poolmgr\n - newdeploy\n - container",
	"MinScale":              "This is only for newdeploy to set up minimum replicas of deployment.",
	"MaxScale":              "This is only for newdeploy to set up maximum replicas of deployment.",
	"TargetCPUPercent":      "Deprecated: use hpaMetrics instead. This is only for executor type newdeploy and container to set up target CPU utilization of HPA. Applicable for executor type newdeploy and container.",
	"SpecializationTimeout": "This is the timeout setting for executor to wait for pod specialization.",
	"hpaMetrics":            "hpaMetrics is the list of metrics used to determine the desired replica count of the Deployment created for the function. Applicable for executor type newdeploy and container.",
	"hpaBehavior":           "hpaBehavior is the behavior of HPA when scaling in up/down direction. Applicable for executor type newdeploy and container.",
}

func (ExecutionStrategy) SwaggerDoc() map[string]string {
	return map_ExecutionStrategy
}

var map_Function = map[string]string{
	"": "Function is function runs within environment runtime with given package and secrets/configmaps.",
}

func (Function) SwaggerDoc() map[string]string {
	return map_Function
}

var map_FunctionList = map[string]string{
	"": "FunctionList is a list of Functions.",
}

func (FunctionList) SwaggerDoc() map[string]string {
	return map_FunctionList
}

var map_FunctionPackageRef = map[string]string{
	"":             "FunctionPackageRef includes the reference to the package also the entrypoint of package.",
	"packageref":   "Package reference",
	"functionName": "FunctionName specifies a specific function within the package. This allows functions to share packages, by having different functions within the same package.\n\nFission itself does not interpret this path. It is passed verbatim to build and runtime environments.\n\nThis is optional: if unspecified, the environment has a default name.",
}

func (FunctionPackageRef) SwaggerDoc() map[string]string {
	return map_FunctionPackageRef
}

var map_FunctionReference = map[string]string{
	"":                "FunctionReference refers to a function",
	"type":            "Type indicates whether this function reference is by name or selector. For now, the only supported reference type is by \"name\".  Future reference types:\n  * Function by label or annotation\n  * Branch or tag of a versioned function\n  * A \"rolling upgrade\" from one version of a function to another\nAvailable value: - name - function-weights",
	"name":            "Name of the function.",
	"functionweights": "Function Reference by weight. this map contains function name as key and its weight as the value. This is for canary upgrade purpose.",
}

func (FunctionReference) SwaggerDoc() map[string]string {
	return map_FunctionReference
}

var map_FunctionSpec = map[string]string{
	"":                "FunctionSpec describes the contents of the function.",
	"environment":     "Environment is the build and runtime environment that this function is associated with. An Environment with this name should exist, otherwise the function cannot be invoked.",
	"package":         "Reference to a package containing deployment and optionally the source.",
	"secrets":         "Reference to a list of secrets.",
	"configmaps":      "Reference to a list of configmaps.",
	"resources":       "cpu and memory resources as per K8S standards This is only for newdeploy to set up resource limitation when creating deployment for a function.",
	"InvokeStrategy":  "InvokeStrategy is a set of controls which affect how function executes",
	"functionTimeout": "FunctionTimeout provides a maximum amount of duration within which a request for a particular function execution should be complete. This is optional. If not specified default value will be taken as 60s",
	"idletimeout":     "IdleTimeout specifies the length of time that a function is idle before the function pod(s) are eligible for deletion. If no traffic to the function is detected within the idle timeout, the executor will then recycle the function pod(s) to release resources.",
	"concurrency":     "Maximum number of pods to be specialized which will serve requests This is optional. If not specified default value will be taken as 500",
	"requestsPerPod":  "RequestsPerPod indicates the maximum number of concurrent requests that can be served by a specialized pod This is optional. If not specified default value will be taken as 1",
	"onceOnly":        "OnceOnly specifies if specialized pod will serve exactly one request in its lifetime and would be garbage collected after serving that one request This is optional. If not specified default value will be taken as false",
	"retainPods":      "RetainPods specifies the number of specialized pods that should be retained after serving requests This is optional. If not specified default value will be taken as 0",
	"podspec":         "Podspec specifies podspec to use for executor type container based functions Different arguments mentioned for container based function are populated inside a pod.",
}

func (FunctionSpec) SwaggerDoc() map[string]string {
	return map_FunctionSpec
}

var map_HTTPTrigger = map[string]string{
	"": "HTTPTrigger is the trigger invokes user functions when receiving HTTP requests.",
}

func (HTTPTrigger) SwaggerDoc() map[string]string {
	return map_HTTPTrigger
}

var map_HTTPTriggerList = map[string]string{
	"": "HTTPTriggerList is a list of HTTPTriggers",
}

func (HTTPTriggerList) SwaggerDoc() map[string]string {
	return map_HTTPTriggerList
}

var map_HTTPTriggerSpec = map[string]string{
	"":              "HTTPTriggerSpec is for router to expose user functions at the given URL path.",
	"host":          "Deprecated: the original idea of this field is not for setting Ingress. Since we have IngressConfig now, remove Host after couple releases.",
	"relativeurl":   "RelativeURL is the exposed URL for external client to access a function with.",
	"prefix":        "Prefix with which functions are exposed. NOTE: Prefix takes precedence over URL/RelativeURL. Note that it does not treat slashes specially (\"/foobar/\" will be matched by the prefix \"/foobar\").",
	"keepPrefix":    "When function is exposed with Prefix based path, keepPrefix decides whether to keep or trim prefix in URL while invoking function.",
	"method":        "Use Methods instead of Method. This field is going to be deprecated in a future release HTTP method to access a function.",
	"methods":       "HTTP methods to access a function",
	"functionref":   "FunctionReference is a reference to the target function.",
	"createingress": "If CreateIngress is true, router will create an ingress definition.",
	"ingressconfig": "IngressConfig for router to set up Ingress.",
}

func (HTTPTriggerSpec) SwaggerDoc() map[string]string {
	return map_HTTPTriggerSpec
}

var map_IngressConfig = map[string]string{
	"":            "IngressConfig is for router to set up Ingress.",
	"annotations": "Annotations will be added to metadata when creating Ingress.",
	"path":        "Path is for path matching. The format of path depends on what ingress controller you used.",
	"host":        "Host is for ingress controller to apply rules. If host is empty or \"*\", the rule applies to all inbound HTTP traffic.",
	"tls":         "TLS is for user to specify a Secret that contains TLS key and certificate. The domain name in the key and crt must match the value of Host field.",
}

func (IngressConfig) SwaggerDoc() map[string]string {
	return map_IngressConfig
}

var map_InvokeStrategy = map[string]string{
	"":                  "InvokeStrategy is a set of controls over how the function executes. It affects the performance and resource usage of the function.\n\nAn InvokeStrategy is of one of two types: ExecutionStrategy, which controls low-level parameters such as which ExecutorType to use, when to autoscale, minimum and maximum number of running instances, etc. A higher-level AbstractInvokeStrategy will also be supported; this strategy would specify the target request rate of the function, the target latency statistics, and the target cost (in terms of compute resources).",
	"ExecutionStrategy": "ExecutionStrategy specifies low-level parameters for function execution, such as the number of instances.",
	"StrategyType":      "StrategyType is the strategy type of function. Now it only supports 'execution'.",
}

func (InvokeStrategy) SwaggerDoc() map[string]string {
	return map_InvokeStrategy
}

var map_KubernetesWatchTrigger = map[string]string{
	"": "KubernetesWatchTrigger watches kubernetes resource events and invokes functions.",
}

func (KubernetesWatchTrigger) SwaggerDoc() map[string]string {
	return map_KubernetesWatchTrigger
}

var map_KubernetesWatchTriggerList = map[string]string{
	"": "KubernetesWatchTriggerList is a list of KubernetesWatchTriggers",
}

func (KubernetesWatchTriggerList) SwaggerDoc() map[string]string {
	return map_KubernetesWatchTriggerList
}

var map_KubernetesWatchTriggerSpec = map[string]string{
	"":              "KubernetesWatchTriggerSpec defines spec of KuberenetesWatchTrigger",
	"type":          "Type of resource to watch (Pod, Service, etc.)",
	"labelselector": "Resource labels",
	"functionref":   "The reference to a function for kubewatcher to invoke with when receiving events.",
}

func (KubernetesWatchTriggerSpec) SwaggerDoc() map[string]string {
	return map_KubernetesWatchTriggerSpec
}

var map_MessageQueueTrigger = map[string]string{
	"": "MessageQueueTrigger invokes functions when messages arrive to certain topic that trigger subscribes to.",
}

func (MessageQueueTrigger) SwaggerDoc() map[string]string {
	return map_MessageQueueTrigger
}

var map_MessageQueueTriggerList = map[string]string{
	"": "MessageQueueTriggerList is a list of MessageQueueTriggers.",
}

func (MessageQueueTriggerList) SwaggerDoc() map[string]string {
	return map_MessageQueueTriggerList
}

var map_MessageQueueTriggerSpec = map[string]string{
	"":                 "MessageQueueTriggerSpec defines a binding from a topic in a message queue to a function.",
	"functionref":      "The reference to a function for message queue trigger to invoke with when receiving messages from subscribed topic.",
	"messageQueueType": "Type of message queue (NATS, Kafka, AzureQueue)",
	"topic":            "Subscribed topic",
	"respTopic":        "Topic for message queue trigger to sent response from function.",
	"errorTopic":       "Topic to collect error response sent from function",
	"maxRetries":       "Maximum times for message queue trigger to retry",
	"contentType":      "Content type of payload",
	"pollingInterval":  "The period to check each trigger source on every ScaledObject, and scale the deployment up or down accordingly",
	"cooldownPeriod":   "The period to wait after the last trigger reported active before scaling the deployment back to 0",
	"minReplicaCount":  "Minimum number of replicas KEDA will scale the deployment down to",
	"maxReplicaCount":  "Maximum number of replicas KEDA will scale the deployment up to",
	"metadata":         "ScalerTrigger fields",
	"secret":           "Secret name",
	"mqtkind":          "Kind of Message Queue Trigger to be created, by default its fission",
	"podspec":          "(Optional) Podspec allows modification of deployed runtime pod with Kubernetes PodSpec The merging logic is briefly described below and detailed MergePodSpec function - Volumes mounts and env variables for function and fetcher container are appended - All additional containers and init containers are appended - Volume definitions are appended - Lists such as tolerations, ImagePullSecrets, HostAliases are appended - Structs are merged and variables from pod spec take precedence",
}

func (MessageQueueTriggerSpec) SwaggerDoc() map[string]string {
	return map_MessageQueueTriggerSpec
}

var map_Package = map[string]string{
	"":       "Package Think of these as function-level images.",
	"status": "Status indicates the build status of package.",
}

func (Package) SwaggerDoc() map[string]string {
	return map_Package
}

var map_PackageList = map[string]string{
	"": "PackageList is a list of Packages.",
}

func (PackageList) SwaggerDoc() map[string]string {
	return map_PackageList
}

var map_PackageRef = map[string]string{
	"":                "PackageRef is a reference to the package.",
	"resourceversion": "Including resource version in the reference forces the function to be updated on package update, making it possible to cache the function based on its metadata.",
}

func (PackageRef) SwaggerDoc() map[string]string {
	return map_PackageRef
}

var map_PackageSpec = map[string]string{
	"":            "PackageSpec includes source/deploy archives and the reference of environment to build the package.",
	"environment": "Environment is a reference to the environment for building source archive.",
	"source":      "Source is the archive contains source code and dependencies file. If the package status is in PENDING state, builder manager will then notify builder to compile source and save the result as deployable archive.",
	"deployment":  "Deployment is the deployable archive that environment runtime used to run user function.",
	"buildcmd":    "BuildCommand is a custom build command that builder used to build the source archive.",
}

func (PackageSpec) SwaggerDoc() map[string]string {
	return map_PackageSpec
}

var map_PackageStatus = map[string]string{
	"":                    "PackageStatus contains the build status of a package also the build log for examination.",
	"buildstatus":         "BuildStatus is the package build status.",
	"buildlog":            "BuildLog stores build log during the compilation.",
	"lastUpdateTimestamp": "LastUpdateTimestamp will store the timestamp the package was last updated metav1.Time is a wrapper around time.Time which supports correct marshaling to YAML and JSON. https://github.com/kubernetes/apimachinery/blob/44bd77c24ef93cd3a5eb6fef64e514025d10d44e/pkg/apis/meta/v1/time.go#L26-L35",
}

func (PackageStatus) SwaggerDoc() map[string]string {
	return map_PackageStatus
}

var map_RouterAuthToken = map[string]string{
	"": "RouterAuthToken defines the authorization token for accessing router",
}

func (RouterAuthToken) SwaggerDoc() map[string]string {
	return map_RouterAuthToken
}

var map_Runtime = map[string]string{
	"":          "Runtime is the setting for environment runtime.",
	"image":     "Image for containing the language runtime.",
	"container": "(Optional) Container allows the modification of the deployed runtime container using the Kubernetes Container spec. Fission overrides the following fields: - Name - Image; set to the Runtime.Image - TerminationMessagePath - ImagePullPolicy\n\nYou can set either PodSpec or Container, but not both. kubebuilder:validation:XPreserveUnknownFields=true",
	"podspec":   "(Optional) Podspec allows modification of deployed runtime pod with Kubernetes PodSpec The merging logic is briefly described below and detailed MergePodSpec function - Volumes mounts and env variables for function and fetcher container are appended - All additional containers and init containers are appended - Volume definitions are appended - Lists such as tolerations, ImagePullSecrets, HostAliases are appended - Structs are merged and variables from pod spec take precedence\n\nYou can set either PodSpec or Container, but not both.",
}

func (Runtime) SwaggerDoc() map[string]string {
	return map_Runtime
}

var map_SecretReference = map[string]string{
	"": "SecretReference is a reference to a kubernetes secret.",
}

func (SecretReference) SwaggerDoc() map[string]string {
	return map_SecretReference
}

var map_TimeTrigger = map[string]string{
	"": "TimeTrigger invokes functions based on given cron schedule.",
}

func (TimeTrigger) SwaggerDoc() map[string]string {
	return map_TimeTrigger
}

var map_TimeTriggerList = map[string]string{
	"": "TimeTriggerList is a list of TimeTriggers.",
}

func (TimeTriggerList) SwaggerDoc() map[string]string {
	return map_TimeTriggerList
}

var map_TimeTriggerSpec = map[string]string{
	"":            "TimeTriggerSpec invokes the specific function at a time or times specified by a cron string.",
	"cron":        "Cron schedule",
	"functionref": "The reference to function",
}

func (TimeTriggerSpec) SwaggerDoc() map[string]string {
	return map_TimeTriggerSpec
}

// AUTO-GENERATED FUNCTIONS END HERE
