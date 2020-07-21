//---------------------------------------------------------------------------

#include <vcl.h>
#pragma hdrstop
#include <tchar.h>
//---------------------------------------------------------------------------
#include <Vcl.Styles.hpp>
#include <Vcl.Themes.hpp>
USEFORM("spLipsHost\frmLipsU.cpp", frmLips);
USEFORM("spLipsHost\frames\FrameSliderU.cpp", FrameSlider); /* TFrame: File Type */
USEFORM("spLipsHost\frames\FrameShellComboBoxU.cpp", FrameShellComboBox); /* TFrame: File Type */
USEFORM("spLipsHost\lipsDownload.cpp", frmDownload);
USEFORM("spLipsHost\frames\FrameListBoxU.cpp", FrameListBox); /* TFrame: File Type */
USEFORM("spLipsHost\frames\FrameCheckBoxU.cpp", FrameCheckBox); /* TFrame: File Type */
USEFORM("spLipsHost\frames\FrameComboBoxU.cpp", FrameComboBox); /* TFrame: File Type */
USEFORM("spLipsHost\frames\FrameImageU.cpp", FrameImage); /* TFrame: File Type */
USEFORM("spLipsHost\frames\FrameImageSliderU.cpp", FrameImageSlider); /* TFrame: File Type */
USEFORM("spLipsHost\frames\FrameImageComboU.cpp", FrameImageCombo); /* TFrame: File Type */
//---------------------------------------------------------------------------
int WINAPI _tWinMain(HINSTANCE, HINSTANCE, LPTSTR, int)
{
	try
	{
		Application->Initialize();
		Application->MainFormOnTaskBar = true;
		TStyleManager::TrySetStyle("Charcoal Dark Slate");
		Application->CreateForm(__classid(TfrmLips), &frmLips);
		Application->Run();
	}
	catch (Exception &exception)
	{
		Application->ShowException(&exception);
	}
	catch (...)
	{
		try
		{
			throw Exception("");
		}
		catch (Exception &exception)
		{
			Application->ShowException(&exception);
		}
	}
	return 0;
}
//---------------------------------------------------------------------------
