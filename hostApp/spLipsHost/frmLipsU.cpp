//---------------------------------------------------------------------------
#include <vcl.h>
#pragma hdrstop
#include "frmLipsU.h"
#include "FrameSliderU.h"
#include "FrameCheckBoxU.h"
#include "FrameComboBoxU.h"
#include "FrameImageSliderU.h"
#include "FrameImageComboU.h"
#include "FrameListBoxU.h"
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
#pragma link "cxRadioGroup"
#pragma link "pxCheckLabel"
#pragma link "dxSkinBlack"
#pragma link "dxSkinCoffee"
#pragma link "dxSkinsDefaultPainters"
#pragma link "dxSkinSilver"
#pragma link "cxDropDownEdit"
#pragma link "cxMaskEdit"
#pragma link "cxShellComboBox"
#pragma link "cxShellCommon"
#pragma link "cxTextEdit"
#pragma link "iemview"
#pragma link "imageenio"
#pragma link "iexLayerMView"
#pragma resource "*.dfm"
TfrmLips *frmLips;
//---------------------------------------------------------------------------
__fastcall TfrmLips::TfrmLips(TComponent* Owner)
	: TForm(Owner)
{
Caption = Caption + " 2019.03. ";
#ifdef __WIN32__
	Caption = Caption + " 32-bit";
#else
    Caption = Caption + " 64-bit";
#endif
//Ie settings
IEGlobalSettings()->AutoFragmentBitmap = false;
IEGlobalSettings()->MsgLanguage = msEnglish;
IEGlobalSettings()->EnableTheming = true;
FImageList = new TList();
FSelRect = 0;
FScriptLoaded = false;
FLuaScripts = new TStringList();
labStatus->Caption = "";
// fill blender combo
for (int i = 0; i < 44; i++)
	comboBlend->Properties->Items->Add(lips_IEN_BLEND[i]);
comboBlend->ItemIndex = 0;
luaMemo->Clear();
loadScriptList(IncludeTrailingBackslash(ExtractFilePath(Application->ExeName))+"lua\\scripts\\");
//FTilePath = IncludeTrailingBackslash(ExtractFilePath(Application->ExeName))+"tiles\\");
comboShellTiles->Path = "D:\\petra\\pixopediaXE3264\\tiles\\";  // test
comboShellTextures->Path = "D:\\petra\\pixopediaXE3264\\textures\\";  // test
// initialize lips engine
lipsInit(IncludeTrailingBackslash(ExtractFilePath(Application->ExeName)).c_str());
}
//---------------------------------------------------------------------------
__fastcall TfrmLips::~TfrmLips()
{
ieView->LayersClear(false);
lipsClose();
TCommonHelper::DeleteIPList();
delete FLuaScripts;
FLuaScripts = 0;
deleteImages();
delete FImageList;
FImageList = 0;
if (FSelRect)
   delete FSelRect;
}
//---------------------------------------------------------------------------
void __fastcall TfrmLips::FormShow(TObject *Sender)
{
pageControl->ActivePageIndex = 0;
pageMVControl->ActivePageIndex = 0;
ieView->Blank();
ieView->SelectionMaskDepth = 8;
// connect luaConsole with memo field
TCommonHelper::luaConsole = luaMemo;
// connect paramBuilder - where to go on require params
TCommonHelper::luaParamBuilder = luaParamBuilder;
// connect inportImage - where to go when lua exports image
TCommonHelper::luaImportImage = luaImportImage;
}
//---------------------------------------------------------------------------
// load soruce image and copy it to target image
void __fastcall TfrmLips::itemOpenSTClick(TObject *Sender)
{
if (openDialog->Execute())
   {
   deleteImages();
   ieView->Zoom = 100;
   TImageEnIO *ienIO = new TImageEnIO(this);
   ienIO->LoadFromFile(openDialog->FileName);
   // add source image
   FImageList->Add(new TIEBitmap(ienIO->IEBitmap));    // source
   mView->InsertImage(0);
   mView->SetImage(0, ienIO->IEBitmap);
   // add source image
   FImageList->Add(new TIEBitmap(ienIO->IEBitmap));    // target = source copy
   mView->InsertImage(1);
   mView->SetImage(1, ienIO->IEBitmap);
   mView->ImageTopText[0] =  "source";
   mView->ImageTag[0] = -1;
   mView->ImageTopText[1] =  "target";
   mView->ImageTag[1] = -2;
   mView->SelectedImage = 0;
   mViewImageSelect(this, mView->SelectedImage);
   checkImageFrames();
   delete ienIO;
   }
}
//---------------------------------------------------------------------------
// load soruce and leave target alive
void __fastcall TfrmLips::itemOpenSClick(TObject *Sender)
{
if (openDialog->Execute())
   {
   deleteImages(2);
   ieView->Zoom = 100;
   TImageEnIO *ienIO = new TImageEnIO(this);
   ienIO->LoadFromFile(openDialog->FileName);
   // append or replace image
   if (mView->ImageCount == 0)
	  {
	  // add source image
	  FImageList->Add(new TIEBitmap(ienIO->IEBitmap));    		// source
	  mView->InsertImage(0);
	  mView->SetImage(0, ienIO->IEBitmap);
	  mView->ImageTopText[0] =  "source";
	  }
   else
	  {
	  TIEBitmap *oldMap = static_cast<TIEBitmap*>(FImageList->Items[0]);
      delete oldMap;
	  FImageList->Items[0] = new TIEBitmap(ienIO->IEBitmap);    // source
	  mView->SetImage(0, ienIO->IEBitmap);
	  }
   mView->SelectedImage = 0;
   mView->ImageTag[0] = -1;
   mViewImageSelect(this, mView->SelectedImage);
   checkImageFrames();
   delete ienIO;
   }
}
//---------------------------------------------------------------------------
// load target and leave source alive
void __fastcall TfrmLips::itemOpenTClick(TObject *Sender)
{
if (mView->ImageCount == 0)
   {
   // source image not loaded
   return;
   }
if (openDialog->Execute())
   {
   deleteImages(2);
   ieView->Zoom = 100;
   TImageEnIO *ienIO = new TImageEnIO(this);
   ienIO->LoadFromFile(openDialog->FileName);
   // append or replace image
   if (mView->ImageCount == 1)
	  {
	  // add target image
	  FImageList->Add(new TIEBitmap(ienIO->IEBitmap));    		// target
	  mView->InsertImage(1);
	  mView->SetImage(1, ienIO->IEBitmap);
	  mView->ImageTopText[1] =  "target";
	  }
   else
	  {
	  TIEBitmap *oldMap = static_cast<TIEBitmap*>(FImageList->Items[1]);
	  delete oldMap;
	  FImageList->Items[1] = new TIEBitmap(ienIO->IEBitmap);    // target
	  mView->SetImage(1, ienIO->IEBitmap);
	  }
   mView->SelectedImage = 1;
   mView->ImageTag[1] = -2;
   mViewImageSelect(this, mView->SelectedImage);
   delete ienIO;
   }
}
//---------------------------------------------------------------------------
// save surrent ieView image
void __fastcall TfrmLips::itemSaveClick(TObject *Sender)
{
if (mView->ImageCount == 0)
   {
   // no images
   return;
   }
if (saveDialog->Execute())
   ieView->IO->SaveToFile(saveDialog->FileName);
}
//---------------------------------------------------------------------------
// onselect mView event
void __fastcall TfrmLips::mViewImageSelect(TObject *Sender, int idx)
{
TIEBitmap *map = static_cast<TIEBitmap*>(FImageList->Items[idx]);
ieView->SetExternalBitmap(map);
ieView->Update();
}
//---------------------------------------------------------------------------
// onselect layersMView event
void __fastcall TfrmLips::layersMViewImageSelect(TObject *Sender, int idx)
{
editBlend->IntValue = ieView->Layers[idx]->Transparency;
comboBlend->ItemIndex = ieView->Layers[idx]->Operation;
panBlender->Enabled = (bool)idx;
}
//---------------------------------------------------------------------------
// onchange transparency
void __fastcall TfrmLips::editBlendValueChange(TObject *Sender)
{
ieView->Layers[ieView->LayersCurrent]->Transparency = editBlend->IntValue;
ieView->Update();
}
//---------------------------------------------------------------------------
// onchange blend mode
void __fastcall TfrmLips::comboBlendPropertiesChange(TObject *Sender)
{
//
ieView->Layers[ieView->LayersCurrent]->Operation = (TIERenderOperation)(comboBlend->ItemIndex);
ieView->Update();
}
//---------------------------------------------------------------------------
// select seletion type
void __fastcall TfrmLips::ItemSelectionClick(TObject *Sender)
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
// clear selection
void __fastcall TfrmLips::itemClearSelectionClick(TObject *Sender)
{
ieView->Deselect();
}
//---------------------------------------------------------------------------
// invert selection
void __fastcall TfrmLips::itemInvertSelectionClick(TObject *Sender)
{
ieView->InvertSelection();
}
//---------------------------------------------------------------------------
// execute sctipr
void __fastcall TfrmLips::btnExecuteClick(TObject *Sender)
{
if (mView->ImageCount == 0)
   {
   labStatus->Caption = "Error: Source image not loaded!";
   return;
   }
int selItem = listScripts->SelectedItemIndex;
if (selItem < 0)
   {
   labStatus->Caption = "Error: No script selected!";
   return;
   }
luaMemo->Clear();
luaMemo->Update();
luaMemo->Lines->Add("Running...");
luaMemo->Lines->Add("Processing...");
//luaConsole->Lines->Strings[0] = "Executing script...";
//luaConsole->Lines->Strings[1] = "Processing...";
// set helper var
TCommonHelper::luaStart = Time();
// target image (if exists)
TIEBitmap *mapTgt = 0;
if (FImageList->Count > 1)
   mapTgt = static_cast<TIEBitmap*>(FImageList->Items[1]);
// create lua image structure
createLipSurface();
int result;
if (FScriptLoaded)
  {
  String params = setParamValues();
  result = lipsExecuteScript(params.c_str());
  }
if (result)
  {
  luaMemo->Text = String(lipsGetLastErrorMessage());
  labStatus->Caption = "Error: script execution terminated!";
  }
else
  {
  labStatus->Caption = "Done!";
  String fmtt;
  DateTimeToString(fmtt, "hh::nn:ss.zzz", (Time() - TCommonHelper::luaStart));
  luaMemo->Lines->Strings[0] = "Running..." + fmtt;
  luaMemo->Lines->Add("Done!");
  // target image has been loaded (TODO: chheck if target image has been modified!!!)
  if (mapTgt && mView->ImageTag[1] == -2)
	 {
	 //luaMemo->Lines->Strings[luaMemo->Lines->Count - 1] = "Done!";
	 if (ieView->Selected)
		 {
		 ieView->MagicWandMaxFilter = chkSelMaxFilter->Checked;
		 ieView->MagicWandMode      = (TIEMagicWandMode)(rgpWandModes->ItemIndex);
		 ieView->MagicWandTolerance = editWandTol->IntValue;
		 ieView->SaveSelection();
		 if (chkSelRestore->Checked && editSelFeather->IntValue > 0)
			{
			ieView->SaveSelection();
			ieView->MakeSelectionFeather(editSelFeather->IntValue);
			}
		 bool tlpe = (ieView->SelectionMask->GetPixel(0, 0) == 0x00);
		 if (tlpe)
			ieView->SelectionMask->SetPixel(0, 0, 0x01);
		 FMapWork->CopyWithMask2(mapTgt, ieView->SelectionMask);
		 if (tlpe)
			ieView->SelectionMask->SetPixel(0, 0, 0x00);
		 if (chkSelRestore->Checked && editSelFeather->IntValue > 0)
			{
			ieView->MakeSelectionFeather(0);
			ieView->RestoreSelection();
			}
		 ieView->RestoreSelection();
		 }
	 else
		 FMapWork->DrawToTIEBitmap(mapTgt, 0, 0);
	 mView->SetImage(1, mapTgt);
	 mView->SelectedImage = mView->ImageCount - 1;  // go to last image
	 mViewImageSelect(this, mView->SelectedImage);
	 }
  ieView->Update();
  // always delete work map
  if (FMapWork)
	 {
	 delete FMapWork;
	 FMapWork = 0;
	 }
  }
}
//---------------------------------------------------------------------------
void __fastcall TfrmLips::btnReloadClick(TObject *Sender)
{
// 0 => reload script
// TODO: 1 => stop execution (move script execution into thread)
// for now, reload script
listScriptsItemSelected(this, listScripts->SelectedItemIndex);
}
//---------------------------------------------------------------------------
void __fastcall TfrmLips::btnToggleModeClick(TObject *Sender)
{
if (mView->ImageCount < 2)    // why to show only one image
   return;
// toggle view mode
if (pageMVControl->ActivePageIndex == 0)
   {
   // collect all images and show them as layers
   // select souce as bacground
   mView->SelectedImage = 0;  // go to source image
   // reset blending mode
   comboBlend->ItemIndex = 0;
   mViewImageSelect(this, mView->SelectedImage);
   for (int i = 1; i < FImageList->Count; i++)
	   {
	   TIEBitmap *map = static_cast<TIEBitmap*>(FImageList->Items[i]);
	   ieView->LayersAdd(map);
	   ieView->Layers[i]->PosX = 0;
	   ieView->Layers[i]->PosY = 0;
	   }
   layersMView->AttachedImageEnView = ieView;
   pageMVControl->ActivePageIndex = 1;  // switch to layers mode
   ieView->MouseInteract  = TIEMouseInteract()<<miMoveLayers;
   }
else
   {
   // delete all layers and switch to normal mode
   ieView->LayersClear(false);
   layersMView->AttachedImageEnView = 0;
   pageMVControl->ActivePageIndex = 0;  // switch to normal mode
   }
}
//---------------------------------------------------------------------------
// delete all images
void __fastcall TfrmLips::btnClearAllClick(TObject *Sender)
{
deleteImages();
ieView->Blank();
}
//---------------------------------------------------------------------------
// clear all imported images (images exported from script)
void __fastcall TfrmLips::btnClearAdditionalClick(TObject *Sender)
{
if (FImageList->Count < 2)
   return;
ieView->SetExternalBitmap(0);
for (int i = FImageList->Count - 1; i > 0; i--)
	{
	if (mView->ImageTag[i] < 0)
	   continue;
	TIEBitmap *map = static_cast<TIEBitmap*>(FImageList->Items[i]);
	delete map;
	FImageList->Delete(i);
	mView->DeleteImage(i);
	}
if (TCommonHelper::luaPushImage)
   {
   delete TCommonHelper::luaPushImage;
   TCommonHelper::luaPushImage = 0;
   }
mView->SelectedImage = 0;
mViewImageSelect(this, mView->SelectedImage);
}
//---------------------------------------------------------------------------
// copy selected image to source
void __fastcall TfrmLips::btnCopyClick(TObject *Sender)
{
if (FImageList->Count < 2)
   return;
TPoint pnt;
if (GetCursorPos(&pnt))
	popupCopy->Popup(pnt.X, pnt.Y);
/*
*/
}
//---------------------------------------------------------------------------
void __fastcall TfrmLips::itemCopyClick(TObject *Sender)
{
//
if (FImageList->Count < 2)
   return;
TPopupMenu *pop = static_cast<TPopupMenu*>(Sender);
int img_from;
int img_to;
switch (pop->Tag)
   {
   case 0:  // selected image to source
   case 10: // selected image <-> source
		if (mView->SelectedImage == 0)
			break;
		img_from = mView->SelectedImage;
		img_to = 0;
		break;
   case 1:  // selected image to target
   case 11: // selected image <-> target
		if (mView->SelectedImage == 1)
			break;
		img_from = mView->SelectedImage;
		img_to = 1;
		break;
   case 2:   // source to target
   case 12:  // source <-> target
		img_from = 0;
		img_to = 1;
		break;
   case 3:  // target to source
		img_from = 1;
		img_to = 0;
		break;
   case 4:
		{
		img_from = mView->SelectedImage;
		// add new additional image
		TIEBitmap *mapFrom = static_cast<TIEBitmap*>(FImageList->Items[img_from]);
		TIEBitmap *mapTo = new TIEBitmap(mapFrom);
		FImageList->Add(mapTo);
		int pos = mView->AppendImage();
		mView->SetImage(pos, mapTo);
		mView->SelectedImage = pos;
		mViewImageSelect(this, mView->SelectedImage);
		}
		return;
   default:
		return;
   }
ieView->SetExternalBitmap(0);
TIEBitmap *mapFrom = static_cast<TIEBitmap*>(FImageList->Items[img_from]);
TIEBitmap *mapTo   = static_cast<TIEBitmap*>(FImageList->Items[img_to]);
if (pop->Tag < 10)
   {
	delete mapTo;
	FImageList->Items[img_to] = new TIEBitmap(mapFrom);
	mView->SetImage(img_to, mapFrom);
	mView->SelectedImage = img_to;
	mViewImageSelect(this, mView->SelectedImage);
   }
else
   {
	FImageList->Items[img_to] = mapFrom;
	FImageList->Items[img_from] = mapTo;
	mView->SetImage(img_to, mapFrom);
	mView->SetImage(img_from, mapTo);
	mView->SelectedImage = img_to;
	mViewImageSelect(this, mView->SelectedImage);
   }
if (TCommonHelper::luaPushImage)
   {
   delete TCommonHelper::luaPushImage;
   TCommonHelper::luaPushImage = 0;
   }
}
//---------------------------------------------------------------------------
void __fastcall TfrmLips::btnLayersMergeClick(TObject *Sender)
{
// merge all layers to new image
if (MessageDlg("Do you want to merge all layers to new image?", mtConfirmation, TMsgDlgButtons() << mbOK<<mbCancel, 0) != mrOk)
   return;
// create new image
TIEBitmap *mapTo = new TIEBitmap();
ieView->LayersSaveMergedTo(mapTo);
FImageList->Add(mapTo);
int save_sel = mView->SelectedImage;
int pos = mView->AppendImage();
mView->SetImage(pos, mapTo);
mView->ImageTopText[pos] =  "merged";
mView->SelectedImage = save_sel;
MessageDlg("Layers merged, new image added. Toggle view mode to see the result.", mtInformation, TMsgDlgButtons() << mbOK, 0);
}
//---------------------------------------------------------------------------
// on script selected event
void __fastcall TfrmLips::listScriptsItemSelected(TObject *Sender, int itemindex)
{
// load script
FScriptLoaded = false;
TCommonHelper::DeleteIPList();
if (itemindex < 0)
   return;
labStatus->Caption = "Selected script: "+listScripts->Items->Items[itemindex]->Caption;
// set call-back functions
TCommonHelper::luaParamsOK = true;
lipsSetCbProgress(&(TCommonHelper::LuaProgress));    	 // progress
lipsSetCbMessage(&(TCommonHelper::LuaMessage));          // get message from lua
lipsSetCbRequireParams(&(TCommonHelper::LuaRequire));    // lua require params
lipsSetCbLoadImage(&(TCommonHelper::LuaLoadImage));      // load image using path
lipsSetCbExportImage(&(TCommonHelper::LuaExportImage));  // import exported script image
lipsSetCbHostDialog(&(TCommonHelper::LuaHostDialog));    // host dialog
int result = lipsLoadScript(FLuaScripts->Strings[itemindex].w_str());
if (result)
  {
  luaMemo->Text = String(lipsGetLastErrorMessage());
  labStatus->Caption = "Error: Script not loaded.";
  }
else
  {
  if (TCommonHelper::luaParamsOK)
	 {
	 luaMemo->Clear();
	 luaMemo->Lines->Add("Script loaded.");
	 FScriptLoaded = true;
	 }
  else
	 FScriptLoaded = false;
  }
}
//---------------------------------------------------------------------------
// private methods
// delete offscreen maps
void __fastcall TfrmLips::deleteImages(int upto)
{
ieView->SetExternalBitmap(0);
ieView->Blank();
// delete thumbnails
if (upto == 0)
   mView->Clear();
else
  {
  while (mView->ImageCount > upto)
	 mView->DeleteImage(mView->ImageCount - 1);
  }
while (FImageList->Count > upto)
   {
   TIEBitmap *map = static_cast<TIEBitmap*>(FImageList->Items[FImageList->Count - 1]);
   delete map;
   FImageList->Delete(FImageList->Count - 1);
   }
if (TCommonHelper::luaPushImage)
   {
   delete TCommonHelper::luaPushImage;
   TCommonHelper::luaPushImage = 0;
   }
}
//---------------------------------------------------------------------------
// check for table image frames (if script is already loaded)
void __fastcall TfrmLips::checkImageFrames(void)
{
if (TCommonHelper::luaInputParams)
  {
   for (int i = 0; i < TCommonHelper::luaInputParams->Count; i++)
	   {
	   TCommonHelper::LuaTKRV *obj = static_cast<TCommonHelper::LuaTKRV*>(TCommonHelper::luaInputParams->Items[i]);
	   if (obj->type == "imagemap")
		  {
		  TFrameImage *fi = static_cast<TFrameImage*>(obj->frame);
		  prepareTableImage(fi, obj);
		  }
	   }
  }
}
//---------------------------------------------------------------------------
// load script list
void __fastcall TfrmLips::loadScriptList(UnicodeString path)
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
   item->CaptionFont->Name = "Courier New";
   item->CaptionFont->Size = 9;
   item->CaptionSelectedFont->Name = "Courier New";
   item->CaptionSelectedFont->Size = 9;
   done = FindNext(ffs);
   }
