//---------------------------------------------------------------------------

#include <vcl.h>
#pragma hdrstop

#include "FrameImageSliderU.h"
//---------------------------------------------------------------------------
#pragma package(smart_init)
#pragma link "cxContainer"
#pragma link "cxControls"
#pragma link "cxEdit"
#pragma link "cxGraphics"
#pragma link "cxGroupBox"
#pragma link "cxLookAndFeelPainters"
#pragma link "cxLookAndFeels"
#pragma link "dxImageSlider"
#pragma link "dxSkinDarkRoom"
#pragma link "dxSkinDarkSide"
#pragma link "dxSkinsCore"
#pragma link "cxClasses"
#pragma link "dxGDIPlusClasses"
#pragma resource "*.dfm"
//TFrameImageSlider *FrameImageSlider;
//---------------------------------------------------------------------------
__fastcall TFrameImageSlider::TFrameImageSlider(TComponent* Owner)
	: TFrame(Owner)
{
FImageList = new TStringList();
}
//---------------------------------------------------------------------------
__fastcall TFrameImageSlider::~TFrameImageSlider()
{
Clear();
delete FImageList;
FImageList = 0;
}
//---------------------------------------------------------------------------
void __fastcall TFrameImageSlider::Clear(void)
{
FImageList->Clear();
imageCollection->Items->Clear();
}
//---------------------------------------------------------------------------
void __fastcall TFrameImageSlider::Add(String filename)
{
FImageList->Add(filename);
imageCollection->Items->Add();
imageCollection->Items->Items[imageCollection->Count - 1]->Picture->LoadFromFile(filename);
}
//---------------------------------------------------------------------------
String __fastcall TFrameImageSlider::SelectedImage(void)
{
return FImageList->Strings[image->ItemIndex];
}
//---------------------------------------------------------------------------
