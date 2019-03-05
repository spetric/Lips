//---------------------------------------------------------------------------

#include <vcl.h>
#pragma hdrstop
#include "FrameImageComboU.h"
//---------------------------------------------------------------------------
#pragma package(smart_init)
#pragma link "AdvCombo"
#pragma link "cxClasses"
#pragma link "cxContainer"
#pragma link "cxControls"
#pragma link "cxDropDownEdit"
#pragma link "cxEdit"
#pragma link "cxGraphics"
#pragma link "cxGroupBox"
#pragma link "cxImageComboBox"
#pragma link "cxLookAndFeelPainters"
#pragma link "cxLookAndFeels"
#pragma link "cxMaskEdit"
#pragma link "cxTextEdit"
#pragma link "dxGDIPlusClasses"
#pragma link "dxSkinDarkRoom"
#pragma link "dxSkinDarkSide"
#pragma link "dxSkinsCore"
#pragma link "ImagePicker"
#pragma link "AdvSmoothEdit"
#pragma link "AdvSmoothEditButton"
#pragma link "AdvSmoothImageListBoxPicker"
#pragma resource "*.dfm"
//TFrameImageCombo *FrameImageCombo;
//---------------------------------------------------------------------------
__fastcall TFrameImageCombo::TFrameImageCombo(TComponent* Owner)
	: TFrame(Owner)
{
}
//---------------------------------------------------------------------------
void __fastcall TFrameImageCombo::Add(String path, String search)
{
FPath = IncludeTrailingBackslash(path);
image->ListBox->AddImagesFromFolder(FPath + search, true, true);
int i = 0;
// remove erroenous
while (i < image->ListBox->Items->Count)
  {
  int pf = image->ListBox->Items->Items[i]->PixelFormat;
  if (pf == 0)
	 image->ListBox->Items->Delete(i);
  else
	 i++;
  }
image->ListBox->Footer->Caption = path;
}
//---------------------------------------------------------------------------
void __fastcall TFrameImageCombo::imageSelectImage(TObject *Sender, int itemindex)
{
File = FPath + image->ListBox->Items->Items[itemindex]->FileName;
}
//---------------------------------------------------------------------------

