object FrameShellComboBox: TFrameShellComboBox
  Left = 0
  Top = 0
  Width = 281
  Height = 50
  TabOrder = 0
  object box: TcxGroupBox
    Left = 0
    Top = 0
    Align = alClient
    Caption = 'Combo shell parameter'
    Style.LookAndFeel.NativeStyle = False
    Style.LookAndFeel.SkinName = 'Darkroom'
    StyleDisabled.LookAndFeel.NativeStyle = False
    StyleDisabled.LookAndFeel.SkinName = 'Darkroom'
    TabOrder = 0
    ExplicitWidth = 317
    ExplicitHeight = 66
    Height = 50
    Width = 281
    object comboShell: TcxShellComboBox
      Left = 16
      Top = 20
      Properties.ReadOnly = False
      Properties.ShowFullPath = sfpAlways
      Properties.ViewOptions = [scvoShowFiles, scvoShowZipFilesWithFolders]
      Style.LookAndFeel.NativeStyle = False
      Style.LookAndFeel.SkinName = 'Darkroom'
      StyleDisabled.LookAndFeel.NativeStyle = False
      StyleDisabled.LookAndFeel.SkinName = 'Darkroom'
      StyleFocused.LookAndFeel.NativeStyle = False
      StyleFocused.LookAndFeel.SkinName = 'Darkroom'
      StyleHot.LookAndFeel.NativeStyle = False
      StyleHot.LookAndFeel.SkinName = 'Darkroom'
      TabOrder = 0
      Width = 249
    end
  end
end
