#include "stdafx.h"
#include "TCommonHelper.h"
using namespace cv;
using namespace std;
//-------------------------------------------------------------------------------------------------
// divide into triengles - used for face swap
void TCommonHelper::swpDivideIntoTriangles(Rect rect, vector<Point2f> &points, vector< std::vector<int> > &Tri)
{

	// Create an instance of Subdiv2D
	Subdiv2D subdiv(rect);
	// Insert points into subdiv
	for (vector<Point2f>::iterator it = points.begin(); it != points.end(); it++)
		subdiv.insert(*it);
	vector<Vec6f> triangleList;
	subdiv.getTriangleList(triangleList);
	vector<Point2f> pt(3);
	vector<int> ind(3);
	for (size_t i = 0; i < triangleList.size(); i++)
	{
		Vec6f triangle = triangleList[i];
		pt[0] = Point2f(triangle[0], triangle[1]);
		pt[1] = Point2f(triangle[2], triangle[3]);
		pt[2] = Point2f(triangle[4], triangle[5]);
		if (rect.contains(pt[0]) && rect.contains(pt[1]) && rect.contains(pt[2])) {
			for (int j = 0; j < 3; j++)
				for (size_t k = 0; k < points.size(); k++)
					if (abs(pt[j].x - points[k].x) < 1.0 && abs(pt[j].y - points[k].y) < 1)
						ind[j] = (int)k;
			Tri.push_back(ind);
		}
	}
}
//-------------------------------------------------------------------------------------------------
// warp triengle - used for face swap
void TCommonHelper::swpWarpTriangle(Mat &img1, Mat &img2, vector<Point2f> &triangle1, vector<Point2f> &triangle2)
{
	Rect rectangle1 = boundingRect(triangle1);
	Rect rectangle2 = boundingRect(triangle2);
	// Offset points by left top corner of the respective rectangles
	vector<Point2f> triangle1Rect, triangle2Rect;
	vector<Point> triangle2RectInt;
	for (int i = 0; i < 3; i++)
	{
		triangle1Rect.push_back(Point2f(triangle1[i].x - rectangle1.x, triangle1[i].y - rectangle1.y));
		triangle2Rect.push_back(Point2f(triangle2[i].x - rectangle2.x, triangle2[i].y - rectangle2.y));
		triangle2RectInt.push_back(Point((int)(triangle2[i].x - rectangle2.x), (int)(triangle2[i].y - rectangle2.y))); // for fillConvexPoly
	}
	// Get mask by filling triangle
	Mat mask = Mat::zeros(rectangle2.height, rectangle2.width, CV_32FC3);
	fillConvexPoly(mask, triangle2RectInt, Scalar(1.0, 1.0, 1.0), 16, 0);
	// Apply warpImage to small rectangular patches
	Mat img1Rect;
	img1(rectangle1).copyTo(img1Rect);
	Mat img2Rect = Mat::zeros(rectangle2.height, rectangle2.width, img1Rect.type());
	Mat warp_mat = getAffineTransform(triangle1Rect, triangle2Rect);
	warpAffine(img1Rect, img2Rect, warp_mat, img2Rect.size(), INTER_LINEAR, BORDER_REFLECT_101);
	multiply(img2Rect, mask, img2Rect);
	multiply(img2(rectangle2), Scalar(1.0, 1.0, 1.0) - mask, img2(rectangle2));
	img2(rectangle2) = img2(rectangle2) + img2Rect;
}
//-------------------------------------------------------------------------------------------------
// get face extrapolation poins
vector<Point2f> TCommonHelper::swpGetExtraPoints(vector<Point2f> &inPts)
{
	// get real bounding rectangle
	float brLeft = HUGE_VAL, brRight = 0, brTop = brLeft, brBottom = 0;
	for (unsigned int i = 0; i < inPts.size(); i++)
	{
		if (inPts[i].x < brLeft)
			brLeft = inPts[i].x;
		if (inPts[i].x > brRight)
			brRight = inPts[i].x;
		if (inPts[i].y < brTop)
			brTop = inPts[i].y;
		if (inPts[i].y > brBottom)
			brBottom = inPts[i].y;
	}
	float brWidth  = brRight - brLeft;
	float brHeight = brBottom - brTop;
	Point2f pt;
	vector<Point2f> exPts;
	// extrapolate additional points
	// top forehead
	pt.x = brLeft + brWidth / 2.0f;
	pt.y = brTop - brHeight* 0.25f; // 0.125f
	if (pt.y < 0.0f)
		pt.y = 0.0f;
	exPts.push_back(pt);
	// left forehead 
	pt.x = brLeft + brWidth / 6.0f;
	pt.y = brTop - brHeight* 0.125f; 
	if (pt.y < 0.0f)
		pt.y = 0.0f;
	exPts.push_back(pt);
	// right forehead 
	pt.x = brRight - brWidth / 6.0f;
	pt.y = brTop - brHeight* 0.125f;
	if (pt.y < 0.0f)
		pt.y = 0.0f;
	exPts.push_back(pt);
	return exPts;
}
//-------------------------------------------------------------------------------------------------
// create oriented kernel
Mat TCommonHelper::spGenerateTrigonoKernel(int fType, unsigned int kernelSize, double hSigma, double theta, double freq, int margin, double vSigma)
{
	unsigned int expKs = kernelSize + (kernelSize >> 1);
	//unsigned int expKs = kernelSize *2;
	Mat bigKernel = Mat::zeros(Size(expKs, expKs), CV_32FC1);
	// cosine kernel
	int radius = expKs;
	double fradius = (double)expKs; 
	Point midPt = Point(bigKernel.cols >> 1, bigKernel.rows >> 1);
	double omega = freq * CV_PI / fradius;
	double shift = 0.0;
	double value;
	Point pt1, pt2, pt1out, pt2out;
	pt1.x = midPt.x;
	pt2.x = midPt.x;
	pt1.y = (expKs - kernelSize) + margin;
	pt2.y = kernelSize - margin;
	pt1out.y = pt1.y;
	pt2out.y = pt2.y;
	//multiply(kernel, 0.0, kernel);
	bigKernel = 0.0;
	for (int i = 0; i < radius; i++)
	{
		// cosine
		switch (fType)
		{
			case 0:
			case 10:
				value = std::cos(i * omega + shift) * exp(-i * hSigma);
			break;
			case 1:
			case 11:
				value = std::sin(i * omega + shift) * exp(-i * hSigma);
				break;
			case 2:
			case 12:
				if (i == 0)
					value = 1.0;
				else
					value = std::cos(i * CV_PI*omega + shift) / (i * CV_PI*omega + shift);
				break;
			case 3:
			case 13:
				if (i == 0)
					value = 1.0;
				else
					value = std::sin(i * CV_PI*omega + shift) / (i * CV_PI*omega + shift);
				break;
			case 4:
				value = 2.0 * std::abs(2.0 * (i / omega - std::floor(i / omega + 0.5)));
				break;
		}
		// square wave
		if (fType >= 10)
			value = (value > 0) - (value < 0);
		// right line
		pt1out.x = pt1.x + i;
		pt2out.x = pt2.x + i;
		line(bigKernel, pt1out, pt2out, value, 1);
		// left line
		pt1out.x = pt1.x - i;
		pt2out.x = pt2.x - i;
		line(bigKernel, pt1out, pt2out, value, 1);
	}
	// apply vertical decay
	if (vSigma > 0 || vSigma < 0)
	{
		for (int i = 0; i < (int)expKs; i++)
			bigKernel.row(i) *= exp(-std::abs(radius - i) * vSigma);
	}
	/*
	Mat kero1, kero2;
	bigKernel.convertTo(kero1, CV_8U, 255.0);
	imshow("big kernel", kero1);
	waitKey(10);
	*/
	if (theta != 0)
	{
		Mat rot2d = getRotationMatrix2D(midPt, theta, 1);
		warpAffine(bigKernel, bigKernel, rot2d, bigKernel.size());
	}
	Rect roi = Rect((expKs - kernelSize)>>1, (expKs -kernelSize)>>1, kernelSize, kernelSize);
	Mat kernel = Mat(bigKernel, roi);
	/* debug
	kernel.convertTo(kero2, CV_8U, 255.0);
	imshow("real kernel", kero2);
	*/
	return kernel;
}
//-------------------------------------------------------------------------------------------------
// calculate bounding box
cv::Rect TCommonHelper::spComputePerspectiveBoundBox(const std::vector<cv::Point> & points, const cv::Mat & homography)
{
    std::vector<cv::Point2f> transformed_points(points.size());

    for (size_t i = 0; i < points.size(); i++)
    {
        // warp the points
        transformed_points[i].x = points[i].x * homography.at<double>(0,0) + points[i].y * homography.at<double>(0,1) + homography.at<double>(0,2) ;
        transformed_points[i].y = points[i].x * homography.at<double>(1,0) + points[i].y * homography.at<double>(1,1) + homography.at<double>(1,2) ;
    }

    // dehomogenization 
    if(homography.rows == 3)
    {
        float homog_comp;
        for (size_t i = 0; i < transformed_points.size(); i++)
        {
            homog_comp = points[i].x * homography.at<double>(2,0) + points[i].y * homography.at<double>(2,1) + homography.at<double>(2,2) ;
            transformed_points[i].x /= homog_comp;
            transformed_points[i].y /= homog_comp;
        }
    }
    // bounding box
    Rect bb = boundingRect(transformed_points);
    return bb;
}

