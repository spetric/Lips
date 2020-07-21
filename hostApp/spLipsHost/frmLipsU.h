//---------------------------------------------------------------------------
#ifndef frmLipsUH
#define frmLipsUH
#ifndef spHostApp	// check
#define spHostApp	// yes, we are the host application
#endif
#include "lipsEngineDll.h"
#include "lipsHelper.h"
#include "FrameImageU.h"
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
#include "cxRadioGroup.hpp"
#include "pxCheckLabel.h"
#include "dxSkinBlack.hpp"
#include "dxSkinCoffee.hpp"
#include "dxSkinsDefaultPainters.hpp"
#include "dxSkinSilver.hpp"
#include "cxDropDownEdit.hpp"
#include "cxMaskEdit.hpp"
#include "cxShellComboBox.hpp"
#include "cxShellCommon.hpp"
#include "cxTextEdit.hpp"
#include <Winapi.ShlObj.hpp>
#include "iemview.hpp"
#include "imageenio.hpp"
#include <Vcl.ToolWin.hpp>
#include "iexLayerMView.hpp"
String const lips_IEN_BLEND[44]  =  {"Normal", "Add", "Sub", "Divide", "Multiply", "OR", "AND", "XOR", "Lighten", "Darken",
								   "Average", "Screen", "Negation", "Exclusion", "Overlay", "HardLight", "SoftLight",
								   "XFader", "ColorEdge", "ColorBurn", "InverseColorDodge", "InverseColorBurn",
								   "SoftDodge", "SoftBurn", "Reflect", "Glow", "Freeze", "Eat", "Subtractive",
								   "Interpolation", "Stamp", "Red", "Green", "Blue", "Hue", "Saturation", "Color", "Luminosity",
								   "StereoBW", "StereoColor", "StereoColorDubois", "StereoEven", "StereoOdd", "Luminosity 2"};
