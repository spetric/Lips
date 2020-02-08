#include "stdafx.h"
#include "ocvDataKeeper.h"
using namespace cv;
// constructor
TocvDataKeeper::TocvDataKeeper(int dkId)
{
	DKId = dkId;
	ObjData[0] = ObjData[1] = ObjData[2] = 0;
}
// destructor
TocvDataKeeper::~TocvDataKeeper()
{
	for (int i = 0; i < 3; i++)
		ClearObjData(i);
}
//-------------------------------------------------------------------------------------------------
// Load cascade and model
//-------------------------------------------------------------------------------------------------
int TocvDataKeeper::LoadClassifierAndFaceModel(const wchar_t *cascade_name, const wchar_t *model_name, TFaceModelType type)
{
	std::wstring wcs(cascade_name);
	objCascadeName = String(wcs.begin(), wcs.end());
	std::wstring wms(model_name);
	objModelName = String(wms.begin(), wms.end());
	if (objCascadeName.empty())
		return 1;
	if (objModelName.empty())
		return 2;
	CascadeClassifier.load(objCascadeName);
	switch (type)
	{
		case OCW_FACE_KAZEMI:
			FaceMark = face::createFacemarkKazemi();
			break;
		case OCW_FACE_LBF:
			FaceMark = face::createFacemarkLBF();
			break;
		case OCW_FACE_AAM:
			FaceMark = face::createFacemarkAAM();
			break;
		default:
			return 3;
	}
	FaceMark->loadModel(objModelName);
	return 0;
}
//-------------------------------------------------------------------------------------------------
// Read dnn model 
//-------------------------------------------------------------------------------------------------
int TocvDataKeeper::DnnReadModel(const wchar_t *model_name, const wchar_t *config_name)
{
	std::wstring wsm(model_name);
	dnnModelName = String(wsm.begin(), wsm.end());
	if (config_name)
	{
		std::wstring wsc(config_name);
		dnnConfigName = String(wsc.begin(), wsc.end());
		DnnNetwork = dnn::readNet(dnnModelName, dnnConfigName);
	}
	else
		DnnNetwork = dnn::readNet(dnnModelName);
	return 0;
}
//-------------------------------------------------------------------------------------------------
// Create obj data
//-------------------------------------------------------------------------------------------------
int TocvDataKeeper::CreateObjData(int idx)
{
	if (idx > 2)
		return OCW_ERR_BAD_INDEX;
	ObjData[idx] = new SocvObjectData;	// create new data
	ObjData[idx]->numObjects = 0;
	return 0;
}
//-------------------------------------------------------------------------------------------------
// Clear obj data
//-------------------------------------------------------------------------------------------------
void TocvDataKeeper::ClearObjData(int idx)
{
	if (idx > 2)
		return;
	if (ObjData[idx])
	{
		if (ObjData[idx]->numObjects)
		{
			delete[](ObjData[idx]->bRects);
			delete[](ObjData[idx]->lPts);
		}
		delete ObjData[idx];
		ObjData[idx] = 0;
	}
}
