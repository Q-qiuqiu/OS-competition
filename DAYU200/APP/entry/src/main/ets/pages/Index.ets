import picker from '@ohos.file.picker';
import fs from '@ohos.file.fs';
import request from '@ohos.request';
import http from '@ohos.net.http';
@Entry
@Component
struct Index {
  @State message: string = ''
  @State imgSrc: string = ''
  @State title: string = '等待传输'
  @State imgName: string = ''
  @State IsLocalFlag:boolean=true;

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
        console.info('app uri:' + uri)
        this.imgSrc = uri
        this.IsLocalFlag=false
      }).catch((err) => {
       console.error('app PhotoViewPicker.select failed with err: ' + err);
     });
   } catch (err) {
      console.error('app PhotoViewPicker failed with err: ' + err);
    }
  }
  uploadImage() { // 将本地应用文件上传至网络服务器
    // 获取应用文件路径
    let context = getContext(this)
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
              AlertDialog.show({ message: '传输已完成' })
              this.title = "推理结果如下:"
              this.message="world_cup"
            }
          });

          uploadTask.on('fail', (taskStates) => {
            for (let i = 0; i < taskStates.length; i++) {
              console.info(`app upload failed taskState: ${JSON.stringify(taskStates[i])}`);
            }
          })

          uploadTask.on('progress', (uploadedSize, totalSize) => {
            this.title=("传输中"+uploadedSize+"/"+totalSize)
            console.info("app upload totalSize:" + totalSize + "  uploadedSize:" + uploadedSize);
          })
        }), (err, data) => {
        if (!err) {
          // data.result为HTTP响应内容，可根据业务需要进行解析
          console.info('xx Result:' + JSON.stringify(data.result));
          console.info('xx code:' + JSON.stringify(data.responseCode));
          // data.header为HTTP响应头，可根据业务需要进行解析
          console.info('xx header:' + JSON.stringify(data.header));
          console.info('xx cookies:' + JSON.stringify(data.cookies)); // 8+
        } else {
          console.error(`app Invoke uploadFile failed, code is ${err.code}, message is ${err.message}`);
        }
      }
    }
    catch (err)
    {
      console.error(`app Invoke uploadFile failed, code is ${err.code}, message is ${err.message}`);
    }
  }
  build() {
    Row() {
      Column() {
        Text("打开相册")
          .fontSize(50)
          .width(100+"%")
          .height("100")
          .textAlign(TextAlign.Center)
          .fontWeight(FontWeight.Bold)
          .backgroundColor("#F0e68c")
          .onClick(() => {
            this.selectImage()
          })
        Text("发送照片")
          .fontSize(50)
          .width(100+"%")
          .height("100")
          .textAlign(TextAlign.Center)
          .fontWeight(FontWeight.Bold)
          .onClick(() => {
          this.uploadImage()
        })
          .backgroundColor("#FFebcd")
        if(this.IsLocalFlag){
          Image($r('app.media.icon'))
            .width(300)
            .height(300)
        }else {
          Image(this.imgSrc)
            .width("80%")
            .height("40%")
        }
        Text(this.title)
          .fontSize(30)
          .width(100+"%")
          .height("100")
          .textAlign(TextAlign.Center)
          .fontWeight(FontWeight.Bold)
          .backgroundColor("#00ccFF ")
        Text(this.message)
          .fontSize(30)
          .width('100%')
          .textAlign(TextAlign.Center)
          .fontWeight(FontWeight.Bold)
      }
      .width('100%')
    }
    .height('100%')
  }
}
