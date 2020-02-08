object LipsDownload: TLipsDownload
  Left = 0
  Top = 0
  BorderStyle = bsDialog
  Caption = 'Download file'
  ClientHeight = 112
  ClientWidth = 493
  Color = clBtnFace
  Font.Charset = DEFAULT_CHARSET
  Font.Color = clWindowText
  Font.Height = -11
  Font.Name = 'Tahoma'
  Font.Style = []
  OldCreateOrder = False
  Position = poMainFormCenter
  PixelsPerInch = 96
  TextHeight = 13
  object labHTML: THTMLabel
    Left = 0
    Top = 0
    Width = 493
    Height = 71
    Align = alClient
    Font.Charset = DEFAULT_CHARSET
    Font.Color = clSilver
    Font.Height = -13
    Font.Name = 'Tahoma'
    Font.Style = []
    HTMLText.Strings = (
      
        'TMS <b>HTML</b> label <br><P align="center"><A href="www.sigmapa' +
        'i-design.com">download click</A></P>')
    ParentFont = False
    Transparent = True
    URLColor = clAqua
    Version = '1.9.4.0'
    ExplicitTop = 2
    ExplicitHeight = 136
  end
  object Panel1: TPanel
    Left = 0
    Top = 71
    Width = 493
    Height = 41
    Align = alBottom
    BevelOuter = bvNone
    TabOrder = 0
    ExplicitTop = 118
    ExplicitWidth = 499
    object btnAbort: TButton
      Left = 208
      Top = 8
      Width = 75
      Height = 25
      Caption = 'Ok'
      TabOrder = 0
      OnClick = btnAbortClick
    end
  end
end
