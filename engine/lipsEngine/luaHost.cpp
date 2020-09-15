//---------------------------------------------------------------------------
#pragma hdrstop
#include "luaHost.h"
#include "lipsImageCommon.h"
#include "lipsError.h"
#include <StrUtils.hpp>
//---------------------------------------------------------------------------
#pragma package(smart_init)
//---------------------------------------------------------------------------
// constructor
TLuaHost::TLuaHost(void)
{
OnLuaProgress = 0;
OnLuaSendMessage = 0;
OnLuaRequireParams = 0;
OnLuaRefreshParams = 0;
OnLuaLoadImage = 0;
OnLuaHostDialog = 0;
OnLuaSendCommand = 0;
OnLuaExportRawData = 0;
FOpenCV.sourceId = 0;
FOpenCV.targetId = 0;
FDKIdx = -1;
FExportContours.numContours = 0;
FCurrentContour = 0;
}
//---------------------------------------------------------------------------
// destructor
TLuaHost::~TLuaHost(void)
{
clearData(clearAll);
}
//---------------------------------------------------------------------------
// private - not exposed to Lua
//---------------------------------------------------------------------------
void TLuaHost::clearData(TClearType type)
{
}
//---------------------------------------------------------------------------
// public - not exposed to Lua
//---------------------------------------------------------------------------
void TLuaHost::SetPushImage(TInternalImage *img)
{
FLuaPushImage = img;
}
//---------------------------------------------------------------------------
// load image from file  - host job!
TLuaImageVoid* TLuaHost::AddPushImage(TInternalImage *img)
{
AnsiString aType = AnsiString(img->type);
TLuaImageVoid *luaImage  = TImageCommon::CreateImage(aType.c_str(), img->luaImage->width, img->luaImage->height /*scanlineAlignement*/);
if (img->type == TYPE_B3)
   {
   TLuaImageByte3 *inpI = (TLuaImageByte3*)(img->luaImage);
   TLuaImageByte3 *outI = (TLuaImageByte3*)luaImage;
   TImageCommon::CopyImage(inpI, outI, 0, 0);
   }
else if (img->type == TYPE_B1)
   {
   TLuaImageByte1 *inpI = (TLuaImageByte1*)(img->luaImage);
   TLuaImageByte1 *outI = (TLuaImageByte1*)luaImage;
   TImageCommon::CopyImage(inpI, outI, 0, 0);
   }
return luaImage;
}
//---------------------------------------------------------------------------
// public - exposed to Lua
//---------------------------------------------------------------------------
// show progress
void TLuaHost::ShowProgress(int done, int total)
{
if (OnLuaProgress)
   OnLuaProgress(done, total);
}
//---------------------------------------------------------------------------
// send message
void TLuaHost::SendMessage(const char *msg)
{
FLuaMessage = AnsiString(msg);
if (OnLuaSendMessage)
   OnLuaSendMessage(FLuaMessage.c_str());
}
//---------------------------------------------------------------------------
// require parameter list
void TLuaHost::RequireParams(const char *parList)
{
FLuaParameters = AnsiString(parList);
/*
 Parametrs request string:
   S -> A | AA
   A -> token
   token -> "name = type(range initial_value);"
   type  -> int | float | bool | combo | list | imagefile | imagemap
   range -> [min, max] | []
   initial_value -> according to type, must be in the range (if range is not empty)
   Parameters request example: S = "size = int([10,20] 10);"
   Parameters request example: S = "perform_something = bool([] true);"
*/
if (OnLuaRequireParams)
   OnLuaRequireParams(FLuaParameters.c_str());
}
//---------------------------------------------------------------------------
// refresh params (previously built by require)
void TLuaHost::RefreshParams(void)
{
AnsiString fakeParam = "all";
if (OnLuaRefreshParams)
   OnLuaRefreshParams(fakeParam.c_str());
}
//---------------------------------------------------------------------------
// require parameter list
int TLuaHost::HostDialog(const char *dialogText, int type)
{
if (OnLuaHostDialog)
   return OnLuaHostDialog(dialogText, type);
else
   return 1;    // proceed
}
//---------------------------------------------------------------------------
// send message
bool TLuaHost::SendCommand(const char *cmd,  const char *parList)
{
FLuaMessage = AnsiString(cmd);
if (OnLuaSendCommand)
   return (OnLuaSendCommand(cmd, parList));
return false;
}
//---------------------------------------------------------------------------
// image routines
//---------------------------------------------------------------------------
// create new internal image
TLuaImageVoid* TLuaHost::CreateImage(const char *type, int width, int height, bool addAlpha)
{
return TImageCommon::CreateImage(type, width, height, addAlpha);
}
//---------------------------------------------------------------------------
// load image from file  - host job!
TLuaImageVoid* TLuaHost::LoadImage(const char *filename, const char *type)
{
if (OnLuaLoadImage)
   {
   if (!OnLuaLoadImage(filename, type))
	  return 0;
   }
else
   return 0;
TLuaImageVoid *img  = TImageCommon::CreateImage(type, FLuaPushImage->luaImage->width, FLuaPushImage->luaImage->height);
if (type == TYPE_B3)
   {
   TLuaImageByte3 *inpI = (TLuaImageByte3*)(FLuaPushImage->luaImage);
   TLuaImageByte3 *outI = (TLuaImageByte3*)img;
   TImageCommon::CopyImage(inpI, outI, 0, 0);
   }
else if (type == TYPE_B1)
   {
   TLuaImageByte1 *inpI = (TLuaImageByte1*)(FLuaPushImage->luaImage);
   TLuaImageByte1 *outI = (TLuaImageByte1*)img;
   TImageCommon::CopyImage(inpI, outI, 0, 0);
   }
// we don't need original shared push image any more
delete FLuaPushImage;
FLuaPushImage = 0;
return img;
}
//---------------------------------------------------------------------------
// export image to host (add image to host image list)
// NOTE:
// 1. image must exists (see CreateImage)
// 2. image must be of type RGB or RGBA (<-TODO)
// 2. OnLuaExportImage must be set
bool TLuaHost::ExportImage(int idExport, const char *exptype, bool show)
{
String texp;
TLuaImageVoid *iexp = TImageCommon::GetImageFromId(idExport, texp);
if (texp != TYPE_B3)
   return false;
FExpImage.exptype = (char *)exptype;
if (iexp->orientation)
	FExpImage.imageBuff = iexp->plane[iexp->height - 1];  // scanlines are inverted
else
	FExpImage.imageBuff = iexp->plane[0];  // scanlines as is
FExpImage.width  = iexp->width;
FExpImage.height = iexp->height;
FExpImage.channels = iexp->channels;
FExpImage.imageStride = iexp->imageStride;
if (iexp->alphaStride > 0)
   {
   if (iexp->orientation)
	   FExpImage.alphaBuff   = iexp->alpha[iexp->height - 1];  // scanlines are inverted
   else
	   FExpImage.alphaBuff   = iexp->alpha[0];  // scanlines as is
   FExpImage.alphaStride = iexp->alphaStride;
   }
else
   {
   FExpImage.alphaBuff   = 0;
   FExpImage.alphaStride = 0;
   }
if (OnLuaExportImage)
   {
   if (!OnLuaExportImage(&FExpImage, (bool)iexp->orientation, show))
	  return false;
   }
return true;
}
//---------------------------------------------------------------------------
// get image by id
TLuaHost::TStrPtr TLuaHost::GetImage(int id)
{
String sType;
TLuaImageVoid *img = TImageCommon::GetImageFromId(id, sType);
FLuaType = AnsiString(sType);
TLuaHost::TStrPtr data;
data.str = FLuaType.c_str();
data.ptr = LPVOID(img);
return data;
}
//---------------------------------------------------------------------------
// release internal image - delete image by id
bool TLuaHost::ReleaseImage(int id)
{
return TImageCommon::ReleaseInternalImage(id);
}
//---------------------------------------------------------------------------
// copy image data
bool TLuaHost::CopyImage(int idInput, int idOutput, TLuaRoi *inpRoi, TLuaRoi *outRoi)
{
String ti, to;
TLuaImageVoid *ii = TImageCommon::GetImageFromId(idInput, ti);
TLuaImageVoid *oi = TImageCommon::GetImageFromId(idOutput, to);
if (ti != to)
   return false;
bool lRet = true;
if (ti == TYPE_B3)
   {
   TLuaImageByte3 *inpI = (TLuaImageByte3*)ii;
   TLuaImageByte3 *outI = (TLuaImageByte3*)oi;
   TImageCommon::CopyImage(inpI, outI, inpRoi, outRoi);
   }
else if (ti == TYPE_B1)
   {
   TLuaImageByte1 *inpI = (TLuaImageByte1*)ii;
   TLuaImageByte1 *outI = (TLuaImageByte1*)oi;
   TImageCommon::CopyImage(inpI, outI, inpRoi, outRoi);
   }
else if (ti == TYPE_F3)
   {
   TLuaImageFloat3 *inpI = (TLuaImageFloat3*)ii;
   TLuaImageFloat3 *outI = (TLuaImageFloat3*)oi;
   TImageCommon::CopyImage(inpI, outI, inpRoi, outRoi);
   }
else
  lRet = false;
return lRet;
}
//---------------------------------------------------------------------------
bool TLuaHost::ExportRawData(int type)
{
void *rawData = 0;
if (OnLuaExportRawData)
   {
   switch (type)
	   {
	   case 0:  // contour
		    rawData = (void*)FCurrentContour;
			break;
	   default:
			break;
	   }
   if (!OnLuaExportRawData(rawData, type))
	  return false;
   }
return true;
}
//---------------------------------------------------------------------------
// color space conversion
//---------------------------------------------------------------------------
// rgb to grayscal
bool TLuaHost::Conv_rgb2gray(int idInput, int idOutput)
{
String ti, to;
TLuaImageVoid *ii = TImageCommon::GetImageFromId(idInput, ti);
TLuaImageVoid *oi = TImageCommon::GetImageFromId(idOutput, to);
if (ii == 0 || oi == 0)
   return false;
if (ti != TYPE_B3)
   return false;
if (ii->height != oi->height || ii->width != oi->width)
   return false;
bool lRet = true;
TLuaImageByte3 *inpI = (TLuaImageByte3*)ii;
if (to == TYPE_B3)
   {
   TLuaImageByte3 *outI = (TLuaImageByte3*)oi;
   TImageCommon::RgbToGray(inpI, outI);
   }
else if (to == TYPE_B1)
   {
   TLuaImageByte1 *outI = (TLuaImageByte1*)oi;
   TImageCommon::RgbToGray(inpI, outI);
   }
/* currently not supported
else if (to == TYPE_F3)
   {
   TLuaImageFloat3 *outI = (TLuaImageFloat3*)oi;
   TImageCommon::RgbToGrayscale(inpI, outI);
   }
*/
else
  lRet = false;
return lRet;
}
//---------------------------------------------------------------------------
// gray to rgb
bool TLuaHost::Conv_gray2rgb(int idInput, int idOutput, bool alpha)
{
String ti, to;
TLuaImageByte1 *ii = (TLuaImageByte1*)TImageCommon::GetImageFromId(idInput, ti);
TLuaImageByte3 *oi = (TLuaImageByte3*)TImageCommon::GetImageFromId(idOutput, to);
if (ii == 0 || oi == 0)
   return false;
if (ti != TYPE_B1 && to != TYPE_B3)
   return false;
if (ii->height != oi->height || ii->width != oi->width)
   return false;
if (alpha && ii->alpha == 0)
   return false;
// convert it
TImageCommon::GrayToRgb(ii, oi, alpha);
return true;
}
//---------------------------------------------------------------------------
// rgb to xyz
bool TLuaHost::Conv_rgb2xyz(int idInput, int idOutput)
{
String ti, to;
TLuaImageByte3  *ii = (TLuaImageByte3*)TImageCommon::GetImageFromId(idInput, ti);
TLuaImageFloat3 *oi = (TLuaImageFloat3*)TImageCommon::GetImageFromId(idOutput, to);
if (ii == 0 || oi == 0)
   return false;
if (ti != TYPE_B3 || to != TYPE_F3)
   return false;
if (ii->height != oi->height || ii->width != oi->width)
   return false;
// convert it
TImageCommon::RgbToXyz(ii, oi);
return true;
}
//---------------------------------------------------------------------------
// xyz to rgb
bool TLuaHost::Conv_xyz2rgb(int idInput, int idOutput)
{
String ti, to;
TLuaImageFloat3 *ii = (TLuaImageFloat3*)TImageCommon::GetImageFromId(idInput, ti);
TLuaImageByte3  *oi = (TLuaImageByte3*)TImageCommon::GetImageFromId(idOutput, to);
if (ii == 0 || oi == 0)
   return false;
if (ti != TYPE_F3 || to != TYPE_B3)
   return false;
if (ii->height != oi->height || ii->width != oi->width)
   return false;
// convert it
TImageCommon::XyzToRgb(ii, oi);
return true;
}
//---------------------------------------------------------------------------
// rgb to lab
bool TLuaHost::Conv_rgb2lab(int idInput, int idOutput)
{
String ti, to;
TLuaImageByte3  *ii = (TLuaImageByte3*)TImageCommon::GetImageFromId(idInput, ti);
TLuaImageFloat3 *oi = (TLuaImageFloat3*)TImageCommon::GetImageFromId(idOutput, to);
if (ii == 0 || oi == 0)
   return false;
if (ti != TYPE_B3 || to != TYPE_F3)
   return false;
if (ii->height != oi->height || ii->width != oi->width)
   return false;
// convert it
TImageCommon::RgbToLab(ii, oi);
return true;
}
//---------------------------------------------------------------------------
// lab to rgb
bool TLuaHost::Conv_lab2rgb(int idInput, int idOutput)
{
String ti, to;
TLuaImageFloat3 *ii = (TLuaImageFloat3*)TImageCommon::GetImageFromId(idInput, ti);
TLuaImageByte3  *oi = (TLuaImageByte3*)TImageCommon::GetImageFromId(idOutput, to);
if (ii == 0 || oi == 0)
   return false;
if (ti != TYPE_F3 || to != TYPE_B3)
   return false;
if (ii->height != oi->height || ii->width != oi->width)
   return false;
// convert it
TImageCommon::LabToRgb(ii, oi);
return true;
}
//---------------------------------------------------------------------------
// blur
bool TLuaHost::Blur(int idInput, int idOutput, int radius)
{
//test - grayscale only
TLuaImageVoid  *ii = TImageCommon::GetImageFromId(idInput);
TLuaImageVoid  *oi = TImageCommon::GetImageFromId(idOutput);
if (ii == 0 || oi == 0)
   return false;
if (ii->height != oi->height || ii->width != oi->width)
   return false;
TImageCommon::spsFastBlur *fbgData = TImageCommon::FBPrepare(ii, oi, 0, radius);
TImageCommon::FBRunGray(fbgData);
TImageCommon::FBUnPrepare(fbgData);
return true;
}
//---------------------------------------------------------------------------
// blend with mask
bool TLuaHost::Blend(int idInput, int idMask, int idOutput, bool alpha)
{
TLuaImageByte3 *ii = (TLuaImageByte3*)TImageCommon::GetImageFromId(idInput);
TLuaImageByte3 *oi = (TLuaImageByte3*)TImageCommon::GetImageFromId(idOutput);
TLuaImageByte1 *mi = (TLuaImageByte1*)TImageCommon::GetImageFromId(idMask);
if (ii == 0 || oi == 0)
   return false;
if (ii->height != oi->height || ii->width != oi->width || ii->height != mi->height || ii->width != mi->width)
   return false;
TImageCommon::BlendWithMask(ii, mi, oi, alpha);
return true;
}
//---------------------------------------------------------------------------
bool TLuaHost::DownSample(int idInput, int idOutput)
{
String ti, to;
TLuaImageVoid *ii = TImageCommon::GetImageFromId(idInput, ti);
TLuaImageVoid *oi = TImageCommon::GetImageFromId(idOutput, to);
if (ii == 0 || oi == 0 || ti != to)
   return false;
if (ti == TYPE_B1)
   {
   // downsample
   TImageCommon::DownsampleGray((TLuaImageByte1*)ii, (TLuaImageByte1*)oi);
   return true;
   }
else if (ti == TYPE_B3)
   {
   // downsample
   TImageCommon::DownsampleRGB((TLuaImageByte3*)ii, (TLuaImageByte3*)oi);
   return true;
   }
return false;
}
//---------------------------------------------------------------------------
// checker
bool TLuaHost::Checker(const char *cmd, const char* param)
{
bool lRet = false;
String sCmd  = String(cmd).LowerCase();
String sParam = String(param).LowerCase();
if (sCmd == "fileexists")
   {
   lRet = FileExists(sParam);
   }
return lRet;
}
//---------------------------------------------------------------------------
// OpenCV block
//---------------------------------------------------------------------------
// opencv set - images and other thing
bool TLuaHost::OcvSet(const char *proc, int id)
{
TLuaImageVoid *img;
String strProc = String(proc).Trim().LowerCase();
TImgType ocvType;
int rc;
if (strProc == "source")
   {
   img = TImageCommon::GetImageFromId(id);
   FOpenCV.sourceId = id;
   ocvType = OCW_IMG_SOURCE_RGB;
   if (id != 0)
	   {
	   if (img->orientation)
		   rc = ocvSetImage(ocvType, img->width, img->height, img->plane[img->height-1], img->imageStride);
	   else
		   rc = ocvSetImage(ocvType, img->width, img->height, img->plane[0], img->imageStride);
	   }
   }
else if (strProc == "target")
   {
   img = TImageCommon::GetImageFromId(id);
   FOpenCV.targetId = id;
   ocvType = OCW_IMG_TARGET_RGB;
   if (id != 0)
	   {
	   if (img->orientation)
		   rc = ocvSetImage(ocvType, img->width, img->height, img->plane[img->height-1], img->imageStride);
	   else
		   rc = ocvSetImage(ocvType, img->width, img->height, img->plane[0], img->imageStride);
	   }
   }
if (strProc == "srcmask")
   {
   img = TImageCommon::GetImageFromId(id);
   FOpenCV.srcMaskId = id;
   ocvType = OCW_MASK_SOURCE;
   if (id != 0)
	   {
	   if (img->orientation)
		   rc = ocvSetImage(ocvType, img->width, img->height, img->plane[img->height-1], img->imageStride);
	   else
		   rc = ocvSetImage(ocvType, img->width, img->height, img->plane[0], img->imageStride);
	   }
   }
else if (strProc == "cleardk")
   {
   ocvClearDK(id);
   FDKIdx = -1;
   }
else if (strProc == "setdkidx")
   {
   FDKIdx = id;
   }
else
   return false;
return true;
}
//---------------------------------------------------------------------------
// opencv set - rois
bool TLuaHost::OcvSet(const char *proc, TLuaRoi * roi)
{
String strProc = String(proc).Trim().LowerCase();
SocvRoi ocvRoi;
TLuaImageVoid  *ii = TImageCommon::GetImageFromId(FOpenCV.sourceId);
TLuaImageVoid  *oi = TImageCommon::GetImageFromId(FOpenCV.targetId);
if (strProc == "host")
   {
   if (TImageCommon::SourceRoi)
	  {
	  if (FOpenCV.sourceId == 0)
		  return false;
	  ocvRoi.left = TImageCommon::SourceRoi->left;
	  ocvRoi.right = TImageCommon::SourceRoi->right;
	  ocvRoi.bottom = ii->height - TImageCommon::SourceRoi->top - 1;
	  ocvRoi.top = ii->height - TImageCommon::SourceRoi->bottom - 1;
	   if (TImageCommon::TargetRoi)
		  ocvSetRoi(OCW_ROI_SOURCE, ocvRoi);
	   else
		  ocvSetRoi(OCW_ROI_ALL, ocvRoi);
	   }
   if (TImageCommon::TargetRoi)
	  {
	  ocvRoi.left = TImageCommon::TargetRoi->left;
	  ocvRoi.right = TImageCommon::TargetRoi->right;
	  ocvRoi.bottom = oi->height - TImageCommon::TargetRoi->top - 1;
	  ocvRoi.top = oi->height - TImageCommon::TargetRoi->bottom - 1;
	  ocvSetRoi(OCW_ROI_TARGET, ocvRoi);
	  }
   }
else if (strProc == "source")
   {
   if (FOpenCV.sourceId == 0)
	  return false;
   if (roi)
	  {
	  ocvRoi.left = roi->left;
	  ocvRoi.right = roi->right;
	  ocvRoi.bottom = ii->height - roi->top - 1;
	  ocvRoi.top = ii->height - roi->bottom - 1;
	  ocvSetRoi(OCW_ROI_SOURCE, ocvRoi);
	  }
   else
	  ocvClearRoi(OCW_ROI_SOURCE);
   }
else if (strProc == "target")
   {
   if (FOpenCV.targetId == 0)
	  return false;
   if (roi)
	  {
	  ocvRoi.left = roi->left;
	  ocvRoi.right = roi->right;
	  ocvRoi.bottom = oi->height - roi->top - 1;
	  ocvRoi.top = oi->height - roi->bottom - 1;
	  ocvSetRoi(OCW_ROI_TARGET, ocvRoi);
	  }
   else
	  ocvClearRoi(OCW_ROI_TARGET);
   }
else if (strProc == "clearall")
   ocvClearRoi(OCW_ROI_ALL);
else
   return false;
return true;
}
//---------------------------------------------------------------------------
// opencv get int - no params
int TLuaHost::OcvGetInt(const char *proc)
{
String strProc = String(proc).Trim().LowerCase();
int nRet = -1;
if (strProc == "lastdkidx")
   nRet = ocvGetLastDKIdx();
else if (strProc == "createdk")
   nRet = ocvCreateDK();
else if (strProc == "lastimageid")
   {
   SocvImageData *ocvImg = ocvGetExportImage();
   TLuaImageVoid *luaImg = TImageCommon::CreateImage(ocvImg);
   nRet = luaImg->id;
   }
else if (strProc == "numcontours")
  {
  nRet = (int)FExportContours.numContours;
  }
return nRet;
}
//---------------------------------------------------------------------------
// opencv get ptr - 1 params set
void* TLuaHost::OcvGetPtr(const char *proc, const char *params)
{
String strProc = String(proc).Trim().LowerCase();
String strParams = String(params).Trim().LowerCase();
void *pRet = 0;
if (strProc == "contour")
   {
   int idx = StrToInt(strParams);
   FCurrentContour = 0;
   if (FExportContours.numContours > 0)
	  {
	  if (idx < (int)FExportContours.numContours)
		 {
		 int *hPtr = FExportContours.hierarchy;	// size = 4 * numContours
		 hPtr = hPtr + (4 * idx);
		 for (int k = 0; k < 4; k++)
			 FExportContours.contours[idx].hierarchy[k] = hPtr[k];
		 FCurrentContour = &(FExportContours.contours[idx]);
		 pRet = (void*)FCurrentContour;
		 }
	   }
   }
else if (strProc == "homographydata")
   {
   ocvGetHomographyData(FLastHomography);
   pRet = (void*)(&FLastHomography);
   }
return pRet;
}
//---------------------------------------------------------------------------
// opencv processing - no params - all in one
const char* TLuaHost::OcvProc(const char *proc, int &rc)
{
if (FOpenCV.sourceId == 0)
   {
   rc = LERR_INVALID_SOURCE;
   return TLipsError::GetMessage(rc).c_str();
   }
if (FOpenCV.targetId == 0)
   {
   rc = LERR_INVALID_TARGET;
   return TLipsError::GetMessageA(rc).c_str();
   }
String strProc = String(proc).Trim().LowerCase();
try {
	if (strProc == "swapfaces")
	   {
	   rc = ocvSwapFaces(OCW_IMAGE_SOURCE, OCW_IMAGE_TARGET, FDKIdx);
	   }
	else if (strProc == "colorize")
	   {
	   rc = ocvColorize(FDKIdx);
	   }
	else if (strProc == "releasecontours")
	   {
	   rc =	ocvReleaseContours(&FExportContours);
	   FExportContours.numContours = 0;
	   }
	else if (strProc == "clearfeatures2d")
	   {
	   rc =	ocvClearFeatures2D();
	   }
	else if (strProc == "recalchomography2d")
	   {
	   rc = ocvRecalcHomography();
	   }
	else
	   {
	   rc = LERR_UNKNOWN_PROC;
	   return TLipsError::GetMessage(strProc, rc).c_str();
	   }
	if (rc)
	   return TLipsError::SetMessage(strProc, rc).c_str();
	}
catch (...)
   {
   rc = LERR_EXCEPTION;
   return TLipsError::GetMessage(strProc, rc).c_str();
   }
return NULL;
}
//---------------------------------------------------------------------------
// opencv processing - single params set - all in one
const char* TLuaHost::OcvProc(const char *proc, const char *params, int &rc)
{
if (FOpenCV.sourceId == 0)
   {
   rc = LERR_INVALID_SOURCE;
   return TLipsError::GetMessage(rc).c_str();
   }
if (FOpenCV.targetId == 0)
   {
   rc = LERR_INVALID_TARGET;
   return TLipsError::GetMessageA(rc).c_str();
   }
String strProc = String(proc).Trim().LowerCase();
TFormatSettings fmt;
fmt.DecimalSeparator = '.';
String strParams = String(params).Trim().LowerCase();
// parse parameters
TStringDynArray tokens = SplitString(strParams, ",");
int tkl = tokens.Length;
try {
	if (strProc == "blur")
	   {
	   if (tkl < 2)
		  throw tkl;
	   unsigned int kx = StrToInt(tokens[0]);
	   unsigned int ky = StrToInt(tokens[1]);
	   if (tokens.Length == 2)
		  rc = ocvBlur(kx, ky);
	   else
		  {
		  int border = StrToInt(tokens[2]);
		  rc = ocvBlur(kx, ky, (TBorderType)border);
		  }
	   }
	else if (strProc == "median")
	   {
	   if (tkl < 1)
		   throw tkl;
	   unsigned int kSize = StrToInt(tokens[0]);
	   rc = ocvMedianBlur(kSize);
	   }
	else if (strProc == "gaussian")
	   {
	   if (tkl < 4)
		  throw tkl;
	   unsigned int kx = StrToInt(tokens[0]);
	   unsigned int ky = StrToInt(tokens[1]);
	   double sigmaX = StrToFloat(tokens[2], fmt);
	   double sigmaY = StrToFloat(tokens[3], fmt);
	   if (tokens.Length == 4)
		  rc = ocvGaussianBlur(kx, ky, sigmaX, sigmaY);
	   else
		  {
		  int border = StrToInt(tokens[4]);
		  rc = ocvGaussianBlur(kx, ky, sigmaX, sigmaY, (TBorderType)border);
		  }
	   }
	else if (strProc == "bilateral")
	   {
	   if (tkl < 3)
		  throw tkl;
	   int dia =  StrToInt(tokens[0]);
	   double sigmaColor = StrToFloat(tokens[1], fmt);
	   double sigmaSpace = StrToFloat(tokens[2], fmt);
	   if (tokens.Length == 3)
		  rc = ocvBilateralFilter(dia, sigmaColor, sigmaSpace);
	   else
		  {
		  int border = StrToInt(tokens[3]);
		  rc = ocvBilateralFilter(dia, sigmaColor, sigmaSpace, (TBorderType)border);
		  }
	   }
	else if (strProc == "laplacian")
	   {
	   if (tkl < 4)
		  throw tkl;
	   unsigned int ksize =  StrToInt(tokens[0]);
	   double scale = StrToFloat(tokens[1], fmt);
	   double delta = StrToFloat(tokens[2], fmt);
	   bool cv2gray = StrToBool(tokens[3].Trim());
	   if (tokens.Length == 4)
		  rc = ocvLaplacian(ksize, scale, delta, cv2gray);
	   else
		  {
		  int border = StrToInt(tokens[4]);
		  rc = ocvLaplacian(ksize, scale, delta, cv2gray, (TBorderType)border);
		  }
	   }
	else if (strProc == "scharr")
	   {
	   if (tkl < 5)
		  throw tkl;
	   unsigned int dx =  StrToInt(tokens[0]);
	   unsigned int dy =  StrToInt(tokens[1]);
	   double scale = StrToFloat(tokens[2], fmt);
	   double delta = StrToFloat(tokens[3], fmt);
	   bool cv2gray = StrToBool(tokens[4].Trim());
	   if (tokens.Length == 5)
		  rc = ocvScharr(dx, dy, scale, delta, cv2gray);
	   else
		  {
		  int border = StrToInt(tokens[5]);
		  rc = ocvScharr(dx, dy, scale, delta, cv2gray, (TBorderType)border);
		  }
	   }
	else if (strProc == "sobel")
	   {
	   if (tkl < 6)
		  throw tkl;
	   unsigned int dx =  StrToInt(tokens[0]);
	   unsigned int dy =  StrToInt(tokens[1]);
	   int ksize   =  StrToInt(tokens[2]);
	   double scale = StrToFloat(tokens[3], fmt);
	   double delta = StrToFloat(tokens[4], fmt);
	   bool cv2gray = StrToBool(tokens[5].Trim());
	   if (tokens.Length == 6)
		  rc = ocvSobel(dx, dy, ksize, scale, delta, cv2gray);
	   else
		  {
		  int border = StrToInt(tokens[6]);
		  rc = ocvSobel(dx, dy, ksize, scale, delta, cv2gray, (TBorderType)border);
		  }
	   }
	else if (strProc == "box")
	   {
	   if (tkl < 3)
		  throw tkl;
	   unsigned int kx = StrToInt(tokens[0]);
	   unsigned int ky = StrToInt(tokens[1]);
	   bool normalize = StrToBool(tokens[2].Trim());
	   if (tokens.Length == 3)
		  rc = ocvBoxFilter(kx, ky, normalize);
	   else
		  {
		  int border = StrToInt(tokens[3]);
		  rc = ocvBoxFilter(kx, ky, normalize, (TBorderType)border);
		  }
	   }
	else if (strProc == "sqrbox")
	   {
	   if (tkl < 3)
		  throw tkl;
	   unsigned int kx = StrToInt(tokens[0]);
	   unsigned int ky = StrToInt(tokens[1]);
	   bool normalize = StrToBool(tokens[2].Trim());
	   if (tokens.Length == 3)
		  rc = ocvSqrBoxFilter(kx, ky, normalize);
	   else
		  {
		  int border = StrToInt(tokens[3]);
		  rc = ocvSqrBoxFilter(kx, ky, normalize, (TBorderType)border);
		  }
	   }
	else if (strProc == "dilate")
	   {
	   if (tkl < 4)
		  throw tkl;
	   unsigned int kx = StrToInt(tokens[0]);
	   unsigned int ky = StrToInt(tokens[1]);
	   int shape = StrToInt(tokens[2]);
	   unsigned int iterations = StrToInt(tokens[3]);
	   if (tokens.Length == 4)
		  rc = ocvDilate(kx, ky, (TMorphShape)shape, iterations);
	   else
		  {
		  int border = StrToInt(tokens[4]);
		  rc = ocvDilate(kx, ky, (TMorphShape)shape, iterations, (TBorderType)border);
		  }
	   }
	else if (strProc == "erode")
	   {
	   if (tkl < 4)
		  throw tkl;
	   unsigned int kx = StrToInt(tokens[0]);
	   unsigned int ky = StrToInt(tokens[1]);
	   int shape = StrToInt(tokens[2]);
	   unsigned int iterations = StrToInt(tokens[3]);
	   if (tokens.Length == 4)
		  rc = ocvErode(kx, ky, (TMorphShape)shape, iterations);
	   else
		  {
		  int border = StrToInt(tokens[4]);
		  rc = ocvErode(kx, ky, (TMorphShape)shape, iterations, (TBorderType)border);
		  }
	   }
	else if (strProc == "morphologyex")
	   {
	   if (tkl < 5)
		  throw tkl;
	   unsigned int kx = StrToInt(tokens[0]);
	   unsigned int ky = StrToInt(tokens[1]);
	   int opType = StrToInt(tokens[2]);
	   int shape = StrToInt(tokens[3]);
	   unsigned int iterations = StrToInt(tokens[4]);
	   if (tokens.Length == 5)
		  rc = ocvMorphologyEx(kx, ky, (TMorphType)opType, (TMorphShape)shape, iterations);
	   else
		  {
		  int border = StrToInt(tokens[5]);
		  rc = ocvMorphologyEx(kx, ky, (TMorphType)opType, (TMorphShape)shape, iterations, (TBorderType)border);
		  }
	   }
	else if (strProc == "threshold")
	   {
	   if (tkl < 3)
		  throw tkl;
	   double thresh = StrToFloat(tokens[0], fmt);
	   double maxval = StrToFloat(tokens[1], fmt);
	   int type = StrToInt(tokens[2]);
	   rc = ocvThreshold(thresh, maxval, (TThresholdType)type);
	   }
	else if (strProc == "adaptivethreshold")
	   {
	   if (tkl < 5)
		  throw tkl;
	   double maxval = StrToFloat(tokens[0], fmt);
	   int method = StrToInt(tokens[1]);
	   int type = StrToInt(tokens[2]);
	   unsigned int radius = StrToInt(tokens[3]);
	   double C = StrToFloat(tokens[4], fmt);
	   rc = ocvAdaptiveThreshold(maxval, (TAdaptiveThresholdType) method, (TThresholdType) type, radius, C);
	   }
	else if (strProc == "distancetransform")
	   {
	   if (tkl < 3)
		  throw tkl;
	   bool gray = StrToBool(tokens[0].Trim());
	   int type = StrToInt(tokens[1]);
	   int maskSize = StrToInt(tokens[2]);
	   rc = ocvDistanceTransform(gray, (TDistanceType)type, (TDistanceMaskType) maskSize);
	   }
	else if (strProc == "canny")
	   {
	   if (tkl < 4)
		  throw tkl;
	   double threshold1 = StrToFloat(tokens[0], fmt);
	   double threshold2 = StrToFloat(tokens[1], fmt);
	   unsigned int apertureSize = StrToInt(tokens[2]);
	   bool L2gradient = StrToBool(tokens[3].Trim());
	   rc = ocvCanny(threshold1, threshold2, apertureSize, L2gradient);
	   }
	else if (strProc == "flip")
	   {
	   if (tkl < 1)
		  throw tkl;
	   int flipCode = StrToInt(tokens[0]);
	   rc = ocvFlip(flipCode);
	   }
	else if (strProc == "gaborius")
	   {
	   if (tkl < 7)
		  throw tkl;
	   unsigned int step = StrToInt(tokens[0]);
	   unsigned int kernelSize = StrToInt(tokens[1]);
	   double sigma = StrToFloat(tokens[2], fmt);
	   double lambda = StrToFloat(tokens[3], fmt);
	   double gamma = StrToFloat(tokens[4], fmt);
	   double psi = StrToFloat(tokens[5], fmt);
	   double ksFactor = StrToFloat(tokens[6], fmt);
	   rc = ocvGaborius(step, kernelSize, sigma, lambda, gamma, psi, ksFactor);
	   }
	else if (strProc == "trigonometrius")
	   {
	   if (tkl < 8)
		  throw tkl;
	   int fType = StrToInt(tokens[0]);
	   unsigned int step = StrToInt(tokens[1]);
	   unsigned int kernelSize = StrToInt(tokens[2]);
	   double hSigma = StrToFloat(tokens[3], fmt);
	   double freq   = StrToFloat(tokens[4], fmt);
	   double ksFactor = StrToFloat(tokens[5], fmt);
	   int margin = StrToInt(tokens[6]);
	   double vSigma = StrToFloat(tokens[7], fmt);
	   rc = ocvTrigonometrius((TKernelFunc)fType, step, kernelSize, hSigma, freq, ksFactor, margin, vSigma);
	   }
	//TODO: gabor filter - simple
	else if (strProc == "detailenhance")
	   {
	   if (tkl < 2)
		  throw tkl;
	   float sigma_s = StrToFloat(tokens[0], fmt);
	   float sigma_r = StrToFloat(tokens[1], fmt);
	   rc = ocvDetailEnhance(sigma_s, sigma_r);
	   }
	else if (strProc == "edgepreservingfilter")
	   {
	   if (tkl < 3)
		  throw tkl;
	   int flags = StrToInt(tokens[0]);
	   float sigma_s = StrToFloat(tokens[1], fmt);
	   float sigma_r = StrToFloat(tokens[2], fmt);
	   rc =	ocvEdgePreservingFilter(flags, sigma_s, sigma_r);
	   }
	else if (strProc == "pencilsketch")
	   {
	   if (tkl < 4)
		  throw tkl;
	   bool grayscale = StrToBool(tokens[0].Trim());
	   float sigma_s = StrToFloat(tokens[1], fmt);
	   float sigma_r = StrToFloat(tokens[2], fmt);
	   float shade_factor = StrToFloat(tokens[3], fmt);
	   rc = ocvPencilSketch(grayscale, sigma_s, sigma_r, shade_factor);
	   }
	else if (strProc == "stylization")
	   {
	   if (tkl < 2)
		  throw tkl;
	   float sigma_s = StrToFloat(tokens[0], fmt);
	   float sigma_r = StrToFloat(tokens[1], fmt);
	   rc = ocvStylization(sigma_s, sigma_r);
	   }
	else if (strProc == "oilpainting")
	   {
	   if (tkl < 2)
		  throw tkl;
	   int size = StrToInt(tokens[0]);
	   int dynRatio = StrToInt(tokens[1]);
	   rc = ocvOilPainting(size, dynRatio);
	   }
	else if (strProc == "styletransfer")
	   {
	   // parse parameters
	   if (tkl < 6)
		  throw tkl;
	   double scale = StrToFloat(tokens[0], fmt);
	   double meanR = StrToFloat(tokens[1], fmt);
	   double meanG = StrToFloat(tokens[2], fmt);
	   double meanB = StrToFloat(tokens[3], fmt);
	   bool normResult = StrToBool(tokens[4].Trim());
	   bool swapRB = StrToBool(tokens[5].Trim());
	   rc = ocvStyleTransfer(FDKIdx, scale, meanR, meanG, meanB, normResult, swapRB);
	   }
	else if (strProc == "warppolar")
	   {
	   // parse parameters
	   if (tkl < 5)
		  throw tkl;
	   int x = StrToInt(tokens[0]);
	   int y = StrToInt(tokens[1]);
	   double maxRadius = StrToFloat(tokens[2], fmt);
	   bool exportImage = StrToBool(tokens[3].Trim());
	   int flag = StrToInt(tokens[4]);
	   rc = ocvWarpPolar(x, y, maxRadius, exportImage, flag);
	   }
	else if (strProc == "features2d")
	   {
	   // parse parameters
	   if (tkl < 1)
		  throw tkl;
	   int type = StrToInt(tokens[0]);
	   int choice = 0;
	   int maxSrcKeys = 0;
	   int maxTgtKeys = 0;
	   if (tokens.Length > 1)
		   choice = StrToInt(tokens[1].Trim());
	   if (tokens.Length > 2)
		   maxSrcKeys = StrToInt(tokens[2].Trim());
	   if (tokens.Length > 3)
		   maxTgtKeys = StrToInt(tokens[3].Trim());
	   // maxSrcKeys and maxTgtKeys represent max numbe of keys to retain after detector is executed.
       // used for algorithms when separeted detection and compute is needed.
	   rc = ocvCalcFeatures2D((TFeature2DType)type, (TFeature2DChoice)choice, maxSrcKeys, maxTgtKeys);
	   }
	else if (strProc == "matches2d")
	   {
	   // parse parameters
	   if (tkl < 3)
		  throw tkl;
	   int type = StrToInt(tokens[0]);
	   double matchParam = StrToFloat(tokens[1], fmt);
	   bool exportImage = StrToBool(tokens[2].Trim());
	   rc = ocvCalcMatches((TMatchType)type, matchParam, exportImage);
	   }
	else if (strProc == "homography2d")
	   {
	   // parse parameters
	   if (tkl < 5)
		  throw tkl;
	   unsigned int minMatches = StrToInt(tokens[0]);
	   int  type    = StrToInt(tokens[1]);
	   bool exportImage = StrToBool(tokens[2].Trim());
	   bool addAlpha = StrToBool(tokens[3].Trim());
	   bool warp2SS = StrToBool(tokens[4].Trim());
	   rc = ocvCalcHomography(minMatches, (THomographyType)type, exportImage, addAlpha, warp2SS);
	   }
	else if (strProc == "reducefeatures2d")
	   {
	   // parse parameters
	   if (tkl < 1)
		  throw tkl;
	   int  type    = StrToInt(tokens[0]);
	   bool exportMatchImage = false;
	   if (tokens.Length > 1)
		  exportMatchImage = StrToBool(tokens[1].Trim());
	   rc = ocvReduceFeatures2D((TFeature2DReduction)type, exportMatchImage);
	   }
	else if (strProc == "seamlessclone")
	   {
	   // parse parameters
	   if (tkl < 2)
		  throw tkl;
	   int centerX = StrToInt(tokens[0]);
	   int centerY = StrToInt(tokens[1]);
	   int type = 0;
	   if (tokens.Length == 3)
		   type = StrToInt(tokens[2]);
	   rc = ocvSeamlessClone(centerX, centerY, (TCloneType) type);
	   }
	else if (strProc == "inpaint")
	   {
	   // parse parameters
	   if (tkl < 1)
		  throw tkl;
	   int type = StrToInt(tokens[0]);
	   double radius = 3.0;
	   if (tokens.Length > 1)
		  radius = StrToFloat(tokens[1], fmt);
	   rc = ocvInpaint((TInpaintType)type, radius);
	   }
	else if (strProc == "colortransfer")
	   {
	   // parse parameters
	   if (tkl != 1)
		  throw tkl;
	   bool exportImage = StrToBool(tokens[0].Trim());
       rc = ocvColorTransfer(exportImage);
	   }
	else if (strProc == "staticsaliency")
	   {
	   // parse parameters
	   if (tkl != 1)
		  throw tkl;
	   int type = StrToInt(tokens[0].Trim());
	   rc = ocvStaticSaliency((TSaliencyType)type);
	   }
	else if (strProc == "findcontours")
	   {
	   // parse parameters
	   if (tkl != 3)
		  throw tkl;
	   int edgeType = StrToInt(tokens[0]);
	   int retMode  = StrToInt(tokens[1]);
	   int appMode  = StrToInt(tokens[2]);
	   rc =	ocvFindContours((TEdgeType) edgeType, (TContourRetrievalMode) retMode, (TContourApproximationMode) appMode, &FExportContours );
	   }
	else if (strProc == "floodfillsimple")
	   {
	   // parse parameters
	   if (tkl < 6)
		  throw tkl;
	   SocvPoint seed;
	   seed.y = StrToInt(tokens[0]);
	   seed.x = StrToInt(tokens[1]);
	   int tolLo = StrToInt(tokens[2]);
	   int tolHi = StrToInt(tokens[3]);
	   int connectivity = StrToInt(tokens[4]);
	   int fillMode = StrToInt(tokens[5]);
	   if (tokens.Length == 9)
		  {
          // input as  r, g, b
		  unsigned char pixBGR[3];
		  pixBGR[2] = StrToInt(tokens[6]);
		  pixBGR[1] = StrToInt(tokens[7]);
		  pixBGR[0] = StrToInt(tokens[8]);
		  rc =	ocvFloodFillSimple(seed, tolLo, tolHi, connectivity, fillMode, pixBGR);
		  }
	   else
		  rc =	ocvFloodFillSimple(seed, tolLo, tolHi, connectivity, fillMode);
	   }
	else
	   {
	   rc = LERR_UNKNOWN_PROC;
	   return TLipsError::GetMessage(strProc, rc).c_str();
	   }
	if (rc)
	   return TLipsError::SetMessage(strProc, rc).c_str();
	}
catch (int thrnum)
   {
   rc = LERR_INVALID_ARGNUM;
   return TLipsError::GetMessage(strProc, rc).c_str();
   }
catch (...)
   {
   rc = LERR_EXCEPTION;
   return TLipsError::GetMessage(strProc, rc).c_str();
   }
return NULL;
}
//---------------------------------------------------------------------------
// opencv processing - double params set - all in one
const char* TLuaHost::OcvProc(const char *proc, const char *params1, const char *params2, int &rc)
{
TFormatSettings fmt;
fmt.DecimalSeparator = '.';
String strProc = String(proc).Trim().LowerCase();
String strParams1 = String(params1);
String strParams2 = String(params2);
TStringDynArray tokens1;
TStringDynArray tokens2;
TImgSelect imgSel;
SocvObjectData *od;
try {
	if (strProc == "filter2d")
	   {
	   strParams1 = strParams1.Trim().LowerCase();
	   strParams2 = strParams2.Trim().LowerCase();
	   // parse parameters
	   tokens1 = SplitString(strParams1, ",");
	   if (tokens1.Length < 3)
		  throw (int)tokens1.Length;
	   unsigned int kernelSize = StrToInt(tokens1[0]);
	   unsigned int kernelLength = kernelSize *kernelSize;
	   float div = StrToFloat(tokens1[1], fmt);
	   float bias = StrToFloat(tokens1[2], fmt);
	   // parse kernel
	   tokens2 = SplitString(strParams2, ",");
	   if ((int)tokens2.Length <  (int)kernelLength)
		  throw (int)tokens2.Length;
	   // parse kernel
	   float kernel[33*33]; // max kernel = 33x33
	   for (unsigned int i = 0; i < kernelLength; i++)
		   kernel[i] = StrToFloat(tokens2[i], fmt);
	   if (tokens1.Length == 3)
		  rc = ocvFilter2D(kernelSize, kernel, div, bias);
	   else
		  {
		  int border = StrToInt(tokens1[3]);
		  rc = ocvFilter2D(kernelSize, kernel, div, bias, (TBorderType)border);
		  }
	   }
	else if (strProc == "__flip")
	   {
	   strParams1 = strParams1.Trim().LowerCase();
	   strParams2 = strParams2.Trim().LowerCase();
	   int flipCode = StrToInt(strParams2);
	   if (strParams1 == "source")
		   rc = ocvFlipInplace(flipCode, OCW_IMAGE_SOURCE);
	   else if (strParams1 == "target")
		   rc = ocvFlipInplace(flipCode, OCW_IMAGE_TARGET);
	   else if (strParams1 == "export")
		   rc = ocvFlipInplace(flipCode, OCW_IMAGE_EXPORT);
	   else
		   {
		   rc = LERR_INVALID_PARAM;
		   return TLipsError::GetMessage(strProc, strParams1, rc).c_str();
		   }
	   }
	else if (strProc == "facelandmarkdetector")
	   {
	   strParams1 = strParams1.Trim().LowerCase();
	   strParams2 = strParams2.Trim().LowerCase();
	   if (strParams1 == "source")
		  imgSel = OCW_IMAGE_SOURCE;
	   else if (strParams1 == "target")
		  imgSel = OCW_IMAGE_TARGET;
	   else
		   {
		   rc = LERR_INVALID_PARAM;
		   return TLipsError::GetMessage(strProc, strParams1, rc).c_str();
		   }
	   // parse parameters
	   tokens2 = SplitString(strParams2, ",");
	   if (tokens2.Length < 5)
		  throw (int)tokens2.Length;
	   bool singleFace = StrToBool(tokens2[0].Trim());
	   bool extrapolate = StrToBool(tokens2[1].Trim());
	   double scaleFactor = StrToFloat(tokens2[2], fmt);
	   unsigned int minNeighbors = StrToInt(tokens2[3]);
	   unsigned int minSize = StrToInt(tokens2[4]);
	   rc = ocvFaceLandmarkDetector(imgSel, FDKIdx, singleFace, extrapolate, scaleFactor, minNeighbors, minSize);
	   }
	else if (strProc == "loaddnn")
	   {
	   strParams1 = strParams1.Trim().LowerCase();
	   strParams2 = strParams2.Trim().LowerCase();
	   rc = ocvDnnReadModel(strParams1.c_str(), strParams2.c_str(), FDKIdx);
	   }
	else
	   {
	   rc = LERR_UNKNOWN_PROC;
	   return TLipsError::GetMessage(strProc, rc).c_str();
	   }
	if (rc)
	   return TLipsError::SetMessage(strProc, rc).c_str();
	}
catch (int thrnum)
   {
   rc = LERR_INVALID_ARGNUM;
   return TLipsError::GetMessage(strProc, rc).c_str();
   }
catch (...)
   {
   rc = LERR_EXCEPTION;
   return TLipsError::GetMessage(strProc, rc).c_str();
   }
return NULL;
}
//---------------------------------------------------------------------------
// opencv processing - triple params set - all in one
const char* TLuaHost::OcvProc(const char *proc, const char *params1, const char *params2, const char *params3, int &rc)
{
TFormatSettings fmt;
fmt.DecimalSeparator = '.';
String strProc = String(proc).Trim().LowerCase();
String strParams1 = String(params1);
String strParams2 = String(params2);
String strParams3 = String(params3).Trim().LowerCase();
TStringDynArray tokens3;
try {
	if (strProc == "loadclassifierandfacemodel")
	   {
	   //TODO: error if FDKIdx == -1
	   // parse parameters
	   tokens3 = SplitString(strParams3, ",");
	   if (tokens3.Length < 1)
		  throw (int)tokens3.Length;
	   String cascade_name = strParams1;
	   String model_name = strParams2;
	   int model_type = StrToInt(tokens3[0]);
	   rc =	ocvLoadClassifierAndFaceModel(cascade_name.c_str(), model_name.c_str(), (TFaceModelType) model_type, FDKIdx);
	   }
	else if (strProc == "dnnsuperresolution")
	   {
	   String model_name = strParams1;
	   int algorithm = StrToInt(strParams2);
       int scale = StrToInt(strParams3);
	   rc = ocvDnnSuperResolution(model_name.c_str(), (TDnnSuperResType)algorithm, scale);
	   }
	else
	   {
	   rc = LERR_UNKNOWN_PROC;
	   return TLipsError::GetMessage(strProc, rc).c_str();
	   }
	if (rc)
	   return TLipsError::SetMessage(strProc, rc).c_str();
	}
catch (int thrnum)
   {
   rc = LERR_INVALID_ARGNUM;
   return TLipsError::GetMessage(strProc, rc).c_str();
   }
catch (...)
   {
   rc = LERR_EXCEPTION;
   return TLipsError::GetMessage(strProc, rc).c_str();
   }
return NULL;
}
//---------------------------------------------------------------------------

