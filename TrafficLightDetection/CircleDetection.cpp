#include <iostream>
#include <opencv2/opencv.hpp>

using namespace std;
using namespace cv;

int main()
{
    cout << "OpenCV Version : " << CV_VERSION << std::endl;
    Mat img;
    namedWindow("Original", 1);
    namedWindow("Circle", 1);
    VideoCapture cap;
    
    cap.open(0); 
    while (cap.isOpened())
    {
        cap >> img;
        Mat gray_img, binary_img, edge_img;
        vector<Vec3f> circles;
        // 색상 영역 변환 BGR -> GRAY
        cvtColor(img, gray_img, COLOR_BGR2GRAY);
        // 영상 이진화 
        threshold(gray_img, binary_img, 200, 255, THRESH_BINARY_INV | THRESH_OTSU);
        blur(gray_img, edge_img, Size(3, 3));
        // 케니 엣지
        Canny(edge_img, edge_img, 125,125*3, 3);
        //허프 Circle 1. 엣지 영상  2. 원 영역 정보를 저장할 벡터 3. 허프 계산 방식 4. 최소 지름 5. 최대 지름 6,7: 민감도(별 쓸모 없음)
        HoughCircles(edge_img, circles, CV_HOUGH_GRADIENT, 2, edge_img.rows/4, 70, 150);
        Mat circle_img = img.clone();
        for (size_t i = 0; i < circles.size(); i++)
        {
            // 객체 설명 참조
            Point center(cvRound(circles[i][0]), cvRound(circles[i][1]));
            int t = cvRound(circles[i][2]);
            circle(circle_img, center, 3, Scalar(0, 255, 0), -1, 8, 0);
            circle(circle_img, center, t, Scalar(0, 255, 0), 3, 8, 0);
        }
        imshow("Circle" ,circle_img);
        imshow("Original", img);

        if(waitKey(30)==27)
        {
            break;
        }
    }
    destroyWindow("Original");
    destroyWindow("Circle");
    return 0;
}