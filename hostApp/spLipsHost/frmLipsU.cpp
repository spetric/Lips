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
#include "FrameShellComboBoxU.h"
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
//
#pragma comment(lib,"lipsEngine")
TfrmLips *frmLips;
//---------------------------------------------------------------------------
__fastcall TfrmLips::TfrmLips(TComponent* Owner)
	: TForm(Owner)
{
Caption = Caption + " 2020.07. ";
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
FSourceMask = 0;
FTargetMask = 0;
FSelectionRoiEnabled = true;
FScriptLoaded = false;
FMouseEvent2Lua = false;
FLuaScripts = new TStringList();
labStatus->Caption = "";
labInfo->Caption = "";
// fill blender combo
for (int i = 0; i < 44; i++)
	comboBlend->Properties->Items->Add(lips_IEN_BLEND[i]);
comboBlend->ItemIndex = 0;
luaMemo->Clear();
String epath = IncludeTrailingBackslash(ExtractFilePath(Application->ExeName));
loadScriptList(epath+"lua\\scripts\\");
#ifdef _DEBUG
  comboShellTiles->Path = "D:\\petra\\pixopediaXE3264\\tiles\\";  // test
  comboShellTextures->Path = "D:\\petra\\pixopediaXE3264\\textures\\";  // test
#else
  comboShellTiles->Path = epath + "images\\tiles\\";
  comboShellTextures->Path = epath + "images\\textures\\";
#endif
// initialize lips engine
lipsInit(IncludeTrailingBackslash(ExtractFilePath(Application->ExeName)).c_str());
}
//---------------------------------------------------------------------------
__fastcall TfrmLips::~TfrmLips()
{
//ieViewSingle->LayersClear(false);
ieViewLayers->LayersClear(false);
lipsClose();
TLipsHelper::DeleteIPList();
delete FLuaScripts;
FLuaScripts = 0;
deleteImages();
delete FImageList;
FImageList = 0;
if (FSelRect)
   delete FSelRect;
if (FSourceMask)
   delete FSourceMask;
if (FTargetMask)
   delete FTargetMask;
if (FSelection.Image)
   delete FSelection.Image;
if (FSelection.Mask)
   delete FSelection.Mask;
}
//---------------------------------------------------------------------------
void __fastcall TfrmLips::FormShow(TObject *Sender)
{
pageControl->ActivePageIndex = 0;
pageMVControl->ActivePageIndex = 0;
ieViewSingle->Blank();
ieViewSingle->SelectionMaskDepth = 8;
ieViewLayers->Blank();
ieViewLayers->SelectionMaskDepth = 8;
layersMView->AttachedImageEnView = ieViewLayers;
// connect luaConsole with memo field
TLipsHelper::luaConsole = luaMemo;
// connect paramBuilder - where to go on require params
TLipsHelper::luaParamBuilder = luaParamBuilder;
// connect paramRefresh - where to go on refresh params
TLipsHelper::luaParamRefresh = luaParamRefresh;
// connect inportImage - where to go when lua exports image
TLipsHelper::luaImportImage = luaImportImage;
// connect importRawData
TLipsHelper::luaImportRawData = luaImportRawData;
// connect layers view
TLipsHelper::luaHostCommand = luaCommand;
// connect image list
TLipsHelper::luaImageList = FImageList;
}
//---------------------------------------------------------------------------
// load soruce image and copy it to target image
void __fastcall TfrmLips::itemOpenSTClick(TObject *Sender)
{
if (openDialog->Execute())
   {
   deleteImages();
   TImageContainer *ic;
   ieViewSingle->Zoom = 100;
   TImageEnIO *ienIO = new TImageEnIO(this);
   ienIO->LoadFromFile(openDialog->FileName);
   // add source image
   ic = new TImageContainer(new TIEBitmap(ienIO->IEBitmap), "source", -1);
   FImageList->Add(ic);    // source
   mView->InsertImage(0);
   setIcmv(0, ic);
   // add target image
   ic = new TImageContainer(new TIEBitmap(ienIO->IEBitmap), "target", -2);
   FImageList->Add(ic);    // target = source copy
   mView->InsertImage(1);
   setIcmv(1, ic);
   mView->SelectedImage = 0;
   mViewImageSelect(this, mView->SelectedImage);
   checkImageFrames();
   delete ienIO;
   resetMouseEvents();
   }
}
//---------------------------------------------------------------------------
// load soruce and leave target alive
void __fastcall TfrmLips::itemOpenSClick(TObject *Sender)
{
if (openDialog->Execute())
   {
   deleteImages(2);
   TImageContainer *ic;
   ieViewSingle->Zoom = 100;
   TImageEnIO *ienIO = new TImageEnIO(this);
   ienIO->LoadFromFile(openDialog->FileName);
   // append or replace image
   if (mView->ImageCount == 0)
	  {
	  // add source image
	  ic = new TImageContainer(new TIEBitmap(ienIO->IEBitmap), "source", -1);
	  FImageList->Add(ic);    		// source
	  mView->InsertImage(0);
	  }
   else
	  {
	  ic = static_cast<TImageContainer*>(FImageList->Items[0]);
	  delete ic;
	  ic = new TImageContainer(new TIEBitmap(ienIO->IEBitmap), "source", -1);
	  FImageList->Items[0] = ic;    // source
	  }
   setIcmv(0, ic);
   mView->SelectedImage = 0;
   mViewImageSelect(this, mView->SelectedImage);
   checkImageFrames();
   delete ienIO;
   resetMouseEvents();
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
   TImageContainer *ic;
   ieViewSingle->Zoom = 100;
   TImageEnIO *ienIO = new TImageEnIO(this);
   ienIO->LoadFromFile(openDialog->FileName);
   // append or replace image
   if (mView->ImageCount == 1)
	  {
	  // add target image
	  ic = new TImageContainer(new TIEBitmap(ienIO->IEBitmap), "target", -2);
	  FImageList->Add(ic);    		// target
	  mView->InsertImage(1);
	  }
   else
	  {
	  ic = static_cast<TImageContainer*>(FImageList->Items[1]);
	  delete ic;
	  ic = new TImageContainer(new TIEBitmap(ienIO->IEBitmap), "target", -2);
	  FImageList->Items[1] = ic;    // target
	  }
   setIcmv(1, ic);
   mView->SelectedImage = 1;
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
   ieViewSingle->IO->SaveToFile(saveDialog->FileName);
}
//---------------------------------------------------------------------------
// onselect mView event
void __fastcall TfrmLips::mViewImageSelect(TObject *Sender, int idx)
{
TIEBitmap *map = getMapFromTag(mView->ImageTag[idx]);
if (map)
   {
   ieViewSingle->SetExternalBitmap(map);
   ieViewSingle->Update();
   }
}
//---------------------------------------------------------------------------
// onselect layersMView event
void __fastcall TfrmLips::layersMViewImageSelect(TObject *Sender, int idx)
{
editBlend->IntValue = ieViewLayers->Layers[idx]->Transparency;
comboBlend->ItemIndex = ieViewLayers->Layers[idx]->Operation;
panBlender->Enabled = (bool)idx;
}
//---------------------------------------------------------------------------
// onchange transparency
void __fastcall TfrmLips::editBlendValueChange(TObject *Sender)
{
ieViewLayers->Layers[ieViewLayers->LayersCurrent]->Transparency = editBlend->IntValue;
ieViewLayers->Update();
}
//---------------------------------------------------------------------------
// onchange blend mode
void __fastcall TfrmLips::comboBlendPropertiesChange(TObject *Sender)
{
//
if (chkLayersSB->Checked)
   {
   for (int i = 1; i < ieViewLayers->LayersCount; i++)
	   ieViewLayers->Layers[i]->Operation = (TIERenderOperation)(comboBlend->ItemIndex);
   }
else
   ieViewLayers->Layers[ieViewLayers->LayersCurrent]->Operation = (TIERenderOperation)(comboBlend->ItemIndex);
ieViewLayers->Update();
}
//---------------------------------------------------------------------------
// select seletion type
void __fastcall TfrmLips::ItemSelectionClick(TObject *Sender)
{
TMenuItem *item = static_cast <TMenuItem*>(Sender);
item->Checked = !item->Checked;
ieViewSingle->MouseInteract = TIEMouseInteract();
ieViewSingle->Deselect();
if (!item->Checked)
   return;
switch (item->Tag)
	{
	case 0:
		 ieViewSingle->MouseInteract  = TIEMouseInteract()<<miSelect;
		 break;
	case 1:
		 ieViewSingle->MouseInteract  = TIEMouseInteract()<<miSelectCircle;
		 break;
	case 2:
		 ieViewSingle->MouseInteract  = TIEMouseInteract()<<miSelectLasso;
		 break;
	case 3:
		 ieViewSingle->MouseInteract  = TIEMouseInteract()<<miSelectPolygon;
		 break;
	case 4:
		 ieViewSingle->MouseInteract  = TIEMouseInteract()<<miSelectMagicWand;
		 break;
	default:
		 ;
	}
}
//---------------------------------------------------------------------------
// clear selection
void __fastcall TfrmLips::itemClearSelectionClick(TObject *Sender)
{
ieViewSingle->Deselect();
}
//---------------------------------------------------------------------------
// invert selection
void __fastcall TfrmLips::itemInvertSelectionClick(TObject *Sender)
{
ieViewSingle->InvertSelection();
}
//---------------------------------------------------------------------------
void __fastcall TfrmLips::itemSelected2BufferClick(TObject *Sender)
{
TRect sr;
TIEBitmap *mapSelection;
if (FSelection.Image)
   delete FSelection.Image;
if (FSelection.Mask)
   delete FSelection.Mask;
FSelection.Image = 0;
FSelection.Mask = 0;
if (ieViewSingle->Selected)
   {
   sr.left   = ieViewSingle->SelectedRect.x;
   sr.right  = ieViewSingle->SelectedRect.width + sr.left;
   sr.top    = ieViewSingle->SelectedRect.y;
   sr.bottom = ieViewSingle->SelectedRect.height + sr.top;
   // save roi
   FSelection.Roi = sr;
   // selection image
   mapSelection = new TIEBitmap(sr.Width(), sr.Height(), ie24RGB);
   ieViewSingle->IEBitmap->CopyRectTo(mapSelection,  sr.left,  sr.top, 0, 0, mapSelection->Width, mapSelection->Height);
   // save selection image
   FSelection.Image = mapSelection;
   // selection mask
   TIEBitmap *mapMask = new TIEBitmap();
   mapMask->CopyFromTIEMask(ieViewSingle->SelectionMask);
   mapSelection = new TIEBitmap(sr.Width(), sr.Height(), ie8g);
   mapMask->CopyRectTo(mapSelection,  sr.left,  sr.top, 0, 0, mapSelection->Width, mapSelection->Height);
   delete mapMask;
   // save selection mask
   FSelection.Mask = mapSelection;
   }
}
//---------------------------------------------------------------------------
// execute sctipr
void __fastcall TfrmLips::btnExecuteClick(TObject *Sender)
{
FSelectionRoiEnabled = true;
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
TLipsHelper::luaStart = Time();
// target image (if exists)
TImageContainer *icTgt = 0;
FExportedAsTarget = false;
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
  if (FMouseEvent2Lua)
     return;
  if (FImageList->Count > 1)
	 icTgt = static_cast<TImageContainer*>(FImageList->Items[1]);
  labStatus->Caption = "Done!";
  String fmtt;
  DateTimeToString(fmtt, "hh::nn:ss.zzz", (Time() - TLipsHelper::luaStart));
  luaMemo->Lines->Strings[0] = "Running..." + fmtt;
  luaMemo->Lines->Add("Done!");
  if (FExportedAsTarget)
	 {
	 setIcmv(1, icTgt);
	 mViewImageSelect(this, mView->SelectedImage);
	 }
  else
	 {
	  // target image has been loaded (TODO: chheck if target image has been modified!!!)
	  //if (icTgt->Image && icTgt->Tag == -2)
	  if (icTgt)
		 {
		 //luaMemo->Lines->Strings[luaMemo->Lines->Count - 1] = "Done!";
		 if (ieViewSingle->Selected && FSelectionRoiEnabled)
			 {
			 ieViewSingle->MagicWandMaxFilter = chkSelMaxFilter->Checked;
			 ieViewSingle->MagicWandMode      = (TIEMagicWandMode)(rgpWandModes->ItemIndex);
			 ieViewSingle->MagicWandTolerance = editWandTol->IntValue;
			 ieViewSingle->SaveSelection();
			 if (chkSelRestore->Checked && editSelFeather->IntValue > 0)
				{
				ieViewSingle->SaveSelection();
				ieViewSingle->MakeSelectionFeather(editSelFeather->IntValue);
				}
			 bool tlpe = (ieViewSingle->SelectionMask->GetPixel(0, 0) == 0x00);
			 if (tlpe)
				ieViewSingle->SelectionMask->SetPixel(0, 0, 0x01);
			 FMapWork->CopyWithMask2(icTgt->Image, ieViewSingle->SelectionMask);
			 if (tlpe)
				ieViewSingle->SelectionMask->SetPixel(0, 0, 0x00);
			 if (chkSelRestore->Checked && editSelFeather->IntValue > 0)
				{
				ieViewSingle->MakeSelectionFeather(0);
				ieViewSingle->RestoreSelection();
				}
			 ieViewSingle->RestoreSelection();
			 }
		 else
			 FMapWork->DrawToTIEBitmap(icTgt->Image, 0, 0);
		 setIcmv(1, icTgt);
		 mView->SelectedImage = mView->ImageCount - 1;  // go to last image
		 mViewImageSelect(this, mView->SelectedImage);
		 }
	 }
  ieViewSingle->Update();
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
// delete all images
void __fastcall TfrmLips::btnClearAllClick(TObject *Sender)
{
deleteImages();
ieViewSingle->Blank();
layersMView->AttachedImageEnView = 0;
ieViewLayers->LayersClear(true);
ieViewLayers->Blank();
comboBlend->ItemIndex = 0;
editBlend->IntValue = 255;
layersMView->AttachedImageEnView = ieViewLayers;
resetMouseEvents();
}
//---------------------------------------------------------------------------
// clear all imported images (images exported from script) do not clear layers
void __fastcall TfrmLips::btnClearAdditionalClick(TObject *Sender)
{
if (FImageList->Count < 2)
   return;
if (ieViewLayers->LayersCount > 1)
   {
   if (MessageDlg("Do you want to clear layers?", mtConfirmation, TMsgDlgButtons() << mbYes<<mbNo, 0) == mrYes)
	  {
	  ieViewLayers->LayersClear(true);
	  ieViewLayers->Blank();
	  }
   }
ieViewSingle->SetExternalBitmap(0);
for (int i = FImageList->Count - 1; i > 0; i--)
	{
	TImageContainer *ic = static_cast<TImageContainer*>(FImageList->Items[i]);
	if (ic->Tag < 0)
	   continue;
	delete ic;
    ic = 0;
	FImageList->Delete(i);
	if (i < mView->ImageCount)
	   mView->DeleteImage(i);
	}
if (TLipsHelper::luaPushImage)
   {
   delete TLipsHelper::luaPushImage;
   TLipsHelper::luaPushImage = 0;
   }
mView->SelectedImage = 0;
mViewImageSelect(this, mView->SelectedImage);
comboBlend->ItemIndex = 0;
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
		TImageContainer *icFrom = static_cast<TImageContainer*>(FImageList->Items[img_from]);
		TImageContainer *icTo = new TImageContainer(new TIEBitmap(icFrom->Image), icFrom->Name);
		FImageList->Add(icTo);
		int pos = mView->AppendImage();
		setIcmv(pos, icTo);
		mView->SelectedImage = pos;
		mViewImageSelect(this, mView->SelectedImage);
		}
		return;
   default:
		return;
   }
ieViewSingle->SetExternalBitmap(0);
TImageContainer *icFrom = static_cast<TImageContainer*>(FImageList->Items[img_from]);
TImageContainer *icTo   = static_cast<TImageContainer*>(FImageList->Items[img_to]);
if (pop->Tag < 10)
   {
	delete icTo;
	icTo = new TImageContainer(new TIEBitmap(icFrom->Image), icFrom->Name);
	FImageList->Items[img_to] = icTo;
	setIcmv(img_to, icTo);
	mView->SelectedImage = img_to;
	mViewImageSelect(this, mView->SelectedImage);
   }
else
   {
	FImageList->Items[img_to] = icFrom;
	FImageList->Items[img_from] = icTo;
	setIcmv(img_to, icFrom);
	setIcmv(img_from, icTo);
	mView->SelectedImage = img_to;
	mViewImageSelect(this, mView->SelectedImage);
   }
if (TLipsHelper::luaPushImage)
   {
   delete TLipsHelper::luaPushImage;
   TLipsHelper::luaPushImage = 0;
   }
}
//---------------------------------------------------------------------------
void __fastcall TfrmLips::btnLayersMergeClick(TObject *Sender)
{
// merge all layers to new image
if (MessageDlg("Do you want to merge all layers to new image?", mtConfirmation, TMsgDlgButtons() << mbOK<<mbCancel, 0) != mrOk)
   return;
// create new image
TImageContainer *icTo = new TImageContainer(new TIEBitmap(), "merged");
ieViewLayers->LayersSaveMergedTo(icTo->Image);
FImageList->Add(icTo->Image);
int save_sel = mView->SelectedImage;
int pos = mView->AppendImage();
setIcmv(pos, icTo);
mView->SelectedImage = save_sel;
MessageDlg("Layers merged, new image added. Go to single view to see the result.", mtInformation, TMsgDlgButtons() << mbOK, 0);
}
//---------------------------------------------------------------------------
// on script selected event
void __fastcall TfrmLips::listScriptsItemSelected(TObject *Sender, int itemindex)
{
// load script
FScriptLoaded = false;
TLipsHelper::DeleteIPList();
resetMouseEvents();
if (itemindex < 0)
   return;
labStatus->Caption = "Selected script: "+listScripts->Items->Items[itemindex]->Caption;
// set call-back functions
TLipsHelper::luaParamsOK = true;
lipsSetCbProgress(&(TLipsHelper::LuaProgress));    	   // progress
lipsSetCbMessage(&(TLipsHelper::LuaMessage));          // get message from lua
lipsSetCbRequireParams(&(TLipsHelper::LuaRequire));    // lua require params
lipsSetCbRefreshParams(&(TLipsHelper::LuaRefresh));    // lua require params
lipsSetCbLoadImage(&(TLipsHelper::LuaLoadImage));      // load image using path
lipsSetCbExportImage(&(TLipsHelper::LuaExportImage));  // import exported script image
lipsSetCbHostDialog(&(TLipsHelper::LuaHostDialog));    // host dialog
lipsSetCbCommand(&TLipsHelper::LuaCommand);            // send command from lua
lipsSetCbExportRawData(&(TLipsHelper::LuaExportRawData));  // import exported script image
int result = lipsLoadScript(FLuaScripts->Strings[itemindex].w_str());
if (result)
  {
  luaMemo->Text = String(lipsGetLastErrorMessage());
  labStatus->Caption = "Error: Script not loaded.";
  }
else
  {
  if (TLipsHelper::luaParamsOK)
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
ieViewSingle->SetExternalBitmap(0);
ieViewSingle->Blank();
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
   TImageContainer *ic = static_cast<TImageContainer*>(FImageList->Items[FImageList->Count - 1]);
   delete ic;
   FImageList->Delete(FImageList->Count - 1);
   }
if (TLipsHelper::luaPushImage)
   {
   delete TLipsHelper::luaPushImage;
   TLipsHelper::luaPushImage = 0;
   }
}
//---------------------------------------------------------------------------
// set ic image to mview
void __fastcall TfrmLips::setIcmv(int idx, TImageContainer *ic)
{
mView->SetImage(idx, ic->Image);
mView->ImageTag[idx] = ic->AddrTag;
mView->ImageTopText[idx] =  ic->Name;
}
//---------------------------------------------------------------------------
// get map from ic set ic image to mview
TIEBitmap* __fastcall TfrmLips::getMapFromTag(int tag)
{
for (int i = 0; i < FImageList->Count; i++)
	{
	TImageContainer *ic = static_cast<TImageContainer*>(FImageList->Items[i]);
	if (tag == ic->AddrTag)
	   return ic->Image;
	}
return 0;
}
//---------------------------------------------------------------------------
// check for table image frames (if script is already loaded)
void __fastcall TfrmLips::checkImageFrames(void)
{
if (TLipsHelper::luaInputParams)
  {
   for (int i = 0; i < TLipsHelper::luaInputParams->Count; i++)
	   {
	   TLipsHelper::LuaTKRV *obj = static_cast<TLipsHelper::LuaTKRV*>(TLipsHelper::luaInputParams->Items[i]);
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
UnicodeString ffil, full_str;
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
TImageContainer* icSrc, *icTgt;
// source image
icSrc = static_cast<TImageContainer*>(FImageList->Items[0]);
w = icSrc->Image->Width;
h = icSrc->Image->Height;
alpha = 0;
if (icSrc->Image->HasAlphaChannel)
	alpha = icSrc->Image->AlphaChannel->Scanline[h - 1];
lipsSetSourceImage(icSrc->Image->ScanLine[h - 1], alpha, w, h, 4);
if (FImageList->Count > 1)
   {
   // target image
   icTgt = static_cast<TImageContainer*>(FImageList->Items[1]);
   // copy to temporary image
   FMapWork = new TIEBitmap(icTgt->Image);
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
// reset source masks
if (FSourceMask)
   {
   delete FSourceMask;
   FSourceMask = 0;
   lipsSetSourceMask(0, 0, 0, 4);
   }
// reset target masks
if (FTargetMask)
   {
   delete FTargetMask;
   FTargetMask = 0;
   lipsSetTargetMask(0, 0, 0, 4);
   }
// set source roi if selection exists
if (ieViewSingle->Selected)
   {
   FSelRect = new TRect;
   FSelRect->left   = ieViewSingle->SelectedRect.x;
   FSelRect->right  = ieViewSingle->SelectedRect.width + FSelRect->left;
   FSelRect->top    = ieViewSingle->SelectedRect.y;
   FSelRect->bottom = ieViewSingle->SelectedRect.height + FSelRect->top;
   lipsSetSourceRoi(FSelRect);   // set source roi
   }
}
//---------------------------------------------------------------------------
void __fastcall TfrmLips::luaParamBuilder(void)
{
// called from TLipsHelper::LuaRequire
if (TLipsHelper::luaInputParams->Count == 0)
   return;
TFormatSettings fmt;
fmt.DecimalSeparator = '.';
scrollBox->Visible = false;
for (int i = TLipsHelper::luaInputParams->Count -1; i >= 0 ; i--)
	{
	// create frame
	TLipsHelper::LuaTKRV *obj = static_cast<TLipsHelper::LuaTKRV*>(TLipsHelper::luaInputParams->Items[i]);
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
	   if (obj->init_value.SubString(1, 1) == "#")  // hashtag - use predefined directories
		  {
		  if (obj->init_value == "#tiles")
			 iPath = comboShellTiles->Path;
		  else if (obj->init_value == "#textures")
			 iPath = comboShellTextures->Path;
		  else       // some other image directory
			 iPath = comboShellTiles->Path; // for now tiles
		  }
	   else
		  iPath = obj->init_value;  // lua supplied directory
	   if (rgpAuxViewStyle->ItemIndex == 0)
		  {
		  TFrameImageSlider *fs = new TFrameImageSlider(this);
		  fs->Name = "fis"+IntToStr(i);
		  fs->box->Caption = obj->key;
		  int done, pos;
		  String ffil, full_str;
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
		  if (!iPath.IsEmpty())
			 {
			 fc->image->SelectedImageIndex = 0;
			 fc->imageSelectImage(this, 0);
			 }
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
	else if (obj->type == "file")
	   {
	   if (rgpFileViewStyle->ItemIndex == 0)
		  {
		   TFrameListBox *fl = new TFrameListBox(this);
		   fl->Name = "flb"+IntToStr(i);
		   fl->box->Caption = obj->key;
		   int done, pos, iWid = 0;
		   String ffil, full_str;
		   TSearchRec ffs;
		   ffil = IncludeTrailingBackslash(obj->init_value) + obj->range_low;
		   done = FindFirst(ffil, 32, ffs);
		   while (done == 0)
			 {
			 full_str = ExtractFileDir(ffil) + "\\" + ffs.Name;
			 if (fl->list->Canvas->TextWidth(full_str + "W") > iWid)
				iWid = fl->list->Canvas->TextWidth(full_str + "W");
			 fl->list->Items->Add(full_str);
			 done = FindNext(ffs);
			 }
		   FindClose(ffs);
		   fl->list->ItemIndex = 0;
		   fl->Parent = scrollBox; //panParams;
		   fl->Align = alTop;
		   fl->list->ScrollWidth = iWid;
		   obj->frame = fl;
		  }
	   else
		  {
		   TFrameShellComboBox *fsc = new TFrameShellComboBox(this);
		   fsc->Name = "fsc"+IntToStr(i);
		   fsc->box->Caption = obj->key;
		   fsc->comboShell->Path = obj->init_value;
		   fsc->comboShell->Properties->FileMask = obj->range_low;
		   fsc->Parent = scrollBox; //panParams;
		   fsc->Align = alTop;
		   obj->frame = fsc;
		  }
	   }
	}
scrollBox->Realign();
scrollBox->Visible = true;
}
//---------------------------------------------------------------------------
void __fastcall TfrmLips::luaParamRefresh(void)
{
int result;
if (FScriptLoaded)
  {
  String params = setParamValues();
  result = lipsRefreshParams(params.c_str());
  }
if (result)
  {
  luaMemo->Text = String(lipsGetLastErrorMessage());
  labStatus->Caption = "Error: refresh params failed!";
  }
}
//---------------------------------------------------------------------------
void __fastcall TfrmLips::luaImportImage(TExportImage *expImage, bool asIs, bool show)
{
String expType = String(expImage->exptype);
TIEBitmap *map;
TImageContainer *ic;
if (expType == "asSource")
   {
   map = new TIEBitmap(expImage->width, expImage->height, ie24RGB);
   TLipsHelper::CopyExpImage2Bitmap(expImage, map, asIs);
   ieViewSingle->Zoom = 100;
   ic = static_cast<TImageContainer*>(FImageList->Items[1]);
   delete ic;
   ic = new TImageContainer(map, "source", -1);   // replace source
   FImageList->Items[0] = ic;
   setIcmv(0, ic);
   }
else if (expType == "asTarget") // new target image
   {
   map = new TIEBitmap(expImage->width, expImage->height, ie24RGB);
   TLipsHelper::CopyExpImage2Bitmap(expImage, map, asIs);
   ieViewSingle->Zoom = 100;
   // append or replace image
   if (mView->ImageCount == 1)
	  {
	  // add target image
	  ic = new TImageContainer(map, "target", -2);
	  FImageList->Add(ic);    		// target
	  mView->InsertImage(1);
	  setIcmv(1, ic);
	  }
   else
	  {
	  ic = static_cast<TImageContainer*>(FImageList->Items[1]);
	  delete ic;
	  ic = new TImageContainer(map, "target", -2);   // replace target
	  FImageList->Items[1] = ic;
	  FExportedAsTarget = true;
	  }
   }
else
   {
   map = new TIEBitmap(expImage->width, expImage->height, ie24RGB);
   TLipsHelper::CopyExpImage2Bitmap(expImage, map, asIs);
   ieViewSingle->Zoom = 100;
   // append image
   ic = new TImageContainer(map, expType);
   FImageList->Add(ic);    		// new image
   if (show)
	  {
	  int idx = mView->AppendImage();
	  setIcmv(idx, ic);
	  }
   //mView->SelectedImage = idx;
   //mViewImageSelect(this, mView->SelectedImage);
   }
}
//---------------------------------------------------------------------------
void __fastcall TfrmLips::luaImportRawData(void *data, int type)
{
switch (type)
	{
	case 0:   // import contour
		{
		TExportContour *contour = static_cast<TExportContour*>(data);
		int numPts = contour->numPts;
		for (unsigned int i = 0; i < (unsigned int)numPts; i++)
			{
			TExportPoint pt = contour->points[i];
            FMapWork->Pixels[pt.x][pt.y] = TColor2TRGB(clWhite);
			}
		ieViewSingle->Update();
        }
		break;
	default:
		break;
	}
}
//---------------------------------------------------------------------------
void __fastcall TfrmLips::prepareTableImage(TFrameImage*fi, TLipsHelper::LuaTKRV *obj)
{
mViewImageSelect(this, 0);  // source image
if (obj->init_value == "#input")
   fi->ieView->IEBitmap->AssignImage(ieViewSingle->IEBitmap);
else if (obj->init_value == "#sobel")
   {
   fi->ieView->IEBitmap->AssignImage(ieViewSingle->IEBitmap);
   fi->ieView->Proc->EdgeDetect_Sobel();
   }
else
   fi->ieView->IEBitmap->AssignImage(ieViewSingle->IEBitmap);
fi->InputImage = ieViewSingle->IEBitmap;
fi->Type = obj->range_high;
if (obj->range_high == "grayscale")
   fi->ieView->Proc->ConvertToGray();
}
//---------------------------------------------------------------------------
void __fastcall TfrmLips::pushTableImage(TImageEnView *fiView, TLipsHelper::LuaTKRV *obj)
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
if (!TLipsHelper::luaInputParams)
   return "";
if (TLipsHelper::luaInputParams->Count == 0)
   return "";
String luaParams = "";
for (int i = 0; i < TLipsHelper::luaInputParams->Count; i++)
	{
	TLipsHelper::LuaTKRV *obj = static_cast<TLipsHelper::LuaTKRV*>(TLipsHelper::luaInputParams->Items[i]);
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
	else if (obj->type == "file")
	   {
	   if (rgpFileViewStyle->ItemIndex == 0)
		  {
		   TFrameListBox *fl = static_cast<TFrameListBox*>(obj->frame);
		   if (fl->list->Count == 0)
			  luaParams = luaParams + obj->key + "=string();";
		   else
			  luaParams = luaParams + obj->key + "=string(" + fl->list->Items->Strings[fl->list->ItemIndex] + ");";
		  }
	   else
		  {
		   TFrameShellComboBox *fsc = static_cast<TFrameShellComboBox*>(obj->frame);
		   luaParams = luaParams + obj->key + "=string(" + fsc->comboShell->Text + ");";
		  }
	   }
	}
return luaParams;
}
//---------------------------------------------------------------------------
bool __fastcall TfrmLips::luaCommand(const char *cmd, const char* parList)
{
FSelectionRoiEnabled = true;
String sCmd   = String(cmd).Trim().LowerCase();
String sParams = String(parList).Trim().LowerCase();
TStringDynArray tokens = SplitString(sParams, ",");
if (sCmd == "layeradd" || sCmd == "layerbackground")
   {
   if (tokens.Length < 1)
	  return false;
   TIEBitmap *map = 0;
   tokens[0] = tokens[0].Trim();
   if (tokens[0] == "source")
	   map = static_cast<TImageContainer*>(FImageList->Items[0])->Image;
   else if (tokens[0] == "target" && FImageList->Count > 1)
	   map = static_cast<TImageContainer*>(FImageList->Items[1])->Image;
   else if (tokens[0] == "last" && FImageList->Count > 0)
	   map = static_cast<TImageContainer*>(FImageList->Items[FImageList->Count - 1])->Image;
   else  // by id
	   {
	   try {
		   int idx = StrToInt(tokens[0]);
		   map = static_cast<TImageContainer*>(FImageList->Items[idx])->Image;
		   }
	   catch (...)
		   {
		   return false;
		   }
	   }
   if (map)
	  {
	  if (sCmd == "layeradd")
		 {
		 ieViewLayers->LayersAdd(map); 				// test - not shared
		 ieViewLayers->CurrentLayer->PosX = 0;
		 ieViewLayers->CurrentLayer->PosY = 0;
		 }
	  else
		 {
		 ieViewLayers->LayersCurrent = 0;
		 ieViewLayers->IEBitmap->AssignImage(map);   // test - not shared
		 }
	  }
   else
	  return false;
   }
else if (sCmd == "layerclearall")
   {
   ieViewLayers->LayersClear(true);
   ieViewLayers->Blank();
   }
else if (sCmd == "info")
   {
   if (tokens.Length < 1)
	  return false;
   labInfo->Caption = tokens[0];
   }
else if (sCmd == "updatescreen")
   {
   ieViewSingle->Update();
   }
else if (sCmd == "registermouseevent")
   {
   FMouseEvent2Lua = false;
   lipsMouse2Lua(false);
   if (tokens.Length < 3)
	  return false;
   if (tokens[0].Trim() == "true")
	  ieViewSingle->OnMouseDown = ieViewSingleMouseDown;
   else
	  ieViewSingle->OnMouseDown = 0;
   if (tokens[1].Trim() == "true")
	  ieViewSingle->OnMouseMove = ieViewSingleMouseMove;
   else
	  ieViewSingle->OnMouseMove = 0;
   if (tokens[2].Trim() == "true")
	  ieViewSingle->OnMouseUp = ieViewSingleMouseUp;
   else
	  ieViewSingle->OnMouseUp = 0;
   lipsMouse2Lua(true);
   FMouseEvent2Lua = true;
   // kill selections and mouse interaction
   ieViewSingle->MouseInteract = TIEMouseInteract();
   ieViewSingle->Deselect();
   }
else if (sCmd == "selection")
   {
   if (tokens.Length < 1)
	  return false;
   if (tokens[0] == "enableroi")
	  FSelectionRoiEnabled = true;
   else if (tokens[0] == "disableroi")
	  FSelectionRoiEnabled = false;
   else if (tokens[0] == "settargetroi")
	  {
	  if (ieViewSingle->Selected)
		 {
		 TRect sr;
		 sr.left   = ieViewSingle->SelectedRect.x;
		 sr.right  = ieViewSingle->SelectedRect.width + sr.left;
		 sr.top    = ieViewSingle->SelectedRect.y;
		 sr.bottom = ieViewSingle->SelectedRect.height + sr.top;
		 lipsSetTargetRoi(&sr, true);   // set target roi and push to lua stack
		 }
	  else
		 lipsSetTargetRoi(0, true);     // set target roi to nil and push to lua stack
	  }
   else if (tokens[0] == "setcustom")
	  {
	  if (FSelection.Image)
		 {
		 int h = FSelection.Image->Height;
		 int w = FSelection.Image->Width;
		 lipsSetCustomImage(FSelection.Image->ScanLine[h - 1], 0, w, h, 4);
		 lipsSetCustomMask(FSelection.Mask->ScanLine[h - 1], w, h, 4);
		 }
	  }
   }
else
   return false;
return true;
}
//---------------------------------------------------------------------------
void __fastcall TfrmLips::rgpAuxViewStylePropertiesChange(TObject *Sender)
{
listScriptsItemSelected(this, listScripts->SelectedItemIndex);
}
//---------------------------------------------------------------------------
// mouse events
void __fastcall TfrmLips::ieViewSingleMouseDown(TObject *Sender, TMouseButton Button,
		  TShiftState Shift, int X, int Y)
{
X = ieViewSingle->XScr2Bmp(X);
Y = ieViewSingle->YScr2Bmp(Y);
lipsMouseDown(Shift, X, Y);
}
//---------------------------------------------------------------------------
void __fastcall TfrmLips::ieViewSingleMouseMove(TObject *Sender, TShiftState Shift,
		  int X, int Y)
{
X = ieViewSingle->XScr2Bmp(X);
Y = ieViewSingle->YScr2Bmp(Y);
lipsMouseMove(Shift, X, Y);
}
//---------------------------------------------------------------------------
void __fastcall TfrmLips::ieViewSingleMouseUp(TObject *Sender, TMouseButton Button,
		  TShiftState Shift, int X, int Y)
{
X = ieViewSingle->XScr2Bmp(X);
Y = ieViewSingle->YScr2Bmp(Y);
lipsMouseUp(Shift, X, Y);
}
//---------------------------------------------------------------------------
void __fastcall TfrmLips::resetMouseEvents(void)
{
if (ieViewSingle->OnMouseDown || ieViewSingle->OnMouseMove || ieViewSingle->OnMouseUp)
   {
   luaMemo->Clear();
   luaMemo->Lines->Add("Done!");
   labInfo->Caption = "";
   }
ieViewSingle->OnMouseDown = 0;
ieViewSingle->OnMouseMove = 0;
ieViewSingle->OnMouseUp  = 0;
FMouseEvent2Lua = false;
lipsMouse2Lua(false);
}
//---------------------------------------------------------------------------
void __fastcall TfrmLips::chkLayersSBValueChange(TObject *Sender)
{
if (chkLayersSB->Checked)
   {
   for (int i = 1; i < ieViewLayers->LayersCount; i++)
	   ieViewLayers->Layers[i]->Operation = (TIERenderOperation)(comboBlend->ItemIndex);
   }
ieViewLayers->Update();
}
//---------------------------------------------------------------------------

