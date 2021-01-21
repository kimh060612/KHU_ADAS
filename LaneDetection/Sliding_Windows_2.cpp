#include <iostream>
#include <vector>
#include <opencv2/opencv.hpp>

using namespace std;
using namespace cv;

vector<Point2f> slidingWindow(Mat image, Rect window);

int main()
{

    VideoCapture Video("RoadVideo.mp4");
    Mat frame;

    if (!Video.isOpened())
    {
        cout << "Not Valid Image!" << endl;
        exit(0);
    }

    namedWindow("OriginalVid");
    namedWindow("Cropped");
    namedWindow("Binary");

    const int H = Video.get(CAP_PROP_FRAME_HEIGHT);
    const int W = Video.get(CAP_PROP_FRAME_WIDTH);

    Point2f ScrPoints[4];
    Point2f dstPoint[4];
    const Size kernelSize = Size(9, 9);
    Mat kernel = Mat::ones(15, 15, CV_8U);

    ScrPoints[0] = Point2f(W/2 + 104, 515);
    ScrPoints[1] = Point2f(755, 515);
    ScrPoints[2] = Point2f(W/2 + 731, 946);
    ScrPoints[3] = Point2f(365, 946);

    dstPoint[0] = Point2f(0,0);
    dstPoint[1] = Point2f(1280,0);
    dstPoint[2] = Point2f(0,720);
    dstPoint[3] = Point2f(1280,720);
    
    Mat PerspectMatrix = getPerspectiveTransform(ScrPoints, dstPoint);
    Mat dst(720, 1280, CV_8UC3);
    Mat invertedPerspective;
    invert(PerspectMatrix, invertedPerspective);

    while (1)
    {
        Video >> frame;
        if (frame.empty()) break;

        warpPerspective(frame, dst, PerspectMatrix, dst.size(), INTER_LINEAR, BORDER_CONSTANT);
        Mat CVTImage, CVCOLORImg;
        cvtColor(dst, CVTImage, COLOR_RGB2GRAY);
        threshold(CVTImage, CVTImage, 150, 255.,THRESH_OTSU);
        GaussianBlur(CVTImage, CVTImage, kernelSize, 0);
        dilate(CVTImage, CVTImage, kernel);
        erode(CVTImage, CVTImage, kernel);
        morphologyEx(CVTImage, CVTImage, MORPH_CLOSE, kernel);
        cvtColor(CVTImage, CVCOLORImg, CV_GRAY2BGR);

        vector<Point2f> pts = slidingWindow(CVTImage, Rect(0, 630, 120, 60));
        vector<Point> allPts;
        vector<Point2f> outPts;
        perspectiveTransform(pts, outPts, invertedPerspective);
        for (int i = 0; i < outPts.size() - 1; ++i)
        {
            line(frame, Point(outPts[i]), Point(outPts[i + 1]), Scalar(255, 0, 0), 3);
            allPts.push_back(Point(outPts[i].x, outPts[i].y));
        }

        allPts.push_back(Point(outPts[outPts.size() - 1].x, outPts[outPts.size() - 1].y));
        for (int i = 0; i < pts.size() - 1; ++i) //Draw a line on the processed image     
            line(CVCOLORImg, Point(pts[i]), Point(pts[i + 1]), Scalar(255, 0, 0));

        pts = slidingWindow(CVTImage, Rect(W/2, 630, 120, 60));
        perspectiveTransform(pts, outPts, invertedPerspective);

        for (int i = 0; i < outPts.size() - 1; ++i)
        {
            line(frame, Point(outPts[i]), Point(outPts[i + 1]), Scalar(0, 0, 255), 3);
            allPts.push_back(Point(outPts[outPts.size() - i - 1].x, outPts[outPts.size() - i - 1].y));
        }
        allPts.push_back(Point(outPts[0].x - (outPts.size() - 1) , outPts[0].y));

        for (int i = 0; i < pts.size() - 1; ++i)
            line(CVCOLORImg, Point(pts[i]), Point(pts[i + 1]), Scalar(0, 0, 255));

        vector<vector<Point>> arr;
        arr.push_back(allPts);
        Mat overlay = Mat::zeros(frame.size(), frame.type());
        fillPoly(overlay, arr, Scalar(0, 255, 100));
        addWeighted(frame, 1, overlay, 0.5, 0, frame);
        imshow("OriginalVid", frame);
        imshow("Cropped", dst);
        imshow("Binary", CVTImage);
        if (waitKey(30)==27)break;
    }

    Video.release();
    destroyAllWindows();
    return 0;
}

vector<Point2f> slidingWindow(Mat image, Rect window)
{
    vector<Point2f> points;
    const Size imgSize = image.size();
    bool shouldBreak = false;

    while (true)
    {
        float currX = window.x + window.width * 0.5f;
        Mat ROI = image(window);
        vector<Point> locations;

        //int count = countNonZero(ROI);
        
        //if(count < 0)
        //{
        findNonZero(ROI,locations);
        //}

        float avgX = 0.0f;
        for (int i = 0; i < locations.size(); ++i)
        {
            float x = locations[i].x;
            avgX += window.x + x;
        }
        avgX = locations.empty() ? currX : avgX / locations.size();

        Point point(avgX, window.y + window.height * 0.5f);
        points.push_back(point);
        window.y -= window.height;
        if (window.y < 0)
        {
            window.y = 0.;
            shouldBreak = true;
        }

        window.x += (point.x - currX);
        if (window.x < 0) window.x = 0;
        if (window.x + window.width >= imgSize.width) window.x = imgSize.width - window.width - 1;

        if (shouldBreak)break;
    }

    return points;
}