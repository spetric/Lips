//---------------------------------------------------------------------------

#include <vcl.h>
#pragma hdrstop

#include "FrameListBoxU.h"
//---------------------------------------------------------------------------
#pragma package(smart_init)
#pragma link "cxContainer"
#pragma link "cxControls"
#pragma link "cxEdit"
#pragma link "cxGraphics"
#pragma link "cxGroupBox"
#pragma link "cxLookAndFeelPainters"
#pragma link "cxLookAndFeels"
#pragma link "dxSkinDarkRoom"
#pragma link "dxSkinDarkSide"
#pragma link "dxSkinsCore"
#pragma resource "*.dfm"
TFrameListBox *FrameListBox;
//---------------------------------------------------------------------------
__fastcall TFrameListBox::TFrameListBox(TComponent* Owner)
	: TFrame(Owner)
{
}
//---------------------------------------------------------------------------
