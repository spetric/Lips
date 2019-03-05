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
OnLuaLoadImage = 0;
OnLuaHostDialog = 0;
FOpenCV.sourceId = 0;
FOpenCV.targetId = 0;
FDKIdx = -1;
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
// require parameter list
int TLuaHost::HostDialog(const char *dialogText)
{
if (OnLuaHostDialog)
   return OnLuaHostDialog(dialogText);
else
   return 1;    // proceed
}
//---------------------------------------------------------------------------
// image routines
//---------------------------------------------------------------------------
// create new internal image
TLuaImageVoid* TLuaHost::CreateImage(const char *type, int width, int height)
{
return TImageCommon::CreateImage(type, width, height);
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
bool TLuaHost::ExportImage(int idExport, const char *exptype)
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
   if (!OnLuaExportImage(&FExpImage, (bool)iexp->orientation))
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
bool TLuaHost::Blur(int idInput, int idOutput, const char *radius)
{
//test - grayscale only
TLuaImageVoid  *ii = TImageCommon::GetImageFromId(idInput);
TLuaImageVoid  *oi = TImageCommon::GetImageFromId(idOutput);
if (ii == 0 || oi == 0)
   return false;
if (ii->height != oi->height || ii->width != oi->width)
   return false;
String r = String(radius);
TImageCommon::spsFastBlur *fbgData = TImageCommon::FBPrepare(ii, oi, 0, StrToInt(r));
TImageCommon::FBRunGray(fbgData);
TImageCommon::FBUnPrepare(fbgData);
return true;
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
	  ocvRoi.top = ii->height - TImageCommon::SourceRoi->top - 1;
	  ocvRoi.bottom = ii->height - TImageCommon::SourceRoi->bottom - 1;
	   if (TImageCommon::TargetRoi)
		  ocvSetRoi(OCW_ROI_SOURCE, ocvRoi);
	   else
		  ocvSetRoi(OCW_ROI_ALL, ocvRoi);
	   }
   if (TImageCommon::TargetRoi)
	  {
	  ocvRoi.left = TImageCommon::TargetRoi->left;
	  ocvRoi.right = TImageCommon::TargetRoi->right;
	  ocvRoi.top = oi->height - TImageCommon::TargetRoi->top - 1;
	  ocvRoi.bottom = oi->height - TImageCommon::TargetRoi->bottom - 1;
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
	  ocvRoi.top = ii->height - roi->top - 1;
	  ocvRoi.bottom = ii->height - roi->bottom - 1;
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
	  ocvRoi.top = oi->height - roi->top - 1;
	  ocvRoi.bottom = oi->height - roi->bottom - 1;
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
// opencv get
int TLuaHost::OcvGet(const char *proc)
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
return nRet;
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
try {
	if (strProc == "blur")
	   {
	   if (tokens.Length < 2)
		  throw tokens.Length;
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
	   if (tokens.Length < 1)
		  throw tokens.Length;
	   unsigned int kSize = StrToInt(tokens[0]);
	   rc = ocvMedianBlur(kSize);
	   }
	else if (strProc == "gaussian")
	   {
	   if (tokens.Length < 4)
		  throw tokens.Length;
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
	   if (tokens.Length < 3)
		  throw tokens.Length;
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
	   if (tokens.Length < 4)
		  throw tokens.Length;
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
	   if (tokens.Length < 5)
		  throw tokens.Length;
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
	   if (tokens.Length < 6)
		  throw tokens.Length;
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
	   if (tokens.Length < 3)
		  throw tokens.Length;
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
	   if (tokens.Length < 3)
		  throw tokens.Length;
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
	   if (tokens.Length < 4)
		  throw tokens.Length;
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
	   if (tokens.Length < 4)
		  throw tokens.Length;
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
	   if (tokens.Length < 5)
		  throw tokens.Length;
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
	   if (tokens.Length < 3)
		  throw tokens.Length;
	   double thresh = StrToFloat(tokens[0], fmt);
	   double maxval = StrToFloat(tokens[1], fmt);
	   int type = StrToInt(tokens[2]);
	   rc = ocvThreshold(thresh, maxval, (TThresholdType)type);
	   }
	else if (strProc == "adaptivethreshold")
	   {
	   if (tokens.Length < 5)
		  throw tokens.Length;
	   double maxval = StrToFloat(tokens[0], fmt);
	   int method = StrToInt(tokens[1]);
	   int type = StrToInt(tokens[2]);
	   unsigned int radius = StrToInt(tokens[3]);
	   double C = StrToFloat(tokens[4], fmt);
	   rc = ocvAdaptiveThreshold(maxval, (TAdaptiveThresholdType) method, (TThresholdType) type, radius, C);
	   }
	else if (strProc == "distancetransform")
	   {
	   if (tokens.Length < 3)
		  throw tokens.Length;
	   bool gray = StrToBool(tokens[0].Trim());
	   int type = StrToInt(tokens[1]);
	   int maskSize = StrToInt(tokens[2]);
	   rc = ocvDistanceTransform(gray, (TDistanceType)type, (TDistanceMaskType) maskSize);
	   }
	else if (strProc == "canny")
	   {
	   if (tokens.Length < 4)
		  throw tokens.Length;
	   double threshold1 = StrToFloat(tokens[0], fmt);
	   double threshold2 = StrToFloat(tokens[1], fmt);
	   unsigned int apertureSize = StrToInt(tokens[2]);
	   bool L2gradient = StrToBool(tokens[3].Trim());
	   rc = ocvCanny(threshold1, threshold2, apertureSize, L2gradient);
	   }
	else if (strProc == "flip")
	   {
	   if (tokens.Length < 1)
		  throw tokens.Length;
	   int flipCode = StrToInt(tokens[0]);
	   rc = ocvFlip(flipCode);
	   }
	else if (strProc == "gaborius")
	   {
	   if (tokens.Length < 7)
		  throw tokens.Length;
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
	   if (tokens.Length < 8)
		  throw tokens.Length;
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
	   if (tokens.Length < 2)
		  throw tokens.Length;
	   float sigma_s = StrToFloat(tokens[0], fmt);
	   float sigma_r = StrToFloat(tokens[1], fmt);
	   rc = ocvDetailEnhance(sigma_s, sigma_r);
	   }
	else if (strProc == "edgepreservingfilter")
	   {
	   if (tokens.Length < 3)
		  throw tokens.Length;
	   int flags = StrToInt(tokens[0]);
	   float sigma_s = StrToFloat(tokens[1], fmt);
	   float sigma_r = StrToFloat(tokens[2], fmt);
	   rc =	ocvEdgePreservingFilter(flags, sigma_s, sigma_r);
	   }
	else if (strProc == "pencilsketch")
	   {
	   if (tokens.Length < 4)
		  throw tokens.Length;
	   bool grayscale = StrToBool(tokens[0].Trim());
	   float sigma_s = StrToFloat(tokens[1], fmt);
	   float sigma_r = StrToFloat(tokens[2], fmt);
	   float shade_factor = StrToFloat(tokens[3], fmt);
	   rc = ocvPencilSketch(grayscale, sigma_s, sigma_r, shade_factor);
	   }
	else if (strProc == "stylization")
	   {
	   if (tokens.Length < 2)
		  throw tokens.Length;
	   float sigma_s = StrToFloat(tokens[0], fmt);
	   float sigma_r = StrToFloat(tokens[1], fmt);
	   rc = ocvStylization(sigma_s, sigma_r);
	   }
	else if (strProc == "oilpainting")
	   {
	   if (tokens.Length < 2)
		  throw tokens.Length;
	   int size = StrToInt(tokens[0]);
	   int dynRatio = StrToInt(tokens[1]);
	   rc = ocvOilPainting(size, dynRatio);
	   }
	else if (strProc == "loaddnn")
	   {
	   rc = ocvDnnReadModel(strParams.c_str(), FDKIdx);
	   }
	else if (strProc == "styletransfer")
	   {
	   // parse parameters
	   if (tokens.Length < 6)
		  throw tokens.Length;
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
	   if (tokens.Length < 5)
		  throw tokens.Length;
	   int x = StrToInt(tokens[0]);
	   int y = StrToInt(tokens[1]);
	   double maxRadius = StrToFloat(tokens[2], fmt);
	   bool exportImage = StrToBool(tokens[3].Trim());
	   int flag = StrToInt(tokens[4]);
	   rc = ocvWarpPolar(x, y, maxRadius, exportImage, flag);
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
		  throw tokens1.Length;
	   unsigned int kernelSize = StrToInt(tokens1[0]);
	   unsigned int kernelLength = kernelSize *kernelSize;
	   float div = StrToFloat(tokens1[1], fmt);
	   float bias = StrToFloat(tokens1[2], fmt);
	   // parse kernel
	   tokens2 = SplitString(strParams2, ",");
	   if (tokens2.Length <  kernelLength)
		  throw tokens2.Length;
	   // parse kernel
	   float kernel[33*33]; // max kernel = 33x33
	   for (int i = 0; i < kernelLength; i++)
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
		  throw tokens2.Length;
	   bool singleFace = StrToBool(tokens2[0].Trim());
	   bool extrapolate = StrToBool(tokens2[1].Trim());
	   double scaleFactor = StrToFloat(tokens2[2], fmt);
	   unsigned int minNeighbors = StrToInt(tokens2[3]);
	   unsigned int minSize = StrToInt(tokens2[4]);
	   rc = ocvFaceLandmarkDetector(imgSel, FDKIdx, singleFace, extrapolate, scaleFactor, minNeighbors, minSize);
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
		  throw tokens3.Length;
	   String cascade_name = strParams1;
	   String model_name = strParams2;
	   int model_type = StrToInt(tokens3[0]);
	   rc =	ocvLoadClassifierAndFaceModel(cascade_name.c_str(), model_name.c_str(), (TFaceModelType) model_type, FDKIdx);
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

