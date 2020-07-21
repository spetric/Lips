//---------------------------------------------------------------------------
#ifndef lipsImageCommonH
#define lipsImageCommonH
#include "lipsGlobals.h"
#include <Types.hpp>
#include <Classes.hpp>
#include "internalImage.h"
#include "ocvGlobals.h"
//---------------------------------------------------------------------------
class TImageCommon
{
	private:
		static TList* FInternalImageList;
		static int _chor, _chog, _chob;
		static TRect getRoiRect(TLuaImageVoid *img, TLuaRoi *roi);
	public:
		 struct spsFastBlur
			 {
			 int radius;
			 int xs;
			 int ys;
			 int w;
			 int h;
			 int wm;
			 int hm;
			 int wh;
			 int div;
			 int *ch;
			 int *vmin;
			 int *vmax;
			 int *dv;
			 TLuaImageVoid *inpImg;
			 TLuaImageVoid *outImg;
			 };
		// host images - filled from main engine
		static TLuaImageByte3 *SourceImage, *TargetImage, *CustomImage;
		static TLuaImageByte1 *SourceMask, *TargetMask, *CustomMask;
		// host rois - filled from main engine
		static TLuaRoi *SourceRoi, *TargetRoi;
        //
		static TLuaImageVoid* CreateImage(const char *type, int width, int height, bool addAlpha = false, unsigned int scanlineAlignement = 0);
		static TLuaImageVoid* CreateImage(SocvImageData *img);
		static void DeleteInternalImages(void);
        static bool ReleaseInternalImage(int idx);
        static TLuaImageVoid* GetImageFromId(int id);
		static TLuaImageVoid* GetImageFromId(int id, String &type);
		// image manipulation
		static void CopyImage(TLuaImageByte1 *inp, TLuaImageByte1 *out, TLuaRoi *srcRoi, TLuaRoi *dstRoi);
		static void CopyImage(TLuaImageByte3 *inp, TLuaImageByte3 *out, TLuaRoi *srcRoi, TLuaRoi *dstRoi);
		static void CopyImage(TLuaImageFloat3 *inp, TLuaImageFloat3 *out, TLuaRoi *srcRoi, TLuaRoi *dstRoi);
		// color space conversion methods
		static void RgbToGray(TLuaImageByte3 *inp, TLuaImageByte3 *out);
		static void RgbToGray(TLuaImageByte3 *inp, TLuaImageByte1 *out);
		static void GrayToRgb(TLuaImageByte1 *inp, TLuaImageByte3 *out, bool alpha = false);
		static void RgbToXyz(TLuaImageByte3 *inp, TLuaImageFloat3 *out);
		static void XyzToRgb(TLuaImageFloat3 *inp, TLuaImageByte3 *out);
		static void RgbToLab(TLuaImageByte3 *inp, TLuaImageFloat3 *out);
		static void LabToRgb(TLuaImageFloat3 *inp, TLuaImageByte3 *out);
		// image processing
		static spsFastBlur* FBPrepare(TLuaImageVoid *intpImg, TLuaImageVoid *outImg, TRect *boundRect, int radius);
		static void FBUnPrepare(spsFastBlur *fbgData);
		static void FBRunGray(spsFastBlur *fbgData);
		static void FBRunRGB(spsFastBlur *fbgData);
		static void BlendWithMask(TLuaImageByte3 *inp, TLuaImageByte1 *mask, TLuaImageByte3 *out, bool alpha);
		static void DownsampleGray(TLuaImageByte1 *imgIn, TLuaImageByte1 *imgOut);
		static void TImageCommon::DownsampleRGB(TLuaImageByte3 *imgIn, TLuaImageByte3 *imgOut);
};
#endif
