#ifndef lipGlobalsH
#define lipGlobalsH
#include <Types.hpp>
//enum TImageType  {titRGB24 = 0, titGray8, titFloat3};
struct TExportImage
	{
	char *exptype;
	int width;
	int height;
	int channels;
	void *imageBuff;
	int imageStride;
	void *alphaBuff;   // not used for now
	int alphaStride;   // not used for now
	};
typedef void (__stdcall *TLuaProgress)(int, int);
typedef void (__stdcall *TLuaString)(const char *);
typedef bool (__stdcall *TLuaLoadImage)(const char *, const char *);
typedef bool (__stdcall *TLuaExportImage)(TExportImage *expImage, bool asIs);
typedef int  (__stdcall *TLuaHostDialog)(const char *);
#endif
