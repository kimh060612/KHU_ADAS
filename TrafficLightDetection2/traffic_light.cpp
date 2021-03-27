#include <opencv2/imgcodecs.hpp>
#include <opencv2/videoio.hpp>
#include <opencv2/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <algorithm>

#define RED 0
#define YELLOW 1
#define LEFT 2
#define RIGHT 3
#include <iostream>
#include <stdio.h>

using namespace cv;
using namespace std;

bool cmp(Point &a, Point &b) {return (a.x < b.x);}

int main(int ac, char** av) {

	//VideoCapture cap("movie.mp4");
	VideoCapture cap(0);

	if (!cap.isOpened())
	{
		printf("Can't open the camera");
		return -1;
	}

	Mat img, gray, img_hsv;
	char light_value[4] = {0,0,0,0};
	char flag = RED;
	Mat rgb;

	while (cap.isOpened())
	{
		cap >> img;

		cvtColor(img, gray, COLOR_BGR2GRAY);
		medianBlur(gray, gray, 5);

		vector<Vec3f> circles;
		vector<Point> centers;

		HoughCircles(gray, circles, CV_HOUGH_GRADIENT, 1, gray.rows / 16, 100, 30, 1, 100);

		int order, x_min_i;

		cout << circles.size();

		for (size_t i = 0; i < circles.size(); i++) {
			Vec3i c = circles[i];
			centers.push_back(Point(c[0], c[1]));
			int radius = c[2];

			circle(img, centers[i], radius, Scalar(0, 255, 0), 2);
			circle(img, centers[i], 2, Scalar(0, 0, 255), 3);
		}

		sort(centers.begin(), centers.end(), cmp);
		
		// HSV 3개의 채널을 각 행렬로 분리

		if (centers.size() == 4) {//신호등 4개만 잘 잡았을 때
			for (int i = 0; i < 4; i++) {

				Mat image = img.clone();

				Mat HSV;
				Mat RGB = image(Rect(centers[i].x, centers[i].y, 1, 1));
				cvtColor(RGB, HSV, COLOR_BGR2HSV);

				Vec3b hsv = HSV.at<Vec3b>(0, 0);
				int H = hsv.val[0];
				int S = hsv.val[1];
				int V = hsv.val[2];

				cout << "이전 값:" << V << endl;
				cout << "현재 값:" << light_value[i] << endl;
				if (V - light_value[i] > 50) {
					flag = i;
					cout << "플래그 설정됨" << i;
				}
				light_value[i] = V;
				putText(img, to_string(i), centers[i], 0, 2, Scalar(0, 0, 0));
			}
		}

		switch (flag)
		{
		case RED:
			putText(img, "RED", Point(30, 30), 0, 2, Scalar(0, 0, 0));
			break;
		case YELLOW:
			putText(img, "YELLOW", Point(30, 30), 0, 2, Scalar(0, 0, 0));
			break;
		case LEFT:
			putText(img, "LEFT", Point(30, 30), 0, 2, Scalar(0, 0, 0));
			break;
		case RIGHT:
			putText(img, "RIGHT", Point(30, 30), 0, 2, Scalar(0, 0, 0));
			break;
		default:
			break;
		}

		imshow("camera img", img);

		if (waitKey(1) == 27)
			break;
	}


	return 0;
}

