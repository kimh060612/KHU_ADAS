#include <iostream>
#include <opencv2/opencv.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>

using namespace std;
using namespace cv;

void RedDetection(Mat original, Mat &result);
void RedDetectionHSV(Mat original, Mat &result);

int main()
{
    cout << "OpenCV Version : " << CV_VERSION << std::endl;
    Mat img;
    namedWindow("Original", 1);
    namedWindow("Threshold1", 1);
    namedWindow("Threshold2", 1);
    VideoCapture cap;
    cap.open(0); // 노트북 카메라는 cap.open(1) 또는 cap.open(-1)
                     // USB 카메라는 cap.open(0);
    while (cap.isOpened())
    {
        cap >> img;
        Mat result, result2;
        // RGB 영역에서 Red 영역 검출
        RedDetection(img, result);
        // HSV 영역에서 Red 영역 검출
        RedDetectionHSV(img, result2);
        imshow("Original", img);
        imshow("Threshold1", result);
        imshow("Threshold2", result2);
        if (waitKey(30)==27)break;
    }
    destroyWindow("Original");
    destroyWindow("Threshold1");
    destroyWindow("Threshold2");
    return 0;
}

unsigned char max(unsigned char a, unsigned char b){ return a > b ? a : b;}

void RedDetection(Mat original, Mat &result)
{
    int row, col;
    row = original.rows;
    col = original.cols;
    // 8 bit unsigned char 1 channel의 row, col 크기의 배열 할당.
    result.create(row, col, CV_8UC1);
    Mat BGR[3];
    // BGR 3개의 채널을 각 행렬로 분리
    split(original, BGR);
    // 각 픽셀별로 접근함.
    for (int i = 0; i < row; i++)
    {
        for (int j = 0; j < col; j++)
        {
            unsigned char RG, RB, tmp;
            // R 영역과 B 영역의 픽셀 값의 차
            RB = BGR[2].at<uchar>(i,j) - BGR[0].at<uchar>(i,j);
            // R 영역과 G 영역의 픽셀 값의 차
            RG = BGR[2].at<uchar>(i,j) - BGR[1].at<uchar>(i,j);
            // 둘중 작은 값 검출
            tmp = max((RG < RB ? RG : RB), 0);
            // 50 <= x <= 120 성능 괜춘
            if (tmp > 60 && tmp <= 150) result.at<uchar>(i,j) = 254;
            else result.at<uchar>(i,j) = 0;
        }
    }
    // 잡음 제거 필터링
    erode(result, result, getStructuringElement(MORPH_ELLIPSE, Size(5, 5)));
	dilate(result, result, getStructuringElement(MORPH_ELLIPSE, Size(5, 5)));
    dilate(result, result, getStructuringElement(MORPH_ELLIPSE, Size(5, 5)));
	erode(result, result, getStructuringElement(MORPH_ELLIPSE, Size(5, 5)));
}

void RedDetectionHSV(Mat original, Mat &result)
{
    // HSV 영역으로 색상 전환
    cvtColor(original, result, COLOR_BGR2HSV);
    Mat mask1,mask2;
    
    // 생상 영역에서의 제한
    inRange(result, Scalar(0, 120, 70), Scalar(10, 255, 255), mask1);
    inRange(result, Scalar(170, 120, 70), Scalar(180, 255, 255), mask2);

    // OR 연산으로 두 영역의 합영역 영상을 구함
    mask1 |= mask2;

    // 잡음 제거 필터링
    erode(mask1, mask1, getStructuringElement(MORPH_ELLIPSE, Size(5, 5)));
	dilate(mask1, mask1, getStructuringElement(MORPH_ELLIPSE, Size(5, 5)));
    dilate(mask1, mask1, getStructuringElement(MORPH_ELLIPSE, Size(5, 5)));
	erode(mask1, mask1, getStructuringElement(MORPH_ELLIPSE, Size(5, 5)));

    result = mask1;
}