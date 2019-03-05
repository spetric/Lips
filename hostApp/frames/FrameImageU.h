//---------------------------------------------------------------------------
#ifndef FrameImageUH
#define FrameImageUH
//---------------------------------------------------------------------------
#include <System.Classes.hpp>
#include <Vcl.Controls.hpp>
#include <Vcl.StdCtrls.hpp>
#include <Vcl.Forms.hpp>
#include "cxContainer.hpp"
#include "cxControls.hpp"
#include "cxEdit.hpp"
#include "cxGraphics.hpp"
#include "cxGroupBox.hpp"
#include "cxLookAndFeelPainters.hpp"
#include "cxLookAndFeels.hpp"
#include "dxSkinDarkRoom.hpp"
#include "dxSkinDarkSide.hpp"
#include "dxSkinsCore.hpp"
#include "hyiedefs.hpp"
#include "hyieutils.hpp"
#include "iesettings.hpp"
#include "ievect.hpp"
#include "ieview.hpp"
#include "iexBitmaps.hpp"
#include "iexLayers.hpp"
#include "iexRulers.hpp"
#include "imageenview.hpp"
#include <Vcl.Buttons.hpp>
#include "ieopensavedlg.hpp"
//---------------------------------------------------------------------------
class TFrameImage : public TFrame
{
__published:	// IDE-managed Components
	TcxGroupBox *box;
	TSpeedButton *btnIen;
	TImageEnView *ieView;
	TComboBox *comboOptions;
	TSpeedButton *btnApply;
	TOpenImageEnDialog *openDialog;
	void __fastcall btnIenClick(TObject *Sender);
	void __fastcall btnApplyClick(TObject *Sender);
private:	// User declarations
public:		// User declarations
	TIEBitmap *InputImage;
    String Type;
	__fastcall TFrameImage(TComponent* Owner);
};
//---------------------------------------------------------------------------
//extern PACKAGE TFrameImage *FrameImage;
//---------------------------------------------------------------------------
#endif
