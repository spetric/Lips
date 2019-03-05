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
FSourceImage = 0;
FScript = "";
FLuaErrorString = "";
FResult = -1;
ProgressCb = 0;
RequireCb = 0;
LoadImageCb = 0;
ExportImageCb = 0;
HostDialogCb = 0;
FLs = 0;
FHostProc = new TLuaHost();
FSourceRoi = 0;
FTargetRoi = 0;
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
if (FSourceRoi)
   delete FSourceRoi;
if (FTargetRoi)
   delete FTargetRoi;
delete FHostProc;
}
//---------------------------------------------------------------------------
// private
//---------------------------------------------------------------------------
String lipsEngineWrapper::stringBetween(String input, String left, String right, String &remainder)
{
String token = input.SubString(input.Pos(left) + 1, input.Pos(right) - input.Pos(left) - 1);
remainder = input.SubString(input.Pos(right) + 1, input.Length());
return token;
}
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
// public
// initialize
bool lipsEngineWrapper::Initialize(wchar_t *exePath)
{
FExePath = AnsiString(exePath);
#ifdef _WIN64
	FExeWin32 = false;
#else
	FExeWin32 = true;
#endif
/*
String ep = String(exePath);
ep = StringReplace(ep, "\\.\\", "/", TReplaceFlags()<<rfReplaceAll);
ep = StringReplace(ep, "\\", "/", TReplaceFlags()<<rfReplaceAll);
FExePath = AnsiString(ep);
*/
// set luaJit mode
//L = luaL_newstate();
//luaL_openlibs(L);
//luaJIT_setmode(L, -1,  LUAJIT_MODE_ENGINE|LUAJIT_MODE_ON);
//TODO: set host interaction (luaHost)
return true;
}
//---------------------------------------------------------------------------
void lipsEngineWrapper::Clean(void)
{
ocvClean();
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
bool lipsEngineWrapper::SetRoi(TSurfaceType type, TRect *roi)
{
TLuaRoi *tRoi;
TInternalImage *iImage;
switch (type)
	{
	case stSource:
		tRoi = FSourceRoi;
		iImage = FSourceImage;
		TImageCommon::SourceRoi = 0;
		break;
	case stTarget:
		tRoi = FTargetRoi;
		iImage = FTargetImage;
		TImageCommon::TargetRoi = 0;
		break;
	default:
		return false;
	}
if (tRoi)
   {
   delete tRoi;
   tRoi = 0;
   }
if (!roi)
   return true;
if (roi->left < 0 || roi->right > iImage->luaImage->width || roi->top < 0 || roi->bottom > iImage->luaImage->height)
   return false;
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
return true;
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
FHostProc->OnLuaLoadImage     = LoadImageCb;
FHostProc->OnLuaExportImage   = ExportImageCb;
FHostProc->OnLuaHostDialog    = HostDialogCb;
// push pointer
lua_pushlightuserdata(FLs, LPVOID(FHostProc));
lua_setglobal(FLs,"cppHost");
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
// parse parameters
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
	  lua_pushnumber(FLs, StrToInt(value));
   else if (type == "float")
	  lua_pushnumber(FLs, StrToFloat(value));
   else if (type == "bool")
	  {
	  int bil = value == "true" ? 1 : 0;
	  lua_pushboolean(FLs, bil);
	  }
   else if (type == "string")
	  lua_pushstring(FLs, AnsiString(value).c_str());
   else
	  {
	  msg = AnsiString(htl+argno+" error: invalid type.").c_str();
	  lua_getglobal(FLs, "error");  // function to be called
	  lua_pushstring(FLs, msg);
	  FResult = lua_pcall(FLs, 1, 0, 0);
	  if (FResult)
         getErrorMessage();
	  return FResult;
	  }
   lua_setglobal(FLs, AnsiString(key).c_str());
   hostParams = hostParams.SubString(pos + 1, hostParams.Length());
   pos = hostParams.Pos(delim);
   arg_count++;
   }
// end parser
// set application path
AnsiString exepv = "ExePath";
lua_pushstring(FLs, AnsiString(FExePath).c_str());
lua_setglobal(FLs, exepv.c_str());
// set if exe is win32
AnsiString exe32 = "ExeWin32";
lua_pushboolean(FLs, FExeWin32);
lua_setglobal(FLs, exe32.c_str());
// execute it  prolog1 - calls main()
lua_getglobal(FLs, "prolog1");  // function to be called (in _prolog.lua)
lua_pushlightuserdata(FLs, LPVOID (FSourceImage->luaImage));
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
//if (FResult)
//   return FResult;
// TODO: try and catch
FResult = lua_pcall(FLs, 4, 0, 0);
if (FResult)
   getErrorMessage();
TImageCommon::DeleteInternalImages();  // clean-up
//lua_close(FLs);
//FLs = 0;
//int n = lua_gettop(FLs);
return FResult;
}

