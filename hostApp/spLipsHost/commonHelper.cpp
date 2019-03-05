//---------------------------------------------------------------------------
#include <vcl.h>
#pragma hdrstop
#include "commonHelper.h"
#ifndef spHostApp	// check
#define spHostApp	// yes, we are the host application
#endif
#include "lipsEngineDll.h"
#pragma package(smart_init)
TMemo *TCommonHelper::luaConsole = 0;
TDateTime TCommonHelper::luaStart = Time();
TList *TCommonHelper::luaInputParams = 0;
TFrameBuilder TCommonHelper::luaParamBuilder = 0;
TImportImage TCommonHelper::luaImportImage = 0;
bool TCommonHelper::luaParamsOK = true;
TIEBitmap *TCommonHelper::luaPushImage = 0;
//---------------------------------------------------------------------------
void __stdcall TCommonHelper::LuaProgress(int done, int total)
{
double percentage = (done * 1.0 / total) * 100;
luaConsole->Lines->Strings[1] = "Processing..." + IntToStr((int)(percentage)) + "%";
//luaConsole->Update();
}
//---------------------------------------------------------------------------
void __stdcall TCommonHelper::LuaMessage(const char *msg)
{
luaConsole->Lines->Add(String(msg));
}
//---------------------------------------------------------------------------
void __stdcall TCommonHelper::LuaRequire(const char *params)
{
luaParamsOK = false;
String luaParams = String(params);
String token, remainder, argno, delim = ";";
String lth = "Lua2Host parameter ";
DeleteIPList();
luaInputParams = new TList();
TFormatSettings fmt;
fmt.DecimalSeparator = '.';
// simple parser
// SplitString works only for Delphi
int arg_count = 1, pos = luaParams.Pos(delim);
while (pos > 0)
   {
   argno = IntToStr(arg_count);
   token = luaParams.SubString(1, pos - 1);
   // check token (key, range, value) and fill the list
   LuaTKRV *obj = new LuaTKRV;
   obj->list_values = 0;
   luaInputParams->Add((void*) obj);
   obj->frame = 0;
   obj->key  = token.SubString(1, token.Pos("=") - 1).Trim();
   obj->type = StringBetween(token, "=", "(", remainder).Trim().LowerCase();
   String rv   = StringBetween(token, "(", ")", remainder).Trim();
   String r    = StringBetween(rv, "[", "]", remainder).Trim();
   obj->init_value = remainder.Trim();
   obj->range_low  = r.SubString(1, r.Pos(",") - 1).Trim();
   obj->range_high = r.SubString(r.Pos(",") + 1, r.Length()).Trim();
   // check if values are ok
   if (obj->key.IsEmpty())
	  {
	  luaConsole->Lines->Strings[0] = lth+argno+" error: name missing.";
	  return;
	  }
   if (obj->type != "int" && obj->type != "float" && obj->type != "bool" && obj->type != "combo" && obj->type != "imagefile" && obj->type != "imagemap" && obj->type != "list")
	  {
	  luaConsole->Lines->Strings[0] = lth+argno+" error: invalid type.";
	  return;
	  }
   if (obj->init_value.IsEmpty())
	  {
	  luaConsole->Lines->Strings[0] = lth+argno+" error: initial value missing.";
	  return;
	  }
   if (obj->type == "int")  // validate value
	  {
	  try {
		  int test = StrToInt(obj->init_value);
		  test = StrToInt(obj->range_low);
		  test = StrToInt(obj->range_high);
		  }
	  catch (...)
		  {
		  luaConsole->Lines->Strings[0] = lth+argno+" error: invalid integer value(s).";
		  return;
		  }
	  }
   else if (obj->type == "float")  // validate value
	  {
	  try {
		  int test = StrToFloat(obj->init_value, fmt);
		  test = StrToFloat(obj->range_low, fmt);
		  test = StrToFloat(obj->range_high, fmt);
		  }
	  catch (...)
		  {
		  luaConsole->Lines->Strings[0] = lth+argno+" error: invalid float value(s).";
		  return;
		  }
	  }
   else if (obj->type == "bool")  // validate value
	  {
	  if (obj->init_value != "false" && obj->init_value != "true")
		 {
		 luaConsole->Lines->Strings[0] = lth+argno+" error: invalid bool value.";
		 return;
		 }
	  }
   else if (obj->type == "combo" || obj->type == "list")
	  {
	  try {
		  int test = StrToInt(obj->init_value);
		  }
	  catch (...)
		  {
		  luaConsole->Lines->Strings[0] = lth+argno+" error: invalid combo/list initial value.";
		  return;
		  }
	  // special case for parsing
	  obj->list_values = new TStringList();
	  String value;
	  int vpos = r.Pos(",");
	  while (vpos > 0)
		  {
		  value = r.SubString(1, r.Pos(",") - 1).Trim();
		  obj->list_values->Add(value);
		  r = r.SubString(vpos + 1, r.Length());
		  vpos = r.Pos(",");
		  }
	  // ostatak
	  value = r.Trim();
	  obj->list_values->Add(value);
	  // check
	  for (int i = 0; i < obj->list_values->Count; i++)
		  {
		  if (obj->list_values->Strings[i].IsEmpty())
			 {
			 luaConsole->Lines->Strings[0] = lth+argno+" error: invalid value(s).";
			 return;
			 }
		  }
	  }
   else if (obj->type == "imagefile")
	  {
	  if (r.IsEmpty())
		 obj->range_low = "*.*";
	  else
		 obj->range_low = r;
	  }
   else if (obj->type == "imagemap")
	  {
	  if (obj->range_high != "grayscale" && obj->range_high != "rgb24")
		 {
		 luaConsole->Lines->Strings[0] = lth+argno+" error: invalid image type.";
		 return;
		 }
	  if (obj->range_low.IsEmpty())
		 obj->range_low = "*.*";
	  }
   luaParams = luaParams.SubString(pos + 1, luaParams.Length());
   pos = luaParams.Pos(delim);
   arg_count++;
   }
luaParamsOK = true;
// So far, so good. Now, call the method to fill slider, checkboxes, etc...
if (luaParamBuilder)
   luaParamBuilder();
return;
}
//---------------------------------------------------------------------------
bool __stdcall TCommonHelper::LuaLoadImage(const char *filename, const char* type)
{
if (luaPushImage)
   delete luaPushImage;
luaPushImage = new TIEBitmap();
luaPushImage->Read(String(filename));
int h = luaPushImage->Height;
int w = luaPushImage->Width;
String sType = String(type);
if (sType == "grayscale" && luaPushImage->ChannelCount > 1)
   {
   TImageEnProc *proc = new TImageEnProc((TComponent*)0);
   proc->AttachedIEBitmap = luaPushImage;
   proc->ConvertTo(ie8g);
   proc->AttachedIEBitmap = 0;
   delete proc;
   }
lipsPushImage(sType.c_str(), luaPushImage->ScanLine[h -1], 0, w, h, 4);
return true;
}
//---------------------------------------------------------------------------
bool __stdcall TCommonHelper::LuaExportImage(TExportImage *expImage, bool asIs)
{
if (expImage->channels != 3)
   return false;
else
   luaImportImage(expImage, asIs);
return true;
}
//---------------------------------------------------------------------------
int __stdcall TCommonHelper::LuaHostDialog(const char *dialogText)
{
String dt = String(dialogText);
if (MessageDlg(dt, mtConfirmation, TMsgDlgButtons() <<mbOK<<mbCancel, 0) == mrOk)
   return 1;
else
   return 0;
}
//---------------------------------------------------------------------------
String TCommonHelper::StringBetween(String input, String left, String right, String &remainder)
{
String token = input.SubString(input.Pos(left) + 1, input.Pos(right) - input.Pos(left) - 1);
remainder = input.SubString(input.Pos(right) + 1, input.Length());
return token;
}
//---------------------------------------------------------------------------
void TCommonHelper::DeleteIPList(void)
{
if (luaInputParams)
   {
   for (int i = 0; i < luaInputParams->Count; i++)
	   {
	   LuaTKRV *obj = static_cast<LuaTKRV*>(luaInputParams->Items[i]);
	   if (obj->frame)
		  {
          delete obj->frame;
		  obj->frame = 0;
		  }
	   if (obj->list_values)
		  {
		  delete obj->list_values;
          obj->list_values = 0;
		  }
	   delete obj;
	   luaInputParams->Items[i] = 0;
	   }
   delete luaInputParams;
   luaInputParams = 0;
   }
}
//---------------------------------------------------------------------------
void  TCommonHelper::CopyExpImage2Bitmap(TExportImage *data, TIEBitmap *bmp, bool asIs)
{
int w = data->width;
int h = data->height;
Byte *imgExp = (Byte*)(data->imageBuff);
Byte *alphaExp = (Byte*)(data->alphaBuff);
if (data->alphaStride)   // there is alpha channel in image (alphaStride > 0)
   bmp->AlphaChannel->Fill(0);
// copy ouput buffer to original image - as is
for (int i = 0; i < h; i++)
   {
   int slp = asIs ? h - i - 1 : i;
   void *imgScl = (void*)bmp->Scanline[slp];
   memcpy(imgScl, (void*)imgExp, w * data->channels);
   imgExp = imgExp + data->imageStride;  	 // forward one stride
   // alpha channel
   if (alphaExp)
	  {
	  void *alphaScl = (void*)bmp->AlphaChannel->Scanline[slp];
	  memcpy(alphaScl, (void*)alphaExp, w);
	  alphaExp = alphaExp + data->alphaStride;  // forward one stride
	  }
   }
}


