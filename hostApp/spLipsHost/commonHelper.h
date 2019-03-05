//---------------------------------------------------------------------------
#ifndef commonHelperH
#define commonHelperH
#include "lipsGlobals.h"
#include "imageenview.hpp"
typedef void __fastcall (__closure *TFrameBuilder)(void);
typedef void __fastcall (__closure *TImportImage)(TExportImage *, bool);
//---------------------------------------------------------------------------
class TCommonHelper
{
  public:
	struct LuaTKRV
	   {
	   String type;
	   String key;
	   String range_low;
	   String range_high;
	   String init_value;
       TStringList *list_values;
       TFrame *frame;
	   };
	static TMemo *luaConsole;
	static TDateTime luaStart;
	static TList *luaInputParams;
	static TFrameBuilder luaParamBuilder;
	static TImportImage luaImportImage;
	static bool luaParamsOK;
	static TIEBitmap *luaPushImage;
	static void __stdcall LuaProgress(int done, int total);
	static void __stdcall LuaMessage(const char *msg);
	static void __stdcall LuaRequire(const char *params);
	static bool __stdcall LuaLoadImage(const char *filename, const char* type);
	static bool __stdcall LuaExportImage(TExportImage *expImage, bool AsIs);
    static int __stdcall LuaHostDialog(const char *dialogText);
	static String StringBetween(String input, String left, String right, String &remainder);
	static void DeleteIPList(void);
	static void CopyExpImage2Bitmap(TExportImage *data, TIEBitmap *bmp, bool AsIs);
};
#endif
