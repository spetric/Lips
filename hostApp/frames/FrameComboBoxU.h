//---------------------------------------------------------------------------

#ifndef FrameComboBoxUH
#define FrameComboBoxUH
//---------------------------------------------------------------------------
#include <System.Classes.hpp>
#include <Vcl.Controls.hpp>
#include <Vcl.StdCtrls.hpp>
#include <Vcl.Forms.hpp>
#include "cxContainer.hpp"
#include "cxControls.hpp"
#include "cxDropDownEdit.hpp"
#include "cxEdit.hpp"
#include "cxGraphics.hpp"
#include "cxGroupBox.hpp"
#include "cxLookAndFeelPainters.hpp"
#include "cxLookAndFeels.hpp"
#include "cxMaskEdit.hpp"
#include "cxTextEdit.hpp"
#include "dxSkinDarkRoom.hpp"
#include "dxSkinDarkSide.hpp"
#include "dxSkinsCore.hpp"
//---------------------------------------------------------------------------
class TFrameComboBox : public TFrame
{
__published:	// IDE-managed Components
	TcxGroupBox *box;
	TComboBox *combo;
private:	// User declarations
public:		// User declarations
	__fastcall TFrameComboBox(TComponent* Owner);
};
//---------------------------------------------------------------------------
//extern PACKAGE TFrameComboBox *FrameComboBox;
//---------------------------------------------------------------------------
#endif
