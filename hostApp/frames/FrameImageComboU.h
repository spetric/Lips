//---------------------------------------------------------------------------

#ifndef FrameImageComboUH
#define FrameImageComboUH
//---------------------------------------------------------------------------
#include <System.Classes.hpp>
#include <Vcl.Controls.hpp>
#include <Vcl.StdCtrls.hpp>
#include <Vcl.Forms.hpp>
#include "AdvCombo.hpp"
#include "cxClasses.hpp"
#include "cxContainer.hpp"
#include "cxControls.hpp"
#include "cxDropDownEdit.hpp"
#include "cxEdit.hpp"
#include "cxGraphics.hpp"
#include "cxGroupBox.hpp"
#include "cxImageComboBox.hpp"
#include "cxLookAndFeelPainters.hpp"
#include "cxLookAndFeels.hpp"
#include "cxMaskEdit.hpp"
#include "cxTextEdit.hpp"
#include "dxGDIPlusClasses.hpp"
#include "dxSkinDarkRoom.hpp"
#include "dxSkinDarkSide.hpp"
#include "dxSkinsCore.hpp"
#include "ImagePicker.hpp"
#include <System.ImageList.hpp>
#include <Vcl.ImgList.hpp>
#include "AdvSmoothEdit.hpp"
#include "AdvSmoothEditButton.hpp"
#include "AdvSmoothImageListBoxPicker.hpp"
#include <Vcl.Mask.hpp>
#include "dxSkinBlack.hpp"
#include "dxSkinCoffee.hpp"
#include "dxSkinsDefaultPainters.hpp"
#include "dxSkinSilver.hpp"
//---------------------------------------------------------------------------
class TFrameImageCombo : public TFrame
{
__published:	// IDE-managed Components
	TcxGroupBox *box;
	TAdvSmoothImageListBoxPicker *image;
	void __fastcall imageSelectImage(TObject *Sender, int itemindex);

private:	// User declarations
	String FPath;
public:		// User declarations
	__fastcall TFrameImageCombo(TComponent* Owner);
	void __fastcall Add(String path, String search);
	String File;
};
//---------------------------------------------------------------------------
//extern PACKAGE TFrameImageCombo *FrameImageCombo;
//---------------------------------------------------------------------------
#endif
