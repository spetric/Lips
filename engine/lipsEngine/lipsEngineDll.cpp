//#include <vcl.h>
#include <windows.h>
#pragma hdrstop
#pragma argsused
#define DLLEXPORT extern "C" __declspec( dllexport )
#include  <SysUtils.hpp>
#include "lipsEngineWrapper.h"
#include "lipsEngineDLL.h"
#include <float.h>
//
//global var wrapper
lipsEngineWrapper gEW;
//---------------------------------------------------------------------------
// exported functions
//---------------------------------------------------------------------------
DLLEXPORT bool __stdcall lipsInit(wchar_t *exePath)
{
// ipp dlls exist
//if (!FileExists(ExtractFilePath(Application->ExeName)+"\\lua51.dll"))
if (!FileExists(String(exePath)+"lua51.dll"))
   {
   return false;
   }
//
// Engine initialization
//
try
  {
  //_clear87();
  //_control87(MCW_EM, MCW_EM);
  gEW.Initialize(exePath);
  return true;
  }
catch (...)
  {
  return false;
  }
}
//---------------------------------------------------------------------------
DLLEXPORT void __stdcall  lipsClose(void)
{
gEW.Clean();
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
DLLEXPORT bool __stdcall lipsSetMaskImage(void *scanOrig, int width, int height, unsigned int scanlineAlignment)
{
// set grayscale image - shared
return gEW.SetImage(lipsEngineWrapper::stMask, scanOrig, 0, width, height,  scanlineAlignment);
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
DLLEXPORT bool __stdcall lipsSetSourceRoi(TRect *roi)
{
// load script
return gEW.SetRoi(lipsEngineWrapper::stSource, roi);
}
//---------------------------------------------------------------------------
DLLEXPORT bool __stdcall lipsSetTargetRoi(TRect *roi)
{
// load script
return gEW.SetRoi(lipsEngineWrapper::stSource, roi);
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
// set message call back
DLLEXPORT void  __stdcall lipsSetCbMessage(TLuaString messageCb)
{
gEW.MessageCb = messageCb;
}
//---------------------------------------------------------------------------
// set load image call back
DLLEXPORT void  __stdcall lipsSetCbLoadImage(TLuaLoadImage loadimageCb)
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
DLLEXPORT int  __stdcall lipsSetCbHostDialog(TLuaHostDialog hostdialogCb)
{
gEW.HostDialogCb = hostdialogCb;
}
//===========================================================================
int WINAPI DllEntryPoint(HINSTANCE hinst, unsigned long reason, void* lpReserved)
{
  return 1;
}

