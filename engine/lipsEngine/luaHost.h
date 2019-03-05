//---------------------------------------------------------------------------
#ifndef luaHostH
#define luaHostH
#include "lipsGlobals.h"
#include "internalImage.h"
#include "ocvWrapper.h"
//#include <Types.hpp>
//#include <Classes.hpp>
//---------------------------------------------------------------------------
class TLuaHost
{
private:
    enum TClearType {clearSource = 1, clearTarget = 2, clearAll = 3};
	AnsiString FLuaParameters;
	AnsiString FLuaMessage;
    AnsiString FLuaType;
	TInternalImage *FLuaPushImage;
	TExportImage FExpImage;
	//luaImageVoid* getImageFromId(int id);
	// openCV
	struct TOpenCV
		{
		int sourceId;
		int targetId;
		SocvRoi sourceRoi;
		SocvRoi targetRoi;
		};
	TOpenCV FOpenCV;
	int FDKIdx;
	void clearData(TClearType type);
protected:
public:
	struct TStrPtr
		{
		char *str;
		void *ptr;
        };
	TLuaHost(void);
	virtual ~TLuaHost(void);
	TLuaProgress OnLuaProgress;  	 // callback
	TLuaString OnLuaRequireParams;   // callback
	TLuaString OnLuaSendMessage;     // callback
	TLuaLoadImage OnLuaLoadImage;    // callback
	TLuaExportImage OnLuaExportImage; // callback
    TLuaHostDialog  OnLuaHostDialog;
	// not exposed to lua
	void SetPushImage(TInternalImage *img);
	TLuaImageVoid* AddPushImage(TInternalImage *img);
	// exposed
	void ShowProgress(int done, int total);
	void RequireParams(const char *parList);
	void SendMessage(const char *msg);
	int  HostDialog(const char *dialogText);
	// image routines
	TLuaImageVoid* CreateImage(const char *type, int width, int height);
	TLuaImageVoid* LoadImage(const char *filename, const char *type);
	bool ExportImage(int idExport, const char *exptype);
	TStrPtr GetImage(int id);
    bool CopyImage(int idInput, int idOutput, TLuaRoi *inpRoi, TLuaRoi *outRoi);
	// color space conversions
	bool Conv_rgb2gray(int idInput, int idOutput);
	bool Conv_rgb2xyz(int idInput, int idOutput);
	bool Conv_xyz2rgb(int idInput, int idOutput);
	bool Conv_rgb2lab(int idInput, int idOutput);
	bool Conv_lab2rgb(int idInput, int idOutput);
	// image processing - internal
	bool Blur(int idInput, int idOutput, const char *radius);
	// OpenCV
	bool OcvSet(const char *proc, int id);
	bool OcvSet(const char *proc, TLuaRoi * roi);
	int  OcvGet(const char *proc);
	// image processing - no params
	const char* OcvProc(const char *proc, int &rc);
	// image processing - opencv single params set
	const char* OcvProc(const char *proc, const char *params, int &rc);
	// image processing - opencv double params set
	const char* OcvProc(const char *proc, const char *params1, const char *params2, int &rc);
	// image processing - opencv tripe params set
	const char* OcvProc(const char *proc, const char *params1, const char *params2, const char *params3, int &rc);

};
#endif
