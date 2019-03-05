//---------------------------------------------------------------------------
#pragma hdrstop
#include "lipsError.h"
#include <StrUtils.hpp>
//---------------------------------------------------------------------------
#pragma package(smart_init)
AnsiString TLipsError::ErrorMessage = "";
//---------------------------------------------------------------------------
AnsiString TLipsError::GetMessage(int errCode)
{
if (errCode >= 0)
   return "";
else
   {
   errCode = -errCode;
   ErrorMessage = LERR_Msg[errCode - 1];
   return ErrorMessage;
   }
}
//---------------------------------------------------------------------------
AnsiString TLipsError::GetMessage(AnsiString proc_name, int errCode)
{
if (errCode >= 0)
   return "";
else
   {
   errCode = -errCode;
   ErrorMessage = proc_name + LERR_Msg[errCode - 1];
   return ErrorMessage;
   }
}
//---------------------------------------------------------------------------
AnsiString TLipsError::GetMessage(AnsiString proc_name, AnsiString param, int errCode)
{
if (errCode >= 0)
   return "";
else
   {
   errCode = -errCode;
   ErrorMessage = proc_name + LERR_Msg[errCode - 1] + param;
   return ErrorMessage;
   }
}
//---------------------------------------------------------------------------
AnsiString TLipsError::SetMessage(AnsiString proc_name, int rc)
{
ErrorMessage = proc_name + " ret_code = " + IntToStr(rc);
return ErrorMessage;
}

