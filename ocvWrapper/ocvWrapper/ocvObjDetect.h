#pragma once
#include "ocvGlobals.h"
#include "opencv2/opencv.hpp"
#include "opencv2/objdetect.hpp"
#include "opencv2/xfeatures2d.hpp"
#include "TCommonHelper.h"
const int MAX_CLUSTERS = 50;
class TocvObjDetect
{
private:
	// vars
	// input objs ptrs
	cv::Mat imgSource, imgTarget, maskSource, maskTarget, alphaSource, alphaTarget;
	cv::Mat *imgExport, *alphaExport;
	// Variables to store keypoints and descriptors
	std::vector<cv::KeyPoint> keypointsSource, keypointsTarget, fullKeypointsTarget;
	cv::Mat descriptorsSource, descriptorsTarget, fullDescriptorsTarget;
	// Match features
	std::vector<cv::DMatch> matches;
	// homography matrix
	cv::Mat homography, homoMask;
	// work image/ alpha
	cv::Mat workImage, workAlpha;
	// last used homography
	SocvHomography lastHomography;
	// cluster vars
	cv::Mat clustLabels;
	std::vector<cv::Point2f> clustPoints;
	int clustIdx;
	// methods
	void clearFeatures2D(void);
	int reduceKpds(std::vector<cv::KeyPoint> &kpts, cv::Mat &descs, cv::Mat &mask, uchar compChar, int minmatches);
	int reduceKpds(std::vector<cv::KeyPoint> &kpts, cv::Mat &descs, cv::Mat &labels, std::vector<cv::Point2f> &clustpts, int clustIdx);
public:
	TocvObjDetect(cv::Mat _imgSource, cv::Mat _imgTarget, cv::Mat _alphaSource, cv::Mat _alphaTarget, cv::Mat _maskSource, cv::Mat _maskTarget,  cv::Mat *_imgExport, cv::Mat *_alphaExport);
	~TocvObjDetect();
	// vars
	// methods
	int CalcFeatures2D(TFeature2DType type = OCW_FD_AKAZE,  TFeature2DChoice choice = OCW_FCH_SRCTGT, int maxSrcKeys = 0, int maxTgtKeys = 0);
	int CalcMatches(TMatchType type = OCW_MT_BRUTE_HAMMING, float matchParam = 0.15f, bool exportImage = true);
	int CalcHomography(unsigned int MinMatches, THomographyType type, bool exportImage, bool addAlpha, bool warpCrop);
	int ReduceFeatures2D(TFeature2DReduction redType = OCW_HOSR_OUTLIERS, bool exportMatchImage = false);
	int RecalcHomography(void);
	SocvHomography GetLastHomography(void);
	int CalcNorm(TNormType normType, bool normGray);
	double Clusterize(int clusterNum, int maxIters = 10, double epsilon = 0.1, int attempts = 3, bool flagKpp = true, cv::OutputArray = cv::noArray());
	int ClusterKDB(int maxIters, double epsilon, int attempts, bool flagKpp); 
	int ClusterKFlann(int branching , int iterations, float cb_index = 0.2f);
};

