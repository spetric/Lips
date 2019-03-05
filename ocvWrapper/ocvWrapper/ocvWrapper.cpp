// ocvWrapper.cpp : Defines the exported functions for the DLL application.
//
#include "stdafx.h"
#include "ocvWrapper.h"
#include "opencv2/opencv.hpp"
#include "opencv2/face.hpp"
#include "opencv2/objdetect.hpp"
#include "opencv2/fuzzy.hpp"
#include "opencv2/xphoto.hpp"
#include "TCommonHelper.h"
#include "ocvDataKeeper.h"
using namespace cv;
// global objects
static Mat imgSource, imgTarget, imgIntermediate, imgExport, maskSource, maskTarget;
static Mat alphaSource, alphaTarget, alphaIntermediate, alphaExport;
static Rect roiSource, roiTarget, roiIntermediate;		// roi rectangles
static Mat roimSource, roimTarget, roimIntermediate;	// roi image Mats
static Mat roalInput, roalOutput, roalIntermediate;		// roi alpha Mats
static std::vector<TocvDataKeeper*> dataKeeperList;
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
	alphaExport = Mat(); // erese always
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
	case OCW_IMG_INTERMEDIATE_RGB:
		imgIntermediate = Mat(height, width, CV_8UC3, imageBuff, imageStride);
		if (alphaBuff)
			alphaIntermediate = Mat(height, width, CV_8UC1, alphaBuff, alphaStride);
		else
			alphaIntermediate = Mat();
		roiTarget = Rect();
		break;
	case OCW_IMG_INTERMEDIATE_RGBA:
		imgIntermediate = Mat(height, width, CV_8UC4, imageBuff, imageStride);
		alphaIntermediate = Mat();
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
		roiIntermediate = Rect(roi.left, roi.top, roi.right - roi.left, roi.bottom - roi.top);
	case OCW_ROI_SOURCE:
		roiSource = Rect(roi.left, roi.top, roi.right - roi.left, roi.bottom - roi.top);
		break;
	case OCW_ROI_TARGET:
		roiTarget = Rect(roi.left, roi.top, roi.right - roi.left, roi.bottom - roi.top);
		break;
	case OCW_ROI_INTERMEDIATE:
		roiIntermediate = Rect(roi.left, roi.top, roi.right - roi.left, roi.bottom - roi.top);
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
		roiIntermediate = Rect();
	case OCW_ROI_SOURCE:
		roiSource = Rect();
		break;
	case OCW_ROI_TARGET:
		roiTarget = Rect();
		break;
	case OCW_ROI_INTERMEDIATE:
		roiIntermediate = Rect();
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
		case OCW_IMAGE_INTERMEDIATE:
			inp = roimIntermediate;
		break;
		case OCW_IMAGE_TARGET:
			inp = roimTarget;
		break;
		default:
			inp = roimSource;
		break;
	}
	switch (outType)
	{
	case OCW_IMAGE_INTERMEDIATE:
		out = roimIntermediate;
		break;
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
int ocvWarpPerspective(const SocvWarpParams &wparams, bool exportImage,  int flag, TBorderType borderType)
{
	int nRet = ocvne_checkImgAndRoi();
	if (nRet)
		return nRet;
	Point2f srcPt[4], dstPt[4];
	for (int i = 0; i < 4; i++)
	{
		srcPt[i].x = wparams.src[i][0];
		srcPt[i].y = wparams.src[i][1];
		dstPt[i].x = wparams.dst[i][0];
		dstPt[i].y = wparams.dst[i][1];
	}
	Mat tMatrix = getPerspectiveTransform(srcPt, dstPt);
	if (exportImage)
		{
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
int ocvWarpAffine(const SocvWarpParams &wparams, bool exportImage, int flag, TBorderType borderType)
{
	int nRet = ocvne_checkImgAndRoi();
	if (nRet)
		return nRet;
	Point2f srcPt[3], dstPt[3];
	for (int i = 0; i < 3; i++)
	{
		srcPt[i].x = wparams.src[i][0];
		srcPt[i].y = wparams.src[i][1];
		dstPt[i].x = wparams.dst[i][0];
		dstPt[i].y = wparams.dst[i][1];
	}
	Mat tMatrix = getAffineTransform(srcPt, dstPt);
	if (exportImage)
	{
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
			flip(roimSource, roimSource, flipCode);
	}
	else if (iSel == OCW_IMAGE_TARGET)
	{
		if (imgTarget.empty())
			return OCW_ERR_NO_TARGET;
		else
			flip(roimTarget, roimTarget, flipCode);
	}
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
int ocvDnnReadModel(const wchar_t *model, int dkId)
{
	TocvDataKeeper *dk = ocvne_getDK(dkId);
	if (dk)
		return dk->DnnReadModel(model);
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
	Mat blob;
	if (swapRB)
		dnn::blobFromImage(roimSource, blob, scale, roimSource.size(), Scalar(meanR, meanG, meanB), swapRB, false);
	else
		dnn::blobFromImage(roimSource, blob, scale, roimSource.size(), Scalar(meanB, meanG, meanR), swapRB, false);
	dk->DnnNetwork.setInput(blob);
	Mat prob = dk->DnnNetwork.forward(); 
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
	else
		img = roimIntermediate;
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
	else
		roimIntermediate.convertTo(img1, CV_32F);
	// output
	if (type2 == OCW_IMAGE_SOURCE)
		img2 = roimSource;
	else if (type2 == OCW_IMAGE_TARGET)
		img2 = roimTarget;
	else
		img2 = roimIntermediate;
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
		int test = contours[i].size();
		dataC[i].numPts = contours[i].size();
		dataC[i].points = new SocvPoint[dataC[i].numPts];
		for (size_t k = 0; k < contours[i].size(); k++)
		{
		   // export contour points	
		   dataC[i].points[k].x = (int)contours[i][k].x;
		   dataC[i].points[k].y = (int)contours[i][k].y;
		}
	}
	exportContours->hierarchy = (void*)exportH.data;
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
// DLL entry - inpaint
//-------------------------------------------------------------------------------------------------
int ocvInpaint(unsigned int patchSize, bool zeroSource)
{
	int nRet = ocvne_checkImgAndRoi();
	if (nRet)
		return nRet;
	// test 1
	/* Use for small scrathes
	Mat output;
	dstMask = 255 - dstMask;
	ft::inpaint(imgSource, dstMask, output, 3, ft::LINEAR, ft::ITERATIVE);
	output.convertTo(imgTarget, CV_8U, 1, 0);
	*/
	// test 2
	/* does not work on 32 bit - throws excpetion
	Mat inpC, outC;
	dstMask = 255 - dstMask;
	cvtColor(imgSource, inpC, COLOR_BGR2Lab);
	//outC = inpC.clone();
	xphoto::inpaint(inpC, dstMask, outC, xphoto::INPAINT_SHIFTMAP);
	*/
	return 0;
}
//TODO: grabcut, watershed, hough circles/rects, etc...














