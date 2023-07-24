#include "widget.h"
#include "ui_widget.h"

Widget::Widget(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::Widget)
{
    mPlayer = new VideoPlayer;
    ui->setupUi(this);
    readconfig();
    initWidget();
    initconnect();

    std::string str = "./yolov7-tiny-nms.trt";
    char* chr = const_cast<char*>(str.c_str());
    myyolo = new Yolo(chr);

    mot_tracker = new sort(3, 3, 0.25);
}

Widget::~Widget()
{
    delete ui;
}


void Widget::readvideo(){
    cv::VideoCapture cap("./video.mp4");
    cv::Mat frame;

    //outputVideo.open('./result,mp4', cv::FOURCC('P','I','M','1'), 20.0, S, true);
    cap>>frame;

    //encode_type = cv::VideoWriter::fourcc('M', 'J', 'P', 'G');
    //outputVideo.open("./result.mp4", encode_type, 25, frame.size(), true);
    while (1) {
        // 把读取的摄像头传入Mat对象中
        cap >> frame;
        // 判断是否成功
        if (frame.empty()) {
            break;
        }
        // 把每一帧图片表示出来
        QImage img = Mat2QImage(frame);
        emit sig_fame(img);
        //imshow("ImageShow", frame);
        cv::waitKey(20);
        }
  frame.release();

  //outputVideo.release();


}
void Widget::readconfig(){
    QSettings settingsread("./config.ini",QSettings::IniFormat);
    rtspaddr = settingsread.value("SetUpOption/camerartsp").toString();
}

void Widget::initWidget(){
    qDebug()<<rtspaddr;
    ui->le_rtstspaddr->setText(rtspaddr);
    //colors = new c(32);



    srand(time(0));
    for (size_t i = 0; i < 100; i++)
        colors.push_back(cv::Vec3b(rand() % 255, rand() % 255, rand() % 255));

        //colors[i] = cv::Vec3b(rand() % 255, rand() % 255, rand() % 255);

}

void Widget::slot_open_or_close(){
    if(ui->btn_openorclose->text()=="open"){
        ui->btn_openorclose->setText("close");
        mPlayer->startPlay();
        //readvideo();
    }else{
        ui->btn_openorclose->setText("open");
        //mPlayer->stopPlay();
    }
}

void Widget::initconnect(){
    connect(ui->btn_openorclose,&QPushButton::clicked,this,&Widget::slot_open_or_close);
    connect(mPlayer,SIGNAL(sig_GetOneFrame(QImage)),this,SLOT(slotGetOneFrame(QImage)));
    connect(this,&Widget::sig_fame,this,&Widget::slotGetOneFrame);
    //connect(mPlayer,&VideoPlayer::SigFinished, mPlayer,&VideoPlayer::deleteLater);//自动释放
}


