//---------------------------------------------------------------------------
#ifndef lipsEngineWrapperH
#define lipsEngineWrapperH
#include <Types.hpp>
#include "lua.hpp"
#include "luaHostWrap.h"
//---------------------------------------------------------------------------
// Define the Lua ClassName
//const char TLuaHostWrap::className[] = "TLuaHostWrap";
//
class lipsEngineWrapper
  {
  public:
	  enum TSurfaceType {stSource = 0, stTarget, stMask};
  private:
	  AnsiString FLuaErrorString, FScript, FExePath;
      bool FExeWin32;
	  int FResult;
	  TLuaHost *FHostProc;
	  lua_State *FLs;
	  //spImage<ptRGB> *FII, *FOI;
	  TInternalImage *FSourceImage, *FTargetImage;
	  //TLuaImageByte3 *FInputImage, *FOutputImage;
	  //TLuaImageByte1 *FMaskImage;
	  TLuaRoi *FSourceRoi, *FTargetRoi;
	  void getErrorMessage(void);
      String stringBetween(String input, String left, String right, String &remainder);
  public:
	  lipsEngineWrapper(void);
	  virtual ~lipsEngineWrapper(void);
	  // public methods
	  bool Initialize(wchar_t *exePath);
      void Clean(void);
	  bool SetImage(TSurfaceType type, void *scanImage, void *scanAlpha, int width, int height, unsigned int scanlineAlignment);
      bool PushImage(wchar_t * type, void *scanImage, void *scanAlpha, int width, int height, unsigned int scanlineAlignment);
	  bool PushTableImage(wchar_t *luaName, wchar_t * type, void *scanImage, void *scanAlpha, int width, int height, unsigned int scanlineAlignment);
	  bool SetRoi(TSurfaceType type, TRect *roi);
	  int LoadScript(wchar_t *path);
	  int ExecuteScript(wchar_t *params);
	  // public vars
	  TLuaProgress ProgressCb;
	  TLuaString RequireCb;
	  TLuaString MessageCb;
	  TLuaLoadImage LoadImageCb;
	  TLuaExportImage ExportImageCb;
      TLuaHostDialog  HostDialogCb;
  // R/O properties
  __property AnsiString LuaErrorMessage = {read = FLuaErrorString};
  /*
  __property spSurfaceContainer* Surf = {read = FSurf};
  __property spRenderDataKeeper* RenderData = {read = FRenderData};
  __property TBrushBasic* Brush = {read = FBrush};
  __property TRenderBasic* Render = {read = FRender};
  __property TGrapBasic *Shape	= {read = FShape};
  __property TspeShapeType ShapeType = {read = FShapeType};
  __property Tspe_UndoBlock UndoBlock = {read = FUndoBlock};
  __property Tspe_UndoBlock RedoBlock = {read = FRedoBlock};
  */
  };
#endif