FindClose(ffs);
}
//---------------------------------------------------------------------------
void __fastcall TfrmLips::createLipSurface(void)
{
// create and fill lua structure that will be passed to lua prolog function
int w, h;
void *alpha;
TIEBitmap* mapSrc, *mapTgt;
// source image
mapSrc = static_cast<TIEBitmap*>(FImageList->Items[0]);
w = mapSrc->Width;
h = mapSrc->Height;
alpha = 0;
if (mapSrc->HasAlphaChannel)
	alpha = mapSrc->AlphaChannel->Scanline[h - 1];
lipsSetSourceImage(mapSrc->ScanLine[h - 1], alpha, w, h, 4);
if (FImageList->Count > 1)
   {
   // target image
   mapTgt = static_cast<TIEBitmap*>(FImageList->Items[1]);
   // copy to temporary image
   FMapWork = new TIEBitmap(mapTgt);
   w = FMapWork->Width;
   h = FMapWork->Height;
   alpha = 0;
   if (FMapWork->HasAlphaChannel)
	   alpha = FMapWork->AlphaChannel->Scanline[h - 1];
   lipsSetTargetImage(FMapWork->ScanLine[h - 1], alpha, w, h, 4);
   }
else
   {
   FMapWork = 0;
   lipsSetTargetImage(0, 0, 0, 0, 4);
   }
if (FSelRect)
   {
   delete FSelRect;
   FSelRect = 0;
   }
// set source roi if selection exists
if (ieView->Selected)
   {
   FSelRect = new TRect;
   FSelRect->left   = ieView->SelectedRect.x;
   FSelRect->right  = ieView->SelectedRect.width + FSelRect->left;
   FSelRect->top    = ieView->SelectedRect.y;
   FSelRect->bottom = ieView->SelectedRect.height + FSelRect->top;
   lipsSetSourceRoi(FSelRect);   // for now, only source roi
   }
}
//---------------------------------------------------------------------------
void __fastcall TfrmLips::luaParamBuilder(void)
{
// called from TCommonHelper::LuaRequire
if (TCommonHelper::luaInputParams->Count == 0)
   return;
TFormatSettings fmt;
fmt.DecimalSeparator = '.';
scrollBox->Visible = false;
for (int i = TCommonHelper::luaInputParams->Count -1; i >= 0 ; i--)
	{
	// create frame
	TCommonHelper::LuaTKRV *obj = static_cast<TCommonHelper::LuaTKRV*>(TCommonHelper::luaInputParams->Items[i]);
	if (obj->type == "int")
	   {
	   TFrameSlider *fs = new TFrameSlider(this);
	   fs->Name = "fs"+IntToStr(i);
	   fs->box->Caption = obj->key;
	   fs->slider->EditMode = TpxSpinSlider::teInt; //teInt;
	   fs->slider->MinInt = StrToInt(obj->range_low);
	   fs->slider->MaxInt = StrToInt(obj->range_high);
	   fs->slider->IntValue = StrToInt(obj->init_value);
	   fs->Parent = scrollBox;//panParams;
	   fs->Align = alTop;
	   obj->frame = fs;
	   }
	else if (obj->type == "float")
	   {
	   TFrameSlider *fs = new TFrameSlider(this);
	   fs->Name = "fs"+IntToStr(i);
	   fs->box->Caption = obj->key;
	   fs->slider->EditMode = TpxSpinSlider::teFloat;
	   fs->slider->MinFloat = StrToFloat(obj->range_low, fmt);
	   fs->slider->MaxFloat = StrToFloat(obj->range_high, fmt);
	   fs->slider->FloatValue = StrToFloat(obj->init_value, fmt);
	   fs->Parent = scrollBox;//panParams;
	   fs->Align = alTop;
	   obj->frame = fs;
	   }
	else if (obj->type == "bool")
	   {
	   TFrameCheckBox *fc = new TFrameCheckBox(this);
	   fc->Name = "fc"+IntToStr(i);
	   fc->chbox->Caption = obj->key;
	   fc->chbox->Checked = StrToBool(obj->init_value);
	   fc->Parent = scrollBox; //panParams;
	   fc->Align = alTop;
	   obj->frame = fc;
	   }
	else if (obj->type == "combo")
	   {
	   TFrameComboBox *fc = new TFrameComboBox(this);
	   fc->Name = "fcb"+IntToStr(i);
	   fc->box->Caption = obj->key;
	   fc->combo->Items->Assign(obj->list_values);
	   fc->combo->ItemIndex = StrToInt(obj->init_value);
	   fc->Parent = scrollBox; //panParams;
	   fc->Align = alTop;
	   obj->frame = fc;
	   }
	else if (obj->type == "list")
	   {
	   TFrameListBox *fl = new TFrameListBox(this);
	   fl->Name = "flb"+IntToStr(i);
	   fl->box->Caption = obj->key;
	   fl->list->Items->Assign(obj->list_values);
	   fl->list->ItemIndex = StrToInt(obj->init_value);
	   fl->Parent = scrollBox; //panParams;
	   fl->Align = alTop;
	   obj->frame = fl;
	   }
	else if (obj->type == "imagefile")
	   {
	   String iPath;
	   if (obj->init_value == "#tiles")
		  iPath = comboShellTiles->Path;
	   else if (obj->init_value == "#textures")
		  iPath = comboShellTextures->Path;
	   else       // some other image directory
		  iPath = comboShellTiles->Path; // for now tiles
	   if (rgpAuxViewStyle->ItemIndex == 0)
		  {
		  TFrameImageSlider *fs = new TFrameImageSlider(this);
		  fs->Name = "fis"+IntToStr(i);
		  fs->box->Caption = obj->key;
		  int done, pos;
		  String temp_str, ffil, full_str;
		  TSearchRec ffs;
		  ffil = IncludeTrailingBackslash(iPath) + obj->range_low;
		  done = FindFirst(ffil, 32, ffs);
		  while (done == 0)
			 {
			 full_str = ExtractFileDir(ffil) + "\\" + ffs.Name;
			 fs->Add(full_str);
			 done = FindNext(ffs);
			 }
		  FindClose(ffs);
		  fs->Parent = scrollBox; //panParams;
		  fs->Align = alTop;
		  obj->frame = fs;
		  }
	   else
		  {
		  TFrameImageCombo *fc = new TFrameImageCombo(this);
		  fc->Name = "fic"+IntToStr(i);
		  fc->box->Caption = obj->key;
		  String ffil = IncludeTrailingBackslash(iPath);
		  fc->Add(ffil, obj->range_low);
		  fc->image->SelectedImageIndex = 0;
		  fc->imageSelectImage(this, 0);
		  fc->Parent = scrollBox; //panParams;
		  fc->Align = alTop;
		  obj->frame = fc;
		  }
	   }
	else if (obj->type == "imagemap")
	   {
	   TFrameImage *fi = new TFrameImage(this);
	   fi->Name = "fi"+IntToStr(i);
	   fi->box->Caption = obj->key;
	   prepareTableImage(fi, obj);
	   fi->Parent = scrollBox; //panParams;
	   fi->Align = alTop;
	   obj->frame = fi;
	   }
	}
scrollBox->Realign();
scrollBox->Visible = true;
}
//---------------------------------------------------------------------------
void __fastcall TfrmLips::luaImportImage(TExportImage *expImage, bool asIs)
{
String expType = String(expImage->exptype);
TIEBitmap *map;
if (expType == "asSource")
   {
   //not for now
   }
else if (expType == "asTarget") // new target image
   {
   map = new TIEBitmap(expImage->width, expImage->height, ie24RGB);
   TCommonHelper::CopyExpImage2Bitmap(expImage, map, asIs);
   ieView->Zoom = 100;
   // append or replace image
   if (mView->ImageCount == 1)
	  {
	  // add target image
	  FImageList->Add(map);    		// target
	  mView->InsertImage(1);
	  mView->SetImage(1, map);
	  mView->ImageTopText[1] =  "target";
	  }
   else
	  {
	  TIEBitmap *oldMap = static_cast<TIEBitmap*>(FImageList->Items[1]);
      delete oldMap;
	  FImageList->Items[1] = map;   // target
	  mView->SetImage(1, map);
	  }
   mView->SelectedImage = 1;
   mViewImageSelect(this, mView->SelectedImage);
   }
else
   {
   map = new TIEBitmap(expImage->width, expImage->height, ie24RGB);
   TCommonHelper::CopyExpImage2Bitmap(expImage, map, asIs);
   ieView->Zoom = 100;
   // append image
   FImageList->Add(map);    		// new image
   int idx = FImageList->Count - 1;
   mView->AppendImage();
   mView->SetImage(idx, map);
   mView->ImageTopText[idx] =  expType;
   mView->SelectedImage = idx;
   mViewImageSelect(this, mView->SelectedImage);
   }
}
//---------------------------------------------------------------------------
void __fastcall TfrmLips::prepareTableImage(TFrameImage*fi, TCommonHelper::LuaTKRV *obj)
{
if (obj->init_value == "#input")
   fi->ieView->IEBitmap->AssignImage(ieView->IEBitmap);
else if (obj->init_value == "#sobel")
   {
   fi->ieView->IEBitmap->AssignImage(ieView->IEBitmap);
   fi->ieView->Proc->EdgeDetect_Sobel();
   }
else
   fi->ieView->IEBitmap->AssignImage(ieView->IEBitmap);
fi->InputImage = ieView->IEBitmap;
fi->Type = obj->range_high;
if (obj->range_high == "grayscale")
   fi->ieView->Proc->ConvertToGray();
}
//---------------------------------------------------------------------------
void __fastcall TfrmLips::pushTableImage(TImageEnView *fiView, TCommonHelper::LuaTKRV *obj)
{
// prepare image and push it to lua table (tab.type, tab.ptr)
TIEBitmap *pi = new TIEBitmap();
pi->AssignImage(fiView->IEBitmap);
if (pi->PixelFormat !=  ie8g && obj->range_high == "grayscale")
   {
   TImageEnProc *proc = new TImageEnProc((TComponent*)0);
   proc->AttachedIEBitmap = pi;
   proc->ConvertTo(ie8g);
   proc->AttachedIEBitmap = 0;
   delete proc;
   }
lipsPushTableImage(obj->key.c_str(), obj->range_high.c_str(), pi->ScanLine[pi->Height -1 ], 0, pi->Width, pi->Height, 4);
delete pi;
}
//---------------------------------------------------------------------------
// set parameters to chosen values (push image if any)
String __fastcall TfrmLips::setParamValues(void)
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
	   luaParams = luaParams + obj->key + "=int(" + IntToStr(fs->slider->IntValue)+ ");";
	   }
	else if (obj->type == "float")
	   {
	   TFrameSlider *fs = static_cast<TFrameSlider*>(obj->frame);
	   luaParams = luaParams + obj->key + "=float(" + FloatToStr(fs->slider->FloatValue)+ ");";
	   }
	else if (obj->type == "bool")
	   {
	   TFrameCheckBox *fc = static_cast<TFrameCheckBox*>(obj->frame);
	   String lVal = fc->chbox->Checked ? "true" : "false";
	   luaParams = luaParams + obj->key + "=bool(" + lVal+");";
	   }
	else if (obj->type == "combo")
	   {
	   TFrameComboBox *fc = static_cast<TFrameComboBox*>(obj->frame);
	   luaParams = luaParams + obj->key + "=int(" + fc->combo->ItemIndex + ");";
	   }
	else if (obj->type == "list")
	   {
	   TFrameListBox *fl = static_cast<TFrameListBox*>(obj->frame);
	   luaParams = luaParams + obj->key + "=int(" + fl->list->ItemIndex + ");";
	   }
	else if (obj->type == "imagefile")
	   {
	   if (rgpAuxViewStyle->ItemIndex == 0)
		  {
		  TFrameImageSlider *fs = static_cast<TFrameImageSlider*>(obj->frame);
		  luaParams = luaParams + obj->key + "=string(" + fs->SelectedImage() + ");";
		  }
	   else
		  {
		  TFrameImageCombo *fc = static_cast<TFrameImageCombo*>(obj->frame);
		  luaParams = luaParams + obj->key + "=string(" + fc->File + ");";
		  }
	   }
	else if (obj->type == "imagemap")
	   {
	   // there is no return params: push table image
	   TFrameImage *fi = static_cast<TFrameImage*>(obj->frame);
	   pushTableImage(fi->ieView, obj);
	   }
	}
return luaParams;
}
//---------------------------------------------------------------------------
void __fastcall TfrmLips::rgpAuxViewStylePropertiesChange(TObject *Sender)
{
listScriptsItemSelected(this, listScripts->SelectedItemIndex);
}
//---------------------------------------------------------------------------

