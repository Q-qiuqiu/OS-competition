#ifndef YOLOV5_INFER_SRC_MODEL_H
#define YOLOV5_INFER_SRC_MODEL_H

#include <string>
#include <vector>
#include <optional>
#include <acl/acl.h>
#include <acl/acl_base.h>
#include <acl/acl_mdl.h>

class AclInit {
public:
    bool init();

    ~AclInit();
};

class AclContext {
public:
    bool init();

    ~AclContext();

    aclrtContext get_context() {
        return context;
    }

    aclrtStream get_stream() {
        return stream;
    }

    bool is_device() {
        return mode == ACL_DEVICE;
    }

private:
    aclrtContext context{};
    aclrtStream stream{};
    aclrtRunMode mode{};
};

struct MemRange {
    void *ptr;
    size_t size;
};

class Dataset {
public:
    Dataset() = default;

    explicit Dataset(const std::vector<MemRange> &vec_mem);

    explicit Dataset(const std::vector<size_t> &vec_size);

    bool init();

    ~Dataset();

    aclmdlDataset *get() {
        return dataset;
    }

private:
    bool need_alloc{};
    std::vector<MemRange> vec_mem;
    aclmdlDataset *dataset{};
};

class ModelLoader {
public:
    explicit ModelLoader(void *model_data, size_t model_size);

    bool init();

    ~ModelLoader();

    std::optional<uint32_t> get_model_id() {
        return model_id;
    }

    aclmdlDesc *get_model_desc() {
        return model_desc;
    }

private:
    void *model_data;
    size_t model_size;
    std::optional<uint32_t> model_id;
    aclmdlDesc *model_desc{};
};

class Model {
public:
    Model(AclContext &context, ModelLoader &model_loader);

    bool init();

    bool execute(const std::vector<MemRange> &input);

protected:
    AclContext &context;
    ModelLoader &model_loader;

    Dataset input_dataset;
    Dataset output_dataset;

    bool create_input_dataset(const std::vector<MemRange> &input);

    bool create_output_dataset();
};

#endif //YOLOV5_INFER_SRC_MODEL_H
