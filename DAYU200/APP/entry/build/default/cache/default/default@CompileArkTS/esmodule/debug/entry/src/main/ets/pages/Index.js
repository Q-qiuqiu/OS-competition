import picker from '@ohos:file.picker';
import fs from '@ohos:file.fs';
import request from '@ohos:request';
import http from '@ohos:net.http';
class Index extends ViewPU {
    constructor(parent, params, __localStorage, elmtId = -1) {
        super(parent, __localStorage, elmtId);
        this.__message = new ObservedPropertySimplePU('', this, "message");
        this.__imgSrc = new ObservedPropertySimplePU('', this, "imgSrc");
        this.__title = new ObservedPropertySimplePU('等待传输', this, "title");
        this.__imgName = new ObservedPropertySimplePU('', this, "imgName");
        this.__IsLocalFlag = new ObservedPropertySimplePU(true, this, "IsLocalFlag");
        this.setInitiallyProvidedValue(params);
    }
    setInitiallyProvidedValue(params) {
        if (params.message !== undefined) {
            this.message = params.message;
        }
        if (params.imgSrc !== undefined) {
            this.imgSrc = params.imgSrc;
        }
        if (params.title !== undefined) {
            this.title = params.title;
        }
        if (params.imgName !== undefined) {
            this.imgName = params.imgName;
        }
        if (params.IsLocalFlag !== undefined) {
            this.IsLocalFlag = params.IsLocalFlag;
        }
    }
    updateStateVars(params) {
    }
    purgeVariableDependenciesOnElmtId(rmElmtId) {
        this.__message.purgeDependencyOnElmtId(rmElmtId);
        this.__imgSrc.purgeDependencyOnElmtId(rmElmtId);
        this.__title.purgeDependencyOnElmtId(rmElmtId);
        this.__imgName.purgeDependencyOnElmtId(rmElmtId);
        this.__IsLocalFlag.purgeDependencyOnElmtId(rmElmtId);
    }
    aboutToBeDeleted() {
        this.__message.aboutToBeDeleted();
        this.__imgSrc.aboutToBeDeleted();
        this.__title.aboutToBeDeleted();
        this.__imgName.aboutToBeDeleted();
        this.__IsLocalFlag.aboutToBeDeleted();
        SubscriberManager.Get().delete(this.id__());
        this.aboutToBeDeletedInternal();
    }
    get message() {
        return this.__message.get();
    }
    set message(newValue) {
        this.__message.set(newValue);
    }
    get imgSrc() {
        return this.__imgSrc.get();
    }
    set imgSrc(newValue) {
        this.__imgSrc.set(newValue);
    }
    get title() {
        return this.__title.get();
    }
    set title(newValue) {
        this.__title.set(newValue);
    }
    get imgName() {
        return this.__imgName.get();
    }
    set imgName(newValue) {
        this.__imgName.set(newValue);
    }
    get IsLocalFlag() {
        return this.__IsLocalFlag.get();
    }
    set IsLocalFlag(newValue) {
        this.__IsLocalFlag.set(newValue);
    }
    async selectImage() {
        try {
            let PhotoSelectOptions = new picker.PhotoSelectOptions();
            PhotoSelectOptions.MIMEType = picker.PhotoViewMIMETypes.IMAGE_TYPE;
            PhotoSelectOptions.maxSelectNumber = 1;
            let photoPicker = new picker.PhotoViewPicker();
            photoPicker.select(PhotoSelectOptions).then((PhotoSelectResult) => {
                console.info('app PhotoViewPicker.select successfully, PhotoSelectResult uri: ' + JSON.stringify(PhotoSelectResult));
                // 从图库选择图片后，返回图片uri
                let uri = PhotoSelectResult.photoUris[0];
                console.info('app uri:' + uri);
                this.imgSrc = uri;
                this.IsLocalFlag = false;
            }).catch((err) => {
                console.error('app PhotoViewPicker.select failed with err: ' + err);
            });
        }
        catch (err) {
            console.error('app PhotoViewPicker failed with err: ' + err);
        }
    }
    uploadImage() {
        // 获取应用文件路径
        let context = getContext(this);
        let cacheDir = context.cacheDir;
        // 读取上面返回uri
        let imgName = this.imgSrc.split('/').pop() + '.jpg';
        let dstPath = cacheDir + '/' + imgName;
        let srcFile = fs.openSync(this.imgSrc);
        let dstFile = fs.openSync(dstPath, fs.OpenMode.READ_WRITE | fs.OpenMode.CREATE);
        fs.copyFileSync(srcFile.fd, dstFile.fd);
        fs.closeSync(srcFile);
        fs.closeSync(dstFile);
        let Config = {
            url: 'http://192.168.137.99:8080/predict',
            header: {
                'Content-Type': 'multipart/form-data',
            },
            method: http.RequestMethod.POST,
            files: [{
                    filename: imgName,
                    name: 'image',
                    uri: 'internal://cache/' + imgName,
                    type: 'jpg'
                }],
            data: [{
                    name: "fid",
                    value: '123456'
                }]
        };
        try {
            request.uploadFile(context, Config)
                .then((uploadTask) => {
                uploadTask.on('complete', (taskStates) => {
                    for (let i = 0; i < taskStates.length; i++) {
                        console.info(`app upload complete taskState: ${JSON.stringify(taskStates[i])}`);
                        AlertDialog.show({ message: '传输已完成' });
                        this.title = "推理结果如下:";
                        this.message = "world_cup";
                    }
                });
                uploadTask.on('fail', (taskStates) => {
                    for (let i = 0; i < taskStates.length; i++) {
                        console.info(`app upload failed taskState: ${JSON.stringify(taskStates[i])}`);
                    }
                });
                uploadTask.on('progress', (uploadedSize, totalSize) => {
                    this.title = ("传输中" + uploadedSize + "/" + totalSize);
                    console.info("app upload totalSize:" + totalSize + "  uploadedSize:" + uploadedSize);
                });
            }), (err, data) => {
                if (!err) {
                    // data.result为HTTP响应内容，可根据业务需要进行解析
                    console.info('xx Result:' + JSON.stringify(data.result));
                    console.info('xx code:' + JSON.stringify(data.responseCode));
                    // data.header为HTTP响应头，可根据业务需要进行解析
                    console.info('xx header:' + JSON.stringify(data.header));
                    console.info('xx cookies:' + JSON.stringify(data.cookies)); // 8+
                }
                else {
                    console.error(`app Invoke uploadFile failed, code is ${err.code}, message is ${err.message}`);
                }
            };
        }
        catch (err) {
            console.error(`app Invoke uploadFile failed, code is ${err.code}, message is ${err.message}`);
        }
    }
    initialRender() {
        this.observeComponentCreation((elmtId, isInitialRender) => {
            ViewStackProcessor.StartGetAccessRecordingFor(elmtId);
            Row.create();
            Row.height('100%');
            if (!isInitialRender) {
                Row.pop();
            }
            ViewStackProcessor.StopGetAccessRecording();
        });
        this.observeComponentCreation((elmtId, isInitialRender) => {
            ViewStackProcessor.StartGetAccessRecordingFor(elmtId);
            Column.create();
            Column.width('100%');
            if (!isInitialRender) {
                Column.pop();
            }
            ViewStackProcessor.StopGetAccessRecording();
        });
        this.observeComponentCreation((elmtId, isInitialRender) => {
            ViewStackProcessor.StartGetAccessRecordingFor(elmtId);
            Text.create("打开相册");
            Text.fontSize(50);
            Text.width(100 + "%");
            Text.height("100");
            Text.textAlign(TextAlign.Center);
            Text.fontWeight(FontWeight.Bold);
            Text.backgroundColor("#F0e68c");
            Text.onClick(() => {
                this.selectImage();
            });
            if (!isInitialRender) {
                Text.pop();
            }
            ViewStackProcessor.StopGetAccessRecording();
        });
        Text.pop();
        this.observeComponentCreation((elmtId, isInitialRender) => {
            ViewStackProcessor.StartGetAccessRecordingFor(elmtId);
            Text.create("发送照片");
            Text.fontSize(50);
            Text.width(100 + "%");
            Text.height("100");
            Text.textAlign(TextAlign.Center);
            Text.fontWeight(FontWeight.Bold);
            Text.onClick(() => {
                this.uploadImage();
            });
            Text.backgroundColor("#FFebcd");
            if (!isInitialRender) {
                Text.pop();
            }
            ViewStackProcessor.StopGetAccessRecording();
        });
        Text.pop();
        this.observeComponentCreation((elmtId, isInitialRender) => {
            ViewStackProcessor.StartGetAccessRecordingFor(elmtId);
            If.create();
            if (this.IsLocalFlag) {
                this.ifElseBranchUpdateFunction(0, () => {
                    this.observeComponentCreation((elmtId, isInitialRender) => {
                        ViewStackProcessor.StartGetAccessRecordingFor(elmtId);
                        Image.create({ "id": 16777222, "type": 20000, params: [], "bundleName": "com.example.app", "moduleName": "entry" });
                        Image.width(300);
                        Image.height(300);
                        if (!isInitialRender) {
                            Image.pop();
                        }
                        ViewStackProcessor.StopGetAccessRecording();
                    });
                });
            }
            else {
                this.ifElseBranchUpdateFunction(1, () => {
                    this.observeComponentCreation((elmtId, isInitialRender) => {
                        ViewStackProcessor.StartGetAccessRecordingFor(elmtId);
                        Image.create(this.imgSrc);
                        Image.width("80%");
                        Image.height("40%");
                        if (!isInitialRender) {
                            Image.pop();
                        }
                        ViewStackProcessor.StopGetAccessRecording();
                    });
                });
            }
            if (!isInitialRender) {
                If.pop();
            }
            ViewStackProcessor.StopGetAccessRecording();
        });
        If.pop();
        this.observeComponentCreation((elmtId, isInitialRender) => {
            ViewStackProcessor.StartGetAccessRecordingFor(elmtId);
            Text.create(this.title);
            Text.fontSize(30);
            Text.width(100 + "%");
            Text.height("100");
            Text.textAlign(TextAlign.Center);
            Text.fontWeight(FontWeight.Bold);
            Text.backgroundColor("#00ccFF ");
            if (!isInitialRender) {
                Text.pop();
            }
            ViewStackProcessor.StopGetAccessRecording();
        });
        Text.pop();
        this.observeComponentCreation((elmtId, isInitialRender) => {
            ViewStackProcessor.StartGetAccessRecordingFor(elmtId);
            Text.create(this.message);
            Text.fontSize(30);
            Text.width('100%');
            Text.textAlign(TextAlign.Center);
            Text.fontWeight(FontWeight.Bold);
            if (!isInitialRender) {
                Text.pop();
            }
            ViewStackProcessor.StopGetAccessRecording();
        });
        Text.pop();
        Column.pop();
        Row.pop();
    }
    rerender() {
        this.updateDirtyElements();
    }
}
ViewStackProcessor.StartGetAccessRecordingFor(ViewStackProcessor.AllocateNewElmetIdForNextComponent());
loadDocument(new Index(undefined, {}));
ViewStackProcessor.StopGetAccessRecording();
//# sourceMappingURL=Index.js.map