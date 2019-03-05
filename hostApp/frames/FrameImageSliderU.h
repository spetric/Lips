//---------------------------------------------------------------------------

#ifndef FrameImageSliderUH
#define FrameImageSliderUH
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
#include "dxImageSlider.hpp"
#include "dxSkinDarkRoom.hpp"
#include "dxSkinDarkSide.hpp"
#include "dxSkinsCore.hpp"
#include "cxClasses.hpp"
#include "dxGDIPlusClasses.hpp"
//---------------------------------------------------------------------------
class TFrameImageSlider : public TFrame
{
__published:	// IDE-managed Components
	TcxGroupBox *box;
	TdxImageSlider *image;
	TcxImageCollection *imageCollection;
private:	// User declarations
	TStringList *FImageList;
public:		// User declarations
	__fastcall TFrameImageSlider(TComponent* Owner);
	virtual __fastcall ~TFrameImageSlider();
    void __fastcall Clear(void);
	void __fastcall Add(String filename);
	String __fastcall SelectedImage(void);
};
//---------------------------------------------------------------------------
//extern PACKAGE TFrameImageSlider *FrameImageSlider;
//---------------------------------------------------------------------------
#endif
