// ocvWrapper.cpp : Defines the exported functions for the DLL application.
//
#include "stdafx.h"
#include "ocvWrapper.h"
#include "opencv2/opencv.hpp"
#include "opencv2/face.hpp"
#include "opencv2/objdetect.hpp"
#include "opencv2/fuzzy.hpp"
#include "opencv2/xphoto.hpp"
#include "opencv2/xfeatures2d.hpp"
#include "opencv2/dnn_superres.hpp"
#include "opencv2/dnn_superres.hpp"
#include "opencv2/saliency.hpp"
#include "TCommonHelper.h"
#include "ocvDataKeeper.h"
#include "imageDiff.h"
using namespace cv;
// global objects
static Mat imgSource, imgTarget, imgExport, maskSource, maskTarget;
static Mat alphaSource, alphaTarget, alphaExport;
static Rect roiSource, roiTarget;		// roi rectangles
static Mat roimSource, roimTarget;	    // roi image Mats
static Mat roalInput, roalOutput;		// roi alpha Mats
static std::vector<TocvDataKeeper*> dataKeeperList;
// Variables to store keypoints and descriptors
static std::vector<KeyPoint> keypointsSource, keypointsTarget;
static Mat descriptorsSource, descriptorsTarget;
// Match features
static std::vector<DMatch> matches;
// homography matrix
static Mat homography, homoMask;
// last used homography
static SocvHomography lastHomography;
// global vars
static bool gEnableAlpha;
// output structures
static SocvImageData exportImgData;
//-------------------------------------------------------------------------------------------------
// internal function - NOT exported
//-------------------------------------------------------------------------------------------------
int ocvne_checkImgAndRoi(void)
{
	if (imgSource.empty())
		return OCW_ERR_NO_SOURCE;
	if (imgTarget.empty())
		return OCW_ERR_NO_TARGET;
	if (roiSource.empty())
		{
		roimSource = imgSource;
		roalInput = alphaSource;
		}
	else
		{
		roimSource = imgSource(roiSource);
		if (!alphaSource.empty())
			roalInput = alphaSource(roiSource);
		}
	if (roiTarget.empty())
		{
		roimTarget = imgTarget;
		roalOutput = alphaTarget;
		}
	else
		{
		roimTarget = imgTarget(roiTarget);
		if (!alphaTarget.empty())
			roalOutput = alphaTarget(roiTarget);
		}
	// no way alphaExport = Mat(); // erese always
	return 0;
}
//-------------------------------------------------------------------------------------------------
// internal function - NOT exported
//-------------------------------------------------------------------------------------------------
TocvDataKeeper* ocvne_createDK(void)
{
	int id = dataKeeperList.size();
	TocvDataKeeper *dk = new TocvDataKeeper(id);
	dataKeeperList.push_back(dk);
	return dk;
}
//-------------------------------------------------------------------------------------------------
// internal function - NOT exported
//-------------------------------------------------------------------------------------------------
TocvDataKeeper* ocvne_getDK(int dkId)
{
	if (dataKeeperList.empty())
		return 0;
	if (dkId < 0) // get last create data keeper
		return dataKeeperList[dataKeeperList.size() - 1];
	else
	{
		for (size_t i = 0; i < dataKeeperList.size(); i++)
		{
			if (dataKeeperList[i]->DKId == dkId)
				return dataKeeperList[i];
		}
	}
	return 0;
}
//-------------------------------------------------------------------------------------------------
// DLL entry - check for DLLs and initialize global vars
//-------------------------------------------------------------------------------------------------
int ocvInit(void)
{
	//TODO: check for DLLs (-> delay loaded)
	gEnableAlpha = true;
	return 0;
}
//-------------------------------------------------------------------------------------------------
// DLL entry - close (cleanup)
//-------------------------------------------------------------------------------------------------
int ocvClean(void)
{
	while (!dataKeeperList.empty())
	{
		TocvDataKeeper *dk = dataKeeperList.back();
		delete dk;
		dataKeeperList.pop_back();
	}
	return 0;
}
//-------------------------------------------------------------------------------------------------
// DLL entry - get last dk id
//-------------------------------------------------------------------------------------------------
int ocvCreateDK(void)
{
	TocvDataKeeper *dk = ocvne_createDK();
	return dk->DKId;
}
//-------------------------------------------------------------------------------------------------
// DLL entry - get last dk id
//-------------------------------------------------------------------------------------------------
int ocvGetLastDKIdx(void)
{
	int id = dataKeeperList.size() - 1;
	return id;
}
//-------------------------------------------------------------------------------------------------
// DLL entry - clear dk
//-------------------------------------------------------------------------------------------------
int ocvClearDK(int idx)
{
	if (idx > ((int)(dataKeeperList.size())) || idx < 0)
		return OCW_ERR_BAD_INDEX;
	TocvDataKeeper *dk = dataKeeperList[idx];
	delete dk;
	dataKeeperList.erase(dataKeeperList.begin() + idx);
	return 0;
}
//-------------------------------------------------------------------------------------------------
// DLL entry - check for DLLs and initialize global vars
//-------------------------------------------------------------------------------------------------
int ocvEnableAlpha(bool value)
{
	gEnableAlpha = value;
	return 0;
}
//-------------------------------------------------------------------------------------------------
// DLL entry - set input/output image 
//-------------------------------------------------------------------------------------------------
int ocvSetImage(TImgType type, int width, int height, void *imageBuff, int imageStride, void *alphaBuff, int alphaStride)
{
	switch (type)
	{
	case OCW_IMG_SOURCE_RGB:
		imgSource = Mat(height, width, CV_8UC3, imageBuff, imageStride);
		if (alphaBuff)
			alphaSource = Mat(height, width, CV_8UC1, alphaBuff, alphaStride);
		else
			alphaSource = Mat();
		roiSource = Rect();
		break;
	case OCW_IMG_SOURCE_RGBA:
		imgSource = Mat(height, width, CV_8UC4, imageBuff, imageStride);
		alphaSource = Mat();
		roiSource = Rect();
		break;
	case OCW_IMG_TARGET_RGB:
		imgTarget = Mat(height, width, CV_8UC3, imageBuff, imageStride);
		if (alphaBuff)
			alphaTarget = Mat(height, width, CV_8UC1, alphaBuff, alphaStride);
		else
			alphaTarget = Mat();
		roiTarget = Rect();
		break;
	case OCW_IMG_TARGET_RGBA:
		imgTarget = Mat(height, width, CV_8UC4, imageBuff, imageStride);
		alphaTarget = Mat();
		roiTarget = Rect();
		break;
	case OCW_MASK_SOURCE:
		maskSource = Mat(height, width, CV_8UC1, imageBuff, imageStride);
		break;
	case OCW_MASK_TARGET:
		maskTarget= Mat(height, width, CV_8UC1, imageBuff, imageStride);
		break;
	default:
		return OCW_ERR_BAD_TYPE;	// bad type
	}
	return 0; // ok
}
//-------------------------------------------------------------------------------------------------
// DLL entry - set input/output roi
//-------------------------------------------------------------------------------------------------
int ocvSetRoi(TRoiType type, SocvRoi roi)
{
	switch (type)
	{
	case OCW_ROI_ALL:
		roiTarget = Rect(roi.left, roi.top, roi.right - roi.left, roi.bottom - roi.top);
		roiSource = Rect(roi.left, roi.top, roi.right - roi.left, roi.bottom - roi.top);
	case OCW_ROI_SOURCE:
		roiSource = Rect(roi.left, roi.top, roi.right - roi.left, roi.bottom - roi.top);
		break;
	case OCW_ROI_TARGET:
		roiTarget = Rect(roi.left, roi.top, roi.right - roi.left, roi.bottom - roi.top);
		break;
	default:
		return OCW_ERR_BAD_TYPE;	// bad type
	}
	return 0; // ok
}
//-------------------------------------------------------------------------------------------------
// DLL entry - clear input/output roi
//-------------------------------------------------------------------------------------------------
int ocvClearRoi(TRoiType type)
{
	switch (type)
	{
	case OCW_ROI_ALL:
		roiTarget = Rect();
		roiSource = Rect();
	case OCW_ROI_SOURCE:
		roiSource = Rect();
		break;
	case OCW_ROI_TARGET:
		roiTarget = Rect();
		break;
	default:
		return OCW_ERR_BAD_TYPE;	// bad type
	}
	return 0; // ok
}
//-------------------------------------------------------------------------------------------------
// DLL entry - release object data (cleanup)
int ocvReleaseObject(SocvObjectData *exportData)
{
	if (exportData->numObjects)
	{
		delete[](exportData->bRects);
		delete[](exportData->lPts);
		exportData->numObjects = 0;
		exportData->bRects = 0;
		exportData->lPts = 0;
	}
	return 0;
}
//-------------------------------------------------------------------------------------------------
// DLL entry - get export image data
//-------------------------------------------------------------------------------------------------
SocvImageData* ocvGetExportImage(void)
{
	exportImgData.width = imgExport.cols;
	exportImgData.height = imgExport.rows;
	exportImgData.channels = imgExport.channels();
	exportImgData.imageBuff = (void*)imgExport.data;
	exportImgData.imageStride = imgExport.step;
	exportImgData.alphaBuff = (void*)alphaExport.data;
	exportImgData.alphaStride = alphaExport.step;
	return &exportImgData;
}
//-------------------------------------------------------------------------------------------------
// DLL entry - copy image
//-------------------------------------------------------------------------------------------------
int ocvCopy(TImgSelect inpType, TImgSelect outType)
{
	int nRet = ocvne_checkImgAndRoi();
	if (nRet)
		return nRet;
	Mat inp, out;
	switch (inpType)
	{
		case OCW_IMAGE_TARGET:
			inp = roimTarget;
		break;
		default:
			inp = roimSource;
		break;
	}
	switch (outType)
	{
	case OCW_IMAGE_SOURCE:
		out = roimSource;
		break;
	default:
		out = roimTarget;
		break;
	}
	inp.copyTo(out);
	return 0;
}
//-------------------------------------------------------------------------------------------------
// DLL entry - proc - blur
//-------------------------------------------------------------------------------------------------
int ocvBlur(unsigned int kX, unsigned int kY, TBorderType type)
{
	int nRet = ocvne_checkImgAndRoi();
	if (nRet)
		return nRet;
	blur(roimSource, roimTarget, Size(kX, kY), Point(-1, -1), type);
	if (!alphaSource.empty() && !alphaTarget.empty() && gEnableAlpha)
		blur(roalInput, roalOutput, Size(kX, kY), Point(-1, -1), type);
	return 0;
}
//-------------------------------------------------------------------------------------------------
// DLL entry - proc - median blur
//-------------------------------------------------------------------------------------------------
int ocvMedianBlur(unsigned int kSize)
{
	int nRet = ocvne_checkImgAndRoi();
	if (nRet)
		return nRet;
	// kSize must be odd number
	if (kSize % 2 == 0)
		return OCW_ERR_BAD_PARAM;
	medianBlur(roimSource, roimTarget, kSize);
	if (!alphaSource.empty() && !alphaTarget.empty() && gEnableAlpha)
		medianBlur(roalInput, roalOutput, kSize);
	return 0;
}
//-------------------------------------------------------------------------------------------------
// DLL entry - proc - gaussian blur
//-------------------------------------------------------------------------------------------------
int ocvGaussianBlur(unsigned int kX, unsigned int kY, double sigmaX, double sigmaY, TBorderType type)
{
	int nRet = ocvne_checkImgAndRoi();
	if (nRet)
		return nRet;
	// kX and kY must be odd numbers
	if (kX % 2 == 0 || kY%2 == 0)
		return OCW_ERR_BAD_PARAM;
	GaussianBlur(roimSource, roimTarget, Size(kX, kY), sigmaX, sigmaY, type);
	if (!alphaSource.empty() && !alphaTarget.empty() && gEnableAlpha)
		GaussianBlur(roalInput, roalOutput, Size(kX, kY), sigmaX, sigmaY, type);
	return 0;
}
//-------------------------------------------------------------------------------------------------
// DLL entry - proc - bilateral filter
//-------------------------------------------------------------------------------------------------
int ocvBilateralFilter( int d, double sigmaColor, double sigmaSpace, TBorderType type)
{
	int nRet = ocvne_checkImgAndRoi();
	if (nRet)
		return nRet;
	bilateralFilter(roimSource, roimTarget, d, sigmaColor, sigmaSpace, type);
	/* do we need that???
	if (!alphaSource.empty() && !alphaTarget.empty())
		bilateralFilter(roalInput, roalOutput, d, sigmaColor, sigmaSpace, type);
	*/
	return 0;
}
//-------------------------------------------------------------------------------------------------
// DLL entry - proc - laplacian
//-------------------------------------------------------------------------------------------------
int ocvLaplacian(unsigned int ksize, double scale, double delta, bool cv2gray, TBorderType type)
{
	int nRet = ocvne_checkImgAndRoi();
	if (nRet)
		return nRet;
	if (cv2gray)
		{
		Mat imGray;
		cvtColor(roimSource, imGray, COLOR_BGR2GRAY);
		Laplacian(imGray, roimTarget, imgTarget.depth(), ksize, scale, delta, type);
		}
	else
		Laplacian(roimSource, roimTarget, imgTarget.depth(), ksize, scale, delta, type);
	return 0;
}
//-------------------------------------------------------------------------------------------------
// DLL entry - proc - scharr
//-------------------------------------------------------------------------------------------------
int ocvScharr(unsigned int dx, unsigned int dy, double scale, double delta, bool cv2gray, TBorderType type)
{
	int nRet = ocvne_checkImgAndRoi();
	if (nRet)
		return nRet;
	if (dx + dy != 1)
		return OCW_ERR_BAD_PARAM;
	if (cv2gray)
	{
		Mat imGray;
		cvtColor(roimSource, imGray, COLOR_BGR2GRAY);
		Scharr(imGray, roimTarget, imgTarget.depth(), dx, dy, scale, delta, type);
	}
	else
		Scharr(roimSource, roimTarget, imgTarget.depth(), dx, dy, scale, delta, type);
	return 0;
}
//-------------------------------------------------------------------------------------------------
// DLL entry - proc - sobel
//-------------------------------------------------------------------------------------------------
int ocvSobel(unsigned int dx, unsigned int dy, int ksize, double scale, double delta, bool cv2gray, TBorderType type)
{
	int nRet = ocvne_checkImgAndRoi();
	if (nRet)
		return nRet;
	if (cv2gray)
	{
		Mat imGray;
		cvtColor(roimSource, imGray, COLOR_BGR2GRAY);
		Sobel(imGray, roimTarget, imgTarget.depth(), dx, dy, ksize, scale, delta, type);
	}
	else
		Sobel(roimSource, roimTarget, imgTarget.depth(), dx, dy, ksize, scale, delta, type);
	return 0;
}
//-------------------------------------------------------------------------------------------------
// DLL entry - proc - box filter
//-------------------------------------------------------------------------------------------------
int ocvBoxFilter(unsigned int kX, unsigned int kY, bool normalize, TBorderType type)
{
	int nRet = ocvne_checkImgAndRoi();
	if (nRet)
		return nRet;
	boxFilter(roimSource, roimTarget, imgTarget.depth(), Size(kX, kY), Point(-1, -1), normalize, type);
	if (!alphaSource.empty() && !alphaTarget.empty() && gEnableAlpha)
		boxFilter(roalInput, roalOutput, imgTarget.depth(), Size(kX, kY), Point(-1, -1), normalize, type);
	return 0;
}
//-------------------------------------------------------------------------------------------------
// DLL entry - proc - squre box filter
//-------------------------------------------------------------------------------------------------
int ocvSqrBoxFilter(unsigned int kX, unsigned int kY, bool normalize, TBorderType type)
{
	int nRet = ocvne_checkImgAndRoi();
	if (nRet)
		return nRet;
	sqrBoxFilter(roimSource, roimTarget, imgTarget.depth(), Size(kX, kY), Point(-1, -1), normalize, type);
	//TODO: alpha
	return 0;
}
//-------------------------------------------------------------------------------------------------
// DLL entry - proc - dilate
//-------------------------------------------------------------------------------------------------
int ocvDilate(unsigned int kX, unsigned int kY, TMorphShape shape, unsigned int iterations, TBorderType type)
{
	int nRet = ocvne_checkImgAndRoi();
	if (nRet)
		return nRet;
	Mat kernel = getStructuringElement(shape, Size(2*kX + 1, 2*kY+1), Point(-1, -1));
	dilate(roimSource, roimTarget, kernel, Point(-1, -1), iterations, type, morphologyDefaultBorderValue());
	//TODO: alpha
	return 0;
}
//-------------------------------------------------------------------------------------------------
// DLL entry - proc - erode
//-------------------------------------------------------------------------------------------------
int ocvErode(unsigned int kX, unsigned int kY, TMorphShape shape, unsigned int iterations, TBorderType type)
{
	int nRet = ocvne_checkImgAndRoi();
	if (nRet)
		return nRet;
	Mat kernel = getStructuringElement(shape, Size(2 * kX + 1, 2 * kY + 1), Point(-1, -1));
	erode(roimSource, roimTarget, kernel, Point(-1, -1), iterations, type, morphologyDefaultBorderValue());
	//TODO: alpha
	return 0;
}
//-------------------------------------------------------------------------------------------------
// DLL entry - proc - morphologyEx
//-------------------------------------------------------------------------------------------------
int ocvMorphologyEx(unsigned int kX, unsigned int kY, TMorphType opType, TMorphShape shape, unsigned int iterations, TBorderType borType)
{
	int nRet = ocvne_checkImgAndRoi();
	if (nRet)
		return nRet;
	Mat kernel = getStructuringElement(shape, Size(2 * kX + 1, 2 * kY + 1), Point(-1, -1));
	if (opType == OCW_MORPH_HITMISS)
	{
		Mat imGrayInput, imGrayOutput;
		if (roiTarget.empty())
		   imGrayOutput = Mat(imgTarget.rows, imgTarget.cols, CV_8UC1);
		else
		   imGrayOutput = Mat(roiTarget.height, roiTarget.width, CV_8UC1);
		cvtColor(roimSource, imGrayInput, COLOR_BGR2GRAY);
		morphologyEx(imGrayInput, imGrayOutput, opType, kernel, Point(-1, -1), iterations, borType, morphologyDefaultBorderValue());
		cvtColor(imGrayOutput, roimTarget, COLOR_GRAY2BGR);
	}
	else
		morphologyEx(roimSource, roimTarget, opType, kernel, Point(-1, -1), iterations, borType, morphologyDefaultBorderValue());
	//TODO: alpha
	return 0;
}
//-------------------------------------------------------------------------------------------------
// DLL entry - proc - threshold
//-------------------------------------------------------------------------------------------------
int ocvThreshold(double thresh, double maxval, TThresholdType type)
{
	int nRet = ocvne_checkImgAndRoi();
	if (nRet)
		return nRet;
	threshold(roimSource, roimTarget, thresh, maxval, type);
	return 0;
}
//-------------------------------------------------------------------------------------------------
// DLL entry - proc - adaptive threshold
//-------------------------------------------------------------------------------------------------
int ocvAdaptiveThreshold(double maxval, TAdaptiveThresholdType method, TThresholdType type, unsigned int radius, double C)
{
	int nRet = ocvne_checkImgAndRoi();
	if (nRet)
		return nRet;
	int blockSize = 2 * radius + 1;
	Mat imGrayInput, imGrayOutput;
	if (roiTarget.empty())
		imGrayOutput = Mat(imgTarget.rows, imgTarget.cols, CV_8UC1);
	else
		imGrayOutput = Mat(roiTarget.height, roiTarget.width, CV_8UC1);
	cvtColor(roimSource, imGrayInput, COLOR_BGR2GRAY);
	adaptiveThreshold(imGrayInput, imGrayOutput, maxval, method, type, blockSize, C);
	cvtColor(imGrayOutput, roimTarget, COLOR_GRAY2BGR);
	return 0;
}
//-------------------------------------------------------------------------------------------------
// DLL entry - proc - distance transform
//-------------------------------------------------------------------------------------------------
int ocvDistanceTransform(bool gray, TDistanceType type, TDistanceMaskType maskSize)
{
	int nRet = ocvne_checkImgAndRoi();
	if (nRet)
		return nRet;
	Mat imGrayInput, imGrayOutput, conv;
	if (roiTarget.empty())
		imGrayOutput = Mat(imgTarget.rows, imgTarget.cols, CV_32F);
	else
		imGrayOutput = Mat(roiTarget.height, roiTarget.width, CV_32F);
	if (gray)
	{
		cvtColor(roimSource, imGrayInput, COLOR_BGR2GRAY);
		distanceTransform(imGrayInput, imGrayOutput, type, maskSize, CV_32F);
		imGrayOutput.convertTo(conv, CV_8U, 1, 0);
		cvtColor(conv, roimTarget, COLOR_GRAY2BGR);
	}
	else
	{
		if (roiSource.empty())
			imGrayInput = Mat(imgSource.rows, imgSource.cols, CV_8U);
		else
			imGrayInput = Mat(roiSource.height, roiSource.width, CV_8U);
		int from_to_forw[] = { 0,0};
		int from_to_back[] = { 0,0};
		size_t npairs = 1;
		for (int i = 0; i < 3; i++)
		{
			mixChannels(roimSource, imGrayInput, from_to_forw, npairs);
			distanceTransform(imGrayInput, imGrayOutput, type, maskSize, CV_8UC1);
			imGrayOutput.convertTo(conv, CV_8U, 1, 0);
			mixChannels(conv, roimTarget, from_to_back, npairs);
			from_to_forw[0]++;
			from_to_back[1]++;
		}
	}
	return 0;
}
//-------------------------------------------------------------------------------------------------
// DLL entry - proc - canny edge detector
//-------------------------------------------------------------------------------------------------
int ocvCanny(double threshold1, double threshold2, unsigned int apertureSize, bool L2gradient)
{
	int nRet = ocvne_checkImgAndRoi();
	if (nRet)
		return nRet;
	Mat imGrayOutput;
	imGrayOutput = Mat(roimTarget.rows, roimTarget.cols, CV_8UC1);
	if (threshold1 == 0 && threshold2 == 0)
		{
		double thrHigh;
		thrHigh = threshold(imGrayOutput, imGrayOutput, 0, 255, THRESH_BINARY + THRESH_OTSU);
		Canny(roimSource, imGrayOutput, thrHigh*0.5, thrHigh, apertureSize, L2gradient);
		}
	else
		Canny(roimSource, imGrayOutput, threshold1, threshold2, apertureSize, L2gradient);
	cvtColor(imGrayOutput, roimTarget, COLOR_GRAY2BGR);
	return 0;
}
//-------------------------------------------------------------------------------------------------
// DLL entry - proc - arbitrary 2D filter (cross-correlation)
//-------------------------------------------------------------------------------------------------
int ocvFilter2D(unsigned int kernelSize, float *kernel, float div, float bias, TBorderType borderType)
{
	int nRet = ocvne_checkImgAndRoi();
	if (nRet)
		return nRet;
	Mat ker = Mat(kernelSize, kernelSize, CV_32FC1);
	unsigned int kk = 0;
	for (unsigned int i = 0; i < kernelSize; i++)
	{
		for (unsigned int j = 0; j < kernelSize; j++)
			ker.at<float>(i, j) = kernel[kk++] / div;
	}
	filter2D(roimSource, roimTarget, -1, ker, Point(-1, -1), bias, borderType);
	return 0;
}
//-------------------------------------------------------------------------------------------------
// DLL entry - proc - warp polar
//-------------------------------------------------------------------------------------------------
int ocvWarpPolar(int x, int y, double maxRadius, bool exportImage, int flag)
{
	int nRet = ocvne_checkImgAndRoi();
	if (nRet)
		return nRet;
	if (exportImage)
	{
		alphaExport = Mat();
		warpPolar(roimSource, imgExport, Size(), Point2f((float)x, (float)y), maxRadius, flag);
		if (!alphaSource.empty() && gEnableAlpha)
			warpPolar(roalInput, alphaExport, Size(), Point2f((float)x, (float)y), maxRadius, flag);
	}
	else
	{
		Mat temp;
		warpPolar(roimSource, temp, roimTarget.size(), Point2f((float)x, (float)y), maxRadius, flag);
		temp.copyTo(roimTarget);
		if (!alphaSource.empty() && !alphaTarget.empty() && gEnableAlpha)
		{
			warpPolar(roalInput, temp, roimTarget.size(), Point2f((float)x, (float)y), maxRadius, flag);
			temp.copyTo(roalOutput);
		}
	}
	return 0;
}
//-------------------------------------------------------------------------------------------------
// DLL entry - proc - warp perspective
//-------------------------------------------------------------------------------------------------
int ocvWarpPerspective(const SocvTuple *wparams, bool exportImage,  int flag, TBorderType borderType)
{
	int nRet = ocvne_checkImgAndRoi();
	if (nRet)
		return nRet;
	Point2f srcPt[4], dstPt[4];
	for (int i = 0; i < 4; i++)
	{
		srcPt[i].x = wparams->src[i][0];
		srcPt[i].y = wparams->src[i][1];
		dstPt[i].x = wparams->dst[i][0];
		dstPt[i].y = wparams->dst[i][1];
	}
	Mat tMatrix = getPerspectiveTransform(srcPt, dstPt);
	if (exportImage)
		{
		alphaExport = Mat();
		warpPerspective(roimSource, imgExport, tMatrix, Size(), flag, borderType, Scalar());
		if (!alphaSource.empty() && gEnableAlpha)
			warpPerspective(roalInput, alphaExport, tMatrix, Size(), flag, borderType, Scalar());
		}
	else
	{
		Mat temp;
		warpPerspective(roimSource, temp, tMatrix, roimTarget.size(), flag, borderType, Scalar());
		temp.copyTo(roimTarget);
		if (!alphaSource.empty() && !alphaTarget.empty() && gEnableAlpha)
		{
			warpPerspective(roalInput, temp, tMatrix, roimTarget.size(), flag, borderType, Scalar());
			temp.copyTo(roalOutput);
		}
	}
	return 0;
}
//-------------------------------------------------------------------------------------------------
// DLL entry - proc - warp affine
//-------------------------------------------------------------------------------------------------
int ocvWarpAffine(const SocvTuple *wparams, bool exportImage, int flag, TBorderType borderType)
{
	int nRet = ocvne_checkImgAndRoi();
	if (nRet)
		return nRet;
	Point2f srcPt[3], dstPt[3];
	for (int i = 0; i < 3; i++)
	{
		srcPt[i].x = wparams->src[i][0];
		srcPt[i].y = wparams->src[i][1];
		dstPt[i].x = wparams->dst[i][0];
		dstPt[i].y = wparams->dst[i][1];
	}
	Mat tMatrix = getAffineTransform(srcPt, dstPt);
	if (exportImage)
	{
		alphaExport = Mat();
		warpAffine(roimSource, imgExport, tMatrix, Size(), flag, borderType, Scalar());
		if (!alphaSource.empty() && gEnableAlpha)
			warpAffine(roalInput, alphaExport, tMatrix, Size(), flag, borderType, Scalar());
	}
	else
	{
		Mat temp;
		warpAffine(roimSource, temp, tMatrix, roimTarget.size(), flag, borderType, Scalar());
		temp.copyTo(roimTarget);
		if (!alphaSource.empty() && !alphaTarget.empty() && gEnableAlpha)
		{
			warpAffine(roalInput, temp, tMatrix, roimTarget.size(), flag, borderType, Scalar());
			temp.copyTo(roalOutput);
		}
	}
	return 0;
}
//-------------------------------------------------------------------------------------------------
// DLL entry - flip image
//-------------------------------------------------------------------------------------------------
int ocvFlip(int flipCode)
{
	int nRet = ocvne_checkImgAndRoi();
	if (nRet)
		return nRet;
	flip(roimSource, roimTarget, flipCode);
	return 0;
}
//-------------------------------------------------------------------------------------------------
// DLL entry - flip image inplace
//-------------------------------------------------------------------------------------------------
int ocvFlipInplace(int flipCode, TImgSelect iSel)
{
	ocvne_checkImgAndRoi();
	if (iSel == OCW_IMAGE_SOURCE)
	{
		if (imgSource.empty())
			return OCW_ERR_NO_SOURCE;
		else
		{
			flip(roimSource, roimSource, flipCode);
			if (!alphaSource.empty())
				flip(roalInput, roalInput, flipCode);
		}
	}
	else if (iSel == OCW_IMAGE_TARGET)
	{
		if (imgTarget.empty())
			return OCW_ERR_NO_TARGET;
		else
		{
			flip(roimTarget, roimTarget, flipCode);
			if (!alphaTarget.empty())
				flip(roalOutput, roalOutput, flipCode);

		}
	}
	else if (iSel == OCW_IMAGE_EXPORT)
	{
		if (imgExport.empty())
			return OCW_ERR_NO_EXPORT;
		else
		{
			flip(imgExport, imgExport, flipCode);
			if (!alphaExport.empty())
				flip(alphaExport, alphaExport, flipCode);
		}
	}
	return 0;
}
//-------------------------------------------------------------------------------------------------
// DLL entry - resize image
//-------------------------------------------------------------------------------------------------
int ocvResize(int width, int height, int flag)
{
	int nRet = ocvne_checkImgAndRoi();
	if (nRet)
		return nRet;
	alphaExport = Mat();
	resize(imgSource, imgExport, Size(width, height), 0, 0, flag );
	if (!alphaSource.empty() && gEnableAlpha)
		resize(alphaSource, alphaExport, Size(width, height), 0, 0, flag );
	return 0;
}
//-------------------------------------------------------------------------------------------------
// DLL entry - NPR - detail enhance
//-------------------------------------------------------------------------------------------------
int ocvDetailEnhance(float sigma_s, float sigma_r)
{
	int nRet = ocvne_checkImgAndRoi();
	if (nRet)
		return nRet;
	detailEnhance(roimSource, roimTarget, sigma_s, sigma_r);
	return 0;
}
//-------------------------------------------------------------------------------------------------
// DLL entry - NPR - edge preserving filter
//-------------------------------------------------------------------------------------------------
int ocvEdgePreservingFilter(int flags, float sigma_s, float sigma_r)
{
	int nRet = ocvne_checkImgAndRoi();
	if (nRet)
		return nRet;
	edgePreservingFilter(roimSource, roimTarget, flags, sigma_s, sigma_r);
	return 0;
}
//-------------------------------------------------------------------------------------------------
// DLL entry - NPR - pencil skectch
//-------------------------------------------------------------------------------------------------
int ocvPencilSketch(bool grayscale, float sigma_s, float sigma_r, float shade_factor)
{
	int nRet = ocvne_checkImgAndRoi();
	if (nRet)
		return nRet;
	Mat gray;
	pencilSketch(roimSource, gray, roimTarget, sigma_s, sigma_r, shade_factor);
	if (grayscale)
		cvtColor(gray, roimTarget, COLOR_GRAY2BGR);
	return 0;
}
//-------------------------------------------------------------------------------------------------
// DLL entry - NPR - stylization
//-------------------------------------------------------------------------------------------------
int ocvStylization(float sigma_s, float	sigma_r)
{
	int nRet = ocvne_checkImgAndRoi();
	if (nRet)
		return nRet;
	stylization(roimSource, roimTarget, sigma_s, sigma_r);
	return 0;
}
//-------------------------------------------------------------------------------------------------
// DLL entry - NPR - oil painting
//-------------------------------------------------------------------------------------------------
int ocvOilPainting(int size, int dynRatio)
{
	int nRet = ocvne_checkImgAndRoi();
	if (nRet)
		return nRet;
	xphoto::oilPainting(roimSource, roimTarget, size, dynRatio, COLOR_BGR2Lab);
	return 0;
}
//-------------------------------------------------------------------------------------------------
// DLL entry - DNN - read model 
//-------------------------------------------------------------------------------------------------
int ocvDnnReadModel(const wchar_t *model, const wchar_t *config, int dkId)
{
	TocvDataKeeper *dk = ocvne_getDK(dkId);
	if (dk)
		return dk->DnnReadModel(model, config);
	else
		return OCW_ERR_NULL_OBJECT;
}
//-------------------------------------------------------------------------------------------------
// DLL entry - DNN - style transfer (TODO: build OpenCV with intel inference - OpenVINO toolkit to gain speed)
//-------------------------------------------------------------------------------------------------
int ocvStyleTransfer(int dkId, double scale, double meanR, double meanG, double meanB, bool normResult, bool swapRB)
{
	int nRet = ocvne_checkImgAndRoi();
	if (nRet)
		return nRet;
	TocvDataKeeper *dk = ocvne_getDK(dkId);
	if (dk == 0)
		return OCW_ERR_NULL_OBJECT;
	// inference engine (OpenVino toolkit)
	/*
	dk->DnnNetwork.setPreferableBackend(dnn::DNN_BACKEND_INFERENCE_ENGINE);
	dk->DnnNetwork.setPreferableTarget(dnn::DNN_TARGET_CPU);	
	*/
	Mat blob, prob;
	if (swapRB)
		dnn::blobFromImage(roimSource, blob, scale, roimSource.size(), Scalar(meanR, meanG, meanB), swapRB, false);
	else
		dnn::blobFromImage(roimSource, blob, scale, roimSource.size(), Scalar(meanB, meanG, meanR), swapRB, false);
	try {
		dk->DnnNetwork.setInput(blob);
		prob = dk->DnnNetwork.forward();
	}
	catch (...)	{
		ocvClearDK(dkId);
		return OCW_ERR_EXCEPTION;
	}
	//int n = prob.size[0];
	//int c = prob.size[1];
	int h = prob.size[2];
	int w = prob.size[3];
	//NOTE: result image w/h do not have to be equal to roi w/h (dword multiple). 
	Mat b(h, w, CV_32F, prob.ptr<float>(0, 0));
	Mat g(h, w, CV_32F, prob.ptr<float>(0, 1)); 
	Mat r(h, w, CV_32F, prob.ptr<float>(0, 2));
	//double min, max;
	//minMaxLoc(b, &min, &max);
	//minMaxLoc(g, &min, &max);
	//minMaxLoc(r, &min, &max);
	if (!normResult)
	{
		r += meanR; // 103.939;
		g += meanG; // 116.779;
		b += meanB; // 123.680;
	}
	Mat final;
	std::vector<Mat> channels;
	channels.push_back(b);
	channels.push_back(g);
	channels.push_back(r);
	merge(channels, final);
	if (normResult) // normalize result image
	   normalize(final, final, 0, 255.0, NORM_MINMAX);
	Mat froi = Mat(final, Rect(0, 0, roimTarget.cols, roimTarget.rows));
	froi.convertTo(roimTarget, CV_8U);	// now it works correctly
	return 0;
}
//-------------------------------------------------------------------------------------------------
// DLL entry - load cascade classifier and model
//-------------------------------------------------------------------------------------------------
int ocvLoadClassifierAndFaceModel(const wchar_t *cascade_name, const wchar_t *model_name, TFaceModelType type, int dkId)
{	
	TocvDataKeeper *dk = ocvne_getDK(dkId);
	if (dk)
		return dk->LoadClassifierAndFaceModel(cascade_name, model_name, type);
	else
		return OCW_ERR_NULL_OBJECT;
}
//-------------------------------------------------------------------------------------------------
// DLL entry - face detect
//-------------------------------------------------------------------------------------------------
int ocvFaceLandmarkDetector(TImgSelect type, int dkId, bool singleFace, bool extrapolate, double scaleFactor, unsigned int minNeighbors, unsigned int minSize)
{
	int nRet = ocvne_checkImgAndRoi();
	if (nRet)
		return nRet;
	TocvDataKeeper *dk = ocvne_getDK(dkId);
	if (dk == 0)
		return OCW_ERR_NULL_OBJECT;
	int idx = (int)type;
	if (idx > 2)
		return OCW_ERR_BAD_INDEX;
	dk->CreateObjData(idx);		// create new data
	SocvObjectData *exportData = dk->ObjData[idx];
	//extrapolated points - forehead
	unsigned int extraNum = extrapolate ? 3 : 0;
	std::vector<Point2f> extraPts;
	std::vector<Rect> faces;
	std::vector<std::vector<Point2f> > landmarks;
	CascadeClassifier face_cascade = dk->CascadeClassifier;
	Ptr<face::Facemark> facemark = dk->FaceMark;
	Mat img, gray, exportBrects, exportLpts;
	int *dataBrects;
	float *dataLpts;
	if (type == OCW_IMAGE_SOURCE)
		img = roimSource;
	else if (type == OCW_IMAGE_TARGET)
		img = roimTarget;
	if (img.channels() > 1)
		cvtColor(img, gray, COLOR_BGR2GRAY);
	else
		gray = img.clone();
	equalizeHist(gray, gray);
	face_cascade.detectMultiScale(gray, faces, scaleFactor, minNeighbors, 0, Size(minSize, minSize));
	if (faces.empty())
		return OCW_ERR_EMPTY_DATA;
	int mrArea = 0;
	unsigned int mrIdx = 0;
	if (facemark->fit(img, faces, landmarks))
	{
		for (unsigned int i = 0; i < faces.size(); i++)
		{
			if (singleFace)
			{
				if (faces[i].area() > mrArea)
				{
					mrArea = faces[i].area();
					mrIdx = i;
				}
			}
		}
		if (singleFace)
		{
			exportData->numObjects = 1;
			unsigned int lpNum = landmarks[mrIdx].size() + extraNum;
			dataBrects = new int[5];
			dataLpts = new float[2 * lpNum];
			exportBrects = Mat(1, 5, CV_32SC1, dataBrects);
			exportLpts = Mat(lpNum, 2, CV_32FC1, dataLpts);
			//
			exportBrects.at<int>(0, 0) = faces[mrIdx].x;
			exportBrects.at<int>(0, 1) = faces[mrIdx].y;
			exportBrects.at<int>(0, 2) = faces[mrIdx].width;
			exportBrects.at<int>(0, 3) = faces[mrIdx].height;
			exportBrects.at<int>(0, 4) = lpNum;
			//
			for (unsigned int k = 0; k < landmarks[mrIdx].size(); k++)
			{
				exportLpts.at<float>(k, 0) = landmarks[mrIdx][k].x;
				exportLpts.at<float>(k, 1) = landmarks[mrIdx][k].y;
			}
			// get extrapolation points (forehead)
			if (extrapolate)
			{
				extraPts = TCommonHelper::swpGetExtraPoints(landmarks[mrIdx]);
				for (unsigned int k = 0; k < extraPts.size(); k++)
				{
					int jj = landmarks[mrIdx].size() + k;
					exportLpts.at<float>(jj, 0) = extraPts[k].x;
					exportLpts.at<float>(jj, 1) = extraPts[k].y;
				}
			}
		}
		else
		{
			exportData->numObjects = faces.size();
			dataBrects = new int[5 * faces.size()];
			unsigned int lpNum = 0;
			for (unsigned long i = 0; i < faces.size(); i++)
				lpNum += landmarks[i].size();
			dataLpts = new float[2 * lpNum];
			exportBrects = Mat(faces.size(), 5, CV_32SC1, dataBrects);
			exportLpts = Mat(lpNum, 2, CV_32FC1, dataLpts);
			//
			for (unsigned int i = 0; i < faces.size(); i++)
			{
				exportBrects.at<int>(i, 0) = faces[i].x;
				exportBrects.at<int>(i, 1) = faces[i].y;
				exportBrects.at<int>(i, 2) = faces[i].width;
				exportBrects.at<int>(i, 3) = faces[i].height;
				exportBrects.at<int>(i, 4) = landmarks[i].size();				
				for (unsigned int k = 0; k < landmarks[mrIdx].size(); k++)
				{
					exportLpts.at<float>(k, 0) = landmarks[i][k].x;
					exportLpts.at<float>(k, 1) = landmarks[i][k].y;
				}
			}
		}
		exportData->bRects = (int*)exportBrects.data;
		exportData->lPts = (float*)exportLpts.data;
	}
	return 0;
}
//-------------------------------------------------------------------------------------------------
// DLL entry - face swap
//-------------------------------------------------------------------------------------------------
int ocvSwapFaces(TImgSelect type1, TImgSelect type2, int dkId)
{
	// we assume that input and output images exists and that landmark detection is calculate
	int nRet = ocvne_checkImgAndRoi();
	if (nRet)
		return nRet;

	TocvDataKeeper *dk = ocvne_getDK(dkId);
	if (dk == 0)
		return OCW_ERR_NULL_OBJECT;
	Point2f lp;
	float *lpPtr;
	int *brPtr, numMarks;
	std::vector<Point2f> points1, points2;
	// face 1
	int idx = (int)type1;
	if (idx > 2)
		return OCW_ERR_BAD_INDEX;
	if (dk->ObjData[idx] == 0)
		return OCW_ERR_EMPTY_DATA;
	if (dk->ObjData[idx]->numObjects == 0)
		return OCW_ERR_EMPTY_DATA;
	brPtr = dk->ObjData[idx]->bRects;
	lpPtr = dk->ObjData[idx]->lPts;
	numMarks = brPtr[4];
	for (int i = 0; i < numMarks; i++)
	{
		lp.x = *lpPtr++;
		lp.y = *lpPtr++;
		points1.push_back(lp);
	}
	// face 2
	idx = (int)type2;
	if (idx > 2)
		return OCW_ERR_BAD_INDEX;
	if (dk->ObjData[idx] == 0)
		return OCW_ERR_EMPTY_DATA;
	if (dk->ObjData[idx]->numObjects == 0)
		return OCW_ERR_EMPTY_DATA;
	brPtr = dk->ObjData[idx]->bRects;
	lpPtr = dk->ObjData[idx]->lPts;
	numMarks = brPtr[4];
	for (int i = 0; i < numMarks; i++)
	{
		lp.x = *lpPtr++;
		lp.y = *lpPtr++;
		points2.push_back(lp);
	}
	Mat img1, img2, img1Warped;
	// input image
	if (type1 == OCW_IMAGE_SOURCE)
		roimSource.convertTo(img1, CV_32F);
	else if (type1 == OCW_IMAGE_TARGET)
		roimTarget.convertTo(img1, CV_32F);
	// output
	if (type2 == OCW_IMAGE_SOURCE)
		img2 = roimSource;
	else if (type2 == OCW_IMAGE_TARGET)
		img2 = roimTarget;
	img2.convertTo(img1Warped, CV_32F);
	//unsigned int numswaps = (unsigned int)std::min((unsigned int)points1.size(), (unsigned int)points2.size());
	// Find convex hull
	std::vector<Point2f> boundary_image1;
	std::vector<Point2f> boundary_image2;
	std::vector<int> index;
	//convexHull(Mat(points2), index, false, false);
	convexHull(points2, index, false, false);
	for (size_t i = 0; i < index.size(); i++)
		{
		boundary_image1.push_back(points1[index[i]]);
		boundary_image2.push_back(points2[index[i]]);
		}
	// Triangulation for points on the convex hull
	std::vector< std::vector<int> > triangles;
	Rect rect(0, 0, img1Warped.cols, img1Warped.rows);
	TCommonHelper::swpDivideIntoTriangles(rect, boundary_image2, triangles);
	// Apply affine transformation to Delaunay triangles
	for (size_t i = 0; i < triangles.size(); i++)
	{
		std::vector<Point2f> triangle1, triangle2;
		// Get points for img1, img2 corresponding to the triangles
		for (int j = 0; j < 3; j++)
		{
			triangle1.push_back(boundary_image1[triangles[i][j]]);
			triangle2.push_back(boundary_image2[triangles[i][j]]);
		}
		TCommonHelper::swpWarpTriangle(img1, img1Warped, triangle1, triangle2);
	}
	// Calculate mask
	std::vector<Point> hull;
	for (size_t i = 0; i < boundary_image2.size(); i++)
	{
		Point pt((int)boundary_image2[i].x, (int)boundary_image2[i].y);
		hull.push_back(pt);
	}
	Mat mask = Mat::zeros(img2.rows, img2.cols, img2.depth());
	fillConvexPoly(mask, &hull[0], (int)hull.size(), Scalar(255, 255, 255));
	// Clone seamlessly.
	Rect r = boundingRect(boundary_image2);
	Point center = (r.tl() + r.br()) / 2;
	Mat output;
	img1Warped.convertTo(img1Warped, CV_8UC3);
	//img1Warped.copyTo(img2); // ok
	//mask.copyTo(img2); // ok
	seamlessClone(img1Warped, img2, mask, center, img2, NORMAL_CLONE);
	return 0;
}
//-------------------------------------------------------------------------------------------------
// DLL entry - find contours
//-------------------------------------------------------------------------------------------------
int ocvFindContours(TEdgeType edgeType, TContourRetrievalMode retMode, TContourApproximationMode appMode, SocvContourData *exportContours)
{
	//TODO: raspisati s razlièitim parametrima
	int nRet = ocvne_checkImgAndRoi();
	if (nRet)
		return nRet;
	std::vector<Vec4i> hierarchy;
	std::vector <std::vector<Point>> contours;
	Mat gray = Mat(roimSource.rows, roimSource.cols, CV_8UC1);
	cvtColor(roimSource, gray, COLOR_BGR2GRAY);
	double thrHigh;
	thrHigh = threshold(gray, gray, 0, 255, THRESH_BINARY + THRESH_OTSU);
	switch (edgeType)
	{
	case OCW_ET_CANNY:
		Canny(gray, gray, thrHigh*0.5, thrHigh);
		break;
	case OCW_ET_SOBEL_V:
		Sobel(gray, gray, gray.depth(), 1, 0);
		break;
	case OCW_ET_SOBEL_H:
		Sobel(gray, gray, gray.depth(), 0, 1);
		break;
	default:
		break;
	}
	findContours(gray, contours, hierarchy, retMode, appMode);
	exportContours->numContours = hierarchy.size();
	int *dataH;
	SocvContourPts *dataC;
	dataH = new int[4 * exportContours->numContours];
	Mat exportH = Mat(exportContours->numContours, 4, CV_32SC1, dataH);
	exportContours->contours = new SocvContourPts[exportContours->numContours];
	dataC = exportContours->contours;
	for (size_t i = 0; i < exportContours->numContours; i++)
	{
		// export hierarchy
		exportH.at<int>(i, 0) = (int)hierarchy[i][0];
		exportH.at<int>(i, 1) = (int)hierarchy[i][1];
		exportH.at<int>(i, 2) = (int)hierarchy[i][2];
		exportH.at<int>(i, 3) = (int)hierarchy[i][3];
		dataC[i].numPts = contours[i].size();
		dataC[i].points = new SocvPoint[dataC[i].numPts];
		for (size_t k = 0; k < contours[i].size(); k++)
		{
		   // export contour points	
		   dataC[i].points[k].x = (int)contours[i][k].x;
		   dataC[i].points[k].y = (int)contours[i][k].y;
		}
	}
	exportContours->hierarchy = (int*)exportH.data;
	// test
    // iterate through all the top-level contours,
    // draw each connected component with its own random color
	/*
	roimTarget = cv::Scalar(0, 0, 0);
	int idx = 0;
    for( ; idx >= 0; idx = hierarchy[idx][0] )
    {
        Scalar color( rand()&255, rand()&255, rand()&255 );
		if (contours[idx].size() < 2)
			continue;
        drawContours( roimTarget, contours, idx, color, FILLED, 8, hierarchy );
    }
	*/
	return 0;
}
//-------------------------------------------------------------------------------------------------
// DLL entry - release contours data
//-------------------------------------------------------------------------------------------------
int ocvReleaseContours(SocvContourData *importContours)
{
	if (importContours)
	{
		// release memory
		for (unsigned int i = 0; i < importContours->numContours; i++)
		{
			SocvPoint *pts = importContours->contours[i].points;
			delete[]pts;
		}
		delete[]importContours->contours;
		delete[]importContours->hierarchy;
	}
	return 0;
}
//-------------------------------------------------------------------------------------------------
// DLL entry - detect lines
//-------------------------------------------------------------------------------------------------
int ocvDetectLines(TEdgeType type, SocvLineData *exportLines, bool c2bw)
{
	int nRet = ocvne_checkImgAndRoi();
	if (nRet)
		return nRet;
	Mat gray = Mat(roimSource.rows, roimSource.cols, CV_8UC1);
	cvtColor(roimSource, gray, COLOR_BGR2GRAY);
	double thrHigh;
	if (c2bw)
		thrHigh = threshold(gray, gray, 0, 255, THRESH_BINARY + THRESH_OTSU);
	else
	{
		Mat temp = Mat(roimSource.rows, roimSource.cols, CV_8UC1);
		thrHigh = threshold(gray, temp, 0, 255, THRESH_BINARY + THRESH_OTSU);
	}
	switch (type)
	{
		case OCW_ET_CANNY:
			Canny(gray, gray, thrHigh*0.5, thrHigh);
		break;
		case OCW_ET_SOBEL_V:
			Sobel(gray, gray, gray.depth(), 1, 0);
		break;
		case OCW_ET_SOBEL_H:
			Sobel(gray, gray, gray.depth(), 0, 1);
		break;
		default:
		break;
	}
	cvtColor(gray, roimTarget, COLOR_GRAY2BGR);
	std::vector<Vec4f> lines;
	Ptr<LineSegmentDetector> detector = createLineSegmentDetector(LSD_REFINE_STD);
	detector->detect(gray, lines);
	exportLines->numLines = lines.size();
	if (lines.empty())
		return OCW_ERR_EMPTY_DATA;
	float *dataLines;
	dataLines = new float[4 * exportLines->numLines];
	Mat exportLSD = Mat(exportLines->numLines, 4, CV_32FC1, dataLines);
	for (size_t i = 0; i < lines.size(); i++)
	{
		exportLSD.at<float>(i, 0) = lines[i][0];
		exportLSD.at<float>(i, 1) = lines[i][1];
		exportLSD.at<float>(i, 2) = lines[i][2];
		exportLSD.at<float>(i, 3) = lines[i][3];
	}
	exportLines->line = (void*)exportLSD.data;
	return 0;
}
//-------------------------------------------------------------------------------------------------
// DLL entry - hough lines
//-------------------------------------------------------------------------------------------------
int ocvHoughLines(TEdgeType type, SocvLineData *exportLines, bool c2bw)
{
	int nRet = ocvne_checkImgAndRoi();
	if (nRet)
		return nRet;
	Mat gray = Mat(roimSource.rows, roimSource.cols, CV_8UC1);
	cvtColor(roimSource, gray, COLOR_BGR2GRAY);
	double thrHigh;
	if (c2bw)
		thrHigh = threshold(gray, gray, 0, 255, THRESH_BINARY + THRESH_OTSU);
	else
	{
		Mat temp = Mat(roimSource.rows, roimSource.cols, CV_8UC1);
		thrHigh = threshold(gray, temp, 0, 255, THRESH_BINARY + THRESH_OTSU);
	}
	switch (type)
	{
	case OCW_ET_CANNY:
		Canny(gray, gray, thrHigh*0.5, thrHigh); 
	break;
	case OCW_ET_SOBEL_V:
		Sobel(gray, gray, gray.depth(), 1, 0);
	break;
	case OCW_ET_SOBEL_H:
		Sobel(gray, gray, gray.depth(), 0, 1);
	break;
	default:
	break;
	}
	cvtColor(gray, roimTarget, COLOR_GRAY2BGR);
	std::vector<Vec4i> lines;
	HoughLinesP(gray, lines, 1, CV_PI / 180, 80, 150, 10); // runs the actual detection
	exportLines->numLines = lines.size();
	if (lines.empty())
		return OCW_ERR_EMPTY_DATA;
	float *dataLines;
	dataLines = new float[4 * exportLines->numLines];
	Mat exportLSD = Mat(exportLines->numLines, 4, CV_32FC1, dataLines);
	for (size_t i = 0; i < lines.size(); i++)
	{
		exportLSD.at<float>(i, 0) = (float)lines[i][0];
		exportLSD.at<float>(i, 1) = (float)lines[i][1];
		exportLSD.at<float>(i, 2) = (float)lines[i][2];
		exportLSD.at<float>(i, 3) = (float)lines[i][3];
	}
	exportLines->line = (void*)exportLSD.data;
	return 0;
}
//-------------------------------------------------------------------------------------------------
// DLL entry - Gabor filter
//-------------------------------------------------------------------------------------------------
int ocvGaborFilter(unsigned int kernelSize, double theta, double sigma, double lambda, double gamma, double psi, double ksFactor)
{
	int nRet = ocvne_checkImgAndRoi();
	if (nRet)
		return nRet;
	Mat roimFloat;
	roimSource.convertTo(roimFloat, CV_32FC3, 1.0/255.0);
	Mat kernel = cv::getGaborKernel(cv::Size(kernelSize, kernelSize), sigma, theta, lambda, gamma, psi, CV_32F);
	if (ksFactor > 0)
		multiply(kernel, 1.0 / (ksFactor *sum(kernel)), kernel);
	filter2D(roimFloat, roimFloat, CV_32F, kernel);
	roimFloat.convertTo(roimTarget, CV_8UC3, 255.0);	
	return 0;
}
//-------------------------------------------------------------------------------------------------
// DLL entry - multiple gabor filters - fractalius like effect 
//-------------------------------------------------------------------------------------------------
int ocvGaborius(unsigned int step, unsigned int kernelSize, double sigma, double lambda, double gamma, double psi, double ksFactor)
{
	int nRet = ocvne_checkImgAndRoi();
	if (nRet)
		return nRet;
	Mat roimFin;
	roimSource.convertTo(roimFin, CV_32FC3);
	Mat res = Mat::zeros(roimFin.size(), CV_32FC3);
	Mat fout = Mat::zeros(roimFin.size(), CV_32FC3);
	Mat kernel;
	double theta = 0;
	for (unsigned int i = 0; i < step; i++)
	{
		kernel = cv::getGaborKernel(Size(kernelSize, kernelSize), sigma, theta, lambda, gamma, psi, CV_32F);
		multiply(kernel, 1.0 / (ksFactor *sum(kernel)), kernel);
		filter2D(roimFin, fout, CV_32F, kernel);
		theta += CV_PI / step;
		res = max(res, fout);
	}
	res.convertTo(roimTarget, CV_8UC3);
	return 0;
}
//-------------------------------------------------------------------------------------------------
// DLL entry - multiple oriented filters - another fractalius like effect
//-------------------------------------------------------------------------------------------------
int ocvTrigonometrius(int kFunc, unsigned int step, unsigned int kernelSize, double hSigma, double freq, double ksFactor, int  margin, double vSigma)
{
	// petrius 
	int nRet = ocvne_checkImgAndRoi();
	if (nRet)
		return nRet;
	Mat roimFin;
	roimSource.convertTo(roimFin, CV_32FC3);
	Mat res = Mat::zeros(roimFin.size(), CV_32FC3);
	Mat fout = Mat::zeros(roimFin.size(), CV_32FC3);
	// kernel s nekakvim kvazi waveletom
	double theta = 0.0, theta_step = 180.0/step;
	for (unsigned int i = 0; i < step; i++)
	{
		Mat kernel = TCommonHelper::spGenerateTrigonoKernel(kFunc, kernelSize, hSigma, theta, freq, margin, vSigma);
		multiply(kernel, 1.0 / (ksFactor * sum(kernel)), kernel);
		filter2D(roimFin, fout, CV_32F, kernel);
		theta += theta_step;
		res = max(res, fout);
	}
	res.convertTo(roimTarget, CV_8UC3);
	return 0;
}
//-------------------------------------------------------------------------------------------------
// DLL entry - features2d detect and compute
//-------------------------------------------------------------------------------------------------
int ocvCalcFeatures2D(TFeature2DType type, TFeature2DChoice choice, int maxSrcKeys, int maxTgtKeys)
{
	int nRet = ocvne_checkImgAndRoi();
	if (nRet)
		return nRet;
	// TODO: create feature matching/homography class
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
			ocvClearFeatures2D();
			sm = (choice == OCW_FCH_SRCTGT_MASK && !maskSource.empty());
			tm = (choice == OCW_FCH_SRCTGT_MASK && !maskTarget.empty());
			break;
	}
	if (sm)
	{
		if (maskSource.rows == roimSource.rows && maskSource.cols == roimSource.cols)
			maskSrc = maskSource;
	}
	if (tm)
	{
		if (maskTarget.rows == roimTarget.rows && maskTarget.cols == roimTarget.cols)
			maskTgt = maskTarget;
	}
	//
	Mat imSourceGray = Mat(roimSource.rows, roimSource.cols, CV_8UC1);
	Mat imTargetGray = Mat(roimTarget.rows, roimTarget.cols, CV_8UC1);
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
	return 0;
}
//-------------------------------------------------------------------------------------------------
// DLL entry - calculate good matches
//-------------------------------------------------------------------------------------------------
int ocvCalcMatches(TMatchType type, float matchParam, bool exportImage)
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
		drawMatches(imgSource, keypointsSource, imgTarget, keypointsTarget, matches, imgExport);
		alphaExport = Mat();
	}
	else
		imgExport = Mat();
	// save for recalculation
	lastHomography.mType = type;
	lastHomography.matchParam = matchParam;
	lastHomography.matches = (int)matches.size();
	return 0; 
}
//-------------------------------------------------------------------------------------------------
// DLL entry - calculate homography
//-------------------------------------------------------------------------------------------------
int ocvCalcHomography(unsigned int minMatches, THomographyType type, bool exportImage, bool addAlpha, bool warp2SS)
{
	//TODO: check if previous steps are fullfiled
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
			imgExport = Mat();
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
		Size size = warp2SS ? imgSource.size() : imgTarget.size();
		warpPerspective(imgSource, imgExport, homography, size);
		if (alphaSource.empty())
		{
			if (addAlpha)
			{
				Mat alpha = Mat(imgSource.rows, imgSource.cols, CV_8UC1, Scalar(255));
				warpPerspective(alpha, alphaExport, homography, size);
			}
			else
				alphaExport = Mat();
		}
		else
			warpPerspective(alphaSource, alphaExport, homography, size);
	}
	else
		imgExport = Mat();
	// save for recalculation
	lastHomography.minMatches = minMatches;
	if (lastHomography.initMinMatches == 0)
		lastHomography.initMinMatches = minMatches;
	lastHomography.hType = type;
	lastHomography.exportImage = exportImage;
	lastHomography.addAlpha = addAlpha;
	lastHomography.warp2SS = warp2SS;
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
// DLL entry - recaulcalte homography (outliers -> matches)
//-------------------------------------------------------------------------------------------------
int ocvReduceFeatures2D(TFeature2DReduction redType, bool exportMatchImage)
{
	if (homoMask.step == 0)
		return OCW_ERR_EMPTY_DATA;
	if (matches.size() != homoMask.rows)
		return  OCW_ERR_BAD_DATA;  //bad data
	bool value;
	int remRows, rcMatch;
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
			rcMatch = 0;
			break;
		case  OCW_HOSR_RKD_ASIS:
		case  OCW_HOSR_RKD_ADJUST:
			if (!lastHomography.exportImage)
				return OCW_ERR_NO_EXPORT;
			if (imgExport.empty())
				return OCW_ERR_NO_EXPORT;
			remRows = TCommonHelper::spReduceKpds(keypointsTarget, descriptorsTarget, alphaExport, 0x00, lastHomography.initMinMatches);
			if (remRows == 0)
				return OCW_ERR_EMPTY_DATA;
			// rule of thumb: we'll change minmatches to 1/4 of previous match ti avoid "stupid" warping
			if (redType == OCW_HOSR_RKD_ADJUST)
			{
				lastHomography.minMatches = (int)matches.size() >> 2; // at least 1/4 of original
				if (lastHomography.minMatches < lastHomography.initMinMatches)
					lastHomography.minMatches = lastHomography.initMinMatches;
			}
			rcMatch = ocvCalcMatches(lastHomography.mType, lastHomography.matchParam, exportMatchImage); 
			break;
		default:	// target mask
			if (maskTarget.empty())
				return OCW_ERR_EMPTY_DATA;
			if (maskTarget.rows != roimTarget.rows && maskTarget.cols != roimTarget.cols)
				return  OCW_ERR_BAD_DATA;  
			remRows = TCommonHelper::spReduceKpds(keypointsTarget, descriptorsTarget, maskTarget, 0x00, lastHomography.initMinMatches);
			if (remRows == 0)
				return OCW_ERR_EMPTY_DATA;
			rcMatch = ocvCalcMatches(lastHomography.mType, lastHomography.matchParam, exportMatchImage); 
			break;
	}
	return rcMatch;
}
//-------------------------------------------------------------------------------------------------
// DLL entry -  recalc homography
//-------------------------------------------------------------------------------------------------
int ocvRecalcHomography(void)
{
	if (homoMask.step == 0)
		return OCW_ERR_EMPTY_DATA;
	return ocvCalcHomography(lastHomography.minMatches, lastHomography.hType, lastHomography.exportImage, lastHomography.addAlpha, lastHomography.warp2SS);
}
//-------------------------------------------------------------------------------------------------
// DLL entry -  get homography data
//-------------------------------------------------------------------------------------------------
int ocvGetHomographyData(SocvHomography &homoData)
{
	homoData = lastHomography;
	return 0;
}
//-------------------------------------------------------------------------------------------------
// DLL entry - clear features data
//-------------------------------------------------------------------------------------------------
int ocvClearFeatures2D(void)
{
	matches.clear();
	keypointsSource.clear();
	keypointsTarget.clear();
	descriptorsSource = Mat();
	descriptorsTarget = Mat();
	return 0;
}
//-------------------------------------------------------------------------------------------------
// DLL entry - inpaint
//-------------------------------------------------------------------------------------------------
int ocvInpaint(TInpaintType type, double radius)
{
	int nRet = ocvne_checkImgAndRoi();
	if (nRet)
		return nRet;
	if (maskSource.empty())
		return OCW_ERR_NO_SOURCE_MASK;
	if (maskSource.channels() != 1)
		return OCW_ERR_BAD_SOURCE_MASK;
	Mat output, mask = maskSource;
	threshold(mask, mask, 127, 255, THRESH_BINARY | THRESH_OTSU);
	switch (type)
		{
		case OCW_INPAINT_NS:
			inpaint(imgSource, mask, imgTarget, radius, INPAINT_NS);
			break;
		case OCW_INPAINT_TELEA:
			inpaint(imgSource, mask, imgTarget, radius, INPAINT_TELEA);
			break;
		case OCW_INPAINT_FUZZY_ONESTEP:
			bitwise_not(mask, mask);
			ft::inpaint(imgSource, mask, output, (int)radius, ft::LINEAR, ft::ONE_STEP);
			output.convertTo(imgTarget, CV_8U, 1, 0);
			break;
		case OCW_INPAINT_FUZZY_MULTISTEP:
			bitwise_not(mask, mask);
			ft::inpaint(imgSource, mask, output, (int)radius, ft::LINEAR, ft::MULTI_STEP);
			output.convertTo(imgTarget, CV_8U, 1, 0);
			break;
		case OCW_INPAINT_FUZZY_ITERATIVE:
			bitwise_not(mask, mask);
			ft::inpaint(imgSource, mask, output, (int)radius, ft::LINEAR, ft::ITERATIVE);
			output.convertTo(imgTarget, CV_8U, 1, 0);
			break;
		case OCW_INPAINT_FSR_BEST:
			// 32 bit is extremely slow.
			bitwise_not(mask, mask);
			xphoto::inpaint(imgSource, mask, imgTarget, xphoto::INPAINT_FSR_BEST);
			break;
		case OCW_INPAINT_FSR_FAST:
			bitwise_not(mask, mask);
			xphoto::inpaint(imgSource, mask, imgTarget, xphoto::INPAINT_FSR_FAST);
			break;
		case OCW_INPAINT_SHIFT_MAP:
			// 32 bit is extremely slow. On big images throws excpetion after looong execution time
			{	
			Mat inpC, outC;
			bitwise_not(mask, mask);
			cvtColor(imgSource, inpC, COLOR_BGR2Lab);
			outC = Mat::zeros(inpC.rows, inpC.cols, inpC.depth());
			xphoto::inpaint(inpC, mask, outC, xphoto::INPAINT_SHIFTMAP);
			cvtColor(outC, imgTarget, COLOR_Lab2BGR);
			}
			break;
		default:
			return OCW_ERR_BAD_TYPE;
		}
	return 0;
}
//-------------------------------------------------------------------------------------------------
// DLL entry - DNN - colorization
//-------------------------------------------------------------------------------------------------
int ocvColorize(int dkId)
{
	int nRet = ocvne_checkImgAndRoi();
	if (nRet)
		return nRet;
	TocvDataKeeper *dk = ocvne_getDK(dkId);
	if (dk == 0)
		return OCW_ERR_NULL_OBJECT;
	// the 313 ab cluster centers from pts_in_hull.npy (already transposed)
	static float hull_pts[] = {
		-90., -90., -90., -90., -90., -80., -80., -80., -80., -80., -80., -80., -80., -70., -70., -70., -70., -70., -70., -70., -70.,
		-70., -70., -60., -60., -60., -60., -60., -60., -60., -60., -60., -60., -60., -60., -50., -50., -50., -50., -50., -50., -50., -50.,
		-50., -50., -50., -50., -50., -50., -40., -40., -40., -40., -40., -40., -40., -40., -40., -40., -40., -40., -40., -40., -40., -30.,
		-30., -30., -30., -30., -30., -30., -30., -30., -30., -30., -30., -30., -30., -30., -30., -20., -20., -20., -20., -20., -20., -20.,
		-20., -20., -20., -20., -20., -20., -20., -20., -20., -10., -10., -10., -10., -10., -10., -10., -10., -10., -10., -10., -10., -10.,
		-10., -10., -10., -10., 0., 0., 0., 0., 0., 0., 0., 0., 0., 0., 0., 0., 0., 0., 0., 0., 0., 0., 10., 10., 10., 10., 10., 10., 10.,
		10., 10., 10., 10., 10., 10., 10., 10., 10., 10., 10., 20., 20., 20., 20., 20., 20., 20., 20., 20., 20., 20., 20., 20., 20., 20.,
		20., 20., 20., 30., 30., 30., 30., 30., 30., 30., 30., 30., 30., 30., 30., 30., 30., 30., 30., 30., 30., 30., 40., 40., 40., 40.,
		40., 40., 40., 40., 40., 40., 40., 40., 40., 40., 40., 40., 40., 40., 40., 40., 50., 50., 50., 50., 50., 50., 50., 50., 50., 50.,
		50., 50., 50., 50., 50., 50., 50., 50., 50., 60., 60., 60., 60., 60., 60., 60., 60., 60., 60., 60., 60., 60., 60., 60., 60., 60.,
		60., 60., 60., 70., 70., 70., 70., 70., 70., 70., 70., 70., 70., 70., 70., 70., 70., 70., 70., 70., 70., 70., 70., 80., 80., 80.,
		80., 80., 80., 80., 80., 80., 80., 80., 80., 80., 80., 80., 80., 80., 80., 80., 90., 90., 90., 90., 90., 90., 90., 90., 90., 90.,
		90., 90., 90., 90., 90., 90., 90., 90., 90., 100., 100., 100., 100., 100., 100., 100., 100., 100., 100., 50., 60., 70., 80., 90.,
		20., 30., 40., 50., 60., 70., 80., 90., 0., 10., 20., 30., 40., 50., 60., 70., 80., 90., -20., -10., 0., 10., 20., 30., 40., 50.,
		60., 70., 80., 90., -30., -20., -10., 0., 10., 20., 30., 40., 50., 60., 70., 80., 90., 100., -40., -30., -20., -10., 0., 10., 20.,
		30., 40., 50., 60., 70., 80., 90., 100., -50., -40., -30., -20., -10., 0., 10., 20., 30., 40., 50., 60., 70., 80., 90., 100., -50.,
		-40., -30., -20., -10., 0., 10., 20., 30., 40., 50., 60., 70., 80., 90., 100., -60., -50., -40., -30., -20., -10., 0., 10., 20.,
		30., 40., 50., 60., 70., 80., 90., 100., -70., -60., -50., -40., -30., -20., -10., 0., 10., 20., 30., 40., 50., 60., 70., 80., 90.,
		100., -80., -70., -60., -50., -40., -30., -20., -10., 0., 10., 20., 30., 40., 50., 60., 70., 80., 90., -80., -70., -60., -50.,
		-40., -30., -20., -10., 0., 10., 20., 30., 40., 50., 60., 70., 80., 90., -90., -80., -70., -60., -50., -40., -30., -20., -10.,
		0., 10., 20., 30., 40., 50., 60., 70., 80., 90., -100., -90., -80., -70., -60., -50., -40., -30., -20., -10., 0., 10., 20., 30.,
		40., 50., 60., 70., 80., 90., -100., -90., -80., -70., -60., -50., -40., -30., -20., -10., 0., 10., 20., 30., 40., 50., 60., 70.,
		80., -110., -100., -90., -80., -70., -60., -50., -40., -30., -20., -10., 0., 10., 20., 30., 40., 50., 60., 70., 80., -110., -100.,
		-90., -80., -70., -60., -50., -40., -30., -20., -10., 0., 10., 20., 30., 40., 50., 60., 70., 80., -110., -100., -90., -80., -70.,
		-60., -50., -40., -30., -20., -10., 0., 10., 20., 30., 40., 50., 60., 70., -110., -100., -90., -80., -70., -60., -50., -40., -30.,
		-20., -10., 0., 10., 20., 30., 40., 50., 60., 70., -90., -80., -70., -60., -50., -40., -30., -20., -10., 0.
	};
	// fixed input size for the pretrained network
	const int W_in = 224;
	const int H_in = 224;
	// setup additional layers
	int sz[] = { 2, 313, 1, 1 };
	const Mat pts_in_hull(4, sz, CV_32F, hull_pts);
	Ptr<dnn::Layer> class8_ab = dk->DnnNetwork.getLayer("class8_ab");
	class8_ab->blobs.push_back(pts_in_hull);
	Ptr<dnn::Layer> conv8_313_rh = dk->DnnNetwork.getLayer("conv8_313_rh");
	conv8_313_rh->blobs.push_back(Mat(1, 313, CV_32F, Scalar(2.606)));
	// extract L channel and subtract mean
	Mat lab, L, input;
	Mat imgWork;
	//roimSource.convertTo(imgWork, CV_8U, 1, 0);
	//imgWork.convertTo(imgWork, CV_32F, 1.0 / 255);
	roimSource.convertTo(imgWork, CV_32F, 1.0 / 255);
	cvtColor(imgWork, lab, COLOR_BGR2Lab);
	extractChannel(lab, L, 0);
	resize(L, input, Size(W_in, H_in));
	input -= 50;
	// run the L channel through the network
	Mat inputBlob = dnn::blobFromImage(input);
	dk->DnnNetwork.setInput(inputBlob);
	Mat result = dk->DnnNetwork.forward();
	// retrieve the calculated a,b channels from the network output
	Size siz(result.size[2], result.size[3]);
	Mat a = Mat(siz, CV_32F, result.ptr(0, 0));
	Mat b = Mat(siz, CV_32F, result.ptr(0, 1));
	resize(a, a, roimTarget.size());
	resize(b, b, roimTarget.size());
	// merge, and convert back to BGR
	Mat color, chn[] = { L, a, b };
	merge(chn, 3, lab);
	cvtColor(lab, color, COLOR_Lab2BGR);
	//color = color * 255;
	color.convertTo(color, CV_8U, 255);
	color.copyTo(roimTarget);	
	return 0;
}
//-------------------------------------------------------------------------------------------------
// DLL entry - seamless clonning 
//-------------------------------------------------------------------------------------------------
int ocvSeamlessClone(int centerX, int centerY, TCloneType type)
{
	int nRet = ocvne_checkImgAndRoi();
	if (nRet)
		return nRet;
	Mat mask;
	if (maskSource.empty())
		mask = Mat(imgSource.rows, imgSource.cols, CV_8UC1, Scalar(255));
	else
		mask = maskSource;
	Point p;
	p.x = centerX;
	p.y = centerY;
	seamlessClone(imgSource, imgTarget, mask, p, imgTarget, type);
	return 0;
}
//-------------------------------------------------------------------------------------------------
// DLL entry - dnn super resolution (espcn, fsrcnn, lapsrn, edsr)
//-------------------------------------------------------------------------------------------------
int ocvDnnSuperResolution(const wchar_t *model, TDnnSuperResType type, int scale)
{
	int nRet = ocvne_checkImgAndRoi();
	if (nRet)
		return nRet;
	dnn_superres::DnnSuperResImpl sr;
	imgExport = Mat();
	alphaExport = Mat();
	String algorithm;
	std::wstring wmodel(model);
	String path = String(wmodel.begin(), wmodel.end());
	switch (type)
		{
		case OCW_SUPERRES_ESPCN:
			algorithm = "espcn";
			break;
		case OCW_SUPERRES_FSRCNN:
			algorithm = "fsrcnn";
			break;
		case OCW_SUPERRES_LAPSRN:
			algorithm = "lapsrn";
			break;
		case OCW_SUPERRES_EDSR:
			algorithm = "edsr";
			break;
		default:
			return OCW_ERR_BAD_TYPE;
		}
	sr.readModel(path);
	sr.setModel(algorithm, scale);
	sr.upsample(imgSource, imgExport);
	if (imgExport.empty())
		return OCW_ERR_NO_EXPORT;
	if (!alphaSource.empty()) // resize alpha channel (no super resolution)...don't use transparent images.
		resize(alphaSource, alphaExport, imgExport.size());
	return 0;
}
//-------------------------------------------------------------------------------------------------
// DLL entry - color transfer (Color Transfer between Images by Reinhard et al, 2001.)
//-------------------------------------------------------------------------------------------------
int ocvColorTransfer(bool exportImage)
{
	int nRet = ocvne_checkImgAndRoi();
	if (nRet)
		return nRet;
	// LAB images
	Mat sourceLab, targetLab, result;
	// vectors to store splitted channels
    std::vector<Mat> targetChannels;
    std::vector<Mat> resultChannels;
	// variables to store means and standard deviations
    Scalar meanSource, meanTarget, stdDevSource, stdDevTarget;
	// converting to LAB color space
	cvtColor(roimSource, sourceLab, COLOR_BGR2Lab);
    cvtColor(roimTarget, targetLab, COLOR_BGR2Lab);
	// convert to float32
    sourceLab.convertTo(sourceLab, CV_32F);
    targetLab.convertTo(targetLab, CV_32F);
	//spliting channels
    split(targetLab, targetChannels);  
    split(targetLab, resultChannels);  // another time to resultChannels
    // finding means and standard deviations for different channels for source image
    meanStdDev(sourceLab, meanSource, stdDevSource, Mat());
	meanStdDev(targetLab, meanTarget, stdDevTarget, Mat());
	for (int k = 0; k < 3; k++)
		{
		// subtract the mean of destination image (this in a way removes the base color of the destination image).
	    resultChannels[k] = targetChannels[k] - meanTarget.val[k];
		resultChannels[k] = resultChannels[k] * (stdDevSource.val[k]/ stdDevTarget.val[k]);
		resultChannels[k] = resultChannels[k] + meanSource[k];
		}
    // merge result channels 
    merge(resultChannels, result);
	// inRange(result, Scalar(0, 0, 0), Scalar(1.0, 1.0, 1.0), result);
	// convert back from float32
	result.convertTo(result, CV_8UC3);
	if (exportImage)
		{
		alphaExport = Mat();
		cvtColor(result, imgExport, COLOR_Lab2BGR);
		}
	else
		cvtColor(result, roimTarget, COLOR_Lab2BGR);
	return 0;
}
//-------------------------------------------------------------------------------------------------
// DLL entry - static saliency
//-------------------------------------------------------------------------------------------------
int ocvStaticSaliency(TSaliencyType type)
{
	int nRet = ocvne_checkImgAndRoi();
	if (nRet)
		return nRet;
    //Ptr<saliency::Saliency> saliencyAlgorithm;
	imgExport = Mat();
	alphaExport = Mat();
	if (type == OCW_SALIENCY_FINE_GRAINED)
		{
		saliency::StaticSaliencyFineGrained sa;
		//saliencyAlgorithm = saliency::StaticSaliencyFineGrained::create();
		if (!sa.computeSaliency(roimSource, imgExport))
			return OCW_ERR_EXCEPTION;
		sa.computeBinaryMap( imgExport, alphaExport );
		}
	else
		{
		saliency::StaticSaliencySpectralResidual sa;
		if (!sa.computeSaliency(roimSource, imgExport))
			return OCW_ERR_EXCEPTION;			
		sa.computeBinaryMap( imgExport, alphaExport );
		}
	imgExport.convertTo(imgExport, CV_8UC1, 255.0);	
	//imshow("saliency", imgExport);
	return 0;
}
//-------------------------------------------------------------------------------------------------
// DLL entry - fllod fill simple
//-------------------------------------------------------------------------------------------------
extern "C" OCV_API int ocvFloodFillSimple(SocvPoint seed, int tolLo, int tolHi, int connectivity, int fillMode, unsigned char *pixBGR)
{
	// fill with value under the seeding point
	int nRet = ocvne_checkImgAndRoi();
	if (nRet)
		return nRet;
	Point pSeed;
	pSeed.x = seed.x;
	pSeed.y = seed.y;
	Scalar pix;
	if (pixBGR)
		pix = Scalar(pixBGR[0], pixBGR[1], pixBGR[2]);
	else
	{
		Vec3b pc = imgSource.at<cv::Vec3b>(pSeed.y, pSeed.x);
		pix = Scalar(pc[0], pc[1], pc[2]);
	}
	int flags = connectivity  + (fillMode == 1 ? FLOODFILL_FIXED_RANGE : 0);
	if (roimTarget.size() != imgTarget.size())
	{
		pSeed.y = (roimTarget.rows >> 1);
		pSeed.x = (roimTarget.cols >> 1);
	}
	int area = floodFill(roimTarget, pSeed, pix, 0, Scalar(tolLo, tolLo, tolLo), Scalar(tolHi, tolHi, tolHi), flags);
	return 0;
}
//-------------------------------------------------------------------------------------------------
// DLL entry - fllod fill simple
//-------------------------------------------------------------------------------------------------
extern "C" OCV_API int ocvFloodFillMasked(SocvPoint seed, int tolLo, int tolHi, int connectivity, int fillMode, unsigned char *pixBGR, int maskVal)
{
	// fill with value under the seeding point
	int nRet = ocvne_checkImgAndRoi();
	if (nRet)
		return nRet;
	Point pSeed;
	pSeed.x = seed.x;
	pSeed.y = seed.y;
	Scalar pix;
	if (pixBGR)
		pix = Scalar(pixBGR[0], pixBGR[1], pixBGR[2]);
	else
	{
		Vec3b pc = imgSource.at<cv::Vec3b>(pSeed.y, pSeed.x);
		pix = Scalar(pc[0], pc[1], pc[2]);
	}
	int flags = connectivity | FLOODFILL_MASK_ONLY | (maskVal << 8) + (fillMode == 1 ? FLOODFILL_FIXED_RANGE : 0);
	Mat mask(imgSource.rows + 2, imgSource.cols +2, CV_8UC1, Scalar(0));
	int area = floodFill(imgSource, mask, pSeed, pix, 0, Scalar(tolLo, tolLo, tolLo), Scalar(tolHi, tolHi, tolHi), flags);
	Rect roi1( 1, 1, mask.cols - 2, mask.rows - 2); 
	if (maskVal == 255)
		imgTarget.setTo(pix, mask(roi1));
	else // blend manually
	{
		//TODO...future use
		imgTarget.setTo(pix, mask(roi1));
	}
	return 0;
}
//-------------------------------------------------------------------------------------------------
// DLL entry - detect image difference
//-------------------------------------------------------------------------------------------------
extern "C" OCV_API int ocvImageDiff(int patchRadius, int radius, int iterations, int err_tol, int output_threshold, int out_type)
{
	int nRet = ocvne_checkImgAndRoi();
	if (nRet)
		return nRet;
	if  (roimTarget.channels() == 4) // RGBA
		extractChannel(roimTarget, roalOutput, 3);
	else if (roalOutput.empty())
		return OCW_ERR_ALPHATARGET_EMPTY;
	TImageDiff *imd = new TImageDiff(&roimSource, &roimTarget, &roalOutput);
	int ***out = imd->ExecuteDT(patchRadius, radius, iterations, err_tol, output_threshold, out_type);
	int error;
	int margin = 5;	// image margin <- possible missalignement
	//ptRGB errorColor;
	SocvRoi bRect;  // bounding rectangle
	imgExport = Mat(roimTarget.rows, roimTarget.cols, CV_8UC3, Scalar(255, 255, 255));
	alphaExport = Mat::zeros(roalOutput.rows, roalOutput.cols, CV_8UC1);
	bRect.top =  roimTarget.rows;
	bRect.left = roimTarget.cols;
	bRect.bottom = 0;
	bRect.right = 0;
	unsigned char *atap;
	unsigned char *expp;
	for (int i = 0; i < roimTarget.rows; i++)
		{
		if (i < margin)
		   continue;
		else if (i > roimTarget.rows - margin - 1)
		   continue;
		atap = roalOutput.ptr<unsigned char>(i);
		expp = alphaExport.ptr<unsigned char>(i);
		for (int j = 0; j < roimTarget.cols; j++)
			{
			if (j < margin)
			   continue;
			else if (j > roimTarget.cols - margin - 1)
			   continue;
			if (atap[j] == 0x00)
			   continue;
			int x = out[i][j][imd->idX];
			int y = out[i][j][imd->idY];
			error = out[i][j][imd->idError];
			if (error < INT_MAX)
			   {
			   if (error > 0)
				  {
				  expp[j] = (unsigned char)error;	// error value to alpha
				  if (i < bRect.top)
					 bRect.top = i;
				  if (i > bRect.bottom)
					 bRect.bottom = i;
				  if (j < bRect.left)
					 bRect.left = j;
				  if (j > bRect.right)
					 bRect.right = j;
				  }
			   }
			}
		}	
	delete imd;
	return 0;
}
//TODO: grabcut, watershed, hough circles/rects, etc...














