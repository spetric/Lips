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
		int srcMaskId;
        int tgtMaskId;
		};
	TOpenCV FOpenCV;
	int FDKIdx;
	SocvContourData FExportContours;
	SocvContourPts *FCurrentContour;
	SocvHomography FLastHomography;
    SocvTemplateData  FTemplData;
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
	TLuaProgress OnLuaProgress;  	    // callback
	TLuaString OnLuaRequireParams;      // callback
	TLuaString OnLuaRefreshParams;      // callback
	TLuaString OnLuaSendMessage;        // callback
	TLuaDoubleString OnLuaLoadImage;    // callback
	TLuaExportImage OnLuaExportImage;   // callback
	TLuaHostDialog  OnLuaHostDialog;    // callback
	TLuaDoubleString OnLuaSendCommand;  // callback
	TLuaExportRawData OnLuaExportRawData; // callback
	// not exposed to lua
	void SetPushImage(TInternalImage *img);
	TLuaImageVoid* AddPushImage(TInternalImage *img);
	// exposed
	void ShowProgress(int done, int total);
	void RequireParams(const char *parList);
	void RefreshParams(void);
	void SendMessage(const char *msg);
	int  HostDialog(const char *dialogText, int type);
    bool SendCommand(const char *cmd, const char *parList);
	// image routines
	TLuaImageVoid* CreateImage(const char *type, int width, int height, bool addAlpha);
	TLuaImageVoid* LoadImage(const char *filename, const char *type);
	bool ExportImage(int idExport, const char *exptype, bool show);
    bool ExportRawData(int type);
	TStrPtr GetImage(int id);
    bool ReleaseImage(int id);
    bool CopyImage(int idInput, int idOutput, TLuaRoi *inpRoi, TLuaRoi *outRoi);
	// color space conversions
	bool Conv_rgb2gray(int idInput, int idOutput);
	bool Conv_gray2rgb(int idInput, int idOutput, bool alpha);
	bool Conv_rgb2xyz(int idInput, int idOutput);
	bool Conv_xyz2rgb(int idInput, int idOutput);
	bool Conv_rgb2lab(int idInput, int idOutput);
	bool Conv_lab2rgb(int idInput, int idOutput);
	// image processing - callable
    bool DownSample(int idInput, int idOutput);
	// image processing - internal
	bool Blur(int idInput, int idOutput, int radius);
	bool Blend(int idInput, int idMask, int idOutput, bool alpha);
	// checker
    bool Checker(const char *cmd, const char *param);
	// OpenCV
	bool OcvSet(const char *proc, int id);
	bool OcvSet(const char *proc, TLuaRoi * roi);
	int  OcvGetInt(const char *proc);
	void* OcvGetPtr(const char *proc, const char *params);
	// image processing - no params
	const char* OcvProc(const char *proc, int &rc);
	// image processing - opencv single params set
	const char* OcvProc(const char *proc, const char *params, int &rc);
	// image processing - opencv double params set
	const char* OcvProc(const char *proc, const char *params1, const char *params2, int &rc);
	// image processing - opencv tripe params set
	const char* OcvProc(const char *proc, const char *params1, const char *params2, const char *params3, int &rc);
	//
};
#endif
