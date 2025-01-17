#include <jni.h>
#include <opencv2/opencv.hpp>
#include <android/log.h>

using namespace cv;
using namespace std;

float resize_img(Mat img_src, Mat &img_resize, int resize_width) {
    float scale = resize_width / (float)img_src.cols;

    if(img_src.cols > resize_width) {
        int new_height = cvRound(img_src.rows * scale);
        resize(img_src, img_resize, Size(resize_width, new_height));
    }
    else{
        img_resize = img_src;
    }
    return scale;
}

extern "C"
JNIEXPORT jlong JNICALL
Java_com_boribap_forkids_MainActivity_loadCascade(JNIEnv *env, jobject instance,
                                                          jstring cascadeFileName_) {
    //const char *cascadeFileName = env->GetStringUTFChars(cascadeFileName_, 0);
    const char *nativeFileNameString = env ->GetStringUTFChars(cascadeFileName_, 0);

    string baseDir("/storage/emulated/0/");
    baseDir.append(nativeFileNameString);
    const char *pathDir = baseDir.c_str();

    jlong ret = 0;
    ret = (jlong) new CascadeClassifier(pathDir);
    if(((CascadeClassifier *) ret)->empty()) {
    }
    else {
    }
    env->ReleaseStringUTFChars(cascadeFileName_, nativeFileNameString);

    return ret;
    // TODO

    //env->ReleaseStringUTFChars(cascadeFileName_, cascadeFileName);
}extern "C"
JNIEXPORT void JNICALL
Java_com_boribap_forkids_MainActivity_detect(JNIEnv *env, jobject instance,
                                                     jlong cascadeClassifier_face,
                                                     jlong matAddrInput, jlong matAddrResult, jlong matAddrTensor) {

    // TODO
    Mat &img_input = *(Mat *) matAddrInput;
    Mat &img_result = *(Mat *) matAddrResult;
    Mat &img_tensor = *(Mat *) matAddrTensor;

    img_result = img_input.clone();

    std::vector<Rect> faces;
    Mat img_gray;

    cvtColor(img_input, img_gray, COLOR_BGR2GRAY);
    equalizeHist(img_gray, img_gray);

    Mat img_resize;
    float resizeRatio = resize_img(img_gray, img_resize, 640);

    ((CascadeClassifier *) cascadeClassifier_face)->detectMultiScale(img_resize, faces, 1.1, 2, 0|CASCADE_SCALE_IMAGE, Size(30,30));


    for(int i=0;i<faces.size();i++){
        double real_facesize_x = faces[i].x / resizeRatio;
        double real_facesize_y = faces[i].y / resizeRatio;
        double real_facesize_width = faces[i].width / resizeRatio;
        double real_facesize_height = faces[i].height / resizeRatio;

        Point center(real_facesize_x + real_facesize_width/2 , real_facesize_y + real_facesize_height/2);
        //ellipse(img_result, center, Size(real_facesize_width/2, real_facesize_height/2), 0, 0, 360, Scalar(255,0,255), 30, 8, 0);

        Rect face_area(real_facesize_x, real_facesize_y, real_facesize_width,real_facesize_height);

        rectangle(img_result, face_area, Scalar(255,0,255), 4, 8, 0);
        putText(img_result, to_string(real_facesize_width), center, 2, 3, Scalar(255,0,255), 2, 8, false );

        Mat face_rect = img_input(face_area);
        resize(face_rect, face_rect, Size(48,48));
        img_tensor = face_rect;
    }
}