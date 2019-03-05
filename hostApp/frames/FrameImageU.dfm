object FrameImage: TFrameImage
  Left = 0
  Top = 0
  Width = 281
  Height = 204
  TabOrder = 0
  object box: TcxGroupBox
    Left = 0
    Top = 0
    Align = alClient
    Caption = 'Image parameter'
    Style.LookAndFeel.NativeStyle = False
    Style.LookAndFeel.SkinName = 'Darkroom'
    StyleDisabled.LookAndFeel.NativeStyle = False
    StyleDisabled.LookAndFeel.SkinName = 'Darkroom'
    TabOrder = 0
    Height = 204
    Width = 281
    object btnIen: TSpeedButton
      Left = 9
      Top = 167
      Width = 48
      Height = 22
      Caption = 'Ien'
      OnClick = btnIenClick
    end
    object btnApply: TSpeedButton
      Left = 199
      Top = 167
      Width = 50
      Height = 22
      Caption = 'AppSel'
      OnClick = btnApplyClick
    end
    object ieView: TImageEnView
      Left = 3
      Top = 15
      Width = 275
      Height = 146
      Background = clBtnFace
      Ctl3D = False
      ParentCtl3D = False
      LegacyBitmap = False
      ZoomFilter = rfFastLinear
      AutoFit = True
      MouseWheelParams.Action = iemwZoom
      Align = alTop
      TabOrder = 0
    end
    object comboOptions: TComboBox
      Left = 80
      Top = 167
      Width = 113
      Height = 22
      Style = csOwnerDrawFixed
      TabOrder = 1
      Items.Strings = (
        'Input image'
        'Load image'
        'SC edges'
        'Sobel edges'
        'Invert'
        'Blur r = 1'
        'Blur r = 2'
        'Blur r = 3'
        '')
    end
  end
  object openDialog: TOpenImageEnDialog
    Ctl3D = False
    Filter = 
      'Zajedni'#269'ke grafi'#269'ke datoteke|*.tif;*.tiff;*.fax;*.g3n;*.g3f;*.xi' +
      'f;*.gif;*.jpg;*.jpeg;*.jpe;*.jif;*.pcx;*.bmp;*.dib;*.rle;*.ico;*' +
      '.cur;*.png;*.dcm;*.dic;*.dicom;*.v2;*.wmf;*.emf;*.tga;*.targa;*.' +
      'vda;*.icb;*.vst;*.pix;*.pxm;*.ppm;*.pgm;*.pbm;*.wbmp;*.jp2;*.j2k' +
      ';*.jpc;*.j2c;*.dcx;*.crw;*.cr2;*.dng;*.nef;*.raw;*.raf;*.x3f;*.o' +
      'rf;*.srf;*.mrw;*.dcr;*.bay;*.pef;*.sr2;*.arw;*.kdc;*.mef;*.3fr;*' +
      '.k25;*.erf;*.cam;*.cs1;*.dc2;*.dcs;*.fff;*.mdc;*.mos;*.nrw;*.ptx' +
      ';*.pxn;*.rdc;*.rw2;*.rwl;*.iiq;*.srw;*.psd;*.psb;*.iev;*.lyr;*.a' +
      'll;*.wdp;*.hdp;*.jxr;*.avi;*.mpeg;*.mpg;*.wmv|Sve datoteke (*.*)' +
      '|*.*|JPEG Image (*.jpg;*.jpeg;*.jpe;*.jif)|*.jpg;*.jpeg;*.jpe;*.' +
      'jif|TIFF Image (*.tif;*.tiff;*.fax;*.g3n;*.g3f;*.xif)|*.tif;*.ti' +
      'ff;*.fax;*.g3n;*.g3f;*.xif|GIF Image (*.gif)|*.gif|PaintBrush (*' +
      '.pcx)|*.pcx|Windows Bitmap (*.bmp;*.dib;*.rle)|*.bmp;*.dib;*.rle' +
      '|Windows Icon (*.ico)|*.ico|Windows Cursor (*.cur)|*.cur|Portabl' +
      'e Network Graphics (*.png)|*.png|DICOM Medical Image (*.dcm;*.di' +
      'c;*.dicom;*.v2)|*.dcm;*.dic;*.dicom;*.v2|Windows Metafile (*.wmf' +
      ')|*.wmf|Enhanced Windows Metafile (*.emf)|*.emf|Targa Image (*.t' +
      'ga;*.targa;*.vda;*.icb;*.vst;*.pix)|*.tga;*.targa;*.vda;*.icb;*.' +
      'vst;*.pix|Portable Pixmap, Graymap, Bitmap (*.pxm;*.ppm;*.pgm;*.' +
      'pbm)|*.pxm;*.ppm;*.pgm;*.pbm|Wireless Bitmap (*.wbmp)|*.wbmp|JPE' +
      'G2000 (*.jp2)|*.jp2|JPEG2000 Code Stream (*.j2k;*.jpc;*.j2c)|*.j' +
      '2k;*.jpc;*.j2c|Multipage PCX (*.dcx)|*.dcx|Camera Raw Image (*.c' +
      'r2;*.crw;*.dng;*.nef;*.raw;*.orf;*.nrw...)|*.crw;*.cr2;*.dng;*.n' +
      'ef;*.raw;*.raf;*.x3f;*.orf;*.srf;*.mrw;*.dcr;*.bay;*.pef;*.sr2;*' +
      '.arw;*.kdc;*.mef;*.3fr;*.k25;*.erf;*.cam;*.cs1;*.dc2;*.dcs;*.fff' +
      ';*.mdc;*.mos;*.nrw;*.ptx;*.pxn;*.rdc;*.rw2;*.rwl;*.iiq;*.srw|Pho' +
      'toshop PSD (*.psd;*.psb)|*.psd;*.psb|Vectorial Objects (*.iev)|*' +
      '.iev|Layers (*.lyr)|*.lyr|Layers and Objects (*.all)|*.all|Micro' +
      'soft HD Photo (*.wdp;*.hdp;*.jxr)|*.wdp;*.hdp;*.jxr|Video za win' +
      'dows (*.avi)|*.avi|MPEG Video (*.mpeg;*.mpg)|*.mpeg;*.mpg|Window' +
      's Media Video (*.wmv)|*.wmv'
    Left = 32
    Top = 16
  end
end
