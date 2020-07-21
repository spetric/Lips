//---------------------------------------------------------------------------
#pragma hdrstop
#include "luaHostWrap.h"
#include <StrUtils.hpp>
//---------------------------------------------------------------------------
#pragma package(smart_init)
// Define the methods we will expose to Lua
// Check luaConsoleWrap.h for the definitions...
#define method(class, name) {#name, &class::name}
Luna<TLuaHostWrap>::RegType TLuaHostWrap::methods[] = {
   method(TLuaHostWrap, ShowProgress),
   method(TLuaHostWrap, SendMessage),
   method(TLuaHostWrap, RequireParams),
   method(TLuaHostWrap, RefreshParams),
   method(TLuaHostWrap, HostDialog),
   method(TLuaHostWrap, SendCommand),
   method(TLuaHostWrap, CreateImage),
   method(TLuaHostWrap, LoadImage),
   method(TLuaHostWrap, ExportImage),
   method(TLuaHostWrap, GetImage),
   method(TLuaHostWrap, ReleaseImage),
   method(TLuaHostWrap, CopyImage),
   method(TLuaHostWrap, ConvertColorSpace),
   method(TLuaHostWrap, ProcessImage),
   method(TLuaHostWrap, Checker),
   method(TLuaHostWrap, OpenCVSet),
   method(TLuaHostWrap, OpenCVGet),
   method(TLuaHostWrap, OpenCVProcess),
   method(TLuaHostWrap, ExportRawData),
   {0,0}
};
//---------------------------------------------------------------------------
TLuaHostWrap::TLuaHostWrap(lua_State *L)
{
real_object = (TLuaHost*)lua_touserdata(L, 1);
}
//---------------------------------------------------------------------------
TLuaHostWrap::~TLuaHostWrap()
{
//printf("deleted Lua Object (%p)\n", this);
}
//---------------------------------------------------------------------------
void TLuaHostWrap::setObject(lua_State *L)
{
real_object = (TLuaHost*)lua_touserdata(L, 1);
}
//---------------------------------------------------------------------------
int TLuaHostWrap::ShowProgress(lua_State *L)
{
int k,  n = lua_gettop(L);  // number of arguments
for (k = 1; k <= n; k++)
	{
	if (!lua_isnumber(L, k))
	   {
	   lua_pushstring(L, "incorrect arguments to ShowProgress");
	   lua_error(L);
	   }
	}
int done  = lua_tointeger(L, 1);    // done so far
int total = lua_tointeger(L, 2);    // total
lua_pop(L, 2);
// pop ??
real_object->ShowProgress(done, total);
//
return 0;
}
//---------------------------------------------------------------------------
int TLuaHostWrap::SendMessage(lua_State *L)
{
// only one argument must be passed
if (lua_gettop(L) == 1)
   {
   if (lua_isstring(L, 1))
	  {
	  const char *lua_str = lua_tostring(L, 1);
	  real_object->SendMessage(lua_str);
	  lua_pop(L, 1);
	  }
   }
else
   {
   lua_pushstring(L, "incorrect argument to SendMesage");
   lua_error(L);
   }
return 0;
}
//---------------------------------------------------------------------------
int TLuaHostWrap::RequireParams(lua_State *L)
{
// only one argument must be passed
if (lua_gettop(L) == 1)
   {
   if (lua_isstring(L, 1))
	  {
	  const char *lua_str = lua_tostring(L, 1);
	  real_object->RequireParams(lua_str);
	  lua_pop(L, 1);
	  }
   }
else
   {
   lua_pushstring(L, "incorrect argument to RequreParameters");
   lua_error(L);
   }
return 0;
}
//---------------------------------------------------------------------------
int TLuaHostWrap::RefreshParams(lua_State *L)
{
if (lua_gettop(L) == 0)
   {
   real_object->RefreshParams();
   }
else
   {
   lua_pushstring(L, "incorrect argument to RefreshParameters");
   lua_error(L);
   }
return 0;
}
//---------------------------------------------------------------------------
int TLuaHostWrap::HostDialog(lua_State *L)
{
// only one argument must be passed
int dialogResult;
if (lua_gettop(L) == 2)
   {
   if (lua_isstring(L, 1) && lua_isnumber(L, 2))
	  {
	  const char *lua_str = lua_tostring(L, 1);
	  int lua_int = lua_tonumber(L, 2);
	  dialogResult = real_object->HostDialog(lua_str, lua_int);
	  lua_pop(L, 2);
	  }
   }
else
   {
   lua_pushstring(L, "incorrect argument to HostDialog");
   lua_error(L);
   return 0;
   }
lua_pushnumber(L, dialogResult);
return 1;
}
//---------------------------------------------------------------------------
int TLuaHostWrap::SendCommand(lua_State *L)
{
int parNum = lua_gettop(L);
if (parNum < 1 || parNum > 2)
   {
   lua_pushstring(L, "incorrect number of arguments to SendCommand");
   lua_error(L);
   return 0;
   }
const char *cmd;
const char *parList;
bool procOK = true;
if (lua_isstring(L, 1))
	cmd = lua_tostring(L, 1);
else
	procOK = false;
if (parNum == 2)
   {
   if (lua_isstring(L, 2))
	   parList = lua_tostring(L, 2);
   else
	  procOK = false;
   }
lua_pop(L, parNum);
if (procOK)
   {
   // if ok, send command
   procOK =  real_object->SendCommand(cmd, parList);
   }
if (!procOK)
   {
   lua_pushstring(L, "incorrect arguments to SendCommand");
   lua_error(L);
   return 0;
   }
return 0;
}
//---------------------------------------------------------------------------
int TLuaHostWrap::CreateImage(lua_State *L)
{
const char *type;
int width, height;
bool addAlpha = false;
bool err = false;
int n = lua_gettop(L);
if (n >= 3)
	{
	if (lua_isstring(L, 1))
		type = lua_tostring(L, 1);
	else
		err = true;
	if (lua_isnumber(L, 2))
		width = lua_tointeger(L, 2);
	else
		err = true;
	if (lua_isnumber(L, 3))
		height = lua_tointeger(L, 3);
	else
		err = true;
	if (n > 3)
	   {
	   if (lua_isboolean(L, 4))
		  addAlpha = lua_toboolean(L, 4);
	   else
		  err = true;
	   }
	lua_pop(L, n);
	if (!err)
	   {
	   // if ok, push image onto stack
	   TLuaImageVoid *img =  real_object->CreateImage(type, width, height, addAlpha);
	   if (img)
		   lua_pushlightuserdata(L, LPVOID (img));
	   else
		   err = true;
	   }
	}
else
	err = true;
if (err)
   {
   lua_pushstring(L, "incorrect arguments to CreateImage");
   lua_error(L);
   return 0;
   }
return 1;
}
//---------------------------------------------------------------------------
int TLuaHostWrap::LoadImage(lua_State *L)
{
if (lua_gettop(L) != 2)
   {
   lua_pushstring(L, "incorrect number of arguments to LoadImage");
   lua_error(L);
   return 0;
   }
const char *filename;
const char *type;
bool err = false;
if (lua_isstring(L, 1))
	filename = lua_tostring(L, 1);
else
	err = true;
if (lua_isstring(L, 2))
	type = lua_tostring(L, 2);
else
	err = true;
lua_pop(L, 2);
if (!err)
   {
   // if ok, push image onto stack
   TLuaImageVoid *img =  real_object->LoadImage(filename, type);
   if (img)
	  lua_pushlightuserdata(L, LPVOID (img));
   else
	  err = true;
   }
else
   err = true;
if (err)
   {
   lua_pushstring(L, "incorrect arguments to LoadImage");
   lua_error(L);
   return 0;
   }
return 1;
}
//---------------------------------------------------------------------------
int TLuaHostWrap::ExportImage(lua_State *L)
{
bool lRet = false;
int n = lua_gettop(L);
if (n < 2)
   {
   lua_pushstring(L, "incorrect number of arguments to ExportImage");
   lua_error(L);
   return 0;
   }
if (lua_isnumber(L, 1) && lua_isstring(L, 2))
   {
   int expId = lua_tointeger(L, 1);    // export image id
   const char *lua_str = lua_tostring(L, 2);
   String cType = String(lua_str);
   bool show = true;
   if (n == 3)
	  {
	  if (lua_isboolean(L, 3))
		 show = lua_toboolean(L, 3);
	  }
   lRet = real_object->ExportImage(expId, lua_str, show);
   }
else
   {
   lua_pushstring(L, "incorrect arguments to ExportImage");
   lua_error(L);
   return 0;
   }
lua_pop(L, n);
lua_pushboolean (L, lRet ? 1 : 0);
return 1;
}
//---------------------------------------------------------------------------
int TLuaHostWrap::ExportRawData(lua_State *L)
{
bool lRet = false;
int n = lua_gettop(L);
if (n < 1)
   {
   lua_pushstring(L, "incorrect number of arguments to ExportRawData");
   lua_error(L);
   return 0;
   }
if (lua_isnumber(L, 1))
   {
   int type = lua_tointeger(L, 1);     // export data type
   lRet = real_object->ExportRawData(type);
   }
else
   {
   lua_pushstring(L, "incorrect argument to ExportRawData");
   lua_error(L);
   return 0;
   }
lua_pop(L, n);
lua_pushboolean (L, lRet ? 1 : 0);
return 1;
}
//---------------------------------------------------------------------------
int TLuaHostWrap::GetImage(lua_State *L)
{
int n = lua_gettop(L);
if (n < 1)
   {
   lua_pushstring(L, "incorrect number arguments to GetImage");
   lua_error(L);
   return 0;
   }
if (lua_isnumber(L, 1))
   {
   //TODO: ovo ne šalje dobar ptr
   TLuaHost::TStrPtr data;
   int id = lua_tointeger(L, 1);    // export image id
   data = real_object->GetImage(id);
   lua_pop(L, 1);
   if (data.ptr)
	  {
	  lua_pushstring(L, data.str);
	  lua_pushlightuserdata(L, data.ptr);
	  }
   }
else
   {
   lua_pushstring(L, "incorrect arguments to GetImage");
   lua_error(L);
   return 0;
   }
return 2;
}
//---------------------------------------------------------------------------
int TLuaHostWrap::ReleaseImage(lua_State *L)
{
bool lRet = false;
int n = lua_gettop(L);
if (n < 1)
   {
   lua_pushstring(L, "incorrect number arguments to ReleaseImage");
   lua_error(L);
   return 0;
   }
if (lua_isnumber(L, 1))
   {
   int id = lua_tointeger(L, 1);    		// get image id
   lRet = real_object->ReleaseImage(id);    // release image by id
   }
else
   {
   lua_pushstring(L, "incorrect arguments to ReleaseImage");
   lua_error(L);
   return 0;
   }
lua_pop(L, n);
lua_pushboolean (L, lRet ? 1 : 0);
return 1;
}
//---------------------------------------------------------------------------
// copy image data
int  TLuaHostWrap::CopyImage(lua_State *L)
{
bool lRet = false;
int n = lua_gettop(L);
if (n < 2)
   {
   lua_pushstring(L, "incorrect number arguments to CopyImage");
   lua_error(L);
   return 0;
   }
TLuaRoi *inpRoi = 0;
TLuaRoi *outRoi = 0;
if (lua_isnumber(L, 1) && lua_isnumber(L, 2))
   {
   int inpId = lua_tointeger(L, 1);    // input image id
   int outId = lua_tointeger(L, 2);    // output image id
   if (n > 2)
	   inpRoi = (TLuaRoi*)lua_topointer(L, 3);
   if (n > 3)
	   outRoi = (TLuaRoi*)lua_topointer(L, 4);
   // copy image data
   real_object->CopyImage(inpId, outId, inpRoi, outRoi);
   }
else
   {
   lua_pushstring(L, "incorrect arguments to CopyImage");
   lua_error(L);
   return 0;
   }
lua_pop(L, n);
lua_pushboolean (L, lRet ? 1 : 0);
return 1;
}
//---------------------------------------------------------------------------
// color space conversions
int  TLuaHostWrap::ConvertColorSpace(lua_State *L)
{
bool lRet = false;
if (lua_gettop(L) != 3)
   {
   lua_pushstring(L, "incorrect number of arguments to ConvertColorSpace");
   lua_error(L);
   return 0;
   }
if (lua_isnumber(L, 1) && lua_isnumber(L, 2) && lua_isstring(L, 3))
   {
   int inpId = lua_tointeger(L, 1);    // input image id
   int outId = lua_tointeger(L, 2);    // output image id
   const char *lua_str = lua_tostring(L, 3);
   lua_pop(L, 3);
   String cType = String(lua_str);
   if (cType == "rgb2xyz")
	  lRet = real_object->Conv_rgb2xyz(inpId, outId);
   else if (cType == "xyz2rgb")
	  lRet = real_object->Conv_xyz2rgb(inpId, outId);
   else if (cType == "rgb2lab")
	  lRet = real_object->Conv_rgb2lab(inpId, outId);
   else if (cType == "lab2rgb")
	  lRet = real_object->Conv_lab2rgb(inpId, outId);
   else if (cType == "rgb2grayscale")
	  lRet = real_object->Conv_rgb2gray(inpId, outId);
   else if (cType == "grayscale2rgb")
	  lRet = real_object->Conv_gray2rgb(inpId, outId, false);
   else if (cType == "alpha2rgb")
	  lRet = real_object->Conv_gray2rgb(inpId, outId, true);
   else
	  lRet = false;
   }
else
   {
   lua_pushstring(L, "incorrect arguments to ConvertColorSpace");
   lua_error(L);
   return 0;
   }
lua_pushboolean (L, lRet ? 1 : 0);
return 1;
}
//---------------------------------------------------------------------------
// image preocessing - internal/external
int  TLuaHostWrap::ProcessImage(lua_State *L)
{
bool lRet = false;
if (lua_gettop(L) != 2)
   {
   lua_pushstring(L, "incorrect number of arguments to ProcessImage");
   lua_error(L);
   return 0;
   }
if (!(lua_isstring(L, 1) && lua_isstring(L, 2)))
   {
   lua_pushstring(L, "incorrect arguments to ProcessImage");
   lua_error(L);
   return 0;
   }
const char *lua_proc   = lua_tostring(L, 1);
const char *lua_params = lua_tostring(L, 2);
String strProc = String(lua_proc).Trim().LowerCase();
String strParams = String(lua_params).Trim().LowerCase();
lua_pop(L, 2);
//fmt.DecimalSeparator = '.';
// parse parameters
TStringDynArray tokens = SplitString(strParams, ",");
// internal
if (strProc == "blur")     // grayscale only
   {
   int inpId = StrToInt(tokens[0]);    // source image id
   int outId = StrToInt(tokens[1]);    // target image id
   int radius = StrToInt(tokens[2]);
   lRet = real_object->Blur(inpId, outId, radius);
   }
else if (strProc == "blend")
   {
   int inpId = StrToInt(tokens[0]);    // source image id
   int maskId = StrToInt(tokens[1]);   // mask image id
   int outId = StrToInt(tokens[2]);    // target image id
   bool alpha = StrToBool(tokens[3]);   // blend with alpha channel
   lRet = real_object->Blend(inpId, maskId, outId, alpha);
   }
// extgernal
else if(strProc == "downsample")
   {
   int inpId = StrToInt(tokens[0]);    // source image id
   int outId = StrToInt(tokens[1]);    // target image id
   lRet = real_object->DownSample(inpId, outId);
   }
else
  lRet = false;
lua_pushboolean (L, lRet ? 1 : 0);
return 1;
}
//---------------------------------------------------------------------------
// checker - various types of chcking
int  TLuaHostWrap::Checker(lua_State *L)
{
bool lRet = false;
int n = lua_gettop(L);
if (n != 2)
   {
   lua_pushstring(L, "incorrect number arguments to Checker");
   lua_error(L);
   return 0;
   }
if (lua_isstring(L, 1) && lua_isstring(L, 2))
   {
   const char *cmd   = lua_tostring(L, 1);     // command
   const char *param = lua_tostring(L, 2);    // parameter
   // perform checking
   lRet = real_object->Checker(cmd, param);
   }
else
   {
   lua_pushstring(L, "incorrect arguments to Checker");
   lua_error(L);
   return 0;
   }
lua_pushboolean (L, lRet ? 1 : 0);
return 1;
}
//---------------------------------------------------------------------------
// OpenCV
//---------------------------------------------------------------------------
// opencv - set
int  TLuaHostWrap::OpenCVSet(lua_State *L)
{
int nParams = lua_gettop(L);
if (nParams < 2)
   {
   lua_pushstring(L, "incorrect number of arguments to OpenCV set");
   lua_error(L);
   return 0;
   }
if (!lua_isstring(L, 1))
   {
   lua_pushstring(L, "incorrect arguments to OpenCV set");
   lua_error(L);
   return 0;
   }
const char *lua_proc   = lua_tostring(L, 1);
bool lRet;
if (lua_isnumber(L, 2))     // set opencv image
   {
   int imgId = lua_tointeger(L, 2);
   lRet = real_object->OcvSet(lua_proc, imgId);
   }
else
   {
   TLuaRoi *roi = (TLuaRoi*)lua_topointer(L, 2);
   lRet = real_object->OcvSet(lua_proc, roi);
   }
lua_pop(L, nParams);
lua_pushboolean (L, lRet ? 1 : 0);
return 1;
}
//---------------------------------------------------------------------------
// opencv - get
int  TLuaHostWrap::OpenCVGet(lua_State *L)
{
int nParams = lua_gettop(L);
if (nParams < 1 || nParams > 2)
   {
   lua_pushstring(L, "incorrect number of arguments to OpenCV get");
   lua_error(L);
   return 0;
   }
bool canProceed = true;
for (int i = 1; i <= nParams; i++)
	canProceed = canProceed && lua_isstring(L, i);
if (!canProceed)
   {
   lua_pushstring(L, "incorrect arguments to OpenCV get");
   lua_error(L);
   return 0;
   }
const char *lua_proc = lua_tostring(L, 1);
if (nParams == 1)
   {
   int nRet = real_object->OcvGetInt(lua_proc);
   lua_pop(L, nParams);
   lua_pushnumber(L, nRet);
   }
else
   {
   const char *lua_params = lua_tostring(L, 2);
   void *pRet = real_object->OcvGetPtr(lua_proc, lua_params);
   lua_pop(L, nParams);
   lua_pushlightuserdata(L, LPVOID (pRet));
   }
return 1;
}
//---------------------------------------------------------------------------
// opencv - process
int  TLuaHostWrap::OpenCVProcess(lua_State *L)
{
int nParams = lua_gettop(L);
if (nParams < 1)
   {
   lua_pushstring(L, "incorrect number of arguments to OpenCV process");
   lua_error(L);
   return 0;
   }
int rc;
bool canProceed = true;
for (int i = 1; i <= nParams; i++)
	canProceed = canProceed && lua_isstring(L, i);
if (canProceed)
   {
   const char *lua_proc   = lua_tostring(L, 1);
   String procType = String(lua_proc);
   const char *lua_params1 = nParams > 1 ? lua_tostring(L, 2) : 0;
   const char *lua_params2 = nParams > 2 ? lua_tostring(L, 3) : 0;
   const char *lua_params3 = nParams > 3 ? lua_tostring(L, 4) : 0;
   lua_pop(L, nParams);
   const char *errMsg;
   switch (nParams)
	  {
	  case 1:
		   errMsg = real_object->OcvProc(lua_proc, rc);
		   break;
	  case 2:
		   errMsg = real_object->OcvProc(lua_proc, lua_params1, rc);
		   break;
	  case 3:
		   errMsg = real_object->OcvProc(lua_proc, lua_params1, lua_params2, rc);
		   break;
	  default:
		   errMsg = real_object->OcvProc(lua_proc, lua_params1, lua_params2, lua_params3, rc);
	  }
   if (rc != 0)
	  {
	  if (rc < 0)
		 {
		 lua_pushstring(L, errMsg);
		 lua_error(L);
		 return 0;
		 }
	  else
		 {
		 lua_pushnumber(L, rc);
		 return 1;
         }
	  }
   }
else
   {
   lua_pushstring(L, "incorrect arguments to OpenCV process");
   lua_error(L);
   return 0;
   }
//lua_pushboolean (L, 1);
lua_pushnumber (L, 0);
return 1;
}
//---------------------------------------------------------------------------



