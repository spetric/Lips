//---------------------------------------------------------------------------

#ifndef FrameShellComboBoxUH
#define FrameShellComboBoxUH
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
#include "cxShellComboBox.hpp"
#include "cxShellCommon.hpp"
#include "cxTextEdit.hpp"
#include "dxSkinDarkRoom.hpp"
#include "dxSkinDarkSide.hpp"
#include "dxSkinsCore.hpp"
#include <Vcl.ComCtrls.hpp>
#include <Winapi.ShlObj.hpp>
//---------------------------------------------------------------------------
class TFrameShellComboBox : public TFrame
{
__published:	// IDE-managed Components
	TcxGroupBox *box;
	TcxShellComboBox *comboShell;
private:	// User declarations
public:		// User declarations
	__fastcall TFrameShellComboBox(TComponent* Owner);
};
//---------------------------------------------------------------------------
extern PACKAGE TFrameShellComboBox *FrameShellComboBox;
//---------------------------------------------------------------------------
#endif
