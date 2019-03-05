//---------------------------------------------------------------------------

#ifndef FrameListBoxUH
#define FrameListBoxUH
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
//---------------------------------------------------------------------------
class TFrameListBox : public TFrame
{
__published:	// IDE-managed Components
	TListBox *list;
	TcxGroupBox *box;
private:	// User declarations
public:		// User declarations
	__fastcall TFrameListBox(TComponent* Owner);
};
//---------------------------------------------------------------------------
extern PACKAGE TFrameListBox *FrameListBox;
//---------------------------------------------------------------------------
#endif
