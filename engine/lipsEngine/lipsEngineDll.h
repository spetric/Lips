//---------------------------------------------------------------------------
#ifndef lipsEngineDllH
#define lipsEngineDllH
#include "lipsGlobals.h"
#ifdef spHostApp
//---------------------------------------------------------------------------
#define DLLIMPORT extern "C" __declspec( dllimport )
//---------------------------------------------------------------------------
DLLIMPORT bool __stdcall  lipsInit(wchar_t *exePath);
DLLIMPORT void __stdcall  lipsClose(void);
DLLIMPORT bool __stdcall  lipsSetSourceImage(void *scanOrig, void *scanAlphaOrig, int width, int height, unsigned int scanlineAlignment);
DLLIMPORT bool __stdcall  lipsSetTargetImage(void *scanOrig, void *scanAlphaOrig, int width, int height, unsigned int scanlineAlignment);
DLLIMPORT bool __stdcall  lipsSetMaskImage(void *scanOrig, int width, int height, unsigned int scanlineAlignment);
DLLIMPORT bool __stdcall  lipsPushImage(wchar_t *type, void *scanOrig, void * scanAlphaOrig, int width, int height, unsigned int scanlineAlignment);
DLLIMPORT bool __stdcall  lipsPushTableImage(wchar_t *tableName, wchar_t *type, void *scanOrig, void *scanAlphaOrig, int width, int height, unsigned int scanlineAlignment);
DLLIMPORT bool __stdcall  lipsSetSourceRoi(TRect *roi);
DLLIMPORT bool __stdcall  lipsSetTargetRoi(TRect *roi);
DLLIMPORT int __stdcall   lipsLoadScript(wchar_t *path);
DLLIMPORT int __stdcall   lipsExecuteScript(wchar_t *params);
DLLIMPORT char* __stdcall lipsGetLastErrorMessage(void);
DLLIMPORT void  __stdcall lipsSetCbProgress(TLuaProgress progressCb);
DLLIMPORT void  __stdcall lipsSetCbRequireParams(TLuaString requireCb);
DLLIMPORT void  __stdcall lipsSetCbMessage(TLuaString requireCb);
DLLIMPORT void  __stdcall lipsSetCbLoadImage(TLuaLoadImage loadimageCb);
DLLIMPORT void  __stdcall lipsSetCbExportImage(TLuaExportImage exportimageCb);
DLLIMPORT int  __stdcall lipsSetCbHostDialog(TLuaHostDialog hostdialogCb);
#endif
//---------------------------------------------------------------------------
#endif
