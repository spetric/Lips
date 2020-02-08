//---------------------------------------------------------------------------
#ifndef lipsEngineDllH
#define lipsEngineDllH
#include "lipsGlobals.h"
#ifdef spHostApp
//---------------------------------------------------------------------------
#define DLLIMPORT extern "C" __declspec( dllimport )
//---------------------------------------------------------------------------
DLLIMPORT bool __stdcall  lipsInit(wchar_t *exePath, bool initializeOcv = true);
DLLIMPORT bool __stdcall  lipsClose(void);
DLLIMPORT bool __stdcall  lipsSetSourceImage(void *scanOrig, void *scanAlphaOrig, int width, int height, unsigned int scanlineAlignment);
DLLIMPORT bool __stdcall  lipsSetTargetImage(void *scanOrig, void *scanAlphaOrig, int width, int height, unsigned int scanlineAlignment);
DLLIMPORT bool __stdcall  lipsSetSourceMask(void *scanOrig, int width, int height, unsigned int scanlineAlignment);
DLLIMPORT bool __stdcall  lipsSetTargetMask(void *scanOrig, int width, int height, unsigned int scanlineAlignment);
DLLIMPORT bool __stdcall  lipsSetCustomImage(void *scanOrig, void *scanAlphaOrig, int width, int height, unsigned int scanlineAlignment);
DLLIMPORT bool __stdcall  lipsSetCustomMask(void *scanOrig, int width, int height, unsigned int scanlineAlignment);
DLLIMPORT bool __stdcall  lipsSetSourceRoi(TRect *roi, bool push2lua = false);
DLLIMPORT bool __stdcall  lipsSetTargetRoi(TRect *roi, bool push2lua = false);
//
DLLIMPORT bool __stdcall  lipsPushImage(wchar_t *type, void *scanOrig, void * scanAlphaOrig, int width, int height, unsigned int scanlineAlignment);
DLLIMPORT bool __stdcall  lipsPushTableImage(wchar_t *tableName, wchar_t *type, void *scanOrig, void *scanAlphaOrig, int width, int height, unsigned int scanlineAlignment);
DLLIMPORT bool __stdcall  lipsSetSilentParams(wchar_t *silentParams);
//
DLLIMPORT int __stdcall   lipsLoadScript(wchar_t *path);
DLLIMPORT int __stdcall   lipsExecuteScript(wchar_t *params);
DLLIMPORT char* __stdcall lipsGetLastErrorMessage(void);
DLLIMPORT int __stdcall   lipsRefreshParams(wchar_t *params);
DLLIMPORT void  __stdcall lipsSetCbProgress(TLuaProgress progressCb);
DLLIMPORT void  __stdcall lipsSetCbRequireParams(TLuaString requireCb);
DLLIMPORT void  __stdcall lipsSetCbRefreshParams(TLuaString refreshCb);
DLLIMPORT void  __stdcall lipsSetCbMessage(TLuaString requireCb);
DLLIMPORT void  __stdcall lipsSetCbLoadImage(TLuaDoubleString loadimageCb);
DLLIMPORT void  __stdcall lipsSetCbExportImage(TLuaExportImage exportimageCb);
DLLIMPORT void  __stdcall lipsSetCbHostDialog(TLuaHostDialog hostdialogCb);
DLLIMPORT void  __stdcall lipsSetCbCommand(TLuaDoubleString commandCb);
DLLIMPORT bool __stdcall  lipsOcvInstalled(void);
DLLIMPORT bool __stdcall  lipsOcvInitialized(void);
// test - mouse events
DLLIMPORT void __stdcall  lipsMouse2Lua(bool allow);
DLLIMPORT void __stdcall  lipsMouseDown(TShiftState State, int X, int Y);
DLLIMPORT void __stdcall  lipsMouseMove(TShiftState State, int X, int Y);
DLLIMPORT void __stdcall  lipsMouseUp(TShiftState State, int X, int Y);
#endif
//---------------------------------------------------------------------------
#endif
