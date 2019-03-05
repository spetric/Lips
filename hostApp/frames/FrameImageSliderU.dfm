object FrameImageSlider: TFrameImageSlider
  Left = 0
  Top = 0
  Width = 281
  Height = 138
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
    Height = 138
    Width = 281
    object image: TdxImageSlider
      Left = 62
      Top = 20
      Width = 150
      Height = 100
      Images = imageCollection
      LookAndFeel.NativeStyle = False
      LookAndFeel.SkinName = 'Darkroom'
    end
  end
  object imageCollection: TcxImageCollection
    Left = 48
    Top = 40
  end
end
