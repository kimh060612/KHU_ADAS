#include <iostream>
#include <opencv2/opencv.hpp>
#include <vector>
#include <cmath>
#define PI 3.1415926535

using namespace std;
using namespace cv;

int main()
{
    VideoCapture Video("./TestMovie.mp4");
    Mat frame;
    namedWindow("Original", 1);
    namedWindow("Cropped", 1);
    if (!Video.isOpened())
    {
        cout << "Not Video Available" << endl;
        exit(0);
    }
    
    const int Height = Video.get(cv::CAP_PROP_FRAME_HEIGHT);
    const int Width = Video.get(cv::CAP_PROP_FRAME_WIDTH);
    int half_h = (int)(Height / 2);
    Rect bound(0, 0, Width, Height);
    Rect half(0, (int)(Height / 2), Width, Height);
    cout << half << endl;
    while (1)
    {
        Video >> frame;
        if (frame.empty())break;
        Mat gray_frame;
        gray_frame = frame( half & bound );
        cvtColor(gray_frame, gray_frame, CV_BGR2GRAY);
        imshow("Cropped", gray_frame);
        Mat dst;
        Canny(gray_frame, dst, 60, 150);
        //cvtColor(dst, color_dst, CV_GRAY2BGR);
        vector<Vec4i> lines;
        HoughLinesP(dst, lines, 1, CV_PI/180, 80, 30, 10);
        for (size_t i = 0; i < lines.size(); i++)
        {
            // 각도에 따라 거르는 용도인데 일단 놔두자. 
            // double angle = (atan2((lines[i][3] - lines[i][1]),(lines[i][2] - lines[i][1])) * 180 / PI) + 90;
            // if ((angle > 10 && angle < 85) || ( angle > 92 && angle < 178)) 
            line(frame, Point(lines[i][0], lines[i][1] + half_h), Point(lines[i][2], lines[i][3] + half_h), Scalar(0,0,255), 3, 2);
        }
        // 원래는 Adaptive ROI를 적용하려고 했지만 생각을 곰곰히 해보니 이미 충분히 느리다. 
        // 우리는 주어진 환경이 있기에 굳이 이것이 필요하지는 않을 것 같다.
        imshow("Original", frame);
        if (waitKey(30)==27)break;
    }

    cv::destroyWindow("Original");
    cv::destroyWindow("Cropped");
    return 0;
}