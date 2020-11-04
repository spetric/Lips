#include "stdafx.h"
#include "ocvTemplateMatcher.h"
using namespace cv;
//-------------------------------------------------------------------------------------------------
// constructor
TocvTemplateMatcher::TocvTemplateMatcher(Mat _imgSource, Mat _imgTarget)
{
	imgSource = _imgSource;
	imgTarget = _imgTarget;
	w2 = imgSource.cols >> 1;
	h2 = imgSource.rows >> 1;
	mDist = std::max(w2, h2);
}
//-------------------------------------------------------------------------------------------------
// destructor
TocvTemplateMatcher::~TocvTemplateMatcher()
{
	ptContainer.clear();
}
//-------------------------------------------------------------------------------------------------
//  private block
bool TocvTemplateMatcher::addPoint(int mdist)
{
	for (size_t i = 0; i < ptContainer.size(); i++)
	{
		Point pt = ptContainer[i];
		int dx, dy;
		dx = std::abs(pt.x - matchLoc.x);
		dy = std::abs(pt.y - matchLoc.y);
		// manhattan max
		if (std::max(dx, dy) < mdist)
			return false;
	}
	// ok - add point to container
	ptContainer.push_back(matchLoc);
	return true;
}
//-------------------------------------------------------------------------------------------------
//  public block
//  template matching - find first one
int TocvTemplateMatcher::MatchFirst(TTemplateMatchingModes tmm)
{
	modeTM = tmm;
	ptContainer.clear();
	int match_method = (int)modeTM;
	int result_cols = imgTarget.cols - imgSource.cols + 1;
	int result_rows = imgTarget.rows - imgSource.rows + 1;
	if (result_rows <= 0 || result_cols <= 0)
		return OCW_ERR_BAD_DATA;
	resultTM.create(result_rows, result_cols, CV_32FC1);
	matchTemplate( imgTarget, imgSource, resultTM, match_method);
	normalize(resultTM, resultTM, 0, 1, NORM_MINMAX, -1, Mat() );
	minMaxLoc(resultTM, &dataTM.minVal, &dataTM.maxVal, &matchMinLoc, &matchMaxLoc, Mat());
	if (match_method == TM_SQDIFF || match_method == TM_SQDIFF_NORMED)
	{
	   matchLoc = matchMinLoc; 
	   matchVal = dataTM.minVal;
	}
	else
	{
	   matchLoc = matchMaxLoc; 
	   matchVal = dataTM.maxVal;
	}
	dataTM.posX = matchLoc.x;
	dataTM.posY = matchLoc.y;
	dataTM.valid = true;
	ptContainer.push_back(matchLoc);
	return 0;
}
//-------------------------------------------------------------------------------------------------
//  template matching - find next one
int TocvTemplateMatcher::MatchNext(TTemplateReduction reductType, double threshold, int mdist)
{	
	int rc = 0;
	int match_method = (int)modeTM;
	dataTM.valid = true;
	float flood_val = (match_method == TM_SQDIFF || match_method == TM_SQDIFF_NORMED) ? 1.0f : 0.0f;
	// update points (always)
	resultTM.at<float>(matchMinLoc.x, matchMinLoc.y) = flood_val;
    resultTM.at<float>(matchMaxLoc.x, matchMaxLoc.y) = flood_val;
	switch (reductType)
	{
	case OCW_TEMPLREDUCT_FR:
		rectangle(resultTM, matchLoc, Point(matchLoc.x + imgSource.cols, matchLoc.y + imgSource.rows), Scalar(flood_val), -1);	
		break;
	case OCW_TEMPLREDUCT_HR:
		rectangle( resultTM, matchLoc, Point( matchLoc.x + w2 , matchLoc.y + h2),  Scalar(flood_val), FILLED);
		break;
	case OCW_TEMPLREDUCT_FHR:
		rectangle( resultTM, Point(matchLoc.x - w2, matchLoc.y - h2), Point( matchLoc.x + w2, matchLoc.y + h2),  Scalar(flood_val), -1);
		break;
	}
	// floodFill(resultTM, matchLoc, cv::Scalar(0), 0, cv::Scalar(0.1), cv::Scalar(1.0)); <- try this option, not so good for closed objects 
	// try to find a new one
	minMaxLoc(resultTM, &dataTM.minVal, &dataTM.maxVal, &matchMinLoc, &matchMaxLoc, Mat() );
	if (match_method == TM_SQDIFF || match_method == TM_SQDIFF_NORMED)
	{
		threshold = 1.0 - threshold;
		if (matchVal <= threshold)
		{
			matchLoc = matchMinLoc;
			matchVal = dataTM.minVal;
		}
		else
			rc = OCW_ERR_EMPTY_DATA;	// no new match
	}
	else
	{
		if (matchVal >= threshold)
		{
		   matchLoc = matchMaxLoc; 
		   matchVal = dataTM.maxVal;
		}
		else
			rc = OCW_ERR_EMPTY_DATA;	// no new match
	}
	dataTM.posX = matchLoc.x;
	dataTM.posY = matchLoc.y;
	if (reductType == OCW_TEMPLREDUCT_MDIST)
	{
		if (mdist == 0)
			mdist = mDist;
		dataTM.valid = addPoint(mdist);
	}
	return rc;
}
//-------------------------------------------------------------------------------------------------
//  template matching - point getter
SocvTemplateData TocvTemplateMatcher::GetData(void)
{
	return dataTM;
}