void Widget::slotGetOneFrame(QImage img)
{

    Mat tempmat = QImage2Mat(img);
    if (tempmat.empty()) {
       printf("null img\n");
    }else {
       //printf("get a img\n");
       //imshow("ImageShow", tempmat);
       //cv::waitKey(1);
       float* Boxes = new float[400];
       int* BboxNum = new int[1];
       int* ClassIndexs = new int[100];
       float* Scores = new float[100];

       float conf_thres = 0.25;
       std::vector<int> classes;
       //classes.push_back(1);
       classes.push_back(0);
       // run inference
       auto start = std::chrono::system_clock::now();
       //myyolo->Infer(tempmat.cols, tempmat.rows, tempmat.channels(), tempmat.data, Boxes, ClassIndexs, BboxNum,Scores);

       myyolo->Infer(tempmat.cols, tempmat.rows, tempmat.channels(), tempmat.data, Boxes, ClassIndexs, BboxNum,Scores);
       auto end = std::chrono::system_clock::now();
       //std::cout << std::chrono::duration_cast<std::chrono::milliseconds>(end - start).count() << "ms" << std::endl;

       //float* newBoxes; //= new float[400];
       //int* newBboxNum; //= new int[1];
       //int* newClassIndexs; //= new int[100];
       //float* newScores;// = new float[100];

       float newBoxes[400]; //= new float[400];
       int newBboxNum[1]; //= new int[1];
       int newClassIndexs[100]; //= new int[100];
       float newScores[100];// = new float[100];


       filter(Boxes, ClassIndexs, BboxNum,Scores,0.2,classes,newBoxes, newClassIndexs, newBboxNum,newScores);

       //cout<<newBoxes[0]<<endl;
       //cout<<newClassIndexs[0];



       //myyolo->draw_objects(tempmat, newBoxes, newClassIndexs, newBboxNum, newScores);

       //std::vector<cv::Rect> detections = Boxes;


       std::vector<cv::Rect> detections;

       for (int i =0; i< newBboxNum[0]; i++){

           cv::Rect rect(newBoxes[i * 4], newBoxes[i * 4 + 1], newBoxes[i * 4 + 2], newBoxes[i * 4 + 3]);
           //cv::rectangle(tempmat, rect, cv::Scalar(0x27, 0xC1, 0x36), 2);
           detections.push_back(rect);
       }

       std::vector<std::vector<float>> trackers = mot_tracker->update(detections);
       //qDebug()<<trackers;

       if(!trackers.empty()){
            //std::vector<cv::Vec3b> colors(32);
            //srand(time(0));

            //int size_row = trackers.size();
            //int size_col = trackers[0].size();
            for (auto tracker : trackers)
            {
                cv::rectangle(tempmat, cv::Rect(tracker[0], tracker[1],
                            tracker[2]- tracker[0], tracker[3]- tracker[1]),
                            colors[int(tracker[4])%100], 2);
                cv::putText(tempmat,std::to_string(int(tracker[4])),
                cv::Point(tracker[0], tracker[1]),
                cv::FONT_HERSHEY_PLAIN,1.5,
                colors[int(tracker[4])%100],
                2);
            }
       }


       QImage outimg = Mat2QImage(tempmat);

       //outputVideo.write(tempmat);

       //imshow("ImageShow", tempmat);

       mImage = outimg;
       QImage imageScale = mImage.scaled(QSize(ui->label->width(), ui->label->height()));
       QPixmap pixmap = QPixmap::fromImage(imageScale);
       ui->label->setPixmap(pixmap);
    }

}

void Widget::initYolo(){

}

cv::Mat Widget::QImage2Mat(QImage image)
{
    Mat mat = Mat::zeros(image.height(), image.width(),image.format()); //初始化Mat
    switch(image.format()) //判断image的类型
    {
            case QImage::QImage::Format_Grayscale8:  //灰度图
                mat = Mat(image.height(), image.width(),
                CV_8UC1,(void*)image.constBits(),image.bytesPerLine());
                break;
            case QImage::Format_RGB888: //3通道彩色
                mat = Mat(image.height(), image.width(),
                    CV_8UC3,(void*)image.constBits(),image.bytesPerLine());
                break;
            case QImage::Format_ARGB32: //4通道彩色
                mat = Mat(image.height(), image.width(),
                    CV_8UC4,(void*)image.constBits(),image.bytesPerLine());
                break;
            case QImage::Format_RGBA8888:
                mat = Mat(image.height(), image.width(),
                    CV_8UC4,(void*)image.constBits(),image.bytesPerLine());
        break;
            default:
                return mat;
    }
    cv::cvtColor(mat, mat, cv::COLOR_BGR2RGB);
    return mat;

}

QImage Widget::Mat2QImage(const cv::Mat &mat)
{
    if(mat.type()==CV_8UC1 || mat.type()==CV_8U)
    {
        QImage image((const uchar *)mat.data, mat.cols, mat.rows, mat.step, QImage::Format_Grayscale8);
        return image;
    }
    else if(mat.type()==CV_8UC3)
    {
        QImage image((const uchar *)mat.data, mat.cols, mat.rows, mat.step, QImage::Format_RGB888);
        return image.rgbSwapped();  //r与b调换
    }
}

void Widget::filter(float* Boxes,int* ClassIndexs,int* BboxNum,float* Scores,
            float conf_thres, std::vector<int> classes,float *newBoxes,
                    int *newClassIndexs,int *newBboxNum,float *newScores){

    int count = 0;

    for (int i =0;i<BboxNum[0];i++) {
        if (Scores[i]>conf_thres){
            std::vector<int>::iterator iter=std::find(classes.begin(),classes.end(),ClassIndexs[i]);
            if ( iter!=classes.end())
            {
                //cout << "Not found" << endl;
                for (int j=0;j<4;j++) {
                    newBoxes[count*4+j]=Boxes[i*4+j];

                    //cout<<newBoxes[count*4+j]<<endl;
                }

                newClassIndexs[count]=ClassIndexs[i];

                newScores[count]=Scores[i];

                count++;
            }
        }

    }

    newBboxNum[0]=count;

}
