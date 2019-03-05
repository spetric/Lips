//---------------------------------------------------------------------------

#ifndef FrameCheckBoxUH
#define FrameCheckBoxUH
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
#include "pxCheckLabel.h"
//---------------------------------------------------------------------------
class TFrameCheckBox : public TFrame
{
__published:	// IDE-managed Components
	TpxCheckLabel *chbox;
	TcxGroupBox *box;
private:	// User declarations
public:		// User declarations
	__fastcall TFrameCheckBox(TComponent* Owner);
};
//---------------------------------------------------------------------------
//extern PACKAGE TFrameCheckBox *FrameCheckBox;
//---------------------------------------------------------------------------
#endif
