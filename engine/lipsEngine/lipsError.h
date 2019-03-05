//---------------------------------------------------------------------------
#ifndef lipsErrorH
#define lipsErrorH
#include <Types.hpp>
static const int LERR_INVALID_SOURCE = -1;
static const int LERR_INVALID_TARGET = -2;
static const int LERR_UNKNOWN_PROC   = -3;
static const int LERR_EXCEPTION		 = -4;
static const int LERR_INVALID_ARGNUM = -5;
static const int LERR_INVALID_PARAM  = -6;
static const AnsiString LERR_Msg[6] = {"Invalid source image.", "Invalid target image.", ": unknown proc. name.", ": exception.",
									   ": invalid number of arguments", ": invalid parameter -> " };
//---------------------------------------------------------------------------
class TLipsError
{
	public:
        static AnsiString ErrorMessage;
		static AnsiString GetMessage(int errCode);
		static AnsiString GetMessage(AnsiString proc_name, int errCode);
		static AnsiString GetMessage(AnsiString proc_name, AnsiString param, int errCode);
		static AnsiString SetMessage(AnsiString proc_name, int rc);
};
#endif
