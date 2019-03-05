#pragma once
#include "ocvGlobals.h"
#include "opencv2/opencv.hpp"
#include "opencv2/face.hpp"
#include "opencv2/objdetect.hpp"
#include "opencv2/fuzzy.hpp"
#include "opencv2/xphoto.hpp"
//
class TocvDataKeeper
{
private:
	cv::String objCascadeName;		// object cascade name
	cv::String objModelName;		// obj model name
	cv::String dnnModelName;		// dnn model name
public:
	TocvDataKeeper(int dkId);
	~TocvDataKeeper();
	// vars
	int DKId;
	cv::Mat ExpImage;
	SocvObjectData *ObjData[3];	// source || target || intermediate
	cv::CascadeClassifier CascadeClassifier;
	cv::Ptr<cv::face::Facemark> FaceMark;
	cv::dnn::Net DnnNetwork;
	// methods
	int LoadClassifierAndFaceModel(const wchar_t *cascade_name, const wchar_t *model_name, TFaceModelType type);
	int DnnReadModel(const wchar_t *model_name);
	int CreateObjData(int idx);
	void ClearObjData(int idx);
};

