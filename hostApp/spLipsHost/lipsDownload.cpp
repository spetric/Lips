//---------------------------------------------------------------------------
#include <vcl.h>
#pragma hdrstop
#include "lipsDownload.h"
//---------------------------------------------------------------------------
#pragma package(smart_init)
#pragma link "HTMLabel"
#pragma resource "*.dfm"
//---------------------------------------------------------------------------
__fastcall TLipsDownload::TLipsDownload(TComponent* Owner, String str)
	: TForm(Owner)
{
//TStringDynArray tokens = SplitString(sParams, ",");
//labHTML->HTMLText->Text = tokens[0] + "<A href=\"" + tokens[1] + "\"" + ;
labHTML->HTMLText->Text = str;
}
//---------------------------------------------------------------------------

void __fastcall TLipsDownload::btnAbortClick(TObject *Sender)
{
Close();
}
//---------------------------------------------------------------------------

