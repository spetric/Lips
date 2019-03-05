object FrameListBox: TFrameListBox
  Left = 0
  Top = 0
  Width = 281
  Height = 192
  TabOrder = 0
  object box: TcxGroupBox
    Left = 0
    Top = 0
    Align = alClient
    Caption = 'Listbox parameter'
    Style.LookAndFeel.NativeStyle = False
    Style.LookAndFeel.SkinName = 'Darkroom'
    StyleDisabled.LookAndFeel.NativeStyle = False
    StyleDisabled.LookAndFeel.SkinName = 'Darkroom'
    TabOrder = 0
    ExplicitHeight = 50
    Height = 192
    Width = 281
    object list: TListBox
      Left = 3
      Top = 15
      Width = 275
      Height = 168
      Align = alClient
      Color = clBtnFace
      ItemHeight = 13
      TabOrder = 0
      ExplicitHeight = 140
    end
  end
end
