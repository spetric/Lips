//---------------------------------------------------------------------------
#pragma hdrstop
#include "lipsEngineWrapper.h"
#include "lipsImageCommon.h"
#include <System.SysUtils.hpp>
#include <System.Math.hpp>
//#include "ocvWrapper.h"
// Define the Lua ClassName
const char TLuaHostWrap::className[] = "TLuaHostWrap";
//---------------------------------------------------------------------------
#pragma package(smart_init)
// trivial constructor
lipsEngineWrapper::lipsEngineWrapper(void)
{
FSourceImage = 0;
FTargetImage = 0;
FSourceMask = 0;
FTargetMask = 0;
FCustomImage = 0;
FCustomMask = 0;
FScript = "";
FLuaErrorString = "";
FSilentParams = "";
FResult = -1;
ProgressCb = 0;
RequireCb = 0;
RefreshCb = 0;
LoadImageCb = 0;
ExportImageCb = 0;
HostDialogCb = 0;
FLs = 0;
FHostProc = new TLuaHost();
FSourceRoi = 0;
FTargetRoi = 0;
FMouseEventsOn = false;
}
//---------------------------------------------------------------------------
// destructor
lipsEngineWrapper::~lipsEngineWrapper(void)
{
//ocvClose();
if (FLs)
   {
   lua_close(FLs);
   FLs = 0;
   }
if (FSourceImage)
   delete FSourceImage;
if (FTargetImage)
   delete FTargetImage;
if (FSourceMask)
   delete FSourceMask;
if (FTargetMask)
   delete FTargetMask;
if (FCustomImage)
   delete FCustomImage;
if (FCustomMask)
   delete FCustomMask;
if (FSourceRoi)
   delete FSourceRoi;
if (FTargetRoi)
   delete FTargetRoi;
delete FHostProc;
}
//---------------------------------------------------------------------------
// private
//---------------------------------------------------------------------------
void lipsEngineWrapper::getErrorMessage(void)
{
int top = -1;
int type = -1;
top = lua_gettop(FLs);
if( top > 0 )
   type = lua_type(FLs, top );
if (type == LUA_TSTRING )
   FLuaErrorString = lua_tostring(FLs, top );
lua_settop(FLs, 0);
}
//---------------------------------------------------------------------------
String lipsEngineWrapper::stringBetween(String input, String left, String right, String &remainder)
{
String token = input.SubString(input.Pos(left) + 1, input.Pos(right) - input.Pos(left) - 1);
remainder = input.SubString(input.Pos(right) + 1, input.Length());
return token;
}
//---------------------------------------------------------------------------
int lipsEngineWrapper::parseParams(lua_State *L, wchar_t *params)
{
int nResult = 0;
if (!params)
   return nResult;  // no params
String hostParams = String(params);
String delim = ";";
String htl = "Host2Lua parameter ";
int pos = hostParams.Pos(delim);
int arg_count = 1;
const char *msg;
while (pos > 0)
   {
   String argno = IntToStr(arg_count);
   String remainder;
   String token = hostParams.SubString(1, pos-1);
   String key   = token.SubString(1, token.Pos("=") - 1).Trim();
   String type  = stringBetween(token, "=", "(", remainder).Trim();
   String value = stringBetween(token, "(", ")", remainder).Trim();
   if (type == "int")
	  lua_pushnumber(L, StrToInt(value));
   else if (type == "float")
	  lua_pushnumber(L, StrToFloat(value));
   else if (type == "bool")
	  {
	  int bil = value == "true" ? 1 : 0;
	  lua_pushboolean(L, bil);
	  }
   else if (type == "string")
	  lua_pushstring(L, AnsiString(value).c_str());
   else
	  {
	  msg = AnsiString(htl+argno+" error: invalid type.").c_str();
	  lua_getglobal(L, "error");  // function to be called
	  lua_pushstring(L, msg);
	  nResult = lua_pcall(L, 1, 0, 0);
	  if (!nResult)
		 nResult = -999;
	  return nResult;
	  }
   lua_setglobal(L, AnsiString(key).c_str());
   hostParams = hostParams.SubString(pos + 1, hostParams.Length());
   pos = hostParams.Pos(delim);
   arg_count++;
   }
// end parser
return nResult;
}
//---------------------------------------------------------------------------
void lipsEngineWrapper::setMouseStatus (const char *index, bool value)
{
//lua_getfield(FLs, -1, index);
lua_pushboolean(FLs, value);
lua_setfield(FLs, -2, index);
}
//---------------------------------------------------------------------------
void lipsEngineWrapper::setLuaRoi(TLuaRoi *lRoi, int type)
{
if (!FLs || FResult)
   return;
lua_getglobal(FLs, "hostSetRoi");  // function to be called (in _prolog.lua)
if (lRoi)
   lua_pushlightuserdata(FLs, LPVOID (lRoi));
else
   lua_pushnil(FLs);
lua_pushnumber(FLs, type);
FResult = lua_pcall(FLs, 2, 0, 0);
if (FResult)
   getErrorMessage();
}
//---------------------------------------------------------------------------
// public
// initialize
bool lipsEngineWrapper::Initialize(wchar_t *exePath, bool initializeOcv)
{
FExePath = AnsiString(exePath);
#ifdef _WIN64
	FExeWin32 = false;
#else
	FExeWin32 = true;
#endif
// ocv dlls exist
FOcvAlive = FileExists(FExePath+"ocvWrapper.dll");
if (FOcvAlive)
   {
   if (initializeOcv)
	  {
	   try
		   {
		   ocvInit();
		   FOcvInitialized = true;
		   }
	   catch (...)
		  {
		  FOcvInitialized = false;
		  return false;
		  }
	  }
   else
	  FOcvInitialized = false;
   }
/*
String ep = String(exePath);
ep = StringReplace(ep, "\\.\\", "/", TReplaceFlags()<<rfReplaceAll);
ep = StringReplace(ep, "\\", "/", TReplaceFlags()<<rfReplaceAll);
FExePath = AnsiString(ep);
*/
//_clear87();
//_control87(MCW_EM, MCW_EM);
// set luaJit mode
//L = luaL_newstate();
//luaL_openlibs(L);
//luaJIT_setmode(L, -1,  LUAJIT_MODE_ENGINE|LUAJIT_MODE_ON);
return true;
}
//---------------------------------------------------------------------------
bool lipsEngineWrapper::Clean(void)
{
TImageCommon::DeleteInternalImages();  // clean-up
if (FOcvAlive && FOcvInitialized)
   {
   try
	   {
	   ocvClean();
	   }
   catch (...)
	  {
	  return false;
	  }
   }
return true;
}
//---------------------------------------------------------------------------
// set images
bool lipsEngineWrapper::SetImage(TSurfaceType type, void *scanImage, void *scanAlpha, int width, int height, unsigned int scanlineAlignment)
{
switch (type)
	{
	case stSource:
		 if (FSourceImage)
			delete FSourceImage;
		 if (scanImage)
			{
			FSourceImage = new TInternalImage("rgb24", scanImage, scanAlpha, width, height, scanlineAlignment, true, false);
			FSourceImage->luaImage->id = -1;
			TImageCommon::SourceImage = (TLuaImageByte3*)(FSourceImage->luaImage);
			}
		 else
            FSourceImage = 0;
		 // delete roi if exists
		 if (FSourceRoi)
			{
			delete FSourceRoi;
			FSourceRoi = 0;
			TImageCommon::SourceRoi = FSourceRoi;
			}
		 break;
	case stTarget:
		 if (FTargetImage)
			delete FTargetImage;
		 if (scanImage)
			{
			FTargetImage = new TInternalImage("rgb24", scanImage, scanAlpha, width, height, scanlineAlignment, true, false);
			FTargetImage->luaImage->id = -2;
			TImageCommon::TargetImage = (TLuaImageByte3*)(FTargetImage->luaImage);
			}
		 else
			FTargetImage = 0;
		 if (FTargetRoi)
			{
			delete FTargetRoi;
			FTargetRoi = 0;
			TImageCommon::TargetRoi = FTargetRoi;
			}
		 break;
	case stSourceMask:
		 if (FSourceMask)
			delete FSourceMask;
		 if (scanImage)
			{
			FSourceMask = new TInternalImage("grayscale", scanImage, scanAlpha, width, height, scanlineAlignment, true, false);
			FSourceMask->luaImage->id = -3;
			TImageCommon::SourceMask = (TLuaImageByte1*)(FSourceMask->luaImage);
			}
		 else
			FSourceMask = 0;
		 break;
	case stTargetMask:
		 if (FTargetMask)
			delete FTargetMask;
		 if (scanImage)
			{
			FTargetMask = new TInternalImage("grayscale", scanImage, scanAlpha, width, height, scanlineAlignment, true, false);
			FTargetMask->luaImage->id = -4;
			TImageCommon::TargetMask = (TLuaImageByte1*)(FTargetMask->luaImage);
			}
		 else
			FTargetMask = 0;
		 break;
	case stCustom:
		 if (FCustomImage)
			delete FCustomImage;
		 if (scanImage)
			{
			FCustomImage = new TInternalImage("rgb24", scanImage, scanAlpha, width, height, scanlineAlignment, true, false);
			FCustomImage->luaImage->id = -5;
			TImageCommon::CustomImage = (TLuaImageByte3*)(FCustomImage->luaImage);
			}
		 else
			FCustomImage = 0;
		 break;
	case stCustomMask:
		 if (FCustomMask)
			delete FCustomMask;
		 if (scanImage)
			{
			FCustomMask = new TInternalImage("grayscale", scanImage, scanAlpha, width, height, scanlineAlignment, true, false);
			FCustomMask->luaImage->id = -6;
			TImageCommon::CustomMask = (TLuaImageByte1*)(FCustomMask->luaImage);
			}
		 else
			FCustomMask = 0;
		 break;
	default:
		 return false;
	}
return true;
}
//---------------------------------------------------------------------------
// push image
bool lipsEngineWrapper::PushImage(wchar_t *type, void *scanImage, void * scanAlpha, int width, int height, unsigned int scanlineAlignment)
{
//bool lRet = true;
if (!FLs || FResult)
   return false;
AnsiString aType = AnsiString(type);
TInternalImage *img = new TInternalImage(String(type), scanImage, scanAlpha, width, height, scanlineAlignment, true, false);
FHostProc->SetPushImage(img);
return true;
}
//---------------------------------------------------------------------------
bool lipsEngineWrapper::PushTableImage(wchar_t *tableName, wchar_t * type, void *scanImage, void *scanAlpha, int width, int height, unsigned int scanlineAlignment)
{
//bool lRet = true;
if (!FLs || FResult)
   return false;
AnsiString aType = AnsiString(type);
AnsiString aName = AnsiString(tableName);
TInternalImage *img = new TInternalImage(String(type), scanImage, scanAlpha, width, height, scanlineAlignment, true, false);
// add image to internal list
TLuaImageVoid *luaImage = FHostProc->AddPushImage(img);
// create new table
lua_createtable(FLs, 0, 2);
lua_pushstring(FLs, "type");
lua_pushstring(FLs, aType.c_str());
lua_settable(FLs, -3);
lua_pushstring(FLs, "ptr");
lua_pushlightuserdata(FLs, LPVOID (luaImage));
lua_settable(FLs, -3);
// push it onto stack
//lua_pushlightuserdata(FLs, LPVOID (luaImage));
// set name
lua_setglobal(FLs, aName.c_str());
return true;
}
//---------------------------------------------------------------------------
bool lipsEngineWrapper::SetRoi(TSurfaceType type, TRect *roi, bool push2lua)
{
TLuaRoi *tRoi;
TInternalImage *iImage;
int iType;
switch (type)
	{
	case stSource:
		if (!FSourceImage)
		   return false;
		tRoi = FSourceRoi;
		iImage = FSourceImage;
		TImageCommon::SourceRoi = 0;
		iType = 0;
		break;
	case stTarget:
		if (!FTargetImage)
		   return false;
		tRoi = FTargetRoi;
		iImage = FTargetImage;
		TImageCommon::TargetRoi = 0;
		iType = 1;
		break;
	default:
		iType = -1;
		return false;
	}
if (tRoi)
   {
   delete tRoi;
   tRoi = 0;
   }
if (!roi)
   {
   if (push2lua)
	  setLuaRoi(0, iType);
   return true;
   }
if (roi->left < 0 || roi->right > iImage->luaImage->width || roi->top < 0 || roi->bottom > iImage->luaImage->height)
   {
   if (push2lua)
	  setLuaRoi(0, iType);
   return false;
   }
tRoi = new TLuaRoi;
tRoi->left   = roi->left;
tRoi->right  = roi->right;
tRoi->top    = roi->top;
tRoi->bottom = roi->bottom;
if (type == stSource)
   {
	FSourceRoi = tRoi;
	TImageCommon::SourceRoi = FSourceRoi;
   }
else
   {
	FTargetRoi = tRoi;
	TImageCommon::TargetRoi = FTargetRoi;
   }
if (push2lua)   // push directly to lua stack
   setLuaRoi(tRoi, iType);
return true;
}
//---------------------------------------------------------------------------
bool lipsEngineWrapper::SetSilentParams(wchar_t *silentParams)
{
bool lRet = true;
if (silentParams)
   FSilentParams = AnsiString(silentParams).Trim();
else
   {
   FSilentParams == "";
   lRet = false;
   }
return lRet;
}
//---------------------------------------------------------------------------
// load script
int lipsEngineWrapper::LoadScript(wchar_t * path)
{
/* if there is a need somewhere
TFileStream *fs = new TFileStream(String(path), fmOpenRead);
int size = fs->Size;
FScript.SetLength(size);
fs->Read(FScript.c_str(), size);
*/
//lua_settop(L, 0);
// load script and check for requireParams
TImageCommon::DeleteInternalImages();  // clean-up
FMouseEventsOn = false;
if (FLs)
   lua_close(FLs);
FLs = luaL_newstate();
luaL_openlibs(FLs);
//luaJIT_setmode(FLs, -1,  LUAJIT_MODE_ENGINE|LUAJIT_MODE_ON); // on by default
// Register LuaImageWrap data type with Lua
Luna<TLuaHostWrap>::Register(FLs);
// set call-back progress function
FHostProc->OnLuaProgress      = ProgressCb;
FHostProc->OnLuaSendMessage   = MessageCb;
FHostProc->OnLuaRequireParams = RequireCb;
FHostProc->OnLuaRefreshParams = RefreshCb;
FHostProc->OnLuaLoadImage     = LoadImageCb;
FHostProc->OnLuaExportImage   = ExportImageCb;
FHostProc->OnLuaHostDialog    = HostDialogCb;
FHostProc->OnLuaSendCommand   = CommandCb;
// push pointer
lua_pushlightuserdata(FLs, LPVOID(FHostProc));
lua_setglobal(FLs,"cppHost");
// set global variables
// set application path
AnsiString exepv = "ExePath";
lua_pushstring(FLs, AnsiString(FExePath).c_str());
lua_setglobal(FLs, exepv.c_str());
// set if exe is win32
AnsiString exe32 = "ExeWin32";
lua_pushboolean(FLs, FExeWin32);
lua_setglobal(FLs, exe32.c_str());
// set if ocv dlls are avialable (ocvWrapper)
AnsiString ocvEnabled = "OcvEnabled";
lua_pushboolean(FLs, (FOcvAlive && FOcvInitialized));
lua_setglobal(FLs, ocvEnabled.c_str());
//
SetExceptionMask(exAllArithmeticExceptions);
AnsiString aPath = AnsiString(path);
try {
	FResult = luaL_loadfile(FLs, aPath.c_str());
	}
catch (Exception &ex)
   {
   FLuaErrorString = ex.Message;
   return FResult;
   }
if (FResult)
   {
   getErrorMessage();
   return FResult;
   }
FResult = lua_pcall(FLs, 0, 0, 0);
if (FResult)
   {
   getErrorMessage();
   return FResult;
   }
lua_getglobal(FLs, "prolog0");       // function to be called (in _prolog.lua)
FResult = lua_pcall(FLs, 0, 0, 0);
if (FResult)
   {
   getErrorMessage();
   return FResult;
   }
lua_getglobal(FLs, "RequireParams"); // function to be called (optional)
if (lua_isfunction (FLs, -1))
   {
   FResult = lua_pcall(FLs, 0, 0, 0);
   if (FResult)
	  getErrorMessage();
   }
else
   {
   // pop ??
   }
return FResult;
}
//---------------------------------------------------------------------------
// execute script
int lipsEngineWrapper::ExecuteScript(wchar_t *params)
{
if (FResult)
   return FResult;
/*
FResult = lua_pcall(FLs, 0, 0, 0);
if (FResult)
   {
   getErrorMessage();
   return FResult;
   }
*/
FMouseEventsOn = false;
//
// parse parameters
FResult = parseParams(FLs, params);     // parse and set vars
if (FResult)
   {
   getErrorMessage();
   TImageCommon::DeleteInternalImages();  // clean-up
   return FResult;
   }
// execute it  prolog1 - calls main()
lua_getglobal(FLs, "prolog1");  // function to be called (in _prolog.lua)
lua_pushlightuserdata(FLs, LPVOID (FSourceImage->luaImage));  // source - sine qua non
if (FTargetImage)
	lua_pushlightuserdata(FLs, LPVOID (FTargetImage->luaImage));
else
	lua_pushnil(FLs);
if (FSourceRoi)
   lua_pushlightuserdata(FLs, LPVOID (FSourceRoi));
else
   lua_pushnil(FLs);
if (FTargetRoi)
   lua_pushlightuserdata(FLs, LPVOID (FTargetRoi));
else
   lua_pushnil(FLs);
if (FSourceMask)
	lua_pushlightuserdata(FLs, LPVOID (FSourceMask->luaImage));
else
	lua_pushnil(FLs);
if (FTargetMask)
	lua_pushlightuserdata(FLs, LPVOID (FTargetMask->luaImage));
else
	lua_pushnil(FLs);
if (FCustomImage)
	lua_pushlightuserdata(FLs, LPVOID (FCustomImage->luaImage));
else
	lua_pushnil(FLs);
if (FCustomMask)
	lua_pushlightuserdata(FLs, LPVOID (FCustomMask->luaImage));
else
	lua_pushnil(FLs);
if (FSilentParams.IsEmpty())
	lua_pushnil(FLs);
else
   lua_pushstring(FLs, FSilentParams.c_str());
//if (FResult)
//   return FResult;
// TODO: try and catch
FResult = lua_pcall(FLs, 9, 0, 0);
if (FResult)
   getErrorMessage();
else
   {
   if (FMouseEventsOn)  // no clean-up
	  return FResult;
   }
TImageCommon::DeleteInternalImages();  // clean-up
//lua_close(FLs);
//FLs = 0;
//int n = lua_gettop(FLs);
return FResult;
}
//---------------------------------------------------------------------------
// refresh params
int lipsEngineWrapper::RefreshParams(wchar_t *params)
{
if (FResult)
   return FResult;
// parse parameters
FResult = parseParams(FLs, params);     // parse and set vars
if (FResult)
   getErrorMessage();
return FResult;
}
//---------------------------------------------------------------------------
// execute mouse event (script)
int lipsEngineWrapper::ExecuteMouseEvent(unsigned int event, TShiftState State, int X, int Y)
{
if (FResult)
   return FResult;
// step 1. - set Mouse state
lua_getglobal(FLs, "MouseState");
setMouseStatus("ssShift",  State.Contains(ssShift));
setMouseStatus("ssAlt",    State.Contains(ssAlt));
setMouseStatus("ssCtrl",   State.Contains(ssCtrl));
setMouseStatus("ssLeft",   State.Contains(ssLeft));
setMouseStatus("ssRight",  State.Contains(ssRight));
setMouseStatus("ssMiddle", State.Contains(ssMiddle));
// step 2. - invoke function
// 0 - mouse down, 1 - mouse move, 2 - mouse up
lua_getglobal(FLs, "OnMouseEvents");
switch (event)
   {
   case 0:
		lua_getfield(FLs, -1, "OnMouseDown");
		break;
   case 1:
		lua_getfield(FLs, -1, "OnMouseMove");
		break;
   default:
		lua_getfield(FLs, -1, "OnMouseUp");
		break;
   }
if (!lua_isnil(FLs ,-1))
   {
   lua_pushnumber(FLs, X);
   lua_pushnumber(FLs, Y);
   //int args = 2;
   FResult = lua_pcall(FLs, 2, 0, 0);
   }
return FResult;
}

