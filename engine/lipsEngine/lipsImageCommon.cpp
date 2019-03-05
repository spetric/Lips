//---------------------------------------------------------------------------
#pragma hdrstop
#include "lipsImageCommon.h"
#include <math.h>
#include <algorithm>
#pragma package(smart_init)
TList* TImageCommon::FInternalImageList = 0;
TLuaImageByte3 *TImageCommon::SourceImage = 0;
TLuaImageByte3 *TImageCommon::TargetImage = 0;
TLuaImageByte1 *TImageCommon::MaskImage = 0;
TLuaRoi *TImageCommon::SourceRoi = 0;
TLuaRoi *TImageCommon::TargetRoi = 0;
int TImageCommon::_chor = 2;
int TImageCommon::_chog = 1;
int TImageCommon::_chob = 0;
//---------------------------------------------------------------------------
// private block
//---------------------------------------------------------------------------
TRect TImageCommon::getRoiRect(TLuaImageVoid *img, TLuaRoi *roi)
{
TRect retRoi;
if (roi)
   {
   retRoi.left   = roi->left;
   retRoi.right  = roi->right < img->width - 1 ? roi->right + 1 : img->width;
   retRoi.top    = roi->top;
   retRoi.bottom = roi->bottom < img->height - 1 ?roi->bottom + 1 : img->height;
   }
else
   {
   retRoi.left   = 0;
   retRoi.right  = img->width;
   retRoi.top    = 0;
   retRoi.bottom = img->height;
   }
retRoi.NormalizeRect();
return retRoi;
}
//---------------------------------------------------------------------------
// get image from image id
TLuaImageVoid* TImageCommon::GetImageFromId(int id)
{
String type;
return GetImageFromId(id, type);
}
//---------------------------------------------------------------------------
// get image from image id
TLuaImageVoid* TImageCommon::GetImageFromId(int id, String &type)
{
TLuaImageVoid *img = 0;
if (id < 0)
   {
   if (id == -1)
	  {
	  img = (TLuaImageVoid*)SourceImage;
	  type = TYPE_B3;
	  }
   else if (id == -2)
	  {
	  img = (TLuaImageVoid*)TargetImage;
	  type = TYPE_B3;
	  }
   else if (id == -3)
	  {
	  img = (TLuaImageVoid*)MaskImage;
      type = TYPE_B1;
	  }
   else
	  return 0;
   }
else
   {
   if (!FInternalImageList)
	  return 0;
   for (int i = 0; i < FInternalImageList->Count; i++)
	   {
	   // get image from the list
	   TInternalImage *iImg = (TInternalImage*)(FInternalImageList->Items[i]);
	   if (iImg->luaImage->id == id)
		  {
		  img = iImg->luaImage;
          type = iImg->type;
		  break;
		  }
	   }
   }
return img;
}
//---------------------------------------------------------------------------
// create new internal image
TLuaImageVoid* TImageCommon::CreateImage(const char *type, int width, int height, unsigned int scanlineAlignement)
{
//TImageCommon
String iType = String(type);
if (iType != TYPE_B3 && iType != TYPE_B1 && iType != TYPE_F3)
   return 0;
if (width == 0 || height == 0)
   return 0;
if (!FInternalImageList)
   FInternalImageList = new TList();
TInternalImage *internalImage = new TInternalImage(iType, width, height);
if (internalImage)
   FInternalImageList->Add(internalImage);
else
   return 0;
internalImage->luaImage->id = FInternalImageList->Count;
return internalImage->luaImage;
}
//---------------------------------------------------------------------------
// create new internal image  from Opencv Mat (exportedimage) only TYPE_B3 and TYPE_B1
TLuaImageVoid* TImageCommon::CreateImage(SocvImageData *img)
{
if (!img)
   return 0;
String iType;
if (img->channels == 3)
   iType = TYPE_B3;
else
   iType = TYPE_B1;
int width = img->width;
int height = img->height;
if (!FInternalImageList)
   FInternalImageList = new TList();
TInternalImage *internalImage = new TInternalImage(iType, width, height);
if (internalImage)
   FInternalImageList->Add(internalImage);
else
   return 0;
// copy data
Byte *isrcBuff = (Byte*)(img->imageBuff);
Byte *asrcBuff = (Byte*)(img->alphaBuff);
for (int i = 0; i < height; i++)
	{
	// we don't need to flip it, because it's probably processed as flipped
	memcpy((void*)internalImage->luaImage->plane[i], (void*)isrcBuff, img->imageStride);
	isrcBuff = isrcBuff + img->imageStride;
	}
internalImage->luaImage->id = FInternalImageList->Count;
return internalImage->luaImage;
}
//---------------------------------------------------------------------------
void TImageCommon::DeleteInternalImages(void)
{
if (FInternalImageList)
   {
   for (int i = 0; i < FInternalImageList->Count; i++)
	   {
	   TInternalImage *internalImage = (TInternalImage*)(FInternalImageList->Items[i]);
	   delete internalImage;
       internalImage = 0;
	   }
   }
delete FInternalImageList;
FInternalImageList = 0;
}
//---------------------------------------------------------------------------
void TImageCommon::CopyImage(TLuaImageByte1 *inp, TLuaImageByte1 *out, TLuaRoi *srcRoi, TLuaRoi *dstRoi)
{
TRect inpRoi = getRoiRect((TLuaImageVoid*)inp, srcRoi);
TRect outRoi = getRoiRect((TLuaImageVoid*)out, dstRoi);
int ii = outRoi.top;
for (int i = inpRoi.top; i < inpRoi.bottom; i++)
	{
	int jj = outRoi.left;
	for (int j = inpRoi.left; j < inpRoi.right; j++)
		{
		if (ii >= outRoi.bottom || jj >= outRoi.right)
		   break;
		out->plane[ii][jj].ch[0] = inp->plane[i][j].ch[0];
		jj++;
		}
	ii++;
	}
}
//---------------------------------------------------------------------------
void TImageCommon::CopyImage(TLuaImageByte3 *inp, TLuaImageByte3 *out, TLuaRoi *srcRoi, TLuaRoi *dstRoi)
{
TRect inpRoi = getRoiRect((TLuaImageVoid*)inp, srcRoi);
TRect outRoi = getRoiRect((TLuaImageVoid*)out, dstRoi);
int ii = outRoi.top;
for (int i = inpRoi.top; i < inpRoi.bottom; i++)
	{
	int jj = outRoi.left;
	for (int j = inpRoi.left; j < inpRoi.right; j++)
		{
		if (ii >= outRoi.bottom || jj >= outRoi.right)
		   break;
		for (int k = 0; k < 3; k++)
			out->plane[ii][jj].ch[k] = inp->plane[i][j].ch[k];
		jj++;
		}
	ii++;
	}
}
//---------------------------------------------------------------------------
void TImageCommon::CopyImage(TLuaImageFloat3 *inp, TLuaImageFloat3 *out, TLuaRoi *srcRoi, TLuaRoi *dstRoi)
{
TRect inpRoi = getRoiRect((TLuaImageVoid*)inp, srcRoi);
TRect outRoi = getRoiRect((TLuaImageVoid*)out, dstRoi);
int ii = outRoi.top;
for (int i = inpRoi.top; i < inpRoi.bottom; i++)
	{
	for (int j = inpRoi.left; j < inpRoi.right; j++)
		{
		int jj = outRoi.left;
		if (ii >= outRoi.bottom || jj >= outRoi.right)
		   break;
		for (int k = 0; k < 3; k++)
			out->plane[ii][jj].ch[k] = inp->plane[i][j].ch[k];
		jj++;
		}
    ii++;
	}
}
//---------------------------------------------------------------------------
// Color space conversion methods
//---------------------------------------------------------------------------
// RGB -> 3 channel grayscale by luminance (byte3)
void TImageCommon::RgbToGray(TLuaImageByte3 *inp, TLuaImageByte3 *out)
{
// images must be of the same size
for (int i = 0; i < inp->height; i++)
	{
	for (int j = 0; j < inp->width; j++)
		{
		double r = inp->plane[i][j].ch[_chor];
		double g = inp->plane[i][j].ch[_chog];
		double b = inp->plane[i][j].ch[_chob];
		double value = b*0.11 + g * 0.59 + r * 0.30;
		Byte v = value < 0.0 ? 0x00 : (value > 255.0 ? 0xff : value);
		out->plane[i][j].ch[0] = v;
		out->plane[i][j].ch[1] = v;
		out->plane[i][j].ch[2] = v;
		}
	}
}
//---------------------------------------------------------------------------
// RGB -> 1 channel grayscale by luminance (byte1)
void TImageCommon::RgbToGray(TLuaImageByte3 *inp, TLuaImageByte1 *out)
{
// images must be of the same size
for (int i = 0; i < inp->height; i++)
	{
	for (int j = 0; j < inp->width; j++)
		{
		double r = inp->plane[i][j].ch[_chor];
		double g = inp->plane[i][j].ch[_chog];
		double b = inp->plane[i][j].ch[_chob];
		double value = b*0.11 + g * 0.59 + r * 0.30;
		out->plane[i][j].ch[0] =  value < 0.0 ? 0x00 : (value > 255.0 ? 0xff : (Byte)value);
		}
	}
}
//---------------------------------------------------------------------------
// RGB -> XYZ (actually BGR -> ZYX)
void TImageCommon::RgbToXyz(TLuaImageByte3 *inp, TLuaImageFloat3 *out)
{
// images must be of the same size
for (int i = 0; i < inp->height; i++)
	{
	for (int j = 0; j < inp->width; j++)
		{
		double r = inp->plane[i][j].ch[_chor] / 255.0;
		double g = inp->plane[i][j].ch[_chog] / 255.0;
		double b = inp->plane[i][j].ch[_chob] / 255.0;

		r = ((r > 0.04045) ? pow((r + 0.055) / 1.055, 2.4) : (r / 12.92)) * 100.0;
		g = ((g > 0.04045) ? pow((g + 0.055) / 1.055, 2.4) : (g / 12.92)) * 100.0;
		b = ((b > 0.04045) ? pow((b + 0.055) / 1.055, 2.4) : (b / 12.92)) * 100.0;

		out->plane[i][j].ch[2] = r*0.4124564 + g*0.3575761 + b*0.1804375;
		out->plane[i][j].ch[1] = r*0.2126729 + g*0.7151522 + b*0.0721750;
		out->plane[i][j].ch[0] = r*0.0193339 + g*0.1191920 + b*0.9503041;
		}
	}
}
//---------------------------------------------------------------------------
// XYZ -> RGB  (actually ZYX -> BGR)
void TImageCommon::XyzToRgb(TLuaImageFloat3 *inp, TLuaImageByte3 *out)
{
// images must be of the same size
for (int i = 0; i < inp->height; i++)
	{
	for (int j = 0; j < inp->width; j++)
		{
		double x = inp->plane[i][j].ch[_chor] / 100.0;
		double y = inp->plane[i][j].ch[_chog] / 100.0;
		double z = inp->plane[i][j].ch[_chob] / 100.0;

		double r = x * 3.2404542 + y * -1.5371385 + z * -0.4985314;
		double g = x * -0.9692660 + y * 1.8760108 + z * 0.0415560;
		double b = x * 0.0556434 + y * -0.2040259 + z * 1.0572252;

		r = ((r > 0.0031308) ? (1.055*pow(r, 1 / 2.4) - 0.055) : (12.92*r)) * 255.0;
		g = ((g > 0.0031308) ? (1.055*pow(g, 1 / 2.4) - 0.055) : (12.92*g)) * 255.0;
		b = ((b > 0.0031308) ? (1.055*pow(b, 1 / 2.4) - 0.055) : (12.92*b)) * 255.0;

		out->plane[i][j].ch[_chor] = r < 0.0 ? 0x00 : (r > 255.0 ? 0xff : r);
		out->plane[i][j].ch[_chog] = g < 0.0 ? 0x00 : (g > 255.0 ? 0xff : g);
		out->plane[i][j].ch[_chob] = b < 0.0 ? 0x00 : (b > 255.0 ? 0xff : b);
		}
	}
}
//---------------------------------------------------------------------------
// RGB -> CIE-LAB  (actually BGR -> BAL)
void TImageCommon::RgbToLab(TLuaImageByte3 *inp, TLuaImageFloat3 *out)
{
// convert to xyz...yes, duplicating code, but it's faster..and then convert to lab
for (int i = 0; i < inp->height; i++)
	{
	for (int j = 0; j < inp->width; j++)
		{
		// to xyz
		double r = inp->plane[i][j].ch[_chor] / 255.0;
		double g = inp->plane[i][j].ch[_chog] / 255.0;
		double b = inp->plane[i][j].ch[_chob] / 255.0;

		r = ((r > 0.04045) ? pow((r + 0.055) / 1.055, 2.4) : (r / 12.92)) * 100.0;
		g = ((g > 0.04045) ? pow((g + 0.055) / 1.055, 2.4) : (g / 12.92)) * 100.0;
		b = ((b > 0.04045) ? pow((b + 0.055) / 1.055, 2.4) : (b / 12.92)) * 100.0;

		double x = r*0.4124564 + g*0.3575761 + b*0.1804375;
		double y = r*0.2126729 + g*0.7151522 + b*0.0721750;
		double z = r*0.0193339 + g*0.1191920 + b*0.9503041;

		// to lab
		x = x / 95.047;
		y = y / 100.00;
		z = z / 108.883;
		//
		x = (x > 0.008856) ? pow(x, 1/3.0) : (7.787 * x + 16.0 / 116.0);
		y = (y > 0.008856) ? pow(y, 1/3.0) : (7.787 * y + 16.0 / 116.0);
		z = (z > 0.008856) ? pow(z, 1/3.0) : (7.787 * z + 16.0 / 116.0);

		out->plane[i][j].ch[_chor] = (116.0 * y) - 16;     //l
		out->plane[i][j].ch[_chog] = 500 * (x - y);        //a
		out->plane[i][j].ch[_chob] = 200 * (y - z);        //b
		}
	}
}
//---------------------------------------------------------------------------
// RGB -> CIE-LAB  (actually BGR -> BAL)
void TImageCommon::LabToRgb(TLuaImageFloat3 *inp, TLuaImageByte3 *out)
{
// convert to xyz...yes, duplicating code, but it's faster..and then convert to rgb
for (int i = 0; i < inp->height; i++)
	{
	for (int j = 0; j < inp->width; j++)
		{
		// to xyz
		double y = ((double)(inp->plane[i][j].ch[_chor]) + 16.0) / 116.0;
		double x = (double)(inp->plane[i][j].ch[_chog]) / 500.0 + y;
		double z = y - (double)(inp->plane[i][j].ch[_chob]) / 200.0;

		double x3 = x*x*x;
		double y3 = y*y*y;
		double z3 = z*z*z;

		x = ((x3 > 0.008856) ? x3 : ((x - 16.0 / 116.0) / 7.787)) * 95.047;
		y = ((y3 > 0.008856) ? y3 : ((y - 16.0 / 116.0) / 7.787)) * 100.0;
		z = ((z3 > 0.008856) ? z3 : ((z - 16.0 / 116.0) / 7.787)) * 108.883;

		// to rgb
		x = x / 100.0;
		y = y / 100.0;
		z = z / 100.0;

		double r = x * 3.2404542 + y * -1.5371385 + z * -0.4985314;
		double g = x * -0.9692660 + y * 1.8760108 + z * 0.0415560;
		double b = x * 0.0556434 + y * -0.2040259 + z * 1.0572252;

		r = ((r > 0.0031308) ? (1.055*pow(r, 1 / 2.4) - 0.055) : (12.92*r)) * 255.0;
		g = ((g > 0.0031308) ? (1.055*pow(g, 1 / 2.4) - 0.055) : (12.92*g)) * 255.0;
		b = ((b > 0.0031308) ? (1.055*pow(b, 1 / 2.4) - 0.055) : (12.92*b)) * 255.0;

		out->plane[i][j].ch[_chor] = r < 0.0 ? 0x00 : (r > 255.0 ? 0xff : r);
		out->plane[i][j].ch[_chog] = g < 0.0 ? 0x00 : (g > 255.0 ? 0xff : g);
		out->plane[i][j].ch[_chob] = b < 0.0 ? 0x00 : (b > 255.0 ? 0xff : b);
		}
	}
}
//---------------------------------------------------------------------------
// Image processing methods
//---------------------------------------------------------------------------
// fast blur - prepare
TImageCommon::spsFastBlur* TImageCommon::FBPrepare(TLuaImageVoid *inpImg, TLuaImageVoid *outImg,  TRect *boundRect, int radius)
{
if (radius < 1)
	 return 0;
spsFastBlur *fbgData = new spsFastBlur;
fbgData->radius = radius;
fbgData->xs = boundRect ? boundRect->left : 0;
fbgData->ys = boundRect ? boundRect->top : 0;
fbgData->w  = boundRect ? boundRect->Width() : inpImg->width;
fbgData->h  = boundRect ? boundRect->Height() : inpImg->height;
fbgData->wm = fbgData->w - 1;
fbgData->hm = fbgData->h - 1;
fbgData->wh = fbgData->w * fbgData->h;
fbgData->div = radius + radius + 1;
fbgData->ch = new int[fbgData->wh];
fbgData->vmin = new int[std::max(fbgData->w, fbgData->h)];
fbgData->vmax = new int[std::max(fbgData->w, fbgData->h)];
fbgData->dv = new int[256 * fbgData->div];
for (int i = 0; i < 256 * fbgData->div; i++)
	 fbgData->dv[i]=(i/ fbgData->div);
fbgData->inpImg = inpImg;
fbgData->outImg = outImg;
return fbgData;
}
//---------------------------------------------------------------------------
// fast blur unprepare
void TImageCommon::FBUnPrepare(spsFastBlur *fbgData)
{
if (!fbgData)
   return;
delete [](fbgData->ch);
delete [](fbgData->dv);
delete [](fbgData->vmin);
delete [](fbgData->vmax);
delete fbgData;
}
//---------------------------------------------------------------------------
void TImageCommon::FBRunGray(spsFastBlur *fbgData)
{
if (!fbgData)
   return;
int chSum, x, y, i, yp, yi;
int p1, p2;
TTypeByte1 **pixIn  = (TTypeByte1**)fbgData->inpImg->plane;
TTypeByte1 **pixOut = (TTypeByte1**)fbgData->outImg->plane;
yi = 0;
for (y = 0; y < fbgData->h; y++)
	{
	chSum = 0;
	for (i = -fbgData->radius; i <= fbgData->radius; i++)
		{
		chSum += (int)(pixIn[y + fbgData->ys][std::min(fbgData->wm, std::max(i, 0)) + fbgData->xs].ch[0]);
		}
	for (x = 0; x < fbgData->w; x++)
	  {
	  fbgData->ch[yi] = fbgData->dv[chSum];
	  if (y == 0)
		  {
		  fbgData->vmin[x] = std::min(x + fbgData->radius + 1, fbgData->wm);
		  fbgData->vmax[x] = std::max(x - fbgData->radius, 0);
	 	  }
	  p1 = (int)pixIn[y + fbgData->ys][fbgData->vmin[x] + fbgData->xs].ch[0];
	  p2 = (int)pixIn[y + fbgData->ys][fbgData->vmax[x] + fbgData->xs].ch[0];
	  chSum += p1 - p2;
	  yi++;
	  }
	}
for (x = 0; x < fbgData->w; x++)
	{
	chSum = 0;
	yp = -fbgData->radius * fbgData->w;
	for (i = -fbgData->radius; i <= fbgData->radius; i++)
	   {
	   yi = std::max(0, yp) + x;
	   chSum += fbgData->ch[yi];
	   yp += fbgData->w;
	   }
	yi = x;
	for (y = 0; y < fbgData->h ; y++)
	   {
	   pixOut[y + fbgData->ys][x + fbgData->xs].ch[0] = Byte(fbgData->dv[chSum]);
	   if (x == 0)
		  {
		  fbgData->vmin[y] = std::min(y + fbgData->radius+1, fbgData->hm) * fbgData->w;
		  fbgData->vmax[y] = std::max(y - fbgData->radius, 0) * fbgData->w;
		  }
	  p1 = x + fbgData->vmin[y];   //translate ??
	  p2 = x + fbgData->vmax[y];   //translate ??
	  chSum += fbgData->ch[p1] - fbgData->ch[p2];
	  yi += fbgData->w;
	  }
	}
}
//---------------------------------------------------------------------------
/*
void TImageCommon::FBRunRGB(spsFastBlur *fbgData)
{
if (!fbgData)
   return;
int chSum, x, y, i, yp, yi;
int p1, p2;
TTypeByte3 **pix = (TTypeByte3**)fbgData->img->plane;
yi = 0;
for (int k = 0; i < 3; i++)
	{
	for (y = 0; y < fbgData->h; y++)
		{
		chSum = 0;
		for (i = -fbgData->radius; i <= fbgData->radius; i++)
			{
			chSum += (int)(pix[y + fbgData->ys][std::min(fbgData->wm, std::max(i, 0)) + fbgData->xs].ch[k]);
			}
		for (x = 0; x < fbgData->w; x++)
		  {
		  fbgData->ch[yi] = fbgData->dv[chSum];
		  if (y == 0)
			  {
			  fbgData->vmin[x] = std::min(x + fbgData->radius + 1, fbgData->wm);
			  fbgData->vmax[x] = std::max(x - fbgData->radius, 0);
			  }
		  p1 = (int)pix[y + fbgData->ys][fbgData->vmin[x] + fbgData->xs].ch[k];
		  p2 = (int)pix[y + fbgData->ys][fbgData->vmax[x] + fbgData->xs].ch[k];
		  chSum += p1 - p2;
		  yi++;
		  }
		}
	for (x = 0; x < fbgData->w; x++)
		{
		chSum = 0;
		yp = -fbgData->radius * fbgData->w;
		for (i = -fbgData->radius; i <= fbgData->radius; i++)
		   {
		   yi = std::max(0, yp) + x;
		   chSum += fbgData->ch[yi];
		   yp += fbgData->w;
		   }
		yi = x;
		for (y = 0; y < fbgData->h ; y++)
		   {
		   pix[y + fbgData->ys][x + fbgData->xs].ch[k] = Byte(fbgData->dv[chSum]);
		   if (x == 0)
			  {
			  fbgData->vmin[y] = std::min(y + fbgData->radius+1, fbgData->hm) * fbgData->w;
			  fbgData->vmax[y] = std::max(y - fbgData->radius, 0) * fbgData->w;
			  }
		  p1 = x + fbgData->vmin[y];   //translate ??
		  p2 = x + fbgData->vmax[y];   //translate ??
		  chSum += fbgData->ch[p1] - fbgData->ch[p2];
		  yi += fbgData->w;
		  }
		}
	}
}*/




