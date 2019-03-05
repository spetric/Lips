//---------------------------------------------------------------------------
#ifndef luaHostWrapH
#define luaHostWrapH
// The header file for the real C++ object
#include "luaHost.h"
// Need to include lua headers this way
extern "C"
		{
		#include "lua.h"
		#include "lauxlib.h"
		#include "lualib.h"
		}
#include "luna.h"
//---------------------------------------------------------------------------
class TLuaHostWrap
{
  public:
	// Constants
	static const char className[];
	static Luna<TLuaHostWrap>::RegType methods[];
	// Initialize the pointer
	TLuaHostWrap(lua_State *L);
	~TLuaHostWrap();
	void setObject(lua_State *L);
	// Methods we will use
	int ShowProgress(lua_State *L);
	int RequireParams(lua_State *L);
	int SendMessage(lua_State *L);
	int HostDialog(lua_State *L);
	int CreateImage(lua_State *L);
	int LoadImage(lua_State *L);
	int ExportImage(lua_State *L);
	int GetImage(lua_State *L);
    int CopyImage(lua_State *L);
	int DeleteImage(lua_State *L);
	int ConvertColorSpace(lua_State *L);
	int ProcessImage(lua_State *L);
	int OpenCVSet(lua_State *L);
    int OpenCVGet(lua_State *L);
	int OpenCVProcess(lua_State *L);
  private:
	// The pointer to the "real object" of type TLueConsole
    TLuaHost* real_object;
};
#endif
