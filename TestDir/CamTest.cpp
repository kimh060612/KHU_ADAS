#include <iostream>
#include <opencv2/opencv.hpp>
 
int main()
{
    std::cout << "OpenCV Version : " << CV_VERSION << std::endl;
    cv::Mat img;
    cv::namedWindow("EXAMPLE02", 1);
        cv::VideoCapture cap;
 
    cap.open(0); // 노트북 카메라는 cap.open(1) 또는 cap.open(-1)
                     // USB 카메라는 cap.open(0);
    while (cap.isOpened())
    {
        cap >> img;
        cv::imshow("EXAMPLE02", img);
        if (cv::waitKey(30)==27)
        {
            break;
        }
    }
    cv::destroyWindow("EXAMPLE02");
    return 0;
}