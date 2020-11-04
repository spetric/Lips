#include "stdafx.h"
#include "ocvObjDetect.h"
using namespace cv;
//-------------------------------------------------------------------------------------------------
// constructor
TocvObjDetect::TocvObjDetect(Mat _imgSource, Mat _imgTarget, Mat _alphaSource, Mat _alphaTarget, Mat _maskSource, Mat _maskTarget, Mat *_imgExport, Mat *_alphaExport)
{
	imgSource = _imgSource;
	imgTarget = _imgTarget;
	alphaSource = _alphaSource;
	alphaTarget = _alphaTarget;
	maskSource = _maskSource;
	maskTarget = _maskTarget;
	imgExport = _imgExport;
	alphaExport = _alphaExport;
}
//-------------------------------------------------------------------------------------------------
// destructor
TocvObjDetect::~TocvObjDetect()
{
	clearFeatures2D();
}
//-------------------------------------------------------------------------------------------------
// private block
//-------------------------------------------------------------------------------------------------
// clear features 2D
void TocvObjDetect::clearFeatures2D(void)
{
	matches.clear();
	keypointsSource.clear();
	keypointsTarget.clear();
	descriptorsSource = Mat();
	descriptorsTarget = Mat();
	workImage = Mat();
	workAlpha = Mat();
	fullKeypointsTarget.clear();
	fullDescriptorsTarget = Mat();
}
//-------------------------------------------------------------------------------------------------
// reduce keypoints and descriptors
int TocvObjDetect::reduceKpds(std::vector<KeyPoint> &kpts, Mat &descs, Mat &mask, uchar compChar, int minmatches)
{
	// calculate elements to be removed
	int numBytes, remRows, stride, indexOut;
	int kpx, kpy;
	size_t i;
	remRows = 0;
	for (i = 0; i < kpts.size(); i++)
		{
			kpx = (int)kpts[i].pt.x;
			kpy = (int)kpts[i].pt.y;
			if (mask.at<uchar>(kpy, kpx) > compChar)
				remRows++;
		}
	if (remRows == 0 || remRows >= abs((int)kpts.size() - minmatches))
		return 0;	// nothing to do
	std::vector<KeyPoint> tempKT(kpts);
	kpts.clear();
	Mat tempDT = Mat(descs);
	descs = Mat(tempDT.rows - remRows, tempDT.cols, tempDT.type());
	numBytes = tempDT.elemSize();
	stride = tempDT.step;
	uchar *dataIn = tempDT.data;
	uchar *dataOut = descs.data;
	indexOut = 0;
	for (i = 0; i < tempKT.size(); i++)
		{
		kpx = (int)tempKT[i].pt.x;
		kpy = (int)tempKT[i].pt.y;
		if (mask.at<uchar>(kpy, kpx) == compChar)
			{
			kpts.push_back(tempKT[i]);
			memcpy(dataOut + indexOut * stride, dataIn + i * stride, stride); // copy descriptor rows
			indexOut++;
			}
		}
	return remRows;
}
//-------------------------------------------------------------------------------------------------
// reduce keypoints and descriptors
int TocvObjDetect::reduceKpds(std::vector<KeyPoint> &kpts, Mat &descs, Mat &labels, std::vector<Point2f> &clustpts, int clustIdx)
{
	// calculate elements to be removed
	int numBytes, remRows, stride, indexOut;
	remRows = 0;
	indexOut = 0;
	std::vector<KeyPoint> tempKT(kpts);
	kpts.clear();
	Mat tempDT = Mat(descs);
	descs = Mat(tempDT.rows - remRows, tempDT.cols, tempDT.type());
	numBytes = tempDT.elemSize();
	stride = tempDT.step;
	uchar *dataIn = tempDT.data;
	uchar *dataOut = descs.data;
	for (size_t i = 0; i < tempKT.size(); i++)
	{
        int idx = labels.at<int>(i);
		if (idx == clustIdx)
		{
			if (clustpts.at(i).x == tempKT[i].pt.x && clustpts.at(i).y == tempKT[i].pt.y)
			{
				kpts.push_back(tempKT[i]);
				memcpy(dataOut + indexOut * stride, dataIn + i * stride, stride); // copy descriptor rows
				indexOut++;
				remRows++;
			}
		}
	}
	return remRows;
}
//-------------------------------------------------------------------------------------------------
// public block
//-------------------------------------------------------------------------------------------------
// features2d detect and compute
int TocvObjDetect::CalcFeatures2D(TFeature2DType type, TFeature2DChoice choice, int maxSrcKeys, int maxTgtKeys)
{
	bool src = false, tgt = false, sm = false, tm = false;
	Mat maskSrc = Mat();
	Mat maskTgt = Mat();
	switch (choice)
	{
		case OCW_FCH_SRC:
		case OCW_FCH_SRC_MASK:
			src = true;
			keypointsSource.clear();
			descriptorsSource = Mat();
			sm = (choice == OCW_FCH_SRC_MASK && !maskSource.empty());
			break;
		case OCW_FCH_TGT:
		case OCW_FCH_TGT_MASK:
			tgt = true;
			keypointsTarget.clear();
			descriptorsTarget = Mat();
			tm = (choice == OCW_FCH_TGT_MASK && !maskTarget.empty());
			break;
		default:
			src = true;
			tgt = true;
			clearFeatures2D();
			sm = (choice == OCW_FCH_SRCTGT_MASK && !maskSource.empty());
			tm = (choice == OCW_FCH_SRCTGT_MASK && !maskTarget.empty());
			break;
	}
	if (sm)
	{
		if (maskSource.rows == imgSource.rows && maskSource.cols == imgSource.cols)
			maskSrc = maskSource;
	}
	if (tm)
	{
		if (maskTarget.rows == imgTarget.rows && maskTarget.cols == imgTarget.cols)
			maskTgt = maskTarget;
	}
	//
	Mat imSourceGray = Mat(imgSource.rows, imgSource.cols, CV_8UC1);
	Mat imTargetGray = Mat(imgSource.rows, imgSource.cols, CV_8UC1);
	cvtColor(imgSource, imSourceGray, COLOR_BGR2GRAY);
	cvtColor(imgTarget, imTargetGray, COLOR_BGR2GRAY);
	switch (type)
		{
		case OCW_FD_AKAZE:
			{
			Ptr<AKAZE> fdt = AKAZE::create();
			if (src)
				fdt->detectAndCompute(imSourceGray, maskSrc, keypointsSource, descriptorsSource);
			if (tgt)
				fdt->detectAndCompute(imTargetGray, maskTgt, keypointsTarget, descriptorsTarget);
			}	
			break;
		case OCW_FD_ORB:
			{
			Ptr<Feature2D> fdt = ORB::create(1000);
			if (src)
				fdt->detectAndCompute(imSourceGray, maskSrc, keypointsSource, descriptorsSource);
			if (tgt)
				fdt->detectAndCompute(imTargetGray, maskTgt, keypointsTarget, descriptorsTarget);
			}
			break;
		case OCW_FD_BRISK:
			{
			Ptr<BRISK> fdt = BRISK::create();
			if (src)
				fdt->detectAndCompute(imSourceGray, maskSrc, keypointsSource, descriptorsSource);
			if (tgt)
				fdt->detectAndCompute(imTargetGray, maskTgt, keypointsTarget, descriptorsTarget);
			}
			break;
		case OCW_FD_KAZE:
			{
			Ptr<KAZE> fdt = KAZE::create();
			if (src)
				fdt->detectAndCompute(imSourceGray, maskSrc, keypointsSource, descriptorsSource);
			if (tgt)
				fdt->detectAndCompute(imTargetGray, maskTgt, keypointsTarget, descriptorsTarget);
			}
			break;
		case OCW_FD_SURF:
			{
			int minHessian = 400; //TODO: as input parameter
			Ptr<xfeatures2d::SURF> fdt = xfeatures2d::SURF::create(minHessian);
			if (src)
				fdt->detectAndCompute(imSourceGray, maskSrc, keypointsSource, descriptorsSource);
			if (tgt)
				fdt->detectAndCompute(imTargetGray, maskTgt, keypointsTarget, descriptorsTarget);
			}
			break;
		case OCW_FD_SIFT:
			{
			Ptr<xfeatures2d::SIFT> fdt = xfeatures2d::SIFT::create();
			if (src)
				fdt->detectAndCompute(imSourceGray, maskSrc, keypointsSource, descriptorsSource);
			if (tgt)
				fdt->detectAndCompute(imTargetGray, maskTgt, keypointsTarget, descriptorsTarget);
			}
			break;
		case OCW_FD_FAST_DAISY:
			{
			Ptr<FastFeatureDetector> fkp = FastFeatureDetector::create(); 
			Ptr<xfeatures2d::DAISY> fdt = xfeatures2d::DAISY::create();
			if (src)
			{
				fkp->detect(imSourceGray, keypointsSource, maskSrc);
				if (maxSrcKeys > 0)
					KeyPointsFilter::retainBest(keypointsSource, maxSrcKeys);
				fdt->compute(imSourceGray, keypointsSource, descriptorsSource);
			}
			if (tgt)
			{
				fkp->detect(imTargetGray, keypointsTarget, maskTgt);
				if (maxTgtKeys > 0)
					KeyPointsFilter::retainBest(keypointsTarget, maxTgtKeys);
				fdt->compute(imTargetGray, keypointsTarget, descriptorsTarget);
			}	
			}
			break;
		case OCW_FD_FAST_FREAK:
			{
			Ptr<FastFeatureDetector> fkp = FastFeatureDetector::create(); 
			Ptr<xfeatures2d::FREAK> fdt = xfeatures2d::FREAK::create();
			if (src)
			{
				fkp->detect(imSourceGray, keypointsSource, maskSrc);
				if (maxSrcKeys > 0)
					KeyPointsFilter::retainBest(keypointsSource, maxSrcKeys);
				fdt->compute(imSourceGray, keypointsSource, descriptorsSource);
			}
			if (tgt)
			{
				fkp->detect(imTargetGray, keypointsTarget, maskTgt);
				if (maxTgtKeys > 0)
					KeyPointsFilter::retainBest(keypointsTarget, maxTgtKeys);
				fdt->compute(imTargetGray, keypointsTarget, descriptorsTarget);
			}
			}
			break;
		default:
			return OCW_ERR_BAD_PARAM;
		}
	// initialize last homography data
	lastHomography.fType = type;
	lastHomography.maxSrcKeys = maxSrcKeys;
	lastHomography.maxTgtKeys = maxTgtKeys;
	lastHomography.initMinMatches = 0;
	lastHomography.inliers = 0;
	lastHomography.outliers = 0;
	// save initial full kpts/desc data
	fullKeypointsTarget = keypointsTarget;
	fullDescriptorsTarget = Mat(descriptorsTarget);
	return 0;
}
//-------------------------------------------------------------------------------------------------
// calculate good matches
int TocvObjDetect::CalcMatches(TMatchType type, float matchParam, bool exportImage)
{
	bool brutala = false;
	std::vector< std::vector< DMatch> > matchlist;
	std::vector<DMatch> good_matches;
	matches.clear();
	//matchlist.clear();
	switch (type)
	{
	case OCW_MT_BRUTE_HAMMING:
	{
		if (descriptorsSource.type() != CV_8U) 
			descriptorsSource.convertTo(descriptorsSource, CV_8U);
		if (descriptorsTarget.type() != CV_8U)
			descriptorsTarget.convertTo(descriptorsTarget, CV_8U);
		brutala = true;
		Ptr<DescriptorMatcher> matcher = DescriptorMatcher::create("BruteForce-Hamming");
		matcher->match(descriptorsSource, descriptorsTarget, matches, noArray());
	}
	break;
	case OCW_MT_BFM:
	{
		Ptr<BFMatcher> matcher = BFMatcher::create(NORM_L2);  // brute with knn possible
		matcher->knnMatch(descriptorsSource, descriptorsTarget, matchlist, 2, noArray(), false);  // find the best 2
	}
	break;
	case OCW_MT_FLANN:
	{
		if (descriptorsSource.type() != CV_32F) 
			descriptorsSource.convertTo(descriptorsSource, CV_32F);
		if (descriptorsTarget.type() != CV_32F)
			descriptorsTarget.convertTo(descriptorsTarget, CV_32F);
		Ptr<FlannBasedMatcher>matcher = FlannBasedMatcher::create(); // FLANN - Fast Library for Approximate Nearest Neighbors
		matcher->knnMatch(descriptorsSource, descriptorsTarget, matchlist, 2, noArray());  // find the best 2
	}
	break;
	default:
		return OCW_ERR_BAD_PARAM;
	}
	if (brutala)
	{
		// Sort matches by score
		std::sort(matches.begin(), matches.end());
		// Remove not so good matches - so, so
		const int numGoodMatches = (int)((float)matches.size() * matchParam);
		matches.erase(matches.begin() + numGoodMatches, matches.end());
	}
	else
	{
		for (unsigned int i = 0; i < matchlist.size(); ++i)
		{
			// Apply NNDR
			if (matchlist[i][0].distance <= matchParam * matchlist[i][1].distance)
				good_matches.push_back(matchlist[i][0]);
		}
		matches = good_matches;

	}
	if (matches.size() < lastHomography.initMinMatches)
		return OCW_ERR_NO_EXPORT;
	// Draw top matches
	if (exportImage)
	{
		drawMatches(imgSource, keypointsSource, imgTarget, keypointsTarget, matches, *imgExport);
		*alphaExport = Mat();
	}
	else
		*imgExport = Mat();
	// save for recalculation
	lastHomography.mType = type;
	lastHomography.matchParam = matchParam;
	lastHomography.matches = (int)matches.size();
	return 0; 
}
//-------------------------------------------------------------------------------------------------
// calculate homography
int TocvObjDetect::CalcHomography(unsigned int minMatches, THomographyType type, bool exportImage, bool addAlpha, bool warpCrop)
{
	//  TODO: check if previous steps are fullfiled
	// Extract location of good matches
	std::vector<Point2f> points1, points2;

	for (size_t i = 0; i < matches.size(); i++)
	{
		points1.push_back(keypointsSource[matches[i].queryIdx].pt);
		points2.push_back(keypointsTarget[matches[i].trainIdx].pt);
	}
	if (points1.size() < minMatches)
	{
		if (exportImage)
			*imgExport = Mat();
		return OCW_ERR_NO_EXPORT;
	}
	// Find homography
	int method;
	switch (type)
	{
	case OCW_HOMO_LS:
		method = 0;
		break;
	case OCW_HOMO_RANSAC:
		method = RANSAC;
		break;
	case OCW_HOMO_LMEDS:
		method = LMEDS;
		break;
	case OCW_HOMO_RHO:
		method = RHO;
		break;
	default:
		return OCW_ERR_BAD_PARAM;
	}
	// get homography
	homography = findHomography(points1, points2, homoMask, method);
	if (homography.empty())
		return OCW_ERR_EMPTY_DATA;
	// Use homography to warp image
	if (exportImage)
	{
		Size size = imgTarget.size();
		// calc bounding box
		std::vector<Point> points;
		Point pt;
		// top - left
		pt.x = 0;
		pt.y = 0;
		points.push_back(pt);
		// bottom - left
		pt.x = 0;
		pt.y = imgSource.rows - 1;
		points.push_back(pt);
		// bottom - right
		pt.x = imgSource.cols - 1;
		pt.y = imgSource.rows - 1;
		points.push_back(pt);
		// top - right
		pt.x = imgSource.cols - 1;
		pt.y = 0;
		points.push_back(pt);
		//
		Rect bb = TCommonHelper::spComputePerspectiveBoundBox(points, homography);
		// check for negative values
		if (bb.x < 0)
		{
			bb.width += bb.x;
			bb.x = 0;
		}
		if (bb.y < 0)
		{
			bb.height += bb.y;
			bb.y = 0;
		}
		// check for range
		int over;
		over = imgTarget.cols - (bb.width + bb.x);
		if (over < 0)
			bb.width += over;
		over = imgTarget.rows - (bb.height + bb.y);		
		if (over < 0)
			bb.height += over;
		lastHomography.bbTop = bb.y;
		lastHomography.bbLeft = bb.x;
		lastHomography.bbHeight = bb.height;
		lastHomography.bbWidth = bb.width;
		//
		warpPerspective(imgSource, workImage, homography, size);
		if (alphaSource.empty())
		{
			if (addAlpha)
			{
				Mat alpha = Mat(imgSource.rows, imgSource.cols, CV_8UC1, Scalar(255));
				warpPerspective(alpha, workAlpha, homography, size);
			}
			else
				workAlpha = Mat();
		}
		else
			warpPerspective(alphaSource, workAlpha, homography, size);
		// crop output image
		if (warpCrop)
		{
			lastHomography.shiftX = bb.x; 
			lastHomography.shiftY = bb.y; 
			Mat out = Mat(workImage, bb);
			*imgExport = Mat(bb.height, bb.width, workImage.type());
			out.copyTo(*imgExport);
			if (addAlpha)
			{
				out = Mat(workAlpha, bb);
				*alphaExport = Mat(bb.height, bb.width, CV_8UC1);
				out.copyTo(*alphaExport);
			}
		}
		else
		{
			lastHomography.shiftX = lastHomography.shiftY = 0;
			*imgExport = Mat(workImage);
			if (addAlpha)
				*alphaExport = Mat(workAlpha);
		}
	}
	else
	{
		*imgExport = Mat();
		workImage = Mat();
		workAlpha = Mat();
	}
	// save for recalculation
	lastHomography.minMatches = minMatches;
	if (lastHomography.initMinMatches == 0)
		lastHomography.initMinMatches = minMatches;
	lastHomography.hType = type;
	lastHomography.exportImage = exportImage;
	lastHomography.addAlpha = addAlpha;
	lastHomography.warpCrop = warpCrop;
	lastHomography.inliers = 0;
	lastHomography.outliers = 0;
	// get inliers
	if (homoMask.step == 0)
		return OCW_ERR_EMPTY_DATA;
	bool value;
	for (int i = 0; i < homoMask.rows; i++)
	{
		value = homoMask.at<bool>(i, 0);
		if (value)
			lastHomography.inliers++;
		else
			lastHomography.outliers++;
	}
	return 0;
}
//-------------------------------------------------------------------------------------------------
// reduce features
int TocvObjDetect::ReduceFeatures2D(TFeature2DReduction redType, bool exportMatchImage)
{
	if (redType != OCW_HOSR_CLUSTER)
	{
		if (homoMask.step == 0)
			return OCW_ERR_EMPTY_DATA;
		if (matches.size() != homoMask.rows)
			return  OCW_ERR_BAD_DATA;  //bad data
	}
	bool value;
	int remRows;
	//BYTE *data = (BYTE*)(homoMask.data);
	std::vector<DMatch> newMatches;
	switch (redType)
	{
		case OCW_HOSR_OUTLIERS:		// outliers to matches
			for (int i = 0; i < homoMask.rows; i++) // must be equal the size of matches
			{
				value = homoMask.at<bool>(i, 0);
				if (!value)	// move only outliers
					newMatches.push_back(matches[i]);
			}
			// copy new matches to matches vector
			matches.clear();
			matches = newMatches;
			break;
		case  OCW_HOSR_RKD_ASIS:
		case  OCW_HOSR_RKD_ADJUST:
			if (!lastHomography.exportImage)
				return OCW_ERR_NO_EXPORT;
			if (workImage.empty())
				return OCW_ERR_NO_EXPORT;
			if (workAlpha.empty())
				return OCW_ERR_NO_EXPORT;
			remRows = reduceKpds(keypointsTarget, descriptorsTarget, workAlpha, 0x00, lastHomography.initMinMatches);
			if (remRows == 0)
				return OCW_ERR_EMPTY_DATA;
			// rule of thumb: we'll change minmatches to 1/4 of previous match ti avoid "stupid" warping
			if (redType == OCW_HOSR_RKD_ADJUST)
			{
				lastHomography.minMatches = (int)matches.size() >> 2; // at least 1/4 of original
				if (lastHomography.minMatches < lastHomography.initMinMatches)
					lastHomography.minMatches = lastHomography.initMinMatches;
			}
			break;
		case OCW_HOSR_CLUSTER:
			if (clustPoints.empty())
				return OCW_ERR_EMPTY_DATA;
			// restore kpts/dsc
			keypointsTarget = fullKeypointsTarget;
			descriptorsTarget = Mat(fullDescriptorsTarget);
			// remove everything except points within given cluster idx.
			remRows = reduceKpds(keypointsTarget, descriptorsTarget, clustLabels, clustPoints, clustIdx);
			if (remRows == 0)
				return OCW_ERR_EMPTY_DATA;
			clustIdx++;
			break;
		default:	// target mask
			if (maskTarget.empty())
				return OCW_ERR_EMPTY_DATA;
			if (maskTarget.rows != imgTarget.rows && maskTarget.cols != imgTarget.cols)
				return  OCW_ERR_BAD_DATA;  
			remRows = reduceKpds(keypointsTarget, descriptorsTarget, maskTarget, 0x00, lastHomography.initMinMatches);
			if (remRows == 0)
				return OCW_ERR_EMPTY_DATA;
			break;
	}
	return 0;
}
//-------------------------------------------------------------------------------------------------
//  recalc homography
int TocvObjDetect::RecalcHomography(void)
{
	if (homoMask.step == 0)
		return OCW_ERR_EMPTY_DATA;
	return CalcHomography(lastHomography.minMatches, lastHomography.hType, lastHomography.exportImage, lastHomography.addAlpha, lastHomography.warpCrop);
}
//-------------------------------------------------------------------------------------------------
//  return last homography
SocvHomography TocvObjDetect::GetLastHomography(void)
{
	return lastHomography;
}
//-------------------------------------------------------------------------------------------------
// calculate norm
int TocvObjDetect::CalcNorm(TNormType normType, bool normGray)
{
	if (!lastHomography.warpCrop)
		return OCW_ERR_BAD_DATA;
	// calculate norm diff between export image (cropped source) and target
	Rect tgtRect = Rect(lastHomography.shiftX, lastHomography.shiftY, imgExport->cols, imgExport->rows);
	Mat tgtSub = imgTarget(tgtRect);
	if (normGray)
	{
		Mat gSource, gTarget;
		cvtColor(*imgExport, gSource, COLOR_BGR2GRAY);
		cvtColor(tgtSub, gTarget, COLOR_BGR2GRAY);
		lastHomography.stNorm = cv::norm(gSource, gTarget, (NormTypes)normType);
	}
	else
		lastHomography.stNorm = cv::norm(*imgExport, tgtSub, (NormTypes)normType);
	// normalize result
	lastHomography.stNorm = lastHomography.stNorm / ((double)imgExport->rows * imgExport->cols);
	return 0;
}
//-------------------------------------------------------------------------------------------------
//  clusterize - perform 2D clustering using K-means algorithm
double TocvObjDetect::Clusterize(int clusterNum, int maxIters, double epsilon, int attempts, bool flagKpp, OutputArray centers)
{
	TermCriteria criteria = TermCriteria(TermCriteria::EPS + TermCriteria::COUNT, maxIters, epsilon); 
	KmeansFlags flags;
	clustIdx = 0;
	if (flagKpp)
		flags = KMEANS_PP_CENTERS;
	else
		flags = KMEANS_RANDOM_CENTERS;
	// prepare clustPoints
	clustPoints.clear(); 
	clustPoints.resize(fullKeypointsTarget.size());
	for (size_t i = 0; i < fullKeypointsTarget.size(); i++)
		clustPoints.at(i) = fullKeypointsTarget.at(i).pt;
	return kmeans(clustPoints, clusterNum, clustLabels, criteria, attempts, flags, centers);
}
//-------------------------------------------------------------------------------------------------
//  calculate optimal K using Daives-Bouldin indexing (score)
int TocvObjDetect::ClusterKDB(int maxIters, double epsilon, int attempts, bool flagKpp)
{
	int clusterNum = 1;
	double compactness = 0.0;
	std::vector<Point2f> centers;
	double  between_cluster[MAX_CLUSTERS][MAX_CLUSTERS];
	double  inter_cluster[MAX_CLUSTERS];
	std::vector<double> R_container;
	double quadrance, prev_score = 999999.0, score = 888888.0;
	double average;
	int counter;
	// naive Davies-Bouldin index calculation
	while (score < prev_score && clusterNum < MAX_CLUSTERS) // limit clusters to some value
	{
		clusterNum++;
		prev_score = score;
		compactness = Clusterize(clusterNum, maxIters, epsilon, attempts, flagKpp, centers);
		// calculate distances between each cluster
		for (size_t i = 0; i < centers.size() - 1; i++)
		{
			between_cluster[i][i] = 0.0;
			Point2f rav1 = centers[i];
			for (size_t j = i + 1; j < centers.size(); j++)
			{
				Point2f rav2 = centers[j];
				quadrance = (rav2.x - rav1.x) * (rav2.x - rav1.x) + (rav2.y - rav1.y) * (rav2.y - rav1.y);
				between_cluster[i][j] = std::sqrt(quadrance);
				between_cluster[j][i] = between_cluster[i][j]; // symetric 
			}
		}	
		// calculate inter cluster average distance
		for (size_t i = 0; i < centers.size(); i++)
			{
			Point2f rav1 = centers[i];
			average = 0.0;
			counter = 0;
			for (size_t j = 0; j < clustPoints.size(); j++)
			{
				int idx = clustLabels.at<int>(j);
				if (idx == i)
				{
					Point2f rav2 = clustPoints[j];
					if (rav2.x == rav1.x && rav2.y == rav1.y)
						continue;
					quadrance = (rav2.x - rav1.x) * (rav2.x - rav1.x) + (rav2.y - rav1.y) * (rav2.y - rav1.y);
					average += std::sqrt(quadrance);
					counter++;
				}
			}
			inter_cluster[i] = average / (double)counter;
		}
		// quite naive...innit?
		// calculate Davies-Bouldin index
		double R_max;
		double R;
		double D = 0.0;
		for (size_t i = 0; i < centers.size(); i++)
		{
			R_max = 0.0;
			for (size_t j = 0; j < centers.size(); j++)
			{
				if (i != j)
					R = (inter_cluster[i] + inter_cluster[j]) / between_cluster[i][j];
				else
					R = 0.0;
				if (R > R_max)
					R_max = R;
			}
			D += R_max;
		}
		score = D / clusterNum;
	}
clusterNum--;
return clusterNum;
}
//-------------------------------------------------------------------------------------------------
//  calculate optimal K using FLANN hierarchical clustering
int TocvObjDetect::ClusterKFlann(int branching, int iterations, float cb_index)
{
	// what a mess with namespaces...flann/cvflann, cv::L2, flann::L2.
	cvflann::KMeansIndexParams kmean_params(branching, iterations, cvflann::FLANN_CENTERS_KMEANSPP, cb_index);
	// pickup target keypoint and fill FLANN features
	Mat_<float> features(0, 2);
	for (size_t i = 0; i < fullKeypointsTarget.size(); i++)
		{
		int kpx = (int)fullKeypointsTarget[i].pt.x;
		int kpy = (int)fullKeypointsTarget[i].pt.y;
		Mat row = (cv::Mat_<float>(1, 2) << kpx, kpy);
		features.push_back(row);
		}
	Mat1f centers(MAX_CLUSTERS, 2);
	int clusterNum;
	// compiler problems...inspect!!!
	clusterNum = flann::hierarchicalClustering <flann::L2<float> >(features, centers, kmean_params);
	return clusterNum;
}




