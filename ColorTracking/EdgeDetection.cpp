#include <iostream>
#include <opencv2/opencv.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>

using namespace std;
using namespace cv;

int main()
{
    cout << "OpenCV Version : " << CV_VERSION << std::endl;
    Mat img;
    namedWindow("Original", 1);
    namedWindow("Threshold", 1);
    namedWindow("NoThreshold", 1);
    VideoCapture cap;
    cap.open(0); // 노트북 카메라는 cap.open(1) 또는 cap.open(-1)
                     // USB 카메라는 cap.open(0);
    while (cap.isOpened())
    {
        cap >> img;
        Mat img_edge;
        Mat gray_frame;
        cvtColor(img, gray_frame, CV_BGR2GRAY);
        Mat result_frame;
        //    1. gray scale 영상 2. 결과 영상 3. 최댓값 4. 주변 픽셀의 계산 방식 5. threshold 방식 6. 합칠 block 크기 7. 그냥 상수
        adaptiveThreshold(gray_frame, result_frame, 255, ADAPTIVE_THRESH_MEAN_C, THRESH_BINARY, 21, 7);
        Canny(result_frame, result_frame, 60, 150);
        imshow("Threshold", result_frame);
        
        Canny(img, img_edge, 60, 150);
        imshow("NoThreshold", img_edge);
        
        imshow("Original", img);
        if (waitKey(30)==27)break;
    }
    destroyWindow("Threshold");
    destroyWindow("NoThreshold");
    destroyWindow("Original");
    return 0;
}