//---------------------------------------------------------------------------
class TfrmLips : public TForm
{
__published:	// IDE-managed Components
	TPanel *panMain;
	TImageEnVect *ieViewSingle;
	TPanel *panCtrls;
	TPanel *panTop;
	TPanel *panRightBot;
	TPanel *panCentral;
	TMemo *luaMemo;
	TOpenImageEnDialog *openDialog;
	TSaveImageEnDialog *saveDialog;
	TAdvSmoothListBox *listScripts;
	TLabel *labParams;
	TSpeedButton *btnExecute;
	TSpeedButton *btnReload;
	TMainMenu *MainMenu1;
	TMenuItem *File1;
	TMenuItem *itemOpenST;
	TMenuItem *itemSave;
	TMenuItem *Selection1;
	TMenuItem *ItemSelectRectangle;
	TMenuItem *itemSelEllipse;
	TMenuItem *itemLasso;
	TMenuItem *itemMagicWand;
	TMenuItem *itemPolygon;
	TMenuItem *N1;
	TMenuItem *itemClearSelection;
	TMenuItem *itemInvertSelection;
	TcxPageControl *pageControl;
	TcxTabSheet *cxTabSheet1;
	TcxTabSheet *cxTabSheet2;
	TcxGroupBox *boxSelection;
	TpxSpinSlider *editSelFeather;
	TpxCheckLabel *chkSelRestore;
	TLabel *Label1;
	TpxSpinSlider *editWandTol;
	TpxCheckLabel *chkSelMaxFilter;
	TcxRadioGroup *rgpWandModes;
	TScrollBox *scrollBox;
	TcxGroupBox *boxImages;
	TcxShellComboBox *comboShellTiles;
	TLabel *Label2;
	TcxRadioGroup *rgpAuxViewStyle;
	TLabel *Label3;
	TcxShellComboBox *comboShellTextures;
	TMenuItem *itemOpenS;
	TMenuItem *itemOpenT;
	TImageEnMView *mView;
	TPanel *panLayers;
	TPanel *panStatus;
	TLabel *labStatus;
	TImageEnLayerMView *layersMView;
	TPanel *panBlender;
	TpxSpinSlider *editBlend;
	TLabel *Label4;
	TcxComboBox *comboBlend;
	TLabel *Label5;
	TPopupMenu *popupCopy;
	TMenuItem *itemCopy0;
	TMenuItem *copyItem1;
	TMenuItem *itemCopy2;
	TMenuItem *itemCopy3;
	TMenuItem *N2;
	TMenuItem *itemSwap0;
	TMenuItem *itemSwap1;
	TMenuItem *itemSwap2;
	TMenuItem *itemCopy4;
	TPanel *panMVaction;
	TSpeedButton *btnClearAdditional;
	TSpeedButton *btnCopy;
	TSpeedButton *btnLayersMerge;
	TcxRadioGroup *rgpFileViewStyle;
	TPanel *panSingle;
	TImageEnVect *ieViewLayers;
	TcxPageControl *pageMVControl;
	TcxTabSheet *tabNormal;
	TcxTabSheet *tabLayers;
	TSpeedButton *btnClearAll;
	TLabel *labInfo;
	TMenuItem *N3;
	TMenuItem *itemSelected2Buffer;
	TpxCheckLabel *chkLayersSB;
	void __fastcall btnExecuteClick(TObject *Sender);
	void __fastcall listScriptsItemSelected(TObject *Sender, int itemindex);
	void __fastcall FormShow(TObject *Sender);
	void __fastcall btnReloadClick(TObject *Sender);
	void __fastcall itemOpenSTClick(TObject *Sender);
	void __fastcall itemSaveClick(TObject *Sender);
	void __fastcall ItemSelectionClick(TObject *Sender);
	void __fastcall itemClearSelectionClick(TObject *Sender);
	void __fastcall itemInvertSelectionClick(TObject *Sender);
	void __fastcall rgpAuxViewStylePropertiesChange(TObject *Sender);
	void __fastcall itemOpenSClick(TObject *Sender);
	void __fastcall itemOpenTClick(TObject *Sender);
	void __fastcall mViewImageSelect(TObject *Sender, int idx);
	void __fastcall btnClearAllClick(TObject *Sender);
	void __fastcall btnClearAdditionalClick(TObject *Sender);
	void __fastcall btnCopyClick(TObject *Sender);
	void __fastcall layersMViewImageSelect(TObject *Sender, int idx);
	void __fastcall editBlendValueChange(TObject *Sender);
	void __fastcall comboBlendPropertiesChange(TObject *Sender);
	void __fastcall itemCopyClick(TObject *Sender);
	void __fastcall btnLayersMergeClick(TObject *Sender);
	void __fastcall ieViewSingleMouseDown(TObject *Sender, TMouseButton Button, TShiftState Shift,
          int X, int Y);
	void __fastcall ieViewSingleMouseMove(TObject *Sender, TShiftState Shift, int X,
          int Y);
	void __fastcall ieViewSingleMouseUp(TObject *Sender, TMouseButton Button, TShiftState Shift,
          int X, int Y);
	void __fastcall itemSelected2BufferClick(TObject *Sender);
	void __fastcall chkLayersSBValueChange(TObject *Sender);
private:	// User declarations
	struct SelStruct
		{
		TIEBitmap *Image;
		TIEBitmap *Mask;
        TRect Roi;
		SelStruct() {Image = 0; Mask = 0;}
		};
	TList *FImageList;
	TIEBitmap *FPushImage;
	TStringList *FLuaScripts;
	TRect *FSelRect;
	TIEBitmap *FMapWork, *FSourceMask, *FTargetMask;
    SelStruct FSelection;
	bool FExportedAsTarget;
	bool FScriptLoaded;
	bool FMouseEvent2Lua;
    bool FSelectionRoiEnabled;
	void __fastcall deleteImages(int upto = 0);
	void __fastcall setIcmv(int idx, TImageContainer *ic);
    TIEBitmap* __fastcall getMapFromTag(int tag);
    //
	void __fastcall checkImageFrames(void);
	void __fastcall loadScriptList(UnicodeString path);
	void __fastcall createLipSurface(void);
	void __fastcall luaParamBuilder(void);
	void __fastcall luaParamRefresh(void);
	void __fastcall luaImportImage(TExportImage *expImage, bool asIs, bool show);
	void __fastcall luaImportRawData(void *data, int type);
	void __fastcall prepareTableImage(TFrameImage *fi, TLipsHelper::LuaTKRV *obj);
	void __fastcall pushTableImage(TImageEnView *fiView, TLipsHelper::LuaTKRV *obj);
	String __fastcall setParamValues(void);
	bool __fastcall luaCommand(const char *cmd, const char* parList);
	void __fastcall resetMouseEvents(void);
public:		// User declarations
	__fastcall TfrmLips(TComponent* Owner);
	__fastcall ~TfrmLips();
};
//---------------------------------------------------------------------------
extern PACKAGE TfrmLips *frmLips;
//---------------------------------------------------------------------------
#endif
