//---------------------------------------------------------------------------
#ifndef frmLipUH
#define frmLipUH
#ifndef spHostApp	// check
#define spHostApp	// yes, we are the host application
#endif
#include "lipEngineDll.h"
#include "commonHelper.h"
//---------------------------------------------------------------------------
#include <System.Classes.hpp>
#include <Vcl.Controls.hpp>
#include <Vcl.StdCtrls.hpp>
#include <Vcl.Forms.hpp>
#include "hyiedefs.hpp"
#include "hyieutils.hpp"
#include "ieopensavedlg.hpp"
#include "iesettings.hpp"
#include "ievect.hpp"
#include "ieview.hpp"
#include "iexBitmaps.hpp"
#include "iexLayers.hpp"
#include "iexRulers.hpp"
#include "imageenview.hpp"
#include <Vcl.ComCtrls.hpp>
#include <Vcl.ExtCtrls.hpp>
#include "pxSpinSlider.h"
#include "AdvSmoothListBox.hpp"
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
#include <Vcl.Buttons.hpp>
#include <Vcl.Menus.hpp>
#include "cxPC.hpp"
#include "dxSkinBlack.hpp"
#include "dxSkinBlue.hpp"
#include "dxSkinBlueprint.hpp"
#include "dxSkinCaramel.hpp"
#include "dxSkinCoffee.hpp"
#include "dxSkinDevExpressDarkStyle.hpp"
#include "dxSkinDevExpressStyle.hpp"
#include "dxSkinFoggy.hpp"
#include "dxSkinGlassOceans.hpp"
#include "dxSkinHighContrast.hpp"
#include "dxSkiniMaginary.hpp"
#include "dxSkinLilian.hpp"
#include "dxSkinLiquidSky.hpp"
#include "dxSkinLondonLiquidSky.hpp"
#include "dxSkinMcSkin.hpp"
#include "dxSkinMetropolis.hpp"
#include "dxSkinMetropolisDark.hpp"
#include "dxSkinMoneyTwins.hpp"
#include "dxSkinOffice2007Black.hpp"
#include "dxSkinOffice2007Blue.hpp"
#include "dxSkinOffice2007Green.hpp"
#include "dxSkinOffice2007Pink.hpp"
#include "dxSkinOffice2007Silver.hpp"
#include "dxSkinOffice2010Black.hpp"
#include "dxSkinOffice2010Blue.hpp"
#include "dxSkinOffice2010Silver.hpp"
#include "dxSkinOffice2013DarkGray.hpp"
#include "dxSkinOffice2013LightGray.hpp"
#include "dxSkinOffice2013White.hpp"
#include "dxSkinOffice2016Colorful.hpp"
#include "dxSkinOffice2016Dark.hpp"
#include "dxSkinPumpkin.hpp"
#include "dxSkinsDefaultPainters.hpp"
#include "dxSkinSeven.hpp"
#include "dxSkinSevenClassic.hpp"
#include "dxSkinSharp.hpp"
#include "dxSkinSharpPlus.hpp"
#include "dxSkinSilver.hpp"
#include "dxSkinSpringTime.hpp"
#include "dxSkinStardust.hpp"
#include "dxSkinSummer2008.hpp"
#include "dxSkinTheAsphaltWorld.hpp"
#include "dxSkinValentine.hpp"
#include "dxSkinVisualStudio2013Blue.hpp"
#include "dxSkinVisualStudio2013Dark.hpp"
#include "dxSkinVisualStudio2013Light.hpp"
#include "dxSkinVS2010.hpp"
#include "dxSkinWhiteprint.hpp"
#include "dxSkinXmas2008Blue.hpp"
#include "cxRadioGroup.hpp"
#include "pxCheckLabel.h"
//---------------------------------------------------------------------------
class TfrmLip : public TForm
{
__published:	// IDE-managed Components
	TPanel *panMain;
	TImageEnVect *ieView;
	TStatusBar *statusBar;
	TPanel *panCtrls;
	TPanel *panTop;
	TPanel *panBot;
	TPanel *panCentral;
	TMemo *luaMemo;
	TOpenImageEnDialog *openDialog;
	TSaveImageEnDialog *saveDialog;
	TAdvSmoothListBox *listScripts;
	TLabel *labParams;
	TPanel *panParams;
	TSpeedButton *btnExecute;
	TSpeedButton *btnUndo;
	TSpeedButton *btnAction;
	TMainMenu *MainMenu1;
	TMenuItem *File1;
	TMenuItem *itemOpen;
	TMenuItem *itemSave;
	TMenuItem *itemSaveAs;
	TMenuItem *Selection1;
	TMenuItem *ItemSelectRectangle;
	TMenuItem *itemSelEllipse;
	TMenuItem *itemLasso;
	TMenuItem *itemMagicWand;
	TMenuItem *itemPolygon;
	TMenuItem *N1;
	TMenuItem *itemClearSelection;
	TMenuItem *itemIInvertSelection;
	TcxPageControl *pageControl;
	TcxTabSheet *cxTabSheet1;
	TcxTabSheet *cxTabSheet2;
	TPanel *panSelection;
	TpxCheckLabel *chkSelMaxFilter;
	TpxCheckLabel *chkSelRestore;
	TpxSpinSlider *editSelFeather;
	TpxSpinSlider *editWandTol;
	TcxRadioGroup *rgpWandModes;
	TStaticText *StaticText1;
	void __fastcall FormDestroy(TObject *Sender);
	void __fastcall btnExecuteClick(TObject *Sender);
	void __fastcall listScriptsItemSelected(TObject *Sender, int itemindex);
	void __fastcall FormShow(TObject *Sender);
	void __fastcall btnUndoClick(TObject *Sender);
	void __fastcall btnActionClick(TObject *Sender);
	void __fastcall itemOpenClick(TObject *Sender);
	void __fastcall itemSaveClick(TObject *Sender);
	void __fastcall itemSaveAsClick(TObject *Sender);
	void __fastcall ItemSelectionClick(TObject *Sender);
	void __fastcall itemClearSelectionClick(TObject *Sender);
private:	// User declarations
	TIEBitmap *FOutputMap, *FMaskMap, *FUndoMap;
	TStringList *FLuaScripts;
    TRect *FSelRect;
	int FWidth, FHeight;
    bool FScriptLoaded;
	void __fastcall deleteOfsMaps(void);
	void __fastcall loadScriptList(UnicodeString path);
	void __fastcall createLipSurface(void);
	void __fastcall luaParamBuilder(void);
    String __fastcall setParamValues(void);
public:		// User declarations
	__fastcall TfrmLip(TComponent* Owner);
};
//---------------------------------------------------------------------------
extern PACKAGE TfrmLip *frmLip;
//---------------------------------------------------------------------------
#endif
