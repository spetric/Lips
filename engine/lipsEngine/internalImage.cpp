//---------------------------------------------------------------------------
#pragma hdrstop
#include "internalImage.h"
//---------------------------------------------------------------------------
#pragma package(smart_init)
//---------------------------------------------------------------------------
// non shared
TInternalImage::TInternalImage(String sType, int width, int height, bool addAlpha, unsigned int scanlineAlignment, bool contiguous, bool asIs)
{
type = sType;
speOrientation orient = asIs ? orAsIs : orInvert;
alpha = 0;
if (type == TYPE_B3)
   {
   TLuaImageByte3 *lImage = new TLuaImageByte3;
   spImage<ptRGB> *img = new spImage<ptRGB>(width, height, scanlineAlignment, contiguous, orient);
   image = static_cast<void*>(img);
   lImage->channels = 3;
   lImage->imageStride = img->Stride();
   lImage->align  = img->Alignment();
   lImage->orientation = (int)orient;
   lImage->plane = (TTypeByte3**)img->Data();
   if (addAlpha)
	  {
	  alpha = new spImage<ptGray>(width, height, scanlineAlignment, contiguous, orient);
	  lImage->alpha = (TTypeByte1**) alpha->Data();
	  lImage->alphaStride = alpha->Stride();
	  }
   else
	  {
	  lImage->alpha = 0;
	  lImage->alphaStride = 0;
	  }
   luaImage = (TLuaImageVoid*)(lImage);
   }
else if (type == TYPE_B1)
   {
   TLuaImageByte1 *lImage = new TLuaImageByte1;
   lImage->alpha = 0;
   lImage->alphaStride = 0;
   spImage<ptGray> *img = new spImage<ptGray>(width, height, scanlineAlignment, contiguous, orient);
   image = static_cast<void*>(img);
   lImage->channels = 1;
   lImage->imageStride = img->Stride();
   lImage->align  = img->Alignment();
   lImage->orientation = (int)orient;
   lImage->plane = (TTypeByte1**)img->Data();
   luaImage = (TLuaImageVoid*)(lImage);
   }
else if (type == TYPE_F3)
   {
   TLuaImageFloat3 *lImage = new TLuaImageFloat3;
   lImage->alpha = 0;
   lImage->alphaStride = 0;
   spImage<ptFloat3> *img = new spImage<ptFloat3>(width, height, scanlineAlignment, contiguous);
   image = static_cast<void*>(img);
   lImage->channels = 3;
   lImage->imageStride = img->Stride();
   lImage->align  = img->Alignment();
   lImage->orientation = (int)orient;
   lImage->plane = (TTypeFloat3**)img->Data();
   luaImage = (TLuaImageVoid*)(lImage);
   }
luaImage->width = width;
luaImage->height = height;
}
//---------------------------------------------------------------------------
// shared / non-shared
TInternalImage::TInternalImage(String sType, void *scanImage, void *scanAlpha, int width, int height,
							   unsigned int scanlineAlignment, bool shared, bool asIs)
{
type = sType;
speOrientation orient = asIs ? orAsIs : orInvert;
alpha = 0;
if (type == TYPE_B3)
   {
   TLuaImageByte3 *lImage = new TLuaImageByte3;
   lImage->alpha = 0;
   spImage<ptRGB> *img = new spImage<ptRGB>(scanImage, width, height, scanlineAlignment, shared, orient);
   if (scanAlpha)
	  {
	  alpha = new spImage<ptGray>(scanAlpha, width, height, scanlineAlignment, shared, orient);
	  lImage->alpha = (TTypeByte1**)alpha->Data();
      lImage->alphaStride = alpha->Stride();
	  }
   image = static_cast<void*>(img);
   lImage->channels = 3;
   lImage->imageStride = img->Stride();
   lImage->align  = img->Alignment();
   lImage->orientation = (int)orient;
   lImage->plane = (TTypeByte3**)(img->Data());
   luaImage = (TLuaImageVoid*)(lImage);
   }
else if (type == TYPE_B1)
   {
   TLuaImageByte1 *lImage = new TLuaImageByte1;
   lImage->alpha = 0;
   spImage<ptGray> *img = new spImage<ptGray>(scanImage, width, height, scanlineAlignment, shared, orient);
   if (scanAlpha)
	  {
	  alpha = new spImage<ptGray>(scanAlpha, width, height, scanlineAlignment, shared, orient);
	  lImage->alpha = (TTypeByte1**)alpha->Data();
      lImage->alphaStride = alpha->Stride();
	  }
   image = static_cast<void*>(img);
   lImage->channels = 1;
   lImage->imageStride = img->Stride();
   lImage->align  = img->Alignment();
   lImage->orientation = (int)orient;
   lImage->plane = (TTypeByte1**)img->Data();
   luaImage = (TLuaImageVoid*)(lImage);
   }
else if (type == TYPE_F3)
   {
   TLuaImageFloat3 *lImage = new TLuaImageFloat3;
   lImage->alpha = 0;
   spImage<ptFloat3> *img = new spImage<ptFloat3>(scanImage, width, height, scanlineAlignment, shared, orient);
   if (scanAlpha)
	  {
	  alpha = new spImage<ptGray>(scanAlpha, width, height, scanlineAlignment, shared, orient);
	  lImage->alpha = (TTypeByte1**)alpha->Data();
	  lImage->alphaStride = alpha->Stride();
	  }
   image = static_cast<void*>(img);
   lImage->channels = 3;
   lImage->imageStride = img->Stride();
   lImage->align  = img->Alignment();
   lImage->plane = (TTypeFloat3**)img->Data();
   luaImage = (TLuaImageVoid*)(lImage);
   }
luaImage->width = width;
luaImage->height = height;
}
//---------------------------------------------------------------------------
// destructor
TInternalImage::~TInternalImage(void)
{
if (type == TYPE_B3)
   {
   spImage<ptRGB> *img = static_cast<spImage<ptRGB>*>(image);
   delete img;
   luaImage->plane = 0;
   luaImage->alpha = 0;
   TLuaImageByte3 *lImage = (TLuaImageByte3*)luaImage;
   delete lImage;
   }
else if (type == TYPE_B1)
   {
   spImage<ptGray> *img = static_cast<spImage<ptGray>*>(image);
   delete img;
   luaImage->plane = 0;
   luaImage->alpha = 0;
   TLuaImageByte1 *lImage = (TLuaImageByte1*)luaImage;
   delete lImage;
   }
else if (type == TYPE_F3)
   {
   spImage<ptFloat3> *img = static_cast<spImage<ptFloat3>*>(image);
   delete img;
   luaImage->plane = 0;
   luaImage->alpha = 0;
   TLuaImageFloat3 *lImage = (TLuaImageFloat3*)luaImage;
   delete lImage;
   }
if (alpha)
   delete alpha;
}
//---------------------------------------------------------------------------

