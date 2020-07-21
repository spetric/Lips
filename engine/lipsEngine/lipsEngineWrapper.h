//---------------------------------------------------------------------------
#ifndef lipsEngineWrapperH
#define lipsEngineWrapperH
#include <Types.hpp>
#include <Classes.hpp>
#include "lua.hpp"
#include "luaHostWrap.h"
//---------------------------------------------------------------------------
// Define the Lua ClassName
//const char TLuaHostWrap::className[] = "TLuaHostWrap";
//
class lipsEngineWrapper
  {
  public:
	  enum TSurfaceType {stSource = 0, stTarget, stSourceMask, stTargetMask, stCustom, stCustomMask};
  private:
	  AnsiString FLuaErrorString, FScript, FExePath, FSilentParams;
      bool FExeWin32, FOcvAlive, FOcvInitialized, FMouseEventsOn;
	  int FResult;
	  TLuaHost *FHostProc;
	  lua_State *FLs;
	  //spImage<ptRGB> *FII, *FOI;
	  TInternalImage *FSourceImage, *FTargetImage, *FSourceMask, *FTargetMask, *FCustomImage, *FCustomMask;
	  //TLuaImageByte3 *FInputImage, *FOutputImage;
	  //TLuaImageByte1 *FMaskImage;
	  TLuaRoi *FSourceRoi, *FTargetRoi;
	  String stringBetween(String input, String left, String right, String &remainder);
	  void getErrorMessage(void);
	  int  parseParams(lua_State *L, wchar_t *params);
	  void setMouseStatus (const char *index, bool value);
      void setLuaRoi(TLuaRoi *lRoi, int type);
  public:
	  lipsEngineWrapper(void);
	  virtual ~lipsEngineWrapper(void);
	  // public methods
	  bool Initialize(wchar_t *exePath, bool initializeOcv);
      bool Clean(void);
	  bool SetImage(TSurfaceType type, void *scanImage, void *scanAlpha, int width, int height, unsigned int scanlineAlignment);
      bool PushImage(wchar_t * type, void *scanImage, void *scanAlpha, int width, int height, unsigned int scanlineAlignment);
	  bool PushTableImage(wchar_t *luaName, wchar_t * type, void *scanImage, void *scanAlpha, int width, int height, unsigned int scanlineAlignment);
	  bool SetRoi(TSurfaceType type, TRect *roi, bool push2lua);
	  bool SetSilentParams(wchar_t *silentParams);
	  bool SetCustomRoi(TRect *roi);
	  bool SetCustomImage(void *scanImage, void *scanAlpha, int width, int height, unsigned int scanlineAlignment, bool shared);
	  bool SetCustomMask(void *scanImage, int width, int height, unsigned int scanlineAlignment, bool shared);
	  int LoadScript(wchar_t *path);
	  int ExecuteScript(wchar_t *params);
	  int RefreshParams(wchar_t *params);
	  int ExecuteMouseEvent(unsigned int event, TShiftState State, int X, int Y);
	  // public vars
	  TLuaProgress ProgressCb;
	  TLuaString RequireCb;
	  TLuaString RefreshCb;
	  TLuaString MessageCb;
	  TLuaDoubleString LoadImageCb;
	  TLuaExportImage ExportImageCb;
	  TLuaHostDialog  HostDialogCb;
	  TLuaDoubleString CommandCb;
	  TLuaExportRawData ExportRawDataCb;
  // R/O properties
  __property AnsiString LuaErrorMessage = {read = FLuaErrorString};
  __property bool OcvInstalled  = {read = FOcvAlive};
  __property bool OcvInitialized = {read = FOcvInitialized};
  __property bool Mouse2Lua = {read = FMouseEventsOn, write = FMouseEventsOn};
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
