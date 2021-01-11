#include <iostream>
#include <vector>
#include <opencv2/opencv.hpp>
#include <fstream>

using namespace std;
using namespace cv;

Point2f pts2[4];

void InitializeTrackBars(vector<int> &Initpts, int W, int H);
vector<Point2f> valTrackBars(int W, int H);
void Wrap(Mat Img, Mat &result, vector<Point2f> &pts, int W, int H, bool inv);
void DrawPoints(Mat &img, vector<Point2f> pts);


int main()
{
    VideoCapture Video("TestMovie.mp4");
    ofstream fout("BirdEyePoints");
    if (!Video.isOpened())
    {
        cout << "Not Valid Image!" << endl;
        exit(0);
    }

    namedWindow("OriginalVid");
    namedWindow("Cropped");

    Mat frame;
    const int H = Video.get(CAP_PROP_FRAME_HEIGHT);
    const int W = Video.get(CAP_PROP_FRAME_WIDTH);
    int half_h = (int)(H / 2);
    pts2[0] = Point2f(0,0);
    pts2[1] = Point2f(W,0);
    pts2[2] = Point2f(0,H);
    pts2[3] = Point2f(W,H);
    vector<int> init({537, 603, 72, 870});
    InitializeTrackBars(init, W, H);
    while (1)
    {
        Video >> frame;
        if (frame.empty()) break;

        Mat result, gray_frame;
        Mat dst;
        vector<Point2f> v;
        v = valTrackBars(W, H);
        Wrap(frame, result, v, W, H, false);
        DrawPoints(frame, v);
        imshow("OriginalVid", frame);
        //imshow("Cropped", result);

        cvtColor(result, gray_frame, CV_BGR2GRAY);
        adaptiveThreshold(gray_frame, gray_frame, 255, ADAPTIVE_THRESH_MEAN_C, THRESH_BINARY, 21, 7);
        Canny(gray_frame, dst, 60, 150);
        
        //cvtColor(dst, color_dst, CV_GRAY2BGR);
        vector<Vec4i> lines;
        HoughLinesP(dst, lines, 1, CV_PI/180, 80, 30, 10);
        for (size_t i = 0; i < lines.size(); i++)
        {
            // 각도에 따라 거르는 용도인데 일단 놔두자. 
            // double angle = (atan2((lines[i][3] - lines[i][1]),(lines[i][2] - lines[i][1])) * 180 / PI) + 90;
            // if ((angle > 10 && angle < 85) || ( angle > 92 && angle < 178)) 
            line(result, Point(lines[i][0], lines[i][1]), Point(lines[i][2], lines[i][3]), Scalar(0,0,255), 3, 2);
        }
        imshow("Cropped", result);
        if (waitKey(30)==27)break;
    }
    
    destroyAllWindows();
    return 0;
}

void Wrap(Mat Img, Mat &result, vector<Point2f> &pts, int W, int H, bool inv)
{
    Point2f pts1[4];
    for (int i = 0; i < 4; i++)
    {
        pts1[i] = pts[i];
    }
    Mat Perspective;
    if (inv)Perspective = getPerspectiveTransform(pts2, pts1);
    else Perspective = getPerspectiveTransform(pts1, pts2);

    warpPerspective(Img, result, Perspective, Size(W, H));
}

void InitializeTrackBars(vector<int> &Initpts, int W, int H)
{
    namedWindow("TrackBars");
    resizeWindow("TrackBars", 360, 240);
    createTrackbar("Width Top", "TrackBars", &Initpts[0], (int)(W/2));
    createTrackbar("Height Top", "TrackBars", &Initpts[1], H);
    createTrackbar("Width Bottom", "TrackBars", &Initpts[2], (int)(W/2));
    createTrackbar("Height Bottom", "TrackBars", &Initpts[3], H);
}

vector<Point2f> valTrackBars(int W, int H)
{
    vector<Point2f> result;
    int WidthTop = getTrackbarPos("Width Top", "TrackBars");
    int HeigthTop = getTrackbarPos("Height Top", "TrackBars");
    int WidthBottom = getTrackbarPos("Width Bottom", "TrackBars");
    int HeightBottom = getTrackbarPos("Height Bottom", "TrackBars");

    result.push_back(Point2f((float)WidthTop, (float)HeigthTop));
    result.push_back(Point2f((float)(W - WidthTop), (float)HeigthTop));
    result.push_back(Point2f((float)WidthBottom, (float)HeightBottom));
    result.push_back(Point2f((float)(W - WidthBottom), (float)HeightBottom));

    return result;
}

void DrawPoints(Mat &img, vector<Point2f> pts)
{
    for (int i = 0; i < 4; i++)
    {
        circle(img, Point2i(pts[i]), 30, Scalar(0,255,0), cv::FILLED);
    }
}
