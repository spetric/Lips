//---------------------------------------------------------------------------

#ifndef FrameSliderUH
#define FrameSliderUH
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
#include "pxSpinSlider.h"
//---------------------------------------------------------------------------
class TFrameSlider : public TFrame
{
__published:	// IDE-managed Components
	TcxGroupBox *box;
	TpxSpinSlider *slider;
private:	// User declarations
public:		// User declarations
	__fastcall TFrameSlider(TComponent* Owner);
};
//---------------------------------------------------------------------------
//extern PACKAGE TFrameSlider *FrameSlider;
//---------------------------------------------------------------------------
#endif
