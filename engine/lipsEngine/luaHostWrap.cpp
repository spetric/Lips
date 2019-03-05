//---------------------------------------------------------------------------
#pragma hdrstop
#include "luaHostWrap.h"
//---------------------------------------------------------------------------
#pragma package(smart_init)
// Define the methods we will expose to Lua
// Check luaConsoleWrap.h for the definitions...
#define method(class, name) {#name, &class::name}
Luna<TLuaHostWrap>::RegType TLuaHostWrap::methods[] = {
   method(TLuaHostWrap, ShowProgress),
   method(TLuaHostWrap, SendMessage),
   method(TLuaHostWrap, RequireParams),
   method(TLuaHostWrap, HostDialog),
   method(TLuaHostWrap, CreateImage),
   method(TLuaHostWrap, LoadImage),
   method(TLuaHostWrap, ExportImage),
   method(TLuaHostWrap, GetImage),
   method(TLuaHostWrap, CopyImage),
   method(TLuaHostWrap, ConvertColorSpace),
   method(TLuaHostWrap, ProcessImage),
   method(TLuaHostWrap, OpenCVSet),
   method(TLuaHostWrap, OpenCVGet),
   method(TLuaHostWrap, OpenCVProcess),
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
   lua_pushstring(L, "incorrect argument to RequreParameters");
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
int TLuaHostWrap::HostDialog(lua_State *L)
{
// only one argument must be passed
int dialogResult;
if (lua_gettop(L) == 1)
   {
   if (lua_isstring(L, 1))
	  {
	  const char *lua_str = lua_tostring(L, 1);
	  dialogResult = real_object->HostDialog(lua_str);
	  lua_pop(L, 1);
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
int TLuaHostWrap::CreateImage(lua_State *L)
{
const char *type;
int width, height;
bool err = false;
if (lua_gettop(L) == 3)
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
	lua_pop(L, 3);
	if (!err)
	   {
	   // if ok, push image onto stack
	   TLuaImageVoid *img =  real_object->CreateImage(type, width, height);
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
   lua_pushstring(L, "incorrect number arguments to ExportImage");
   lua_error(L);
   return 0;
   }
if (lua_isnumber(L, 1) && lua_isstring(L, 2))
   {
   int expId = lua_tointeger(L, 1);    // export image id
   const char *lua_str = lua_tostring(L, 2);
   String cType = String(lua_str);
   lRet = real_object->ExportImage(expId, lua_str);
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
// image preocessing - internal
int  TLuaHostWrap::ProcessImage(lua_State *L)
{
bool lRet = false;
if (lua_gettop(L) != 4)
   {
   lua_pushstring(L, "incorrect number of arguments to ProcessImage");
   lua_error(L);
   return 0;
   }
if (lua_isnumber(L, 1) && lua_isnumber(L, 2) && lua_isstring(L, 3) && lua_isstring(L, 4))
   {
   int inpId = lua_tointeger(L, 1);    // source image id
   int outId = lua_tointeger(L, 2);    // target image id
   const char *lua_proc   = lua_tostring(L, 3);
   const char *lua_params = lua_tostring(L, 4);
   lua_pop(L, 4);
   String procType = String(lua_proc);
   if (procType == "blur")
	  lRet = real_object->Blur(inpId, outId, lua_params);
   else
	  lRet = false;
   }
else
   {
   lua_pushstring(L, "incorrect arguments to ProcessImage");
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
if (nParams < 1)
   {
   lua_pushstring(L, "incorrect number of arguments to OpenCV get");
   lua_error(L);
   return 0;
   }
if (!lua_isstring(L, 1))
   {
   lua_pushstring(L, "incorrect arguments to OpenCV get");
   lua_error(L);
   return 0;
   }
const char *lua_proc   = lua_tostring(L, 1);
int nRet = real_object->OcvGet(lua_proc);
lua_pop(L, nParams);
lua_pushnumber(L, nRet);
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
	  lua_pushstring(L, errMsg);
	  lua_error(L);
	  return 0;
	  }
   }
else
   {
   lua_pushstring(L, "incorrect arguments to OpenCV process");
   lua_error(L);
   return 0;
   }
lua_pushboolean (L, 1);
return 1;
}
//---------------------------------------------------------------------------



