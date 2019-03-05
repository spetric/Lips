//---------------------------------------------------------------------------
#include <vcl.h>
#pragma hdrstop
#include "frmLipU.h"
#include "FrameSliderU.h"
#include "FrameCheckBoxU.h"
//---------------------------------------------------------------------------
#pragma package(smart_init)
#pragma link "hyiedefs"
#pragma link "hyieutils"
#pragma link "ieopensavedlg"
#pragma link "iesettings"
#pragma link "ievect"
#pragma link "ieview"
#pragma link "iexBitmaps"
#pragma link "iexLayers"
#pragma link "iexRulers"
#pragma link "imageenview"
#pragma link "pxSpinSlider"
#pragma link "AdvSmoothListBox"
#pragma link "cxContainer"
#pragma link "cxControls"
#pragma link "cxEdit"
#pragma link "cxGraphics"
#pragma link "cxGroupBox"
#pragma link "cxLookAndFeelPainters"
#pragma link "cxLookAndFeels"
#pragma link "dxSkinDarkRoom"
#pragma link "dxSkinDarkSide"
#pragma link "dxSkinsCore"
#pragma link "cxPC"
#pragma link "dxSkinBlack"
#pragma link "dxSkinBlue"
#pragma link "dxSkinBlueprint"
#pragma link "dxSkinCaramel"
#pragma link "dxSkinCoffee"
#pragma link "dxSkinDevExpressDarkStyle"
#pragma link "dxSkinDevExpressStyle"
#pragma link "dxSkinFoggy"
#pragma link "dxSkinGlassOceans"
#pragma link "dxSkinHighContrast"
#pragma link "dxSkiniMaginary"
#pragma link "dxSkinLilian"
#pragma link "dxSkinLiquidSky"
#pragma link "dxSkinLondonLiquidSky"
#pragma link "dxSkinMcSkin"
#pragma link "dxSkinMetropolis"
#pragma link "dxSkinMetropolisDark"
#pragma link "dxSkinMoneyTwins"
#pragma link "dxSkinOffice2007Black"
#pragma link "dxSkinOffice2007Blue"
#pragma link "dxSkinOffice2007Green"
#pragma link "dxSkinOffice2007Pink"
#pragma link "dxSkinOffice2007Silver"
#pragma link "dxSkinOffice2010Black"
#pragma link "dxSkinOffice2010Blue"
#pragma link "dxSkinOffice2010Silver"
#pragma link "dxSkinOffice2013DarkGray"
#pragma link "dxSkinOffice2013LightGray"
#pragma link "dxSkinOffice2013White"
#pragma link "dxSkinOffice2016Colorful"
#pragma link "dxSkinOffice2016Dark"
#pragma link "dxSkinPumpkin"
#pragma link "dxSkinsDefaultPainters"
#pragma link "dxSkinSeven"
#pragma link "dxSkinSevenClassic"
#pragma link "dxSkinSharp"
#pragma link "dxSkinSharpPlus"
#pragma link "dxSkinSilver"
#pragma link "dxSkinSpringTime"
#pragma link "dxSkinStardust"
#pragma link "dxSkinSummer2008"
#pragma link "dxSkinTheAsphaltWorld"
#pragma link "dxSkinValentine"
#pragma link "dxSkinVisualStudio2013Blue"
#pragma link "dxSkinVisualStudio2013Dark"
#pragma link "dxSkinVisualStudio2013Light"
#pragma link "dxSkinVS2010"
#pragma link "dxSkinWhiteprint"
#pragma link "dxSkinXmas2008Blue"
#pragma link "cxRadioGroup"
#pragma link "pxCheckLabel"
#pragma resource "*.dfm"
TfrmLip *frmLip;
//---------------------------------------------------------------------------
__fastcall TfrmLip::TfrmLip(TComponent* Owner)
	: TForm(Owner)
{
//Ie settings
IEGlobalSettings()->AutoFragmentBitmap = false;
IEGlobalSettings()->MsgLanguage = msEnglish;
IEGlobalSettings()->EnableTheming = true;
// offscreen bitmaps
FOutputMap = 0;
FMaskMap = 0;
FUndoMap = 0;
FSelRect = 0;
FScriptLoaded = false;
FLuaScripts = new TStringList();
statusBar->Panels->Items[0]->Text = "";
luaMemo->Clear();
loadScriptList(IncludeTrailingBackslash(ExtractFilePath(Application->ExeName))+"lua\\scripts\\");
// initialize lip engine
lipInit();
}
//---------------------------------------------------------------------------
void __fastcall TfrmLip::FormDestroy(TObject *Sender)
{
//TODO: close lipEngine
TCommonHelper::DeleteIPList();
deleteOfsMaps();
if (FSelRect)
   delete FSelRect;
delete FLuaScripts;
}
//---------------------------------------------------------------------------
void __fastcall TfrmLip::FormShow(TObject *Sender)
{
ieView->Blank();
ieView->SelectionMaskDepth = 8;
// connect luaConsole with memo
TCommonHelper::luaConsole = luaMemo;
TCommonHelper::luaParamBuilder = luaParamBuilder;
}
//---------------------------------------------------------------------------
void __fastcall TfrmLip::itemOpenClick(TObject *Sender)
{
if (openDialog->Execute())
   {
   deleteOfsMaps();
   ieView->Zoom = 100;
   ieView->IO->LoadFromFile(openDialog->FileName);
   FOutputMap = new TIEBitmap();
   FUndoMap = new TIEBitmap();
   FOutputMap->AssignImage(ieView->IEBitmap);
   ieView->Update();
   // reload script if selected (reset paremeters)
   //listScriptsItemSelected(this, listScripts->SelectedItemIndex);
   }
}
//---------------------------------------------------------------------------
void __fastcall TfrmLip::itemSaveClick(TObject *Sender)
{
ieView->SaveToFileAll(openDialog->FileName);
}
//---------------------------------------------------------------------------

