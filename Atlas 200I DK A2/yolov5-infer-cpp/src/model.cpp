#include <memory>
#include "model.h"
//初始化
bool AclInit::init() {
    aclError ret;

    ret = aclInit(nullptr);
    if (ret != ACL_SUCCESS) {
        ACL_APP_LOG(ACL_ERROR, "acl init failed");
        return false;
    }

    ret = aclrtSetDevice(0);
    if (ret != ACL_SUCCESS) {
        ACL_APP_LOG(ACL_ERROR, "acl open device 0 failed");
        return false;
    }

    return true;
}

AclInit::~AclInit() {
    aclError ret;

    ret = aclrtResetDevice(0);
    if (ret != ACL_SUCCESS) {
        ACL_APP_LOG(ACL_ERROR, "acl reset device 0 failed");
    }

    ret = aclFinalize();
    if (ret != ACL_SUCCESS) {
        ACL_APP_LOG(ACL_ERROR, "acl finalize failed");
    }
}

bool AclContext::init() {
    aclError ret;

    ret = aclrtCreateContext(&context, 0);
    if (ret != ACL_SUCCESS) {
        ACL_APP_LOG(ACL_ERROR, "acl create context failed");
        return false;
    }

    ret = aclrtCreateStream(&stream);
    if (ret != ACL_SUCCESS) {
        ACL_APP_LOG(ACL_ERROR, "acl create stream failed");
        return false;
    }

    ret = aclrtGetRunMode(&mode);
    if (ret != ACL_SUCCESS) {
        ACL_APP_LOG(ACL_ERROR, "acl get run mode failed");
        return false;
    }

    return true;
}

AclContext::~AclContext() {
    aclError ret;

    if (stream) {
        ret = aclrtDestroyStream(stream);
        if (ret != ACL_SUCCESS) {
            ACL_APP_LOG(ACL_ERROR, "acl destroy stream failed");
        }
    }

    if (context) {
        ret = aclrtDestroyContext(context);
        if (ret != ACL_SUCCESS) {
            ACL_APP_LOG(ACL_ERROR, "acl destroy context failed");
        }
    }
}

Dataset::Dataset(const std::vector<MemRange> &vec_mem) : need_alloc(false), vec_mem(vec_mem) {
}

Dataset::Dataset(const std::vector<size_t> &vec_size) : need_alloc(true) {
    for (size_t size: vec_size) {
        vec_mem.push_back(MemRange{nullptr, size});
    }
}

bool Dataset::init() {
    aclError ret;

    dataset = aclmdlCreateDataset();//创建初始数据集
    if (dataset == nullptr) {
        ACL_APP_LOG(ACL_ERROR, "create dataset failed");
        return false;
    }

    for (MemRange &mem: vec_mem) {
        if (need_alloc) {
            ret = aclrtMalloc(&mem.ptr, mem.size, ACL_MEM_MALLOC_NORMAL_ONLY);
            if (ret != ACL_SUCCESS) {
                ACL_APP_LOG(ACL_ERROR, "malloc failed");
                return false;
            }
        }
        aclDataBuffer *data_buffer = aclCreateDataBuffer(mem.ptr, mem.size);
        if (data_buffer == nullptr) {
            ACL_APP_LOG(ACL_ERROR, "create data buffer failed");
            return false;
        }

        ret = aclmdlAddDatasetBuffer(dataset, data_buffer);
        if (ret != ACL_SUCCESS) {
            ACL_APP_LOG(ACL_ERROR, "add dataset buffer failed");
            return false;
        }
    }

    vec_mem.clear();
    vec_mem.shrink_to_fit();
    return true;
}

Dataset::~Dataset() {
    if (dataset) {
        size_t num_buffers = aclmdlGetDatasetNumBuffers(dataset);
        for (size_t i = 0; i < num_buffers; i++) {
            aclDataBuffer *data_buffer = aclmdlGetDatasetBuffer(dataset, i);
            if (need_alloc) {
                void *data = aclGetDataBufferAddr(data_buffer);
                aclrtFree(data);
            }
            aclDestroyDataBuffer(data_buffer);
        }
        aclmdlDestroyDataset(dataset);
    }
}
//加载模型
ModelLoader::ModelLoader(void *model_data, size_t model_size) : model_data(model_data), model_size(model_size) {
}

bool ModelLoader::init() {
    aclError ret;

    uint32_t id;
    ret = aclmdlLoadFromMem(model_data, model_size, &id);
    if (ret != ACL_SUCCESS) {
        ACL_APP_LOG(ACL_ERROR, "load model from file failed");
        return false;
    }
    model_id = id;

    model_desc = aclmdlCreateDesc();
    if (model_desc == nullptr) {
        ACL_APP_LOG(ACL_ERROR, "create model desc failed");
        return false;
    }

    ret = aclmdlGetDesc(model_desc, id);
    if (ret != ACL_SUCCESS) {
        ACL_APP_LOG(ACL_ERROR, "get model desc failed");
        return false;
    }

    return true;
}

ModelLoader::~ModelLoader() {
    aclError ret;

    if (model_desc) {
        ret = aclmdlDestroyDesc(model_desc);
        if (ret != ACL_SUCCESS) {
            ACL_APP_LOG(ACL_ERROR, "destroy model desc failed");
        }
    }

    if (model_id.has_value()) {
        ret = aclmdlUnload(model_id.value());
        if (ret != ACL_SUCCESS) {
            ACL_APP_LOG(ACL_ERROR, "unload model failed");
        }
    }
}

Model::Model(AclContext &context, ModelLoader &model_loader) : context(context), model_loader(model_loader) {
}

bool Model::init() {
    if (!create_output_dataset()) {
        return false;
    }
    return true;
}
//模型运行
bool Model::execute(const std::vector<MemRange> &input) {
    if (!create_input_dataset(input)) {
        return false;
    }

    aclError ret = aclmdlExecute(model_loader.get_model_id().value(), input_dataset.get(), output_dataset.get());
    if (ret != ACL_SUCCESS) {
        ACL_APP_LOG(ACL_ERROR, "execute model failed");
        return false;
    }

    return true;
}
//创建输入数据集
bool Model::create_input_dataset(const std::vector<MemRange> &input) {
    size_t input_num = aclmdlGetNumInputs(model_loader.get_model_desc());
    if (input.size() != input_num) {
        ACL_APP_LOG(ACL_ERROR, "input size mismatch");
        return false;
    }

    input_dataset = Dataset(input);
    if (!input_dataset.init()) {
        return false;
    }

    return true;
}
//创建输出数据集
bool Model::create_output_dataset() {
    aclmdlDesc *model_desc = model_loader.get_model_desc();

    size_t output_num = aclmdlGetNumOutputs(model_desc);
    std::vector<size_t> output_sizes;
    output_sizes.reserve(output_num);
    for (size_t i = 0; i < output_num; i++) {
        size_t size = aclmdlGetOutputSizeByIndex(model_desc, i);
        output_sizes.push_back(size);
    }

    output_dataset = Dataset(output_sizes);
    if (!output_dataset.init()) {
        return false;
    }

    return true;
}

