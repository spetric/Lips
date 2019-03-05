#pragma once
#include "opencv2/opencv.hpp"
#include "opencv2/imgcodecs.hpp"
#include "opencv2/highgui/highgui.hpp"
#include "opencv2\features2d\features2d.hpp"
class TCommonHelper
{
public:
	//static methods only
	static void swpDivideIntoTriangles(cv::Rect rect, std::vector<cv::Point2f> &points, std::vector< std::vector<int> > &delaunayTri);
	static void swpWarpTriangle(cv::Mat &img1, cv::Mat &img2, std::vector<cv::Point2f> &triangle1, std::vector<cv::Point2f> &triangle2);
	static std::vector<cv::Point2f> swpGetExtraPoints(std::vector<cv::Point2f> &inPts);
	static cv::Mat spGenerateTrigonoKernel(int fType, unsigned int kernelSize, double sigma, double theta, double freq, int margin, double vSigma);
};

