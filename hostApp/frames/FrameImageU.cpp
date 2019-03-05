//---------------------------------------------------------------------------

#include <vcl.h>
#pragma hdrstop

#include "FrameImageU.h"
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
#pragma link "hyiedefs"
#pragma link "hyieutils"
#pragma link "iesettings"
#pragma link "ievect"
#pragma link "ieview"
#pragma link "iexBitmaps"
#pragma link "iexLayers"
#pragma link "iexRulers"
#pragma link "imageenview"
#pragma link "ieopensavedlg"
#pragma resource "*.dfm"
//TFrameImage *FrameImage;
//---------------------------------------------------------------------------
__fastcall TFrameImage::TFrameImage(TComponent* Owner)
	: TFrame(Owner)
{
InputImage = 0;
}
//---------------------------------------------------------------------------
void __fastcall TFrameImage::btnIenClick(TObject *Sender)
{
ieView->Proc->DoPreviews( TPreviewEffects()<<peAll);
}
//---------------------------------------------------------------------------
void __fastcall TFrameImage::btnApplyClick(TObject *Sender)
{
if (InputImage == 0)
   return;
switch (comboOptions->ItemIndex)
   {
   case 0:  // input image
		ieView->IEBitmap->AssignImage(InputImage);
		break;
   case 1:
		if (openDialog->Execute())
		   ieView->IO->LoadFromFile(openDialog->FileName);
		break;
   case 2:
		ieView->Proc->EdgeDetect_ShenCastan(0.99, 0.9, 7, 0, true);
		break;
   case 3:
		ieView->Proc->EdgeDetect_Sobel();
		break;
   case 4:
		ieView->Proc->Negative();
		break;
   case 5:
		ieView->Proc->Blur(1);
		break;
   case 6:
		ieView->Proc->Blur(2);
		break;
   case 7:
		ieView->Proc->Blur(3);
		break;
   default:
   ;
   }
if (Type == "grayscale")
   ieView->Proc->ConvertToGray();
}
//---------------------------------------------------------------------------