void __fastcall TfrmLip::itemSaveAsClick(TObject *Sender)
{
//
if (saveDialog->Execute())
   {
   ieView->SaveToFileAll(saveDialog->FileName);
   }
}
//---------------------------------------------------------------------------
void __fastcall TfrmLip::ItemSelectionClick(TObject *Sender)
{
TMenuItem *item = static_cast <TMenuItem*>(Sender);
item->Checked = !item->Checked;
ieView->MouseInteract = TIEMouseInteract();
ieView->Deselect();
if (!item->Checked)
   return;
switch (item->Tag)
	{
	case 0:
		 ieView->MouseInteract  = TIEMouseInteract()<<miSelect;
		 break;
	case 1:
		 ieView->MouseInteract  = TIEMouseInteract()<<miSelectCircle;
		 break;
	case 2:
		 ieView->MouseInteract  = TIEMouseInteract()<<miSelectLasso;
		 break;
	case 3:
		 ieView->MouseInteract  = TIEMouseInteract()<<miSelectPolygon;
		 break;
	case 4:
		 ieView->MouseInteract  = TIEMouseInteract()<<miSelectMagicWand;
		 break;
	default:
		 ;
	}
}
//---------------------------------------------------------------------------
void __fastcall TfrmLip::itemClearSelectionClick(TObject *Sender)
{
ieView->Deselect();
}
//---------------------------------------------------------------------------
void __fastcall TfrmLip::btnExecuteClick(TObject *Sender)
{
if (!FOutputMap)
   {
   statusBar->Panels->Items[0]->Text = "Error: No image loaded!";
   return;
   }
int selItem = listScripts->SelectedItemIndex;
if (selItem < 0)
   {
   statusBar->Panels->Items[0]->Text = "Error: No script selected!";
   return;
   }
// new undo map
FUndoMap->AssignImage(ieView->IEBitmap);
luaMemo->Clear();
luaMemo->Update();
luaMemo->Lines->Add("Running...");
luaMemo->Lines->Add("Processing...");
//luaConsole->Lines->Strings[0] = "Executing script...";
//luaConsole->Lines->Strings[1] = "Processing...";
// set helper var
TCommonHelper::luaStart = Time();
// create lua image structure
createLipSurface();
int result;
if (FScriptLoaded)
   {
   String params = setParamValues();
   result = lipExecuteScript(params.c_str());
   }
if (result)
  {
  luaMemo->Text = String(lipGetLastErrorMessage());
  statusBar->Panels->Items[0]->Text = "Error: script execution terminated!";
  }
else
  {
  statusBar->Panels->Items[0]->Text = "Done!";
  luaMemo->Lines->Add("Done!");
  //luaMemo->Lines->Strings[luaMemo->Lines->Count - 1] = "Done!";
  if (ieView->Selected)
	 {
	 ieView->SaveSelection();
	 bool tlpe = (ieView->SelectionMask->GetPixel(0, 0) == 0x00);
	 if (tlpe)
		ieView->SelectionMask->SetPixel(0, 0, 0x01);
	 FOutputMap->CopyWithMask2(ieView->IEBitmap, ieView->SelectionMask);
	 if (tlpe)
		ieView->SelectionMask->SetPixel(0, 0, 0x00);
	 ieView->RestoreSelection();
	 }
  else
     FOutputMap->DrawToTIEBitmap(ieView->IEBitmap, 0, 0);
  ieView->Update();
  }
}
//---------------------------------------------------------------------------
// undo - last change
void __fastcall TfrmLip::btnUndoClick(TObject *Sender)
{
if (FOutputMap)
   {
   FUndoMap->DrawToTIEBitmap(FOutputMap, 0, 0);
   FUndoMap->DrawToTIEBitmap(ieView->IEBitmap, 0, 0);
   ieView->Update();
   }
}
//---------------------------------------------------------------------------
void __fastcall TfrmLip::listScriptsItemSelected(TObject *Sender, int itemindex)
{
FScriptLoaded = false;
TCommonHelper::DeleteIPList();
if (itemindex < 0)
   return;
statusBar->Panels->Items[0]->Text = "Selected script: "+listScripts->Items->Items[itemindex]->Caption;
// set call-back functions
lipSetCbProgress(&(TCommonHelper::LuaProgress));    	// progress
lipSetCbMessage(&(TCommonHelper::LuaMessage));          // get message from lua
lipSetCbRequireParams(&(TCommonHelper::LuaRequire));    // lua require params
int result = lipLoadScript(FLuaScripts->Strings[itemindex].w_str());
if (result)
  {
  luaMemo->Text = String(lipGetLastErrorMessage());
  statusBar->Panels->Items[0]->Text = "Error: Script not loaded.";
  }
else
  {
  luaMemo->Clear();
  luaMemo->Lines->Add("Script loaded.");
  FScriptLoaded = true;
  }
}
//---------------------------------------------------------------------------
// private methods
// delete all offscreen maps
void __fastcall TfrmLip::deleteOfsMaps(void)
{
if (FOutputMap)
   delete FOutputMap;
if (FMaskMap)
   delete FMaskMap;
if (FUndoMap)
   delete FUndoMap;
FOutputMap = 0;
FMaskMap = 0;
FUndoMap = 0;
}
//---------------------------------------------------------------------------
// load script list
void __fastcall TfrmLip::loadScriptList(UnicodeString path)
{
// load file list
FLuaScripts->Clear();
listScripts->Items->Clear();
UnicodeString temp_str, ffil, full_str;
TSearchRec ffs;
//
ffil = path + "*.lua";
int done = FindFirst(ffil, 32, ffs);
while (done == 0)
   {
   full_str = ExtractFileDir(ffil) + "\\" + ffs.Name;
   if (ffs.Name.SubString(1, 1) == "_")
	  {
	  done = FindNext(ffs);
	  continue;
	  }
   FLuaScripts->Add(full_str);
   TAdvSmoothListBoxItem *item = listScripts->Items->Add();
   item->Caption = ChangeFileExt(ffs.Name, "");
   done = FindNext(ffs);
   }
FindClose(ffs);
}
//---------------------------------------------------------------------------
void __fastcall TfrmLip::createLipSurface(void)
{
// create and fill lua structure that will be passed to lua prolog function
FWidth  = ieView->IEBitmap->Width;
FHeight = ieView->IEBitmap->Height;
void *alpha = 0;
if (ieView->IEBitmap->HasAlphaChannel)
	alpha = ieView->AlphaChannel->Scanline[FHeight - 1];
lipSetInputImage(ieView->IEBitmap->ScanLine[FHeight -1 ], alpha, FWidth, FHeight, 4);
if (FOutputMap->HasAlphaChannel)
   alpha = FOutputMap->AlphaChannel->Scanline[FHeight - 1];
lipSetOutputImage(FOutputMap->ScanLine[FHeight -1 ], alpha, FWidth, FHeight, 4);
if (FSelRect)
   {
   delete FSelRect;
   FSelRect = 0;
   }
// set roi if selection exists
if (ieView->Selected)
   {
   FSelRect = new TRect;
   FSelRect->left   = ieView->SelectedRect.x;
   FSelRect->right  = ieView->SelectedRect.width + FSelRect->left;
   FSelRect->top    = ieView->SelectedRect.y;
   FSelRect->bottom = ieView->SelectedRect.height + FSelRect->top;
   lipSetRoi(FSelRect);
   }
}
//---------------------------------------------------------------------------
void __fastcall TfrmLip::luaParamBuilder(void)
{
//
if (TCommonHelper::luaInputParams->Count == 0)
   return;
for (int i = TCommonHelper::luaInputParams->Count -1; i >= 0 ; i--)
	{
	// create frame
	TCommonHelper::LuaTKRV *obj = static_cast<TCommonHelper::LuaTKRV*>(TCommonHelper::luaInputParams->Items[i]);
	if (obj->type == "int")
	   {
	   TFrameSlider *fs = new TFrameSlider(this);
	   fs->Name = "fs"+IntToStr(i);
	   fs->box->Caption = obj->key;
	   fs->slider->EditMode = 1; //teInt
	   fs->slider->MinInt = StrToInt(obj->range_low);
	   fs->slider->MaxInt = StrToInt(obj->range_high);
	   fs->slider->IntValue = StrToInt(obj->init_value);
	   fs->Parent = panParams;
	   fs->Align = alTop;
	   obj->frame = fs;
	   }
	else if (obj->type == "float")
	   {
	   TFrameSlider *fs = new TFrameSlider(this);
	   fs->Name = "fs"+IntToStr(i);
	   fs->box->Caption = obj->key;
	   fs->slider->EditMode = 0; //teFloat
	   fs->slider->MinFloat = StrToFloat(obj->range_low);
	   fs->slider->MaxFloat = StrToFloat(obj->range_high);
	   fs->slider->FloatValue = StrToFloat(obj->init_value);
	   fs->Parent = panParams;
	   fs->Align = alTop;
	   obj->frame = fs;
	   }
	else if (obj->type == "bool")
	   {
	   TFrameCheckBox *fc = new TFrameCheckBox(this);
	   fc->Name = "fc"+IntToStr(i);
	   fc->box->Caption = obj->key;
	   fc->chbox->Caption = "Enable "+obj->key;
	   fc->chbox->Checked = StrToBool(obj->init_value);
	   fc->Parent = panParams;
	   fc->Align = alTop;
	   obj->frame = fc;
	   }
	}
}
//---------------------------------------------------------------------------
String __fastcall TfrmLip::setParamValues(void)
{
if (!TCommonHelper::luaInputParams)
   return "";
if (TCommonHelper::luaInputParams->Count == 0)
   return "";
String luaParams = "";
for (int i = 0; i < TCommonHelper::luaInputParams->Count; i++)
	{
	TCommonHelper::LuaTKRV *obj = static_cast<TCommonHelper::LuaTKRV*>(TCommonHelper::luaInputParams->Items[i]);
	if (obj->type == "int")
	   {
	   TFrameSlider *fs = static_cast<TFrameSlider*>(obj->frame);
	   luaParams = luaParams + obj->key + "=" + IntToStr(fs->slider->IntValue)+ ";";
	   }
	else if (obj->type == "float")
	   {
	   TFrameSlider *fs = static_cast<TFrameSlider*>(obj->frame);
	   luaParams = luaParams + obj->key + "=" + FloatToStr(fs->slider->FloatValue)+ ";";
	   }
	}
return luaParams;
}
//---------------------------------------------------------------------------
void __fastcall TfrmLip::btnActionClick(TObject *Sender)
{
// 0 => reload script
// TODO: 1 => stop execution (move script execution into thread)
// for now, reload script
listScriptsItemSelected(this, listScripts->SelectedItemIndex);
}
//---------------------------------------------------------------------------

