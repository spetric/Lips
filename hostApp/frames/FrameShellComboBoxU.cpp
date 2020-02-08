//---------------------------------------------------------------------------

#include <vcl.h>
#pragma hdrstop

#include "FrameShellComboBoxU.h"
//---------------------------------------------------------------------------
#pragma package(smart_init)
#pragma link "cxContainer"
#pragma link "cxControls"
#pragma link "cxDropDownEdit"
#pragma link "cxEdit"
#pragma link "cxGraphics"
#pragma link "cxGroupBox"
#pragma link "cxLookAndFeelPainters"
#pragma link "cxLookAndFeels"
#pragma link "cxMaskEdit"
#pragma link "cxShellComboBox"
#pragma link "cxShellCommon"
#pragma link "cxTextEdit"
#pragma link "dxSkinDarkRoom"
#pragma link "dxSkinDarkSide"
#pragma link "dxSkinsCore"
#pragma resource "*.dfm"
TFrameShellComboBox *FrameShellComboBox;
//---------------------------------------------------------------------------
__fastcall TFrameShellComboBox::TFrameShellComboBox(TComponent* Owner)
	: TFrame(Owner)
{
}
//---------------------------------------------------------------------------
