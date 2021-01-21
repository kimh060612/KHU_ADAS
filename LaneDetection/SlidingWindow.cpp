#include <iostream>
#include <opencv2/opencv.hpp>

using namespace std;
using namespace cv;

void InitializeTrackBars(vector<int> &Initpts, int W, int H);
vector<Point2f> valTrackBars(int W, int H);
void Wrap(Mat Img, Mat &result, vector<Point2f> &pts, int W, int H, bool inv);
void DrawPoints(Mat &img, vector<Point2f> pts);
void CurveFitting(vector<Point2d> Data, vector<double> &coeff);
void GetPointsFromImage(Mat &SourceImg, vector<Point2d> &PointData_right, vector<Point2d> &PointData_left, int H, int W, int q_y);
void GetPolynomialPointAll(vector<double> coeff, int H, int qu_y, vector<Point2d> &pts);

Point2f pts2[4];

int main()
{

    namedWindow("OriginalVid");
    namedWindow("Cropped");
    namedWindow("Binary");

    Mat Image;
    Image = imread("Lane.jpeg");
    if (Image.empty())
    {
        cout << "Not Valid Image!" << endl;
        exit(0);
    }

    const int H = Image.rows;
    const int W = Image.cols;
    vector<int> init({340, 366, 31, 504});
    pts2[0] = Point2f(0,0);
    pts2[1] = Point2f(W,0);
    pts2[2] = Point2f(0,H);
    pts2[3] = Point2f(W,H);
    InitializeTrackBars(init, W, H);

    Mat result, gray_frame;
    Mat dst;
    const Size kernelSize = Size(9, 9);
    Mat kernel = Mat::ones(15, 15, CV_8U);
    vector<Point2f> v;
    while (1)
    {
        Image = imread("Lane.jpeg");
        if (Image.empty())break;
        v = valTrackBars(W, H);
        Wrap(Image, result, v, W, H, false);
        DrawPoints(Image, v);

        Mat CVTImage, CVCOLORImg;
        cvtColor(result, CVTImage, CV_BGR2GRAY);
        threshold(CVTImage, CVTImage, 150, 255.,THRESH_OTSU);
        GaussianBlur(CVTImage, CVTImage, kernelSize, 0);
        dilate(CVTImage, CVTImage, kernel);
        erode(CVTImage, CVTImage, kernel);
        morphologyEx(CVTImage, CVTImage, MORPH_CLOSE, kernel);
        cvtColor(CVTImage, CVCOLORImg, CV_GRAY2BGR);

        vector<Point2d> LeftPoint;
        vector<Point2d> RightPoint;
        vector<double> Lcoeff, Rcoeff;
        GetPointsFromImage(CVTImage, RightPoint, LeftPoint, H, W, 5);
        CurveFitting(RightPoint, Rcoeff);
        CurveFitting(LeftPoint, Lcoeff);
        
        vector<Point2d> LCurvePts;
        vector<Point2d> RCurvePts;
        GetPolynomialPointAll(Rcoeff, H, 5, RCurvePts);
        GetPolynomialPointAll(Lcoeff, H, 5, LCurvePts);
        
        fillPoly(CVCOLORImg, RightPoint, Scalar(0, 255, 255));
        fillPoly(CVCOLORImg, LeftPoint, Scalar(0, 255, 255));

        imshow("OriginalVid", Image);
        imshow("Cropped", result);
        imshow("Binary", CVCOLORImg);
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
        circle(img, Point2i(pts[i]), 10, Scalar(0,255,0), cv::FILLED);
    }
}

void CurveFitting(vector<Point2d> Data, vector<double> &coeff)
{
    int N;
    N = Data.size();

    Mat A = Mat::zeros(N, 3, CV_32FC1);
    Mat B = Mat::zeros(N, 1, CV_32FC1);

    for (int i = 0; i < N; i++)
    {
        A.at<float>(i, 0) = 1;
        A.at<float>(i, 1) = Data[i].y;
        A.at<float>(i, 2) = Data[i].y * Data[i].y;
        B.at<float>(i, 0) = Data[i].x;
    }
    Mat Penrose_A = (A.t() * A).inv() * A.t();
    Mat res = Penrose_A * B;
    cout << res << endl;
    coeff.resize(3);
    for (int i = 0; i < 3; i++) coeff[i] = res.at<uchar>(i, 0);
}

void GetPointsFromImage(Mat &SourceImg, vector<Point2d> &PointData_right, vector<Point2d> &PointData_left, int H, int W, int q_y)
{
    if (SourceImg.channels() > 1)
    {
        cout << "Not Valid Image for this function" << endl;
        return;
    }
    for (int y = 0; y < H; y += q_y)
    {
        int mid_point = (int)(W/2);
        int num_pix_X_left = 0, num_pix_X_right = 0;
        double X_center_left = 0., X_center_right = 0.; 
        for (int x = 0; x < mid_point; x++)
        {
            if (SourceImg.at<uchar>(y,x) > 125)
            {
                num_pix_X_left++;
                X_center_left += x;
            }
        }
        if (num_pix_X_left != 0)
        {
            X_center_left /= (double)num_pix_X_left;
            PointData_left.push_back(Point2d(X_center_left, (double)y));
        }
        
        for (int x = mid_point; x < W; x++)
        {
            if (SourceImg.at<uchar>(y,x) > 125)
            {
                num_pix_X_right++;
                X_center_right += x;
            }
        }
        if (X_center_right != 0)
        {
            X_center_right /= num_pix_X_right;
            PointData_right.push_back(Point2d(X_center_right, (double)y));
        }
        
    }
}

void GetPolynomialPointAll(vector<double> coeff, int H, int qu_y, vector<Point2d> &pts)
{
    for (int y = 0; y < H; y += qu_y)
    {
        double x = coeff[0] + coeff[1] * y + coeff[2] * y * y;
        pts.push_back(Point2d(x, (double)y));
    }
}