#pragma once
#include "ocvGlobals.h"
#include "opencv2/opencv.hpp"
#include "opencv2/objdetect.hpp"
#include "TCommonHelper.h"
class TocvTemplateMatcher
{
private:
	// input objs ptrs
	cv::Mat imgSource, imgTarget;
	// vars
	int h2, w2, mDist;
	std::vector<cv::Point> ptContainer;
	cv::Mat resultTM;
	cv::Point matchLoc;
	cv::Point matchMinLoc, matchMaxLoc;
	double matchVal;
	TTemplateMatchingModes modeTM;
	SocvTemplateData dataTM;
	// methods
	bool addPoint(int mdist);
public:
	TocvTemplateMatcher(cv::Mat _imgSource, cv::Mat _imgTarget);
	~TocvTemplateMatcher();
	int MatchFirst(TTemplateMatchingModes tmm);
	int MatchNext(TTemplateReduction reductType, double threshold, int mdist);
	SocvTemplateData GetData(void);
};

