//---------------------------------------------------------------------------
#ifndef internalImageH
#define internalImageH
//#include <Types.hpp>
#include "spImage.hpp"
#include "spPixelTypes.hpp"
struct TTypeByte3
  {
  unsigned char ch[3];
  };
struct TTypeByte1
  {
  unsigned char ch[1];
  };
struct TTypeFloat3
  {
  float ch[3];
  };
struct TLuaImageByte3
  {
  int id;
  int channels;
  int width;
  int height;
  int imageStride;
  int alphaStride;
  int align;
  int orientation;
  TTypeByte3 **plane;
  TTypeByte1 **alpha;
  };
struct TLuaImageByte1
  {
  int id;
  int channels;
  int width;
  int height;
  int imageStride;
  int alphaStride;
  int align;
  int orientation;
  TTypeByte1 **plane;
  TTypeByte1 **alpha;
  };
struct TLuaImageFloat3
  {
  int id;
  int channels;
  int width;
  int height;
  int imageStride;
  int alphaStride;
  int align;
  int orientation;
  TTypeFloat3 **plane;
  TTypeByte1 **alpha;
  };
struct TLuaImageVoid
  {
  int id;
  int channels;
  int width;
  int height;
  int imageStride;
  int alphaStride;
  int align;
  int orientation;
  void **plane;
  void **alpha;
  };
struct TLuaRoi
  {
  int left;
  int right;
  int top;
  int bottom;
  };
const String TYPE_B3  =  "rgb24";
const String TYPE_B1   = "grayscale";
const String TYPE_F3   = "float3";
//---------------------------------------------------------------------------
class TInternalImage
{
	private:
	public:
		// non shared
		TInternalImage(String sType, int width, int height, unsigned int scanlineAlignment = 0, bool contiguous = true, bool asIs = false);
		// shared
		TInternalImage(String sType, void *scanImage, void *scanAlpha, int width, int height, unsigned int scanlineAlignment, bool shared = true, bool asIs = false);
		virtual ~TInternalImage(void);
		String type;
		void *image;
        spImage<ptGray> *alpha;
		TLuaImageVoid *luaImage;
};
//---------------------------------------------------------------------------
#endif
