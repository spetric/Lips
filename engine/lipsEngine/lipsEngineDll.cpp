//#include <vcl.h>
#include <windows.h>
#pragma hdrstop
#pragma argsused
#define DLLEXPORT extern "C" __declspec( dllexport )
#include  <SysUtils.hpp>
#include <Classes.hpp>
#include "lipsEngineWrapper.h"
#include "lipsEngineDLL.h"
#include <float.h>
//
//global var wrapper
lipsEngineWrapper gEW;
//---------------------------------------------------------------------------
// exported functions
//---------------------------------------------------------------------------
DLLEXPORT bool __stdcall lipsInit(wchar_t *exePath, bool initializeOcv)
{
// ipp dlls exist
//if (!FileExists(ExtractFilePath(Application->ExeName)+"\\lua51.dll"))
if (!FileExists(String(exePath)+"lua51.dll"))
   {
   return false;    // missing jit
   }
//
// Engine initialization
//
return gEW.Initialize(exePath, initializeOcv);
}
//---------------------------------------------------------------------------
DLLEXPORT bool __stdcall  lipsClose(void)
{
return gEW.Clean();
}
//---------------------------------------------------------------------------
DLLEXPORT bool __stdcall lipsSetSourceImage(void *scanOrig, void *scanAlphaOrig, int width, int height, unsigned int scanlineAlignment)
{
// set input image - shared
return gEW.SetImage(lipsEngineWrapper::stSource, scanOrig, scanAlphaOrig, width, height,  scanlineAlignment);
}
//---------------------------------------------------------------------------
DLLEXPORT bool __stdcall lipsSetTargetImage(void *scanOrig, void *scanAlphaOrig, int width, int height, unsigned int scanlineAlignment)
{
// set output image - shared
return gEW.SetImage(lipsEngineWrapper::stTarget, scanOrig, scanAlphaOrig, width, height,  scanlineAlignment);
}
//---------------------------------------------------------------------------
DLLEXPORT bool __stdcall lipsSetSourceMask(void *scanOrig, int width, int height, unsigned int scanlineAlignment)
{
// set grayscale image - shared
return gEW.SetImage(lipsEngineWrapper::stSourceMask, scanOrig, 0, width, height,  scanlineAlignment);
}
//---------------------------------------------------------------------------
DLLEXPORT bool __stdcall lipsSetTargetMask(void *scanOrig, int width, int height, unsigned int scanlineAlignment)
{
// set grayscale image - shared
return gEW.SetImage(lipsEngineWrapper::stTargetMask, scanOrig, 0, width, height,  scanlineAlignment);
}
//---------------------------------------------------------------------------
DLLEXPORT bool __stdcall lipsSetCustomImage(void *scanOrig, void *scanAlphaOrig, int width, int height, unsigned int scanlineAlignment)
{
// set custom image - shared
return gEW.SetImage(lipsEngineWrapper::stCustom, scanOrig, scanAlphaOrig, width, height,  scanlineAlignment);
}
//---------------------------------------------------------------------------
DLLEXPORT bool __stdcall lipsSetCustomMask(void *scanOrig, int width, int height, unsigned int scanlineAlignment)
{
// set cgrayscale image - shared
return gEW.SetImage(lipsEngineWrapper::stCustomMask, scanOrig, 0, width, height,  scanlineAlignment);
}
//---------------------------------------------------------------------------
DLLEXPORT bool __stdcall lipsPushImage(wchar_t *type, void *scanOrig, void *scanAlphaOrig, int width, int height, unsigned int scanlineAlignment)
{
// push new image (loadimage) - copy
return gEW.PushImage(type, scanOrig, scanAlphaOrig, width, height,  scanlineAlignment);
}
//---------------------------------------------------------------------------
DLLEXPORT bool __stdcall lipsPushTableImage(wchar_t *tabelName, wchar_t *type, void *scanOrig, void *scanAlphaOrig, int width, int height, unsigned int scanlineAlignment)
{
// push image and give it a global name - after script is loaded (requireparams - image) - copy
return gEW.PushTableImage(tabelName, type, scanOrig, scanAlphaOrig, width, height, scanlineAlignment);
}
//---------------------------------------------------------------------------
DLLEXPORT bool __stdcall lipsSetSourceRoi(TRect *roi, bool push2lua)
{
// set source roi
return gEW.SetRoi(lipsEngineWrapper::stSource, roi, push2lua);
}
//---------------------------------------------------------------------------
DLLEXPORT bool __stdcall lipsSetTargetRoi(TRect *roi, bool push2lua)
{
// set target roi
return gEW.SetRoi(lipsEngineWrapper::stTarget, roi, push2lua);
}
//---------------------------------------------------------------------------
DLLEXPORT bool __stdcall lipsSetSilentParams(wchar_t *silentParams)
{
return gEW.SetSilentParams(silentParams);
}
//---------------------------------------------------------------------------
DLLEXPORT int __stdcall lipsLoadScript(wchar_t* path)
{
// load script
return gEW.LoadScript(path);
}
//---------------------------------------------------------------------------
// execute loaded script
DLLEXPORT int __stdcall lipsExecuteScript(wchar_t *params)
{
// load script
return gEW.ExecuteScript(params);
}
//---------------------------------------------------------------------------
// get lua's last error message
DLLEXPORT char* __stdcall lipsGetLastErrorMessage(void)
{
return gEW.LuaErrorMessage.c_str();
}
//---------------------------------------------------------------------------
DLLEXPORT int __stdcall lipsRefreshParams(wchar_t *params)
{
return gEW.RefreshParams(params);
}
//---------------------------------------------------------------------------
// call-back functions
// set progress call back
DLLEXPORT void  __stdcall lipsSetCbProgress(TLuaProgress progressCb)
{
gEW.ProgressCb = progressCb;
}
//---------------------------------------------------------------------------
// set require params call back
DLLEXPORT void  __stdcall lipsSetCbRequireParams(TLuaString requireCb)
{
gEW.RequireCb = requireCb;
}
//---------------------------------------------------------------------------
// set refresh params call back
DLLEXPORT void  __stdcall lipsSetCbRefreshParams(TLuaString refreshCb)
{
gEW.RefreshCb = refreshCb;
}
//---------------------------------------------------------------------------
// set message call back
DLLEXPORT void  __stdcall lipsSetCbMessage(TLuaString messageCb)
{
gEW.MessageCb = messageCb;
}
//---------------------------------------------------------------------------
// set load image call back
DLLEXPORT void  __stdcall lipsSetCbLoadImage(TLuaDoubleString loadimageCb)
{
gEW.LoadImageCb = loadimageCb;
}
//---------------------------------------------------------------------------
// set export image call back
DLLEXPORT void  __stdcall lipsSetCbExportImage(TLuaExportImage exportimageCb)
{
gEW.ExportImageCb = exportimageCb;
}
//---------------------------------------------------------------------------
// set host dialog call back
DLLEXPORT void  __stdcall lipsSetCbHostDialog(TLuaHostDialog hostdialogCb)
{
gEW.HostDialogCb = hostdialogCb;
}
//---------------------------------------------------------------------------
// set export contour call back
DLLEXPORT void  __stdcall lipsSetCbExportRawData(TLuaExportRawData exportrawdataCb)
{
gEW.ExportRawDataCb = exportrawdataCb;
}
//---------------------------------------------------------------------------
// set command  call back
DLLEXPORT void  __stdcall lipsSetCbCommand(TLuaDoubleString commandCb)
{
gEW.CommandCb = commandCb;
}
//---------------------------------------------------------------------------
DLLEXPORT bool __stdcall lipsOcvInstalled(void)
{
return gEW.OcvInstalled;
}
//---------------------------------------------------------------------------
DLLEXPORT bool __stdcall  lipsOcvInitialized(void)
{
return gEW.OcvInitialized;
}
//---------------------------------------------------------------------------
DLLEXPORT void __stdcall  lipsMouse2Lua(bool allow)
{
gEW.Mouse2Lua = allow;
}
//---------------------------------------------------------------------------
// mouse event down
DLLEXPORT void __stdcall  lipsMouseDown(TShiftState State, int X, int Y)
{
gEW.ExecuteMouseEvent(0, State, X, Y);
}
//---------------------------------------------------------------------------
// mouse event move
DLLEXPORT void __stdcall  lipsMouseMove(TShiftState State, int X, int Y)
{
gEW.ExecuteMouseEvent(1, State, X, Y);
}
//---------------------------------------------------------------------------
// mouse event up
DLLEXPORT void __stdcall  lipsMouseUp(TShiftState State, int X, int Y)
{
gEW.ExecuteMouseEvent(2, State, X, Y);
}
//===========================================================================
int WINAPI DllEntryPoint(HINSTANCE hinst, unsigned long reason, void* lpReserved)
{
  return 1;
}

